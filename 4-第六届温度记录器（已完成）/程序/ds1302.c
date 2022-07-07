/*
  程序说明: DS1302驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台 8051，12MHz
  日    期: 2011-8-9
*/

#include <reg52.h>
#include <intrins.h>
#define u8 unsigned char
extern u8 sec,min,hour;
sbit SCK=P1^7;		
sbit SDA=P2^3;		
sbit RST = P1^3;   // DS1302复位												

void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK=0;
		SDA=temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

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

u8 BDC_DEC(u8 bcd)
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
	sec = BDC_DEC(d);
	d = Read_Ds1302_Byte(0x83);
	min = BDC_DEC(d);
	d = Read_Ds1302_Byte(0x85);
	hour = BDC_DEC(d);
}




