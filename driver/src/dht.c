#include "dht.h"


/*
 使用到PA8
*/


void PA8_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}

void PA8_OUT(void)//输出模式
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}

void PA8_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_8;
	//GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}


void PA8_Rest(void)//起始信号
{
	PA8_OUT();//输出电平
	
	PA8_DOWN();//拉低18ms以上
	SysTick_MS(20);//延时
	
	PA8_UP();//拉高20us到40us
	SYSTICK_US(30);
}

/*
GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)
读引脚的值
	如果是1，以返回值的形式返回1
	如果是0，就返回0
*/
u8 DHT_read_signal(void)
{
	u8 retry=0;
	PA8_IN();
	while(1==GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)&&retry<101)
	{
		retry++;
		SYSTICK_US(1);
	}
	if(retry>100)//已经超过80
		return -1;
	else
		retry=0;
	//上为低电平的判断
	
	while(0==GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)&&retry<101)
	{
		retry++;
		SYSTICK_US(1);
	}
	if(retry>100)//已经超过80
		return -1;
	
	return 0;
	//返回-1出错 返回0正确
}

u8 Read_DHT_BIT(void)//读一个位
{
	u8 retry=0;
	while(PA8_READ()==1 && retry<101)//等待温湿度模块应答信号结束
	{
		retry++;
		SYSTICK_US(1);
	}
	retry=0;
	
	while(PA8_READ()==0 && retry<101)//等待温湿度模块发送数据起始信号结束
	{
		retry++;
		SYSTICK_US(1);
	}
	
	//正式开始传输数据
	SYSTICK_US(40);//先延时40us，再去查看这个引脚是否还是高电平
	
	if(PA8_READ()==1)
		return 1;//是
	else 
		return 0;//不是
}

//一共需要发40个bit
u8 READ_DHT_Byte(void)//读一个字节函数
{
	u8 i,dat;
	for(i=0;i<8;i++)
	{
		dat =	dat << 1 ;
		dat = dat|Read_DHT_BIT();
	}
	return dat;
}

u8 DHT_READ_DATA(u8 *t,u8 *h)
{
	//一共是40bit---》buf[5];
	u8 buf[5]={0};
	u8 i;
	PA8_Rest();//起始信号
	if(0==DHT_read_signal())//判断温湿度是否有回应
	{
		//连续读5个字节
		for(i=0;i<5;i++)
		{
			buf[i]=READ_DHT_Byte();
		}
		if(buf[0]+buf[1]+buf[2]+buf[3]==buf[4])
		{
			*h=buf[0];
			*t=buf[2];
		}
		else
		{
			*h=buf[0];
			*t=buf[2];
			return -1;
		}
		
	}
	return 0;
}


