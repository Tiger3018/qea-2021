#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include "user.h"

/* 本小车的RESET确实就是软件复位键。 */

int main()
{
    u32 cnt = 0, cnt2 = 0;
    SYS_Init();
    OLED_Init();
    USART3_Init(9600);
    UART5_Init(115200);
    while(1)
    {
        DELAY_ms(555);
        UART5_Send(0xA5250000, 0);
        OLED_ShowString(25, 25, "Init...\n");
        OLED_RefreshGram();
        RP_SaveData();
        OLED_ShowString(25, 25, "Gone...\n");
        OLED_RefreshGram();
        //DELAY_ms(100);
        // //USART_SendData(USART3, 35);
        // ++ cnt;
        // if(cnt == 200000000)
        // {
        //     cnt = 0;
        //     ++ cnt2;
        //     UART5_Interact(cnt2);
        //     //OLED_Clear();
        //     OLED_ShowString(25, 35, "done!");
        //     OLED_ShowNumber(25, 45, cnt2, 2, 12);
        //     OLED_RefreshGram();
        // }
    }
    // led_show();
}