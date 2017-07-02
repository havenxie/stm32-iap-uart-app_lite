#include <string.h>
#include "led.h"
#include "timer.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "iap.h"
//A6 A7 B0 B1

//#define USE_IAP
static u8 current_ch = 0;//ch1->pwm1 ch2->pwm2 ch3->pwm3 ch4->pwm4
#define CH1 1
#define CH2 2
#define CH3 3
#define CH4 4
void Ltelligent_lamp_Handle(u8 * cmd)
{
	u16 current_val = 0;
	if(cmd[0] == '@')//channel select
	{
		switch(cmd[1])
		{
			case CH1 + '0'://channel 1
				current_ch = CH1;
				printf("@1");
				break;
			case CH2 + '0'://channel 2
				current_ch = CH2;
				printf("@2");
				break;
			case CH3 + '0'://current 3
				current_ch = CH3;
				printf("@3");
				break;
			case CH4 + '0'://current 4
				current_ch = CH4;
				printf("@4");
				break;
			default:
				current_ch = 0;
				break;
		}
	}
	else if(cmd[0] == '$')//value set
	{
		current_val = (cmd[1]-'0') * 100 + (cmd[2]-'0') * 10 + (cmd[3]-'0');
		switch(current_ch)
		{
			case CH1:
				TIM_SetCompare1(TIM3, current_val);	
				break;
			case CH2:
				TIM_SetCompare2(TIM3, current_val);	
				break;
			case CH3:
				TIM_SetCompare3(TIM3, current_val);	
				break;
			case CH4:
				TIM_SetCompare4(TIM3, current_val);	
				break;
			default:
				break;
		}	
	}
}

int main(void)
{
	IAP_Init();          //程序进入之后首先执行iap初始化 将标志位清除
	delay_init(72);	     //延时初始化
	NVIC_Configuration();//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);   //串口初始化为9600
 	LED_Init();	         //LED端口初始化
	TIM3_PWM_Init(719, 0);//不分频。PWM频率=72000/719=10Khz
	while(1)
	{
		if(USART_RX_STA&0x80)
		{					 
			USART_RX_STA=0;
			IAP_Handle(USART_RX_BUF);
			Ltelligent_lamp_Handle(USART_RX_BUF);
		}
	}	 
}



