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
        DELAY_ms(1000);
        //UART_Send(UART5, 0xA5250000, 0);
        OLED_ShowString(25, 25, "Init...\n");
        OLED_RefreshGram();
        RP_SaveData();
    }
    // led_show();
}