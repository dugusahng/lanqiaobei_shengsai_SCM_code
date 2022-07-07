/*
  程序说明: IIC总线驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台 8051，12MHz
  日    期: 2011-8-9
*/

#include "reg52.h"
#include "intrins.h"
#include"iic.h"

#define DELAY_TIME 5

#define SlaveAddrW 0xA0
#define SlaveAddrR 0xA1

//总线引脚定义
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */

void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}
//总线启动条件
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//总线停止条件
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//发送应答
void IIC_SendAck(bit ackbit)
{
    SCL = 0;
    SDA = ackbit;  					// 0：应答，1：非应答
    IIC_Delay(DELAY_TIME);
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SCL = 0; 
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//等待应答
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;
    SCL = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}

//通过I2C总线发送数据
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL  = 0;  
}

//从I2C总线上接收数据
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL = 1;
	IIC_Delay(DELAY_TIME);
	da <<= 1;
	if(SDA) da |= 1;
	SCL = 0;
	IIC_Delay(DELAY_TIME);
    }
    return da;    
}
//==========================//
//void write_8591(unsigned char dac)   //写入电压值
//{
//	IIC_Start();
//	IIC_SendByte(0x90);		//写入90，读取a1
//	IIC_WaitAck();
//	
//	IIC_SendByte(0x40);
//	IIC_WaitAck();
//	
//	IIC_SendByte(dac);
//	IIC_WaitAck();
//	
//	IIC_Stop(); 
//}

unsigned char read_8591_channel(unsigned char channel_n)	   //读电压
{
	IIC_Start();
	IIC_SendByte(0x90);		
	IIC_WaitAck();

	IIC_SendByte(channel_n);		//channel_n只能填0、1、2、3
	IIC_WaitAck();

	IIC_Start();
	IIC_SendByte(0x91);		
	IIC_WaitAck();

	channel_n = IIC_RecByte();		   //每次读到的都是上次转化的结果，读两次，
	IIC_SendAck(0);
	channel_n = IIC_RecByte();

	IIC_Stop();

	return channel_n;
}

//=============单字节写入=================//
void write_byte_at24c02(unsigned char adrr, unsigned char dat)
{
	IIC_Start();
	IIC_SendByte(0xA0);		//写入a0，读取a1
	IIC_WaitAck();
	
	IIC_SendByte(adrr);
	IIC_WaitAck();
	
	IIC_SendByte(dat);
	IIC_WaitAck();
	
	IIC_Stop(); 
}

//=============单字节读取=================//
unsigned char read_byte_at24c02(unsigned char adrr)
{
	unsigned char temp;

	IIC_Start();
	IIC_SendByte(0xA0);		//写入a0，读取a1
	IIC_WaitAck();

	IIC_SendByte(adrr);
	IIC_WaitAck();

	IIC_Start();
	IIC_SendByte(0xA1);		//写入a0，读取a1
	IIC_WaitAck();

	temp = IIC_RecByte();

	IIC_Stop();
	
	return temp;
}





