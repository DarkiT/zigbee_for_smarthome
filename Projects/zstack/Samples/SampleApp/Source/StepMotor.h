#ifndef __STEPMOTOR_H_
#define __STEPMOTOR_H_

#include <ioCC2530.h>
#include "OnBoard.h"

#define IN1 P1_2    
#define IN2 P1_3
#define IN3 P1_4
#define IN4 P1_1

void StepMotorInit(void);
void StepMotorTurn(int mode,int couter);

#endif