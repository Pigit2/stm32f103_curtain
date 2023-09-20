#include "dht.h"


/*
 ʹ�õ�PA8
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

void PA8_OUT(void)//���ģʽ
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


void PA8_Rest(void)//��ʼ�ź�
{
	PA8_OUT();//�����ƽ
	
	PA8_DOWN();//����18ms����
	SysTick_MS(20);//��ʱ
	
	PA8_UP();//����20us��40us
	SYSTICK_US(30);
}

/*
GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)
�����ŵ�ֵ
	�����1���Է���ֵ����ʽ����1
	�����0���ͷ���0
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
	if(retry>100)//�Ѿ�����80
		return -1;
	else
		retry=0;
	//��Ϊ�͵�ƽ���ж�
	
	while(0==GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)&&retry<101)
	{
		retry++;
		SYSTICK_US(1);
	}
	if(retry>100)//�Ѿ�����80
		return -1;
	
	return 0;
	//����-1���� ����0��ȷ
}

u8 Read_DHT_BIT(void)//��һ��λ
{
	u8 retry=0;
	while(PA8_READ()==1 && retry<101)//�ȴ���ʪ��ģ��Ӧ���źŽ���
	{
		retry++;
		SYSTICK_US(1);
	}
	retry=0;
	
	while(PA8_READ()==0 && retry<101)//�ȴ���ʪ��ģ�鷢��������ʼ�źŽ���
	{
		retry++;
		SYSTICK_US(1);
	}
	
	//��ʽ��ʼ��������
	SYSTICK_US(40);//����ʱ40us����ȥ�鿴��������Ƿ��Ǹߵ�ƽ
	
	if(PA8_READ()==1)
		return 1;//��
	else 
		return 0;//����
}

//һ����Ҫ��40��bit
u8 READ_DHT_Byte(void)//��һ���ֽں���
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
	//һ����40bit---��buf[5];
	u8 buf[5]={0};
	u8 i;
	PA8_Rest();//��ʼ�ź�
	if(0==DHT_read_signal())//�ж���ʪ���Ƿ��л�Ӧ
	{
		//������5���ֽ�
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


