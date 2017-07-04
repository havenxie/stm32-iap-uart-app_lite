#include <string.h>
#include "key.h"
#include "timer.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "iap.h"

//#define USE_IAP
static u8 current_ch = 0;
static u16 current_val = 0;
static u16 ch1_val, ch2_val, ch3_val, ch4_val = 0;

#define CH1 1            //PA6
#define CH2 2            //PA7
#define CH3 3            //PB0
#define CH4 4            //PB1

#define FUN_KEY 1        //PB10
#define VAL_ADD_KEY 2    //PB11
#define VAL_SUB_KEY 3    //PB12
//#define 

#define MAX_VAL     710
#define MIN_VAL     0
#define STEP_VAL    10

#define CHx_Change_Val(rule, value) \
{\
	if(rule == VAL_ADD_KEY)\
		value = value >= MAX_VAL ? MAX_VAL : value + STEP_VAL;\
	else if(rule == VAL_SUB_KEY)\
		value = value <= MIN_VAL ? MIN_VAL : value - STEP_VAL;\
	current_val = value;\
}

void Calc_CHx_Val(u8 rule)
{
	switch(current_ch)
	{
		case CH1:
			CHx_Change_Val(rule, ch1_val);
			break;
		case CH2:
			CHx_Change_Val(rule, ch2_val);
			break;
		case CH3:
			CHx_Change_Val(rule, ch3_val);
			break;
		case CH4:
			CHx_Change_Val(rule, ch4_val);
			break;
		default: 
			break;
	}
}

void Send_Meg(u8 rule)
{
	static u16 this_val = 0;
	if(rule == FUN_KEY)
	{
		USART_TX_BUF[0] = '@';
		USART_TX_BUF[1] = current_ch + '0';
		USART_TX_BUF[2] = '\r';
		USART_TX_BUF[3] = '\n';
		printf("%s", (char *)USART_TX_BUF);
		memset(USART_TX_BUF, 0, 4);
	}
	else if(this_val != current_val)
	{
		this_val = current_val;
		USART_TX_BUF[0] = '$';
		USART_TX_BUF[1] = current_val / 100 + '0';
		USART_TX_BUF[2] = current_val % 100 / 10 + '0';
		USART_TX_BUF[3] = current_val % 10 + '0';
		USART_TX_BUF[4] = '\r';
		USART_TX_BUF[5] = '\n';
		printf("%s", (char *)USART_TX_BUF);	
		memset(USART_TX_BUF, 0, 6);
	}

}

void Ltelligent_lamp_Handle(u8 * cmd);

int main(void)
{
	IAP_Init();          //程序进入之后首先执行iap初始化 将标志位清除
	delay_init(72);	     //延时初始化
	NVIC_Configuration();//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);   //串口初始化为115200
 	KEY_Init();	         //LED端口初始化
	
	while(1)
	{		
		switch(KEY_Scan())
		{
			case FUN_KEY:
				current_ch = (current_ch + 1) % 5;
				current_ch = current_ch == 0 ? 1 : current_ch;
				Send_Meg(FUN_KEY);
				break;
			case VAL_ADD_KEY:
				Calc_CHx_Val(VAL_ADD_KEY);
				Send_Meg(VAL_ADD_KEY);
				break;
			case VAL_SUB_KEY:
				Calc_CHx_Val(VAL_SUB_KEY);
				Send_Meg(VAL_SUB_KEY);
				break;
			default: 
				break;
		}
	}	 
}



