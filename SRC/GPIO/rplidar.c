//#include "rplidar.h"
#include "usart.h"
#include "oled.h"
#include <stdio.h>
#include <math.h>

#define RP_USART UART5
#define RP_Send(...) UART_Send(RP_USART, __VA_ARGS__)
#define RP_MAX_TIME 60000 // 600 min value, so STM32 faster than rpLIDAR (??)

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

u16 saveData[1009][3]; /* theta, distance, first */

s32 RP_SaveData(void)
{
    u8 buffer[10] = {0};
    u16 cnt = 0, tctn = 0, tot = 0;
    nullVar = RP_USART -> SR; nullVar = RP_USART -> DR; // Clear any ERROR Flag
    RP_Send(0xA5200000, 0);
    buffer[0] = buffer[1] = 0;
    for(; cnt != 2; ++ cnt)
        RP_GET_OR(buffer, 0, tctn);
    if(buffer[0] != 0xFF)
    {
        RP_Send(0xA5250000, 0);
        printf("err in open scanning!! AB|CD = %d sending 0xA5, 0x25\n", buffer[0]);
        return 1;
    } 
    for(; cnt != 7; ++ cnt)
        RP_GET_XOR(buffer, 1, tctn);
    if(buffer[1] != 0xC4)
    {
        RP_Send(0xA5250000, 0);
        // DEBUG printf("err in open scanning!! ELSE XOR %d %d %d %d sending 0xA5, 0x25\n", buffer[2], buffer[3], buffer[4], buffer[5]);
        printf("err in open scanning!! ELSE XOR %d sending 0xA5, 0x25\n", buffer[1]);
        return 1;
    }
    printf("such a brilliant!");
    // DEBUG
    /*RP_GET(buffer, 0, tctn); RP_GET(buffer, 1, tctn); RP_GET(buffer, 2, tctn); RP_GET(buffer, 3, tctn); RP_GET(buffer, 4, tctn);
    OLED_ShowNumber(0, 0, buffer[0], 2, 12); OLED_ShowNumber(12, 0, buffer[1], 2, 12);
    OLED_ShowNumber(24, 0, buffer[2], 2, 12); OLED_ShowNumber(36, 0, buffer[3], 2, 12);
    OLED_ShowNumber(48, 0, buffer[4], 2, 12); OLED_RefreshGram(); -> 0 0 0 0 0
    DELAY_ms(30000);*/
    while(1)
    {
        RP_GET(buffer, 0, tctn);
        if(buffer[0] != 0x3E && buffer[0] != 0x02)
            continue;
        for(cnt = 1; cnt != 5; ++ cnt)
            RP_GET(buffer, cnt, tctn);
        if((buffer[1] & 1) && (buffer[3] & buffer[4])) // many points with distance 0
        {
            saveData[tot][0] = ((u16)buffer[2] << 7) | (buffer[1] >> 1);
            saveData[tot][1] = ((u16)buffer[4] << 8) | (buffer[3]);
            saveData[tot][2] = buffer[0];
            if(++ tot >= 100) break;
            OLED_ShowNumber(0, 10, tot, 2, 12); 
            //OLED_RefreshGram(); // Data Loss...
        }
        // TODO: May not found any valid dot.
    }
    RP_Send(0xA5250000, 0);
    OLED_Clear();
    for(int i = 0; i < tot; ++ i)
    {
        u32 th = saveData[i][0] >> 6, ro = saveData[i][1] >> 2;
        double x = ro * cos(th * 3.1415926 / 180), y = ro * sin(th * 3.1415926 / 180);
        //OLED_ShowNumber(i, 10, i + 200, 3, 12); // ProgressBar
        OLED_DrawPoint((u8)(63.5 + x / 4), (u8)(31.5 + y / 4), 1); // DotMap
        OLED_RefreshGram();
        printf("%d [%u] th %u ro %u <%.4f, %.4f>\n", i, saveData[i][2], th, ro, x, y);
    }
    return 0;
}