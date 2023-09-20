#ifndef __L298N_H__
#define __L298N_H__

#include "stm32f10x.h"

#define MOTOR_1_GPIO_PIN              GPIO_Pin_5
#define MOTOR_2_GPIO_PIN              GPIO_Pin_1
#define MOTOR_GPIO_PORT             	GPIOB
#define MOTOR_GPIO_CLK              RCC_APB2Periph_GPIOB

#define    ON        1
#define    OFF       0

#define   Motor(a)   if(a) \
	                       GPIO_SetBits(MOTOR_GPIO_PORT, MOTOR_1_GPIO_PIN); \
                     else  GPIO_ResetBits(MOTOR_GPIO_PORT, MOTOR_1_GPIO_PIN);
void PD2(void);
void Motor_Init(void);

#endif

