#include "input.h"

void InitGas()
{
  P1SEL &= ~0X08;     
  P1DIR &= ~0X08;    
  P1INP &= ~0x08;   
}

void InitRain()
{
    P0SEL &= ~0X40;  
    P0DIR &= ~0X40;   
    P0INP &= ~0x40;    
}

void InitFire()     
{
    P1SEL &= ~0X04;    
    P1DIR &= ~0X04;    
    P1INP &= ~0x04; 
}

void InitMan()
{
    P1SEL &= ~0X40;    
    P1DIR &= ~0X40;    
    P1INP &= ~0x40; 
}  