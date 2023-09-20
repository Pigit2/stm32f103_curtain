#include "usart.h"
#include "oled.h"

/*
	USART1--串口---- TX    RX
									PA9    PA10
									GPIOA
	1、初始化引脚---找到对应的引脚---查看手册
	2、初始化USART1的功能---串口1
	3、使能串口1
*/

/*
		
		发送函数：void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
		接收函数：uint16_t USART_ReceiveData(USART_TypeDef* USARTx);
								
*/

u8 Head_Flag=0;//头针
u8 Tail_Flag=0;//尾针
u8 mainbuf_flag=0;//控制标志位

char buf[20] = "";//命令缓存数组
char mainbuf[20] = "";//命令操作数组

void SYSTICK_US(u32 how_many_us)//滴答延时函数 us 填1就是1us 
{
	SysTick->CTRL &= ~(0x05);//
	SysTick->CTRL &= ~(1<<16);//清0
	SysTick->LOAD =how_many_us*9;//滴答定时器是系统时钟线分过的---8分频--72/8=9
	SysTick->VAL  =0;

	SysTick->CTRL |= (0x01);
	while(0==(SysTick->CTRL & (1<<16)));
	SysTick->CTRL &= ~(0x05);//清0
	SysTick->CTRL &= ~(1<<16);//清0
}

void SysTick_TIMES(unsigned int times)//滴答延时函数 0.1s
{
	for(;times>0;times--)
	{
		SYSTICK_US(100000);
	}
}

void SysTick_MS(unsigned int times)//滴答延时函数 0.1s
{
	for(;times>0;times--)
	{
		SYSTICK_US(1000);
	}
}

void USART1_GPIO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//模式---外发送--输出模式--复用--推挽输出
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9;//引脚
	GPIO_InitStruct.GPIO_Speed=	GPIO_Speed_50MHz;//反转评率---由于我们是使用此引脚作为发送消息
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);//TX---PA9
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN_FLOATING;//模式---输入模式---浮空输入
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_10;//引脚
	//GPIO_InitStruct.GPIO_Speed=;不用填---在输入模式下这个成员没有意义
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);//RX---PA10--接收--输入
}

void USART1_INIT(void)
{
	USART_InitTypeDef USART_InitStruct;
	//打开串口1的时钟线---》APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	USART_InitStruct.USART_BaudRate=115200;//波特率---》115200 9600 
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//硬件流控制---》选择不用
	USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//模式---》TX模式 RX模式 
	USART_InitStruct.USART_StopBits=USART_StopBits_1;//停止位 1bit
	USART_InitStruct.USART_Parity=USART_Parity_No;//不加校验
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//字大小--》8bit
	
	//初始化串口1---》USART_Init();
	//参数1：初始化串口who，参数2：串口配置结构体指针
	USART_Init(USART1,&USART_InitStruct);
	
}

void USART1_IT_INIT(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	//NVIC--->处理中断优先级 misc.c
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;//通道——————开启中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//使能-ENABLE
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);
	
	//参数1:开启那个串口的中断 参数2：什么时候产生中断 参数3：使能
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//使能串口1---USART_Cmd();开启中断后 才打开串口的功能
	USART_Cmd(USART1,ENABLE);
}

/*
 在STM32F103中 中断函数是系统写好
		需要在启动文件中的 中断向量表
											找到中断函数的名字

			中断函数是系统产生中断时自动执行

中断服务函数中，不能处理太多东西---在中断函数中的时间不能过长

					
*/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE))//得到一个接收标志
	{
		
		
		USART_SendData(USART1,USART_ReceiveData(USART1));
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//清除这个标志
	}
}


//Usart2的初始化：PA2 PA3

void USART2_GPIO_INIT(void)
{
		GPIO_InitTypeDef  GPIO_InitStruct;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//推挽输出--高低电平 复用 
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStruct);//TX
	
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;
		GPIO_Init(GPIOA,&GPIO_InitStruct);//RX
}

void USART2_INIT(void)
{
	USART_InitTypeDef  USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	USART_InitStruct.USART_BaudRate=115200;//wifi默认是115200
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;//??
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	USART_Cmd(USART2,ENABLE);

}

