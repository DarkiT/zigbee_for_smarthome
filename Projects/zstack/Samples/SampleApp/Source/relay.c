#include "relay.h"

void MotorInit(void)
{
  P1DIR |= 0x04;     //P1_2定义为输出
  P1INP |= 0X04;     //打开下拉
  
  P1DIR |= 0x08;     //P1_3定义为输出
  P1INP |= 0X08;     //打开下拉        
}

void TurnMotorLeft()
{
  IN_1 = 1;
  IN_2 = 0;
}

void TurnMotorRight()
{
  IN_1 = 0;
  IN_2 = 1;
}

void StopMotor()
{
  IN_1 = 1;
  IN_2 = 1;
}