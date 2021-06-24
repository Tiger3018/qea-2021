//#include "rplidar.h"
#include "usart.h"
#include "oled.h"
#include <stdio.h>
#include <math.h>

#define RP_USART UART5
#define RP_Send(...) UART5_Send(__VA_ARGS__)
#define RP_MAX_TIME 60000 // 600

#define RP_GET(bufName, num, tct) \
    ({ tct = 0;\
        while(tct != RP_MAX_TIME && USART_GetFlagStatus(RP_USART, USART_FLAG_RXNE) != SET) ++ tct; \
        bufName[num] =  ((tct == RP_MAX_TIME) ? 0 : RP_USART -> DR); \
    })
#define RP_GET_OR(bufName, num, tct) \
    ({ tct = 0;\
        while(tct != RP_MAX_TIME && USART_GetFlagStatus(RP_USART, USART_FLAG_RXNE) != SET) ++ tct; \
        bufName[num] |= ((tct == RP_MAX_TIME) ? 0 : RP_USART -> DR); \
    })
#define RP_GET_XOR(bufName, num, tct) \
    ({ tct = 0;\
        while(tct != RP_MAX_TIME && USART_GetFlagStatus(RP_USART, USART_FLAG_RXNE) != SET) ++ tct; \
        bufName[num] ^= ((tct == RP_MAX_TIME) ? 0 : RP_USART -> DR); \
    })

u32 saveData[1009][3]; /* theta, distance, first */

s32 RP_SaveData(void)
{
    u8 buffer[10] = {0};
    u16 cnt = 0, tctn = 0, tot = 0;
    /*RP_Send(0xA5200000, 0);
    for(int i = 1; i <= 100; ++ i)
    {
    RP_GET(buffer, 0, tctn);
    USART3_Send((u32)buffer[0] << 24);
    }
    USART3_Send(0xffffffff);
    return 0;*/
    //RP_Send((int)1, (int)2, (int)3, (int)4, (int)-1);
    RP_Send(0xA5200000, 0);
    buffer[0] = buffer[1] = 0;
    for(; cnt != 2; ++ cnt)
        RP_GET_OR(buffer, 0, tctn);
    for(; cnt != 7; ++ cnt)
        RP_GET_XOR(buffer, 1, tctn);
    if(buffer[0] != 0xFF || buffer[1] != 0xC4)
    {
        RP_Send(0xA5250000, 0);
        printf("err in open scanning!! %d %d sending 0xA5, 0x25\n", buffer[0], buffer[1]);
        //while(USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET);
        return 1;
    }
    RP_GET(buffer, 0, tctn);
    OLED_ShowNumber(0, 0, buffer[0], 2, 12);
    OLED_RefreshGram();
    DELAY_ms(3000);
    RP_GET(buffer, 0, tctn);
    RP_GET(buffer, 1, tctn);
    OLED_ShowNumber(0, 0, buffer[1], 2, 12);
    OLED_RefreshGram();
    DELAY_ms(3000);
    //printf("such a brilliant!");
    while(1)
    {
        RP_GET(buffer, 0, tctn);
        //printf("ff");
        if(buffer[0] != 0x3E && buffer[0] != 0x02)
            continue;
        for(++ cnt; cnt <= 5; ++ cnt)
            RP_GET(buffer, cnt, tctn);
        OLED_ShowNumber(0, 0, buffer[0], 2, 12);
        OLED_ShowNumber(10, 0, buffer[1], 2, 12);
        OLED_RefreshGram();
        if((buffer[1] & 1) && (buffer[3] & buffer[4])) // many points with distance 0
        {
            saveData[tot][0] = ((u16)buffer[2] << 7) | (buffer[1] >> 1);
            saveData[tot][1] = ((u16)buffer[4] << 8) | (buffer[3]);
            saveData[tot][2] = buffer[0];
            if(++ tot >= 50) break;
            OLED_ShowNumber(20, 0, tot, 2, 12);
            OLED_RefreshGram();
        }
    }
    //return 0;
    OLED_Clear();
    for(int i = 0; i < tot; ++ i)
    {
        u32 th = saveData[i][0] >> 6, ro = saveData[i][1] >> 2,
            x = ro * cosh(th), y = ro * sinh(th);
        OLED_ShowNumber(i, 10, buffer[0], 2, 12);
        OLED_RefreshGram();
        printf("%d [%u] th %u ro %u <%u, %u>\n", i, th, ro, x, y);
    }
    //DELAY_ms(700);
    //RP_Send(0xA5000000, 0);
    //while(USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET);
    return 0;
}