void USART2_IRQHandler(void)//中断函数不需要声明，也不需要调用
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE))
	{
		//把串口2的数据发送至串口1中
		u8 temp;
		static u8 point =0;
		temp = USART_ReceiveData(USART2);
		
		if(3==Head_Flag)
		{
			buf[point++]=temp;
			if(1==Tail_Flag)//尾针收到第一帧以后
			{
				if('\n'==temp)//尾针收到第二针，命令结束
				{
					Tail_Flag=2;
					//进行控制
					strcpy(mainbuf,buf);//将buf复制给到mainbuf
					mainbuf_flag=1;
				}
				else
				{
					Tail_Flag=0;//如果不是尾针复位
				}
			}
		}
		//如果失败--复位所有
		//当命令获取成功后，要将刚才操作的所有标志位，全部复位
		if(2==Tail_Flag||point>19)
		{
			Head_Flag=0;
			Tail_Flag=0;
			point=0;
			memset(buf,0,sizeof(buf));
			//strcpy(buf,"");
		}
		
		if('\r'==temp)//尾针
		{                
			Tail_Flag=1;
		}
		
		/*以下代码都是头针检测*/
		if(2==Head_Flag)
		{
			if('D'==temp)//确定是D，意味着我们连续收到了ORD
			{
				Head_Flag=3;//确定这是一条命令，将后面的所有信息收到buf[]
			}
			else 
			{
				Head_Flag=0;//如果不是我们把头针复位--为初始状态
			}
		}
		if(1==Head_Flag)
		{
			if('R'==temp)//确定是R，意味着我们连续收到了OR
			{
				Head_Flag=2;//继续检测下一个字母是不是D
			}
			else 
			{
				Head_Flag=0;//如果不是我们把头针复位--为初始状态
			}
		}
		if(('O'==temp)&&(0==Head_Flag))//头针在初始状态下，收到了一个O
		{
			Head_Flag=1;//认为这是命令的开头----接下来检测后面的两个字母是不是R D
		}
		USART_SendData(USART1,USART_ReceiveData(USART2));//必须加
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}

/*参数1：你需要发送的指令（信息）参数2:发送到那个串口*/
/*字符类型指针，字符数组名字----》指针  解引用   结尾都会给自动加上\0*/
void SEND_BITS_TO_USART(char *buf, USART_TypeDef* USARTx)
{
	USART_ClearFlag(USARTx,USART_FLAG_TXE);//清标志
	while('\0'!=*buf)
	{
		USART_SendData(USARTx,*buf++);
		while(RESET==USART_GetFlagStatus(USARTx,USART_FLAG_TXE));//等待发送完成
		USART_ClearFlag(USARTx,USART_FLAG_TXE);//清标志
	}
}



int fputc(int ch, FILE *f)
{
  USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);  
	return ch;
}

int fgetc(FILE *f)
{

 while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)==RESET);
 return USART_ReceiveData(USART1);	

}

///*wifi连接函数*/
//void WIFI_LOGIN(void)
//{
//	OLED_Clear();
//	OLED_ShowCHinese2(22,1,24,5);//正在初始化
//	/*向出串口发送一句指令*/
//	SEND_BITS_TO_USART("DEL\r\n",USART2);//删除?
//	SysTick_TIMES(10);
//	SEND_BITS_TO_USART("AT+RST\r\n",USART2);//重启
//	SysTick_TIMES(10);
//	SEND_BITS_TO_USART("AT+CWMODE=1\r\n",USART2);//模式--选择连接路由器的模式
//	SysTick_TIMES(5);
//	SEND_BITS_TO_USART("AT+CWJAP=\"PDCN\",\"13539928668\"\r\n",USART2);//连接路由
//	SysTick_TIMES(60);
//	SEND_BITS_TO_USART("AT+CIPSTART=\"TCP\",\"114.215.65.250\",4508\r\n",USART2);
//	SysTick_TIMES(6);
//	SEND_BITS_TO_USART("AT+CIPSEND=18\r\n",USART2);//辅助指令，告诉wifi模块，真实指令的长度
//	SysTick_TIMES(6);
//	SEND_BITS_TO_USART("KEY:peiqi2+1122334\r\n",USART2);//真实指令
//	SysTick_TIMES(5);
//	SEND_BITS_TO_USART("FINISH\r\n",USART1);//test
//	SysTick_TIMES(10);
//	OLED_Clear();
//}


/*wifi热点初始化*/
void WIFI_LOGIN(void)
{
	OLED_Clear();
	OLED_ShowCHinese2(22,1,24,5);//正在初始化
	/*向出串口发送一句指令*/
	SEND_BITS_TO_USART("DEL\r\n",USART2);//删除?
	SysTick_TIMES(10);
	SEND_BITS_TO_USART("AT+RST\r\n",USART2);//重启
	SysTick_TIMES(10);
	SEND_BITS_TO_USART("AT+CWMODE=3\r\n",USART2);//模式
	SysTick_TIMES(5);
	SEND_BITS_TO_USART("AT+CWSAP=\"ESP8266\",\"12345678\",1,3\r\n",USART2);//创建WiFi
	SysTick_TIMES(60);
	SEND_BITS_TO_USART("AT+CWLIF\r\n",USART2);//验证
	SysTick_TIMES(10);
	SEND_BITS_TO_USART("AT+CIPMUX=1\r\n",USART2);//多路连接
	SysTick_TIMES(20);
	SEND_BITS_TO_USART("AT+CIPSERVER=1,8080\r\n",USART2);
	SysTick_TIMES(30);
	SEND_BITS_TO_USART("AT+CIFSR\r\n",USART2);//返回信息
	SysTick_TIMES(10);
	SEND_BITS_TO_USART("FINISH\r\n",USART1);//test
	SysTick_TIMES(10);
	OLED_Clear();
}


