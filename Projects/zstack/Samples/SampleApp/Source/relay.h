#ifndef __DELAY_H__
#define __DELAY_H__

#include <ioCC2530.h>
#include "OnBoard.h"

#define IN_1 P1_2    
#define IN_2 P1_3

void MotorInit();
void TurnMotorLeft();
void TurnMotorRight();
void StopMotor();

#endif


