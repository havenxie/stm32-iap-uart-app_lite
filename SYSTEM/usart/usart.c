#include "sys.h"
#include "usart.h"
////////////////////////////////////////////////////////////////////////////////// 	  

#ifdef  SWO_PRINTF

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))  
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))  
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))  
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))  
#define TRCENA          0x01000000  

volatile int32_t ITM_RxBuffer=ITM_RXBUFFER_EMPTY;

int fputc(int ch, FILE *f)
{      
	if (DEMCR & TRCENA)  
    {  
        while (ITM_Port32(0) == 0);  
  
        ITM_Port8(0) = ch;  
    }  
    return(ch);  
}

int fgetc(FILE *f)
{
  while (ITM_CheckChar() == 0);
  return ITM_ReceiveChar();
}
 

int ferror(FILE *f)
{
    /* Your implementation of ferror */
    return EOF;
}
 
//void _ttywrch(int c)
//{
//    fputc(c, 0);
//}
 
int __backspace()
{
    return 0;
}

#else 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;

}
#endif

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
FILE __stdin;  

//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 


 
u8 USART_RX_BUF[64];     //接收缓冲,最大64个字节.
u8 USART_TX_BUF[6];
//接收状态
//bit7，接收完成标志
//bit6，接收到0x0d
//bit5~0，接收到的有效字节数目
u8 USART_RX_STA=0;       //接收状态标记

void uart_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
  
   //USART 初始化设置
   
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
   

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
   
    USART_Cmd(USART1, ENABLE);                    //使能串口 
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		if((USART_RX_STA&0x80)==0)//接收未完成
		{
			if(USART_RX_STA&0x40)//接收到了0x0d
			{
				if(Res!=0x0a)
					USART_RX_STA=0;//接收错误,重新开始
				else //接收完成了 
				{
					USART_RX_STA|=0x80;	//做标志
					//IAP_Handle();
				}					
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x40;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3F]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>63)USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
     } 
} 
