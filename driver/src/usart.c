#include "usart.h"
#include "oled.h"

/*
	USART1--����---- TX    RX
									PA9    PA10
									GPIOA
	1����ʼ������---�ҵ���Ӧ������---�鿴�ֲ�
	2����ʼ��USART1�Ĺ���---����1
	3��ʹ�ܴ���1
*/

/*
		
		���ͺ�����void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
		���պ�����uint16_t USART_ReceiveData(USART_TypeDef* USARTx);
								
*/

u8 Head_Flag=0;//ͷ��
u8 Tail_Flag=0;//β��
u8 mainbuf_flag=0;//���Ʊ�־λ

char buf[20] = "";//���������
char mainbuf[20] = "";//�����������

void SYSTICK_US(u32 how_many_us)//�δ���ʱ���� us ��1����1us 
{
	SysTick->CTRL &= ~(0x05);//
	SysTick->CTRL &= ~(1<<16);//��0
	SysTick->LOAD =how_many_us*9;//�δ�ʱ����ϵͳʱ���߷ֹ���---8��Ƶ--72/8=9
	SysTick->VAL  =0;

	SysTick->CTRL |= (0x01);
	while(0==(SysTick->CTRL & (1<<16)));
	SysTick->CTRL &= ~(0x05);//��0
	SysTick->CTRL &= ~(1<<16);//��0
}

void SysTick_TIMES(unsigned int times)//�δ���ʱ���� 0.1s
{
	for(;times>0;times--)
	{
		SYSTICK_US(100000);
	}
}

void SysTick_MS(unsigned int times)//�δ���ʱ���� 0.1s
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
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//ģʽ---�ⷢ��--���ģʽ--����--�������
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9;//����
	GPIO_InitStruct.GPIO_Speed=	GPIO_Speed_50MHz;//��ת����---����������ʹ�ô�������Ϊ������Ϣ
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);//TX---PA9
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN_FLOATING;//ģʽ---����ģʽ---��������
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_10;//����
	//GPIO_InitStruct.GPIO_Speed=;������---������ģʽ�������Աû������
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);//RX---PA10--����--����
}

void USART1_INIT(void)
{
	USART_InitTypeDef USART_InitStruct;
	//�򿪴���1��ʱ����---��APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	USART_InitStruct.USART_BaudRate=115200;//������---��115200 9600 
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//Ӳ��������---��ѡ����
	USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//ģʽ---��TXģʽ RXģʽ 
	USART_InitStruct.USART_StopBits=USART_StopBits_1;//ֹͣλ 1bit
	USART_InitStruct.USART_Parity=USART_Parity_No;//����У��
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//�ִ�С--��8bit
	
	//��ʼ������1---��USART_Init();
	//����1����ʼ������who������2���������ýṹ��ָ��
	USART_Init(USART1,&USART_InitStruct);
	
}

void USART1_IT_INIT(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	//NVIC--->�����ж����ȼ� misc.c
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;//ͨ�������������������ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//ʹ��-ENABLE
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);
	
	//����1:�����Ǹ����ڵ��ж� ����2��ʲôʱ������ж� ����3��ʹ��
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//ʹ�ܴ���1---USART_Cmd();�����жϺ� �Ŵ򿪴��ڵĹ���
	USART_Cmd(USART1,ENABLE);
}

/*
 ��STM32F103�� �жϺ�����ϵͳд��
		��Ҫ�������ļ��е� �ж�������
											�ҵ��жϺ���������

			�жϺ�����ϵͳ�����ж�ʱ�Զ�ִ��

�жϷ������У����ܴ���̫�ණ��---���жϺ����е�ʱ�䲻�ܹ���

					
*/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE))//�õ�һ�����ձ�־
	{
		
		
		USART_SendData(USART1,USART_ReceiveData(USART1));
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//��������־
	}
}


//Usart2�ĳ�ʼ����PA2 PA3

