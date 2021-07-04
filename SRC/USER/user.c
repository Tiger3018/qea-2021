#include "user.h"

/* 本小车的RESET确实就是软件复位键。 */

int main()
{
    u32 cnt = 0, cnt2 = 0;
    SYS_Init();
    PWM_Init(7199, 0); //初始化PWM 10KHZ，用于驱动电机
    PWM_Set(3000, 2000);
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