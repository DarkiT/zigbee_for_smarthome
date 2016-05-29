#include "pwm.h"

void Gpio_Init()
{
  P1SEL &=~0x80;
  P1DIR |= 0x80;
  P1_7 = 0;
}

void Timer_Init()
{
  PERCFG |= 0x20;
  P2SEL |= 0x20;
  
  T3CCTL1 = 0;
  
  T3CCTL0 = 0x10 |0x04;
  T3CC0 = 254;
  T3CCTL1 = 0x18 | 0x04;
  T3CC1 = 1;
  T3CTL = 0xA0 | 0x10 | 0x02;
}

void Pwm_Init()
{
  Gpio_Init();
  Timer_Init();
}

void GpioCarrConfig( unsigned char carr1)
{
  unsigned char carr;
  
  carr = ~carr1;
  GPIOCLOSEPWM();
  if(carr>0)
  {
    T3CC1 = carr;
  }
  else
  {
    T3CC1 = 1;
  }
  GPIOPWM();
}


// void main()
// {
//   unsigned char duty;
//    Gpio_Init();
//    Timer_Init();
//    while(1)
//    {
//    GpioCarrConfig( duty);
//    delay(10);
//    duty++;
//    }
// }

