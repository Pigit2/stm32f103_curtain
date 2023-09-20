#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f10x.h"

#define NUM 30   //���Բɼ�30��
#define CHANNEL 2//�ɼ�����ͨ��--��PC0 PA0


void Light_AIR_ADC1_INIT(void);
void DMA_INIT(void);
float  ADC_GET_VAL(u8 channel);
float ADC_AIR_LIGHT_VAL(u8 channel);
#endif

