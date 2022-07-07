#include "iic.h"

#define DELAY_TIME 5

//I2C�����ڲ���ʱ����
void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}

//I2C���������ź�
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//I2C����ֹͣ�ź�
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//����Ӧ����Ӧ���ź�
//void IIC_SendAck(bit ackbit)
//{
//    SCL = 0;
//    SDA = ackbit;  					
//    IIC_Delay(DELAY_TIME);
//    SCL = 1;
//    IIC_Delay(DELAY_TIME);
//    SCL = 0; 
//    SDA = 1;
//    IIC_Delay(DELAY_TIME);
//}

//�ȴ�Ӧ��
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

//I2C���߷���һ���ֽ�����
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

//I2C���߽���һ���ֽ�����
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

void write_24c02(u8 adrr,u8 dat)
{
	IIC_Start();
	IIC_SendByte(0xa0);	   //a0д�룬a1��ȡ
	IIC_WaitAck();

	IIC_SendByte(adrr);
	IIC_WaitAck();

	IIC_SendByte(dat);
	IIC_WaitAck();

	IIC_Stop();
}

u8 read_24c02(u8 adrr)
{
	u8 temp;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();

	IIC_SendByte(adrr);
	IIC_WaitAck();

	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();

	temp = IIC_RecByte();
	IIC_Stop();
	return temp;
}
