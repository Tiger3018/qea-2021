#include "user.h"

/* 本小车的RESET确实就是软件复位键。 */

int main()
{
    s32 ret;
    SYS_Init();
    OLED_Init();
    USART3_Init(9600);
    UART5_Init(115200);
    PWM_Init(7199, 0); //初始化PWM 10KHZ，用于驱动电机
    if(TIM2_InitEncoder(), TIM3_Init(7199, 49), TIM4_InitEncoder(), 1);
    while(1)
    {
        //UART_Send(UART5, 0xA5250000, 0);
        OLED_ShowString(10, 10, "Init...\n");
        OLED_RefreshGram();
            DELAY_ms(6000);
        if((ret = RP_SaveData()) < 0)
        {
            MOTOR_Set(0, 0);
            OLED_Clear();
            OLED_ShowString(10, 10, "Return:\n");
            OLED_ShowNumber(52, 10, ret, 1, 12);
            OLED_RefreshGram();
            DELAY_ms(500);
        }
        else
        {
            printf("%d!\n", ret);
            STATUS_CirclePoints(dotData, dotDouble, ret);
            MOTOR_Set(200, 200);
            DELAY_ms(4000);
        }
    }
    // led_show();
}