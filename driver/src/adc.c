#include "adc.h"



u16 air_Light[NUM][CHANNEL]={0};
/*
	定义一个函数，初始化一个ADC 用于采集与转换光敏电阻的模拟量
	光敏电阻的特性：
					随着光照强度的增加，电压量会变小，
												减少						大
	初始化一个芯片内部的器件
			1、判断这个器件有没有使用到GPIO，借用到引脚？
					必须初始化引脚（GPIO）
			2、初始化这个器件
			3、使能（开启）器件的工作
			
	查看手册，查看原理图----光敏电阻连接的gpio----PC0
															ADC通道-----0号通道
	stm32f103系列---3个12位精度的ADC
										ADC1 ADC2 ADC3
										
	ADC的时钟频率是不能超过14MHz
	
	stm32f103系列-主频-——-72Mhz
	将主频分频--分频到低于14Mhz
	
*/

//PC0--->光敏电阻
void Light_AIR_ADC1_INIT(void)//初始化PC0的光敏电阻ADC采集
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	//必须初始化引脚（GPIO）
	//开时钟线---GPIOC---》APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AIN;//模式---》读其他器件电压值 ADC特有一种--》模拟输入
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;//第0号
	GPIO_Init(GPIOC,&GPIO_InitStruct);//采集引脚初始化--PC0

	
	//初始化这个器件--->ADC（按着套路来）---打开时钟线--ADC----XXX_Init();
	ADC_InitStruct.ADC_ContinuousConvMode=ENABLE;//连续转换模式--开启/关闭--DMA
	ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;//对齐方式--左对齐 右对齐
	ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//外部触发
	ADC_InitStruct.ADC_Mode=ADC_Mode_Independent;//模式，选择独立模式
	ADC_InitStruct.ADC_NbrOfChannel=2;//开启多少通道-->1-->2
	ADC_InitStruct.ADC_ScanConvMode=ENABLE;//连续扫描---扫描在多通道的时候开启
	
	ADC_Init(ADC1,&ADC_InitStruct);
	//3、使能（开启）器件的工作  xxx_Cmd();
	ADC_Cmd(ADC1,ENABLE);
	
	
	//之前转换数据的时候---转换的寄存器需要被复位
	//复位校准器
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	//启动校准器
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	/*采集PA0-----》0号通道*/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
	
	/*采集PC0-----》10号通道*/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_10,2,ADC_SampleTime_239Cycles5);//采样时间越长，数据就越准确
	
	/*打开DMA的时候使用++++++++++++++++++++++++++++++*/
	ADC_DMACmd(ADC1,ENABLE);
	DMA_INIT();
	/*打开DMA的时候使用===============================*/
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//软件开启我们的ADC采集
}


/*
	初始化DMA函数----DMA1
	STM32F103RCT6
*/
void DMA_INIT(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	//开时钟---DMA1
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitStruct.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//源地址---》片上外设地址（片上外设就是位于芯片内部的器件）
	DMA_InitStruct.DMA_MemoryBaseAddr=(u32)air_Light;//内存地址
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC;//搬运方向
	
	DMA_InitStruct.DMA_BufferSize=NUM*CHANNEL;//内存大小
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//自增 不变
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;//需要自增
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//数据大小
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;//循环搬运---30
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;//最高
	DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;//memory to memory
	
	
	//DMA1初始化----XXX_Init();
	DMA_Init(DMA1_Channel1,&DMA_InitStruct);
	
	//打开DMA---XXX_Cmd();
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
}
/*采集数据获取函数*/
float ADC_GET_VAL(u8 channel)
{
	u32 add=0;
	u8 i;
	for(i=0;i<NUM;i++)
	{
		add = add + air_Light[i][channel];
	}
	return (float)(add/NUM);
}

/*
0-4095
0-3.3

3.3x/4095=实际的电压值

*/
/*采集换算函数*/
float ADC_AIR_LIGHT_VAL(u8 channel)
{
	return 3.3*ADC_GET_VAL(channel)/4095*100;
}



