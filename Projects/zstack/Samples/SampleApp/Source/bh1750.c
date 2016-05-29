#include <ioCC2530.h>
#include "stdio.h"
#include "OnBoard.h"
#include "bh1750.h"

char Light[4];
char  BUF[1];

char  ge,shi,bai,qian,wan;            //显示变量

unsigned char buffer[];

void Bh1750_Init()     //cc2530的GPIO设置
{ 
  P1SEL &= 0XF3;     //设置P1.4，P1.5为普通IO口  
  P1INP &= 0xF3;     //打开P1.4，P1.5上拉电阻,不影响     1111 0011
}

   
//*********************************************************
void conversion(uint temp_data)  //  数据转换出 个，十，百，千，万
{  
  wan=temp_data/10000+0x30 ;
  temp_data=temp_data%10000;   //取余运算
  qian=temp_data/1000+0x30 ;
  temp_data=temp_data%1000;    //取余运算
  bai=temp_data/100+0x30   ;
  temp_data=temp_data%100;     //取余运算
  shi=temp_data/10+0x30    ;
  temp_data=temp_data%10;      //取余运算
  ge=temp_data+0x30;         
  Light[0]=(unsigned char)wan;
  Light[1]=(unsigned char)qian;
  Light[2]=(unsigned char)bai;
  Light[3]=(unsigned char)shi;
  Light[4]=(unsigned char)ge;
}

/***********************************************************************************
* Function: Delay_us;
*
* Description: 延时程序, 延时时间范围: 0~65535us;
*
* Input:  times, 延时时间变量;
*
* Output: none;
*
* Return: none;
*
* Note:   延时时间最大是65535us;
************************************************************************************/
void Delay_1u(unsigned int times) //1 us延时
{
    while(times--)
    {
        MicroWait(1);     
    }
}


void Delay_1ms(unsigned int times)
{
  unsigned int i;

  for (i=0; i<times; i++)        
    Delay_1u(1000);         // 调用延时函数,延时1ms                
}

void Delay_1s(unsigned int times)
{
  unsigned int i;

  for (i=0; i<times; i++)
  Delay_1ms(1000);           // 调用延时函数,延时1s
}


void Delay5us()
{
  Delay_1u(5);
}

/**************************************
起始信号
**************************************/
void WriteSDA1(void)//SDA 输出1,相当于51里面的SDA=1   
{
  //P1DIR |= 0x20;
  P1DIR |= 0x08;
  SDA = 1;
}
     
void WriteSDA0(void)//SDA 输出0   
{
  //P1DIR |= 0x20;
  P1DIR |= 0x08;
  SDA = 0;
}
     
void WriteSCL1(void)//SCL 输出1   
{
  //P1DIR |= 0x10;
  P1DIR |= 0x04;
  SCL = 1;
}
     
void WriteSCL0(void)//SCL 输出0   
{
  //P1DIR |= 0x10;
  P1DIR |= 0x04;
  SCL = 0;
}
     
void ReadSDA(void)//这里设置SDA对应IO口DIR可以接收数据   
{
  //P1DIR &= 0xDF;
  P1DIR &= 0xF7;
}

void BH1750_Start()
{
  WriteSDA1();
  WriteSCL1();
  Delay_1u(5);
  WriteSDA0();
  Delay_1u(5);
  WriteSCL0();
  Delay_1u(5);
}

