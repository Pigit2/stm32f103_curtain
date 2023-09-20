#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"

extern char mainbuf[20];
extern u8 mainbuf_flag;
void USART1_GPIO_INIT(void);
void USART1_INIT(void);
void USART1_IT_INIT(void);

void USART2_GPIO_INIT(void);
void USART2_INIT(void);
void SEND_BITS_TO_USART(char *buf, USART_TypeDef* USARTx);
void WIFI_LOGIN(void);

void SYSTICK_US(u32 how_many_us);
void SysTick_TIMES(unsigned int times);
void SysTick_MS(unsigned int times);
#endif
