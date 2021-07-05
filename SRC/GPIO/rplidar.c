#include "rplidar.h"
#include "usart.h"
#include "oled.h"
#include <stdio.h>
#include <math.h>

#define RP_USART UART5
#define RP_Send(...) UART_Send(RP_USART, __VA_ARGS__)
#define RP_MAX_TIME 60000 // 600 min value, so STM32 faster than rpLIDAR (??)
#define RP_NONE_DATA_THRESHOLD 1000
#define RP_MAP_RATIO 25
//#define RP_DOT_DATA_THRESHOLD 350

#define CARE(VAL) ({ \
        __typeof__ (VAL) _VAL = (VAL); \
        ((_VAL > 50) ? -1 : ((_VAL < 0) ? -1 : _VAL)); \
    })

#define RP_GET(bufName, num, tct) \
    ({ tct = 0;\
        while(tct != RP_MAX_TIME && USART_GetFlagStatus(RP_USART, USART_FLAG_RXNE) != SET) ++ tct; \
        bufName[num] =  ((tct == RP_MAX_TIME) ? 0 : USART_ReceiveData(RP_USART)); \
    })
#define RP_GET_OR(bufName, num, tct) \
    ({ tct = 0;\
        while(tct != RP_MAX_TIME && USART_GetFlagStatus(RP_USART, USART_FLAG_RXNE) != SET) ++ tct; \
        bufName[num] |= ((tct == RP_MAX_TIME) ? 0 : USART_ReceiveData(RP_USART)); \
    })
#define RP_GET_XOR(bufName, num, tct) \
    ({ tct = 0;\
        while(tct != RP_MAX_TIME && USART_GetFlagStatus(RP_USART, USART_FLAG_RXNE) != SET) ++ tct; \
        bufName[num] ^= ((tct == RP_MAX_TIME) ? 0 : USART_ReceiveData(RP_USART)); \
    })

static u16 saveData[RP_DOT_DATA_THRESHOLD][4], maxTimeTempU16Variable; /* theta, distance */
u16 dotData[RP_DOT_DATA_THRESHOLD][4];
double dotDouble[RP_DOT_DATA_THRESHOLD][2];
#define VAR_TCTN maxTimeTempU16Variable
/*static u8 tmp[1009];*/

s32 RP_SaveData(void)
{
    u8 buffer[10] = {0};
    u16 cnt = 0, tot = 0, non = 0, flag = 3;
    /** Software Reset **/
    nullVar = RP_USART -> SR; nullVar = RP_USART -> DR; // Clear any ERROR Flag
    RP_Send(0xA525, 0);
    DELAY_ms(100);
    /** **/
    RP_Send(0xA520, 0);
    buffer[0] = buffer[1] = 0;
    for(; cnt != 2; ++ cnt)
        RP_GET_OR(buffer, 0, VAR_TCTN);
    if(buffer[0] != 0xFF)
    {
        buffer[9] = RP_USART -> SR;
        RP_Send(0xA525, 0);
        printf("err in open scanning!! AB|CD = %d SR = %d sending 0xA5, 0x25\n", buffer[0], buffer[9]);
        return -1;
    } 
    for(; cnt != 7; ++ cnt)
        RP_GET_XOR(buffer, 1, VAR_TCTN);
    if(buffer[1] != 0xC4)
    {
        RP_Send(0xA525, 0);
        printf("err in open scanning!! ELSE XOR %d sending 0xA5, 0x25\n", buffer[1]);
        return -2;
    }
    printf("%%START_DOT%%\n");
    /** WAIT until a new scan start... @see u16 flag**/
    while(non <= RP_NONE_DATA_THRESHOLD)
    {
        RP_GET(buffer, 0, VAR_TCTN);
        if(buffer[0])
        {
            for(cnt = 1; cnt != 5; ++ cnt)
                RP_GET(buffer, cnt, VAR_TCTN);
            if((buffer[0] & 0x3) == 0b01)
            {
                if(flag --)
                    break;
            }
        }
        else
        {
            ++ non;
        }
    }
    while(non < RP_NONE_DATA_THRESHOLD)
    {
        if((buffer[1] & 1) && (buffer[3] & buffer[4])) // many points with distance 0
        {
            saveData[tot][0] = ((u16)buffer[2] << 7) | (buffer[1] >> 1);
            saveData[tot][1] = ((u16)buffer[4] << 8) | (buffer[3]);
            saveData[tot][2] = buffer[0];
            if(++ tot == RP_DOT_DATA_THRESHOLD) break;
        }
        /*
        WITHOUT USART message, OLED_Refresh, every dot will received properly.
        if(USART_GetFlagStatus(RP_USART, USART_FLAG_ORE))
        {
            ++ non;
            USART_ClearFlag(RP_USART, USART_FLAG_ORE);
        }
        */
        RP_GET(buffer, 0, VAR_TCTN);
        if((buffer[0] & 0x3) != 0b10) // != 0x3E && buffer[0] != 0x02 && buffer[0] != 0x3A
        {
            if(!buffer[0])
                {++non; continue;}
            if((buffer[0] & 0x3) == 0b01)
                {break;}
            RP_Send(0xA525, 0);
            printf("err in broken package!! (tot=%d | %d) FIRST HEX NUMBER %d sending 0xA5, 0x25\n", tot, buffer[2], buffer[0]);
            return -3;
        }
        for(cnt = 1; cnt != 5; ++ cnt)
            RP_GET(buffer, cnt, VAR_TCTN);
        // TODO: May not found any valid dot.
    }
    RP_Send(0xA525, 0);
    OLED_Clear();
    OLED_DrawPoint(25, 25, 1);
    OLED_ShowString(0, 52, "center point...");
    OLED_RefreshGram();
    DELAY_ms(800); // where is the center point?
    OLED_Clear();
    for(int i = 0; i < tot; ++ i)
    {
        dotData[i][0] = saveData[i][0] >> 6;
        dotData[i][1] = saveData[i][0] & 0x3F;
        dotData[i][2] = saveData[i][1] >> 2;
        dotData[i][3] = saveData[i][1] & 0x3;
        //dotData[i][4] = dotData[i][0] * 
        dotDouble[i][0] = dotData[i][2] * cos(dotData[i][0] * 3.1415926 / 180);
        dotDouble[i][1] = dotData[i][2] * sin(dotData[i][0] * 3.1415926 / 180);
        //printf("[%d, %u.%u, %u.%u, %u, %.4f, %.4f]\n", i, dotData[i][0], dotData[i][1],
        // dotData[i][2], dotData[i][3], saveData[i][2], dotDouble[i][0], dotDouble[i][1]);
        OLED_ShowNumber(0, 52, i, 2, 12); // Progress
        OLED_ShowString(12, 52, ":   du   </>"); // Progress
        OLED_ShowNumber(18, 52, dotData[i][0], 3, 12); // Progress
        OLED_ShowNumber(54, 52, RP_MAP_RATIO, 2, 12);
        /* RE?? */
        OLED_DrawPoint(CARE(25 + dotDouble[i][1] / RP_MAP_RATIO), CARE(25 - dotDouble[i][0] / RP_MAP_RATIO), 1); // DotMap - SO it will show a UP map.
        OLED_RefreshGram();
    }
    DELAY_ms(6000); // where is the center point?
    return tot;
}