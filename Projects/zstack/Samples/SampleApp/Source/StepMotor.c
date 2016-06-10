#include "StepMotor.h"

void StepMotorInit(void)
{
  P1DIR |= 0x04;     //P1_2定义为输出  1.2          0000 0100
  P1INP |= 0X04;     //打开下拉        1.3          0000 1000
  P1DIR |= 0x08;     //P1_3定义为输出  1.4          0001 0000
  P1INP |= 0X08;     //打开下拉        1.5          0010 0000
  P1DIR |= 0x10;     //P1_4定义为输出
  P1INP |= 0X10;     //打开下拉  
  P1DIR |= 0x02;     //P1_5定义为输出
  P1INP |= 0X02;     //打开下拉  
}

void StepMotorTurn(int mode,int couter)
{
    switch(mode)
    {
        case 2:
        switch(couter)
        {
            case 0:
            IN1 = 1;
            IN2 = 1;
            IN3 = 0;
            IN4 = 0;
            break;

            case 1:
            IN1 = 0;
            IN2 = 1;
            IN3 = 1;
            IN4 = 0;
            break;

            case 2:
            IN1 = 0;
            IN2 = 0;
            IN3 = 1;
            IN4 = 1;
            break;

            case 3:
            IN1 = 1;
            IN2 = 0;
            IN3 = 0;
            IN4 = 1;
            break;
        }
        break;

        case 1:
        switch(couter)
        {
            case 0:
            IN1 = 1;
            IN2 = 0;
            IN3 = 0;
            IN4 = 1;
            break;

            case 1:
            IN1 = 0;
            IN2 = 0;
            IN3 = 1;
            IN4 = 1;
            break;

            case 2:
             IN1 = 0;
            IN2 = 1;
            IN3 = 1;
            IN4 = 0;
            break;

            case 3:
            IN1 = 1;
            IN2 = 1;
            IN3 = 0;
            IN4 = 0;
            break;
        }
        break;

        case 0:
        IN1 = 1;
        IN2 = 1;
        IN3 = 1;
        IN4 = 1;
        break;

    }
}
