#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"

//********************************************************************************
 
void KEY_Init(void) //IO初始化
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	//init GPIOA.13,GPIOA.15  上拉输入
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//init GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 

	GPIO_Init(GPIOA, &GPIO_InitStructure);
		 

}
u8 KEY_Scan(void)
{	 
	static u8 key_up=1;//按键按松开标志	
 
 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	if(key_up&&(KEY0==0||KEY1==0||KEY2==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)
		{
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
			return 1;
		}
		else if(KEY1==0)
		{
	     	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
			return 2;
		}
		else if(KEY2==1)
		{
	    	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
			return 3;
		}
	}else if(KEY0==1&&KEY1==1&&KEY2==0)key_up=1; 	    
 
 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	return 0;// 无按键按下
}
