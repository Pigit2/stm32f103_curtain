#include "adc.h"



u16 air_Light[NUM][CHANNEL]={0};
/*
	����һ����������ʼ��һ��ADC ���ڲɼ���ת�����������ģ����
	������������ԣ�
					���Ź���ǿ�ȵ����ӣ���ѹ�����С��
												����						��
	��ʼ��һ��оƬ�ڲ�������
			1���ж����������û��ʹ�õ�GPIO�����õ����ţ�
					�����ʼ�����ţ�GPIO��
			2����ʼ���������
			3��ʹ�ܣ������������Ĺ���
			
	�鿴�ֲᣬ�鿴ԭ��ͼ----�����������ӵ�gpio----PC0
															ADCͨ��-----0��ͨ��
	stm32f103ϵ��---3��12λ���ȵ�ADC
										ADC1 ADC2 ADC3
										
	ADC��ʱ��Ƶ���ǲ��ܳ���14MHz
	
	stm32f103ϵ��-��Ƶ-����-72Mhz
	����Ƶ��Ƶ--��Ƶ������14Mhz
	
*/

//PC0--->��������
void Light_AIR_ADC1_INIT(void)//��ʼ��PC0�Ĺ�������ADC�ɼ�
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	//�����ʼ�����ţ�GPIO��
	//��ʱ����---GPIOC---��APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AIN;//ģʽ---��������������ѹֵ ADC����һ��--��ģ������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;//��0��
	GPIO_Init(GPIOC,&GPIO_InitStruct);//�ɼ����ų�ʼ��--PC0

	
	//��ʼ���������--->ADC��������·����---��ʱ����--ADC----XXX_Init();
	ADC_InitStruct.ADC_ContinuousConvMode=ENABLE;//����ת��ģʽ--����/�ر�--DMA
	ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;//���뷽ʽ--����� �Ҷ���
	ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//�ⲿ����
	ADC_InitStruct.ADC_Mode=ADC_Mode_Independent;//ģʽ��ѡ�����ģʽ
	ADC_InitStruct.ADC_NbrOfChannel=2;//��������ͨ��-->1-->2
	ADC_InitStruct.ADC_ScanConvMode=ENABLE;//����ɨ��---ɨ���ڶ�ͨ����ʱ����
	
	ADC_Init(ADC1,&ADC_InitStruct);
	//3��ʹ�ܣ������������Ĺ���  xxx_Cmd();
	ADC_Cmd(ADC1,ENABLE);
	
	
	//֮ǰת�����ݵ�ʱ��---ת���ļĴ�����Ҫ����λ
	//��λУ׼��
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	//����У׼��
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	/*�ɼ�PA0-----��0��ͨ��*/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
	
	/*�ɼ�PC0-----��10��ͨ��*/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_10,2,ADC_SampleTime_239Cycles5);//����ʱ��Խ�������ݾ�Խ׼ȷ
	
	/*��DMA��ʱ��ʹ��++++++++++++++++++++++++++++++*/
	ADC_DMACmd(ADC1,ENABLE);
	DMA_INIT();
	/*��DMA��ʱ��ʹ��===============================*/
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//����������ǵ�ADC�ɼ�
}


/*
	��ʼ��DMA����----DMA1
	STM32F103RCT6
*/
void DMA_INIT(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	//��ʱ��---DMA1
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitStruct.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//Դ��ַ---��Ƭ�������ַ��Ƭ���������λ��оƬ�ڲ���������
	DMA_InitStruct.DMA_MemoryBaseAddr=(u32)air_Light;//�ڴ��ַ
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC;//���˷���
	
	DMA_InitStruct.DMA_BufferSize=NUM*CHANNEL;//�ڴ��С
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//���� ����
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;//��Ҫ����
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//���ݴ�С
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;//ѭ������---30
	DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;//���
	DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;//memory to memory
	
	
	//DMA1��ʼ��----XXX_Init();
	DMA_Init(DMA1_Channel1,&DMA_InitStruct);
	
	//��DMA---XXX_Cmd();
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
}
/*�ɼ����ݻ�ȡ����*/
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

3.3x/4095=ʵ�ʵĵ�ѹֵ

*/
/*�ɼ����㺯��*/
float ADC_AIR_LIGHT_VAL(u8 channel)
{
	return 3.3*ADC_GET_VAL(channel)/4095*100;
}



