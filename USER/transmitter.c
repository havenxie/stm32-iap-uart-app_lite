#include <string.h>
#include "led.h"
#include "timer.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "iap.h"
//A6 A7 B0 B1

//#define USE_IAP
#define CH1 1
#define CH2 2
#define CH3 3
#define CH4 4


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
		}
	}	 
}
