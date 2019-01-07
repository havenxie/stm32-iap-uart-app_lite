

## STM32串口IAP的Application部分使用说明

> [在上一篇STM32串口IAP的boot部分使用说明](https://github.com/havenxie/stm32-iap-uart-boot)中我们提到了app部分，这一篇我们来简单说一下app的基本配置使用。

1. 该项目实现通过PC的串口对STM32系列MCU进行IAP。

2. 该项目包含三个部分（三套代码）：
    
    - 运行在STM32平台的Boot；
    - 运行在STM32平台的App(我做了两个，另一个是支持[usmart的重量版](https://github.com/havenxie/stm32-iap-uart-app)，这个是很简洁的轻量版)；
    - 运行在Windows平台的上位机操作工具。

3. 本篇是属于运行在STM32平台的Application部分（轻量版），另外两篇介绍请参阅：
    
    - [windows平台操作工具](https://github.com/havenxie/winapp-iap-uart)
    - [STM32平台的Boot](https://github.com/havenxie/stm32-iap-uart-boot)    
	
4. 这套代码很简洁，对外围电路的要求只用到了一颗LED(PC13)。用它来指示系统运行。

*****

## 工程目录结构 

- "Binary": 包含将hex文件转换成bin文件的工具，你也可以不用这个工具，直接用Keil安装目录中的hex转bin工具。我在这里用了这个工具是因为每个人的安装目录不同，不可能做到一致，直接将转换工具放在项目中可以保证每个人都会有一致的效果。（这里我已经设置好了，使用者不必关心）

- "CORE": STM32的启动文件和内核文件

- "HAREWARE": STM32外设的驱动文件
    + "LED": LED的驱动代码，这里用PC13作为LED的驱动管脚；
    + "STMFLASH": STM32内部flash的驱动代码

- "OUTPUT": 生成的中间文件及目标文件都存放在这里

- "STM32F10X-FWLib": ST官方库文件

- "SYSTEM": 程序运行的一些必要代码文件
    + "delay": 延时相关的代码
    + "iap": iap功能实现的相关代码
    + "sys": 
    + "usart": 串口功能实现的相关代码

- "USER": KEIL的工程文件及user文件
            
- "keilkill": 用来清除中间文件的bat脚本，双击运行即可

- "README": 自述文件
 
      
***** 

## 使用APP工程的方法：

1. 打开USER文件夹下的STM32-IAP-APP_LITE.uvproj工程

2. 工程配置：
    + 1.在工具栏点击“魔术棒”打开“Options for Target”面板；
    + 2.点击“Device”选择你的硬件平台（我这里是STM32F103C8）；
    + 3.点击“Target”找到下面的"IROM1",将Start中的0x8000000改成0x8003000（因为我们的app要从0x3000地址处开始存放）
    + 4.将Size中的值减去0x3000（直接16进制运算），然后将结果重新填入此处。（我这里是0x10000-0x3000=0x0D000,所以我填0x0D000,你要根据的片子进行计算一下）


3. 代码配置：
    + 找到main函数，里面第一个执行的函数要是“IAP_Init()”，该函数里我们做了两件事，第一件是把flash标志清除，第二件是重新将中断向量表设置到app的起始位置。
    + 打开iap.h文件，配置IAP_FLASH_SIZE的大小，即BOOT区的大小。（我这里是0X3000）
    + 我们在while(1)循环中轮询检测是否接收到了一个完整的命令，当接收到完整命令后清除接收标志，然后进入IAP指令处理句柄，根据不同的指令执行不同的响应。（当然你也可将这部分放在串口接收中断里）

5. 编译工程，会在 OUTPUT 文件夹下生成hex文件和bin文件。

6. 通过SWD或者JTAG等其他方式将boot部分烧录片内，该片子应该选择MD的boot(在烧录之前将flash全片擦除干净)。

7. 打开上位机工具，通过串口和MCU进行连接。

7. 选择上述生成的bin文件，将app通过上位机工具写入flash。

8. 烧完之后会自动运行app。

注：boot部分只需要烧录一次即可，之后所有操作都通过上位机工具完成。


*****

## 上位机工具配置：
    
    + 数据位长度 = 8 Bits
    + 1位停止位
    + 无校验位
    + 波特率 = 115200 baud（根据你的项目需求选择合适的波特率）
    + 硬件流控: None 
    + “包长度”指的是数据传输过程中每一包的长度。可根据你的需要选择合适的包长度。

*****

## 版本说明：
- 用户使用master版本即可。

- 智能灯项目ltelligent_lamp，内含两个工程，一个用来发送命令，另外一个用来接收命令并输出PWM。

## 更新说明：

> 代码时隔一年多再次更新，增加了通过后备寄存器来存储各种状态标志，使用方法见下：

选择iap.h文件中的`define USE_BKP_SAVE_FLAG     1  //1:使用后备寄存器存储flag标志，0:使用flash存储flag标志（之前的版本就是这样）`
注意boot部分的USE_BKP_SAVE_FLAG值要和boot保持一致