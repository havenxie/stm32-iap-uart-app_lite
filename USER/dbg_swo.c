#include <string.h>
#include "delay.h"
#include "sys.h"
#include "usart.h"

u8 write_buff[30] = "\n\r\n\rPlease enter your name:";
u8 read_buff[30]= "";
u8 name[] = "\n\rYour name is : ";
    
int main(void)
{
    uint32_t cnt = 0;
	delay_init(72);	     //延时初始化
	NVIC_Configuration();//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    printf("\r\nHello World\r\n");
	while(1)
	{	
        printf((const char *)write_buff);
        memset(read_buff, 0, 30);
        scanf("%s",read_buff);
       
        printf((const char*)name);
        printf((const char*)read_buff);

//        printf("Swo test times = %d\r\n", cnt++);
//        delay_ms(100);
        
	}	 
}



