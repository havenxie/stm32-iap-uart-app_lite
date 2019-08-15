#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f10x.h"

typedef enum 
{  
	Button_KEY0 = 0,
	Button_KEY1 = 1,
	Button_WAKEUP = 2
} Button_TypeDef;

typedef struct  
{   GPIO_TypeDef*       BUTTON_PORT ;
    uint16_t            BUTTON_PIN ;
    GPIOMode_TypeDef    BUTTON_MODE;
}Button_PinModeDef;	

#define KEY0 PBin(10)   //PB10
#define KEY1 PBin(11)	//PB11 
#define KEY2 PBin(12)	//PB12
#define KEY3 PBin(13)   //PB13
	 
void KEY_Init(void);//IO初始化
u8 KEY_Scan(void);  //按键扫描函数					    
#endif