/**************************************
停止信号
**************************************/
void BH1750_Stop()
{
  WriteSDA0();
  WriteSCL1();
  Delay_1u(5);
  WriteSDA1();
  Delay_1u(5);
  WriteSCL0();
  Delay_1u(5);
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void BH1750_SendACK(uchar ack)
{
  if(ack)
  {
    WriteSDA1();
  }
  else
  {
    WriteSDA0();
  }  

  Delay_1u(5);
  WriteSCL1();   
  Delay_1u(5);
  WriteSCL0();  
     //延时
}

/**************************************
接收应答信号
**************************************/
uchar BH1750_RecvACK()
{        
  unsigned short i=10;
  ReadSDA();F0=SDA;
  WriteSCL1();
  Delay_1u(5);
  WriteSCL0();
  Delay_1u(5);
  P1DIR |= 0x08;
  return F0;
}

/*发送0，在SCL为高电平时使SDA信号为低*/
void SEND_0_1(void)   /* SEND ACK */
{
  WriteSDA0();
  Delay_1u(5);
  WriteSCL1();
  Delay_1u(5);
  WriteSCL0();
  Delay_1u(5);
}

/*发送1，在SCL为高电平时使SDA信号为高*/
void SEND_1_1(void)
{
  WriteSDA1();
  Delay_1u(5);
  WriteSCL1();
  Delay_1u(5);
  WriteSCL0();
  Delay_1u(5);
}
/**************************************
向IIC总线发送一个字节数据
**************************************/
void BH1750_SendByte(uchar dat)
{
  char i;
  WriteSCL0();
  for(i=0;i<8;i++)
  {
    if((dat<<i)&0x80)
    {
      SEND_1_1();
    }
    else
    {
      SEND_0_1();
    }
  }
  BH1750_RecvACK();   
}

/**************************************
从IIC总线接收一个字节数据（一稿）
**************************************/
/*uchar BH1750_RecvByte()
{
         char b=0,i;
         WriteSDA1();

         for(i=0;i<8;i++)
         {   
             
             WriteSCL0(); 
             Delay_1u(5);
             WriteSCL1();
             Delay_1u(5);
             
             ReadSDA();
             F0=SDA;//寄存器中的一位,用于存储SDA中的一位数据

           if(F0)
             {
               b=b<<1;
               b=b|0x01;
             }
             else
               {b=b<<1;}
             
         }
         WriteSCL0();
         P1DIR |= 0x20;
         
         return b; 
         
}*/

/**************************************
从IIC总线接收一个字节数据（另一种）
**************************************/
uchar BH1750_RecvByte()
{
 char b=0,i;

 WriteSCL0(); 
 Delay_1u(5);

 for(i=0;i<8;i++)
 {   
  ReadSDA();
  WriteSCL1();
  Delay_1u(5);

  F0=SDA;//寄存器中的一位,用于存储SDA中的一位数据

  if(F0)
  {
    b=b<<1;    
    b=b|0x01;
  }
  else
  {
    b=b<<1;
  }
  WriteSCL0();
  Delay_1u(5);
}

  P1DIR |= 0x08;

  return b; 
}

void Single_Write_BH1750(uchar REG_Address)
{   
    uchar REG_data;
    BH1750_Start();                  //起始信号
    BH1750_SendByte(SlaveAddress);   //发送设备地址+写信号
    BH1750_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
    BH1750_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    BH1750_Stop();                   //发送停止信号
}

int Single_Read_BH1750(void)     //连续读出BH1750内部数据
{   
    uchar i;uchar t0;uchar t1;int t;

    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress);           //发送设备地址+写信号
    if(F0)
    { 
      BH1750_Start();                          //起始信号
      BH1750_SendByte(SlaveAddress);           //发送设备地址+写信号
    }
    BH1750_SendByte(0x00);                   // 断电
    if(F0)
    { 
      BH1750_SendByte(0x00);       
    }
    BH1750_Stop();
    Delay_1ms(30); 

    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress);           //发送设备地址+写信号
    if(F0)
    { 
      BH1750_Start();                          //起始信号
      BH1750_SendByte(SlaveAddress);           //发送设备地址+写信号
    }
    BH1750_SendByte(0x01);                   // 上电
    if(F0)
    { 
      BH1750_SendByte(0x01);       
    }
    BH1750_Stop();

    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress);           //发送设备地址+写信号
    if(F0)
    { 
      BH1750_Start();                          //起始信号
      BH1750_SendByte(SlaveAddress);           //发送设备地址+写信号
    }
    BH1750_SendByte(0x10);                  // H- resolution mode
    if(F0)
    { 
      BH1750_SendByte(0x10);       
    }
    BH1750_Stop();
    Delay_1ms(180);               //延时180ms                //发送存储单元地址，从0开始        
    
    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    if(F0)
    { 
      BH1750_Start();                          //起始信号
      BH1750_SendByte(SlaveAddress+1);           //发送设备地址+写信号
    }

    t0= BH1750_RecvByte();          //BUF[0]存储0x32地址中的数据
    BH1750_SendACK(0);                //最后一个数据需要回NOACK

    t1= BH1750_RecvByte();
    BH1750_SendACK(1);                //回应ACK
    BH1750_Stop();                           //停止信号

    t=((t0<<8)+t1)/1.2;
    Delay_1ms(5);

    return t;
}


// float lx;

// void main(void) 
// {   
//   char i; 
//   char TempValue[6];  
//   float AvgTemp;         
//   uint  dis_data;

//   lightinit();
//   while(1) 
//   { 
//     lx=Single_Read_BH1750();      //连续读出数据，存储在lx中  
//     conversion(lx);           //计算数据和显示 
//     Delay_1ms(1000);
//   } 
// }
