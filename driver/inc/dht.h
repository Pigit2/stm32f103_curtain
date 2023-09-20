#ifndef __DHT_H__
#define __DHT_H__

#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"


#define PA8_UP() 			GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define PA8_DOWN() 		GPIO_ResetBits(GPIOA,GPIO_Pin_8)
#define PA8_READ() 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)


void PA8_INIT(void);//使用
void PA8_OUT(void);
void PA8_IN(void);

void PA8_Rest(void);
u8 DHT_read_signal(void);
u8 Read_DHT_BIT(void);
u8 READ_DHT_Byte(void);
u8 DHT_READ_DATA(u8 *t,u8 *h);//使用
void PC10_INIT(void);
	
#endif

