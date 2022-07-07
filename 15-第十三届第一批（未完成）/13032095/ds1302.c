#include "ds1302.h"  									

extern u8 sec,min,hour;

//写字节
void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//向DS1302寄存器写入数据
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

//从DS1302寄存器读出数据
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

u8 BCD_DEC(u8 bcd)
{
	u8 x;
	x = bcd >> 4;
	bcd &= 0x0f;
	bcd = x * 10 + bcd;
	return bcd;
}

void init_time()
{
	Write_Ds1302_Byte(0x8e,0x00);	
	Write_Ds1302_Byte(0x80,sec);
	Write_Ds1302_Byte(0x82,min);
	Write_Ds1302_Byte(0x84,hour);
	Write_Ds1302_Byte(0x8e,0x80);
}

void read_ds1302()
{
	u8 d;
	d = Read_Ds1302_Byte(0x81);
	sec = BCD_DEC(d);
	d = Read_Ds1302_Byte(0x83);
	min = BCD_DEC(d);
	d = Read_Ds1302_Byte(0x85);
	hour = BCD_DEC(d);
}