void USART2_GPIO_INIT(void)
{
		GPIO_InitTypeDef  GPIO_InitStruct;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//�������--�ߵ͵�ƽ ���� 
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

	USART_InitStruct.USART_BaudRate=115200;//wifiĬ����115200
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

void USART2_IRQHandler(void)//�жϺ�������Ҫ������Ҳ����Ҫ����
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE))
	{
		//�Ѵ���2�����ݷ���������1��
		u8 temp;
		static u8 point =0;
		temp = USART_ReceiveData(USART2);
		
		if(3==Head_Flag)
		{
			buf[point++]=temp;
			if(1==Tail_Flag)//β���յ���һ֡�Ժ�
			{
				if('\n'==temp)//β���յ��ڶ��룬�������
				{
					Tail_Flag=2;
					//���п���
					strcpy(mainbuf,buf);//��buf���Ƹ���mainbuf
					mainbuf_flag=1;
				}
				else
				{
					Tail_Flag=0;//�������β�븴λ
				}
			}
		}
		//���ʧ��--��λ����
		//�������ȡ�ɹ���Ҫ���ղŲ��������б�־λ��ȫ����λ
		if(2==Tail_Flag||point>19)
		{
			Head_Flag=0;
			Tail_Flag=0;
			point=0;
			memset(buf,0,sizeof(buf));
			//strcpy(buf,"");
		}
		
		if('\r'==temp)//β��
		{                
			Tail_Flag=1;
		}
		
		/*���´��붼��ͷ����*/
		if(2==Head_Flag)
		{
			if('D'==temp)//ȷ����D����ζ�����������յ���ORD
			{
				Head_Flag=3;//ȷ������һ������������������Ϣ�յ�buf[]
			}
			else 
			{
				Head_Flag=0;//����������ǰ�ͷ�븴λ--Ϊ��ʼ״̬
			}
		}
		if(1==Head_Flag)
		{
			if('R'==temp)//ȷ����R����ζ�����������յ���OR
			{
				Head_Flag=2;//���������һ����ĸ�ǲ���D
			}
			else 
			{
				Head_Flag=0;//����������ǰ�ͷ�븴λ--Ϊ��ʼ״̬
			}
		}
		if(('O'==temp)&&(0==Head_Flag))//ͷ���ڳ�ʼ״̬�£��յ���һ��O
		{
			Head_Flag=1;//��Ϊ��������Ŀ�ͷ----�������������������ĸ�ǲ���R D
		}
		USART_SendData(USART1,USART_ReceiveData(USART2));//�����
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}

/*����1������Ҫ���͵�ָ���Ϣ������2:���͵��Ǹ�����*/
/*�ַ�����ָ�룬�ַ���������----��ָ��  ������   ��β������Զ�����\0*/
void SEND_BITS_TO_USART(char *buf, USART_TypeDef* USARTx)
{
	USART_ClearFlag(USARTx,USART_FLAG_TXE);//���־
	while('\0'!=*buf)
	{
		USART_SendData(USARTx,*buf++);
		while(RESET==USART_GetFlagStatus(USARTx,USART_FLAG_TXE));//�ȴ��������
		USART_ClearFlag(USARTx,USART_FLAG_TXE);//���־
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

///*wifi���Ӻ���*/
//void WIFI_LOGIN(void)
//{
//	OLED_Clear();
//	OLED_ShowCHinese2(22,1,24,5);//���ڳ�ʼ��
//	/*������ڷ���һ��ָ��*/
//	SEND_BITS_TO_USART("DEL\r\n",USART2);//ɾ��?
//	SysTick_TIMES(10);
//	SEND_BITS_TO_USART("AT+RST\r\n",USART2);//����
//	SysTick_TIMES(10);
//	SEND_BITS_TO_USART("AT+CWMODE=1\r\n",USART2);//ģʽ--ѡ������·������ģʽ
//	SysTick_TIMES(5);
//	SEND_BITS_TO_USART("AT+CWJAP=\"PDCN\",\"13539928668\"\r\n",USART2);//����·��
//	SysTick_TIMES(60);
//	SEND_BITS_TO_USART("AT+CIPSTART=\"TCP\",\"114.215.65.250\",4508\r\n",USART2);
//	SysTick_TIMES(6);
//	SEND_BITS_TO_USART("AT+CIPSEND=18\r\n",USART2);//����ָ�����wifiģ�飬��ʵָ��ĳ���
//	SysTick_TIMES(6);
//	SEND_BITS_TO_USART("KEY:peiqi2+1122334\r\n",USART2);//��ʵָ��
//	SysTick_TIMES(5);
//	SEND_BITS_TO_USART("FINISH\r\n",USART1);//test
//	SysTick_TIMES(10);
//	OLED_Clear();
//}


/*wifi�ȵ��ʼ��*/
void WIFI_LOGIN(void)
{
	OLED_Clear();
	OLED_ShowCHinese2(22,1,24,5);//���ڳ�ʼ��
	/*������ڷ���һ��ָ��*/
	SEND_BITS_TO_USART("DEL\r\n",USART2);//ɾ��?
	SysTick_TIMES(10);
	SEND_BITS_TO_USART("AT+RST\r\n",USART2);//����
	SysTick_TIMES(10);
	SEND_BITS_TO_USART("AT+CWMODE=3\r\n",USART2);//ģʽ
	SysTick_TIMES(5);
	SEND_BITS_TO_USART("AT+CWSAP=\"ESP8266\",\"12345678\",1,3\r\n",USART2);//����WiFi
	SysTick_TIMES(60);
	SEND_BITS_TO_USART("AT+CWLIF\r\n",USART2);//��֤
	SysTick_TIMES(10);
	SEND_BITS_TO_USART("AT+CIPMUX=1\r\n",USART2);//��·����
	SysTick_TIMES(20);
	SEND_BITS_TO_USART("AT+CIPSERVER=1,8080\r\n",USART2);
	SysTick_TIMES(30);
	SEND_BITS_TO_USART("AT+CIFSR\r\n",USART2);//������Ϣ
	SysTick_TIMES(10);
	SEND_BITS_TO_USART("FINISH\r\n",USART1);//test
	SysTick_TIMES(10);
	OLED_Clear();
}


