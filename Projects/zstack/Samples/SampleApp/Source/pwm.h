#ifndef __PWM_H__
#define __PWM_H__

#include <ioCC2530.h>
#include "OnBoard.h"
#define GPIOPWM() do{P1SEL |= 0x80;}while(0);
#define GPIOCLOSEPWM() do{P1SEL  &= ~0x80; P1_7 = 0;}while(0);

void Gpio_Init();
void Timer_Init();
void Pwm_Init();
void GpioCarrConfig( unsigned char carr1);

#endif