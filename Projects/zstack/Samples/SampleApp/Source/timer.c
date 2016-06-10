#include "timer.h"

static int count = 0;
static int couter = 0;
static int v = 0;
int StepMotorTurnMode = 1;

void InitT3()
{     
  T3CTL |= 0x08 ;            
  T3IE = 1;                 
  T3CTL|=0XE0;               
  T3CTL &= ~0X03;            
  T3CTL |=0X10;              
  EA = 1; 
}

void TimerInit()
{   
  InitT3();        
}

#pragma vector = T3_VECTOR 
 __interrupt void T3_ISR(void) 
 { 
    IRCON = 0x00;                  
    if(++count>254)               
    {          
      count = 0;                   
    } 

    v++;
    if(v > 1)//5
        v = 0;

    if(v == 3)
    {
        couter ++;
        if(couter > 3)
            couter = 0;
        StepMotorTurn(StepMotorTurnMode,couter);
    }
 }

