#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f10x.h"

#define NUM 30   //可以采集30次
#define CHANNEL 2//采集两个通道--》PC0 PA0


void Light_AIR_ADC1_INIT(void);
void DMA_INIT(void);
float  ADC_GET_VAL(u8 channel);
float ADC_AIR_LIGHT_VAL(u8 channel);
#endif

