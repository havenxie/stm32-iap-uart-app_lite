#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "iap.h"

int main(void)
{
	u16 times=0;  

	IAP_Init();//程序进入之后首先执行iap初始化 将标志位清除
	
	delay_init(72);	     //延时初始化
	NVIC_Configuration();//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);   //串口初始化为9600
 	LED_Init();	         //LED端口初始化
	while(1)
	{
		if(USART_RX_STA&0x80)
		{					 
			USART_RX_STA=0;
			IAP_Handle(USART_RX_BUF);
		}else
		{
			times++;
			if(times%50==0)LED0=!LED0;//闪烁LED(PC13),提示系统正在运行.
			delay_ms(10);   
		}
	}	 
}

