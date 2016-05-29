#ifndef __BH1750_H__
#define __BH1750_H__

#define   uint unsigned int
#define   uchar unsigned char
typedef   unsigned char BYTE;
typedef   unsigned short WORD;
#define          SlaveAddress   0x46 

#define  IIC_WRITE      0                 // WRITE direction bit
#define  IIC_READ       1                 // READ direction bit
#define  TRUE  1
#define  FALSE 0

/*我的管脚定义是 SDA定义为P1.3 SCL定义为P1.3 */
#define SCL P1_2
#define SDA P1_3

void Bh1750_Init();
void conversion(uint temp_data);
void Delay_1u(unsigned int times);
void Delay_1ms(unsigned int times);
void Delay_1s(unsigned int times);
void Delay5us();
void WriteSDA1(void);
void WriteSDA0(void);    
void WriteSCL1(void);     
void WriteSCL0(void);      
void ReadSDA(void);
void BH1750_Start();
void BH1750_Stop();
void BH1750_SendACK(uchar ack);
uchar BH1750_RecvACK();
void SEND_0_1(void);
void SEND_1_1(void);
void BH1750_SendByte(uchar dat);
uchar BH1750_RecvByte();
void Single_Write_BH1750(uchar REG_Address);
int Single_Read_BH1750(void);

#endif