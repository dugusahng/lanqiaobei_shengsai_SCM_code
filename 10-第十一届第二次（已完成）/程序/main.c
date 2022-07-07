#include"reg52.h"
#include"iic.h"
#include"key.h"
#include"onewire.h"
#include "intrins.h"
sfr AUXR = 0x8e;
#define u8 unsigned char
#define u16 unsigned int

u8 code smg_du[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
u8 code smg_we[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8 smg_temp[8];	   //C 0xc6
u8 smg_temp_val[8]; 	   //P 0x8c

u8 temperature,count_key,count_temp;
u16;
bit key_flag,temp_flag;
extern u8 trg = 0,cont = 0;
extern u8 temp_up_old = 30,temp_low_old = 20,temp_up_new = 30,temp_low_new = 20;
extern u16;
extern bit table_flag = 0,temp_val_mode = 1,L4_flag = 0;

void Timer0Init(void);
void Delay100ms();
void table_convert();
void LEDproc();
void DACproc();

void main()
{
	u8 x;
	P0 = 0x00;P2 = (P2 & 0x1f) | 0xa0;P2 &= 0x1f;
	P0 = 0xff;P2 = (P2 & 0x1f) | 0x80;P2 &= 0x1f;
//	read_8591(1);
	for(x = 0;x <= 10;x++){rd_temperature();Delay100ms();}
	Timer0Init();
	while(1)
	{	
		table_convert();
	   	if(temp_flag){temp_flag = 0;temperature = rd_temperature();}
		if(key_flag){key_flag = 0;Keyproc();}
		DACproc();
		LEDproc();
		
	}
}

void display();
void Timer0isr() interrupt 1
{
	count_key++;count_temp++;
	if(count_key >= 6){count_key = 0;key_flag = 1;}
	if(count_temp >= 250){count_temp = 0; temp_flag = 1;}
	display();		
}

void display()
{
	static u8 i;
	P0 = 0xff;P2 = (P2 & 0x1f) | 0xe0;P2 &= 0x1f;
	P0 = smg_we[i];P2 = (P2 & 0x1f) | 0xc0;P2 &= 0x1f;
	if(table_flag)
	{
		P0 = smg_temp_val[i];P2 = (P2 & 0x1f) | 0xe0;P2 &= 0x1f;
	}
	else
	{
		P0 = smg_temp[i];P2 = (P2 & 0x1f) | 0xe0;P2 &= 0x1f;
	}
	i++;if(i >= 8)i = 0;
}

void DACproc()
{
	
	if(temperature > temp_up_new)
	{
		write_8591(210);  //理论204			比实际值小6
	}
	else if((temperature >= temp_low_new)&&(temperature <= temp_up_new))
	{
		write_8591(158);   //理论153	  比实际值小5
	}
	else if(temperature < temp_low_new)
	{
		write_8591(107); //理论102		比实际值小5
	}
		
}

void LEDproc()
{
	P0 = 0xff;
	EA = 0;
	if(temperature > temp_up_new)
	{
		P0 &= ~0x01;P2 = (P2 & 0x1f) | 0x80;P2 &= 0x1f;	
	}
	else
	{
		P0 |= 0x01;P2 = (P2 & 0x1f) | 0x80;P2 &= 0x1f;
	}

	if((temperature >= temp_low_new)&&(temperature <= temp_up_new))
	{
		P0 &= ~0x02;P2 = (P2 & 0x1f) | 0x80;P2 &= 0x1f;
	}
	else
	{
		P0 |= 0x02;P2 = (P2 & 0x1f) | 0x80;P2 &= 0x1f;
	}

	if(temperature < temp_low_new)
	{
		P0 &= ~0x04;P2 = (P2 & 0x1f) | 0x80;P2 &= 0x1f;
	}
	else
	{
		P0 |= 0x04;P2 = (P2 & 0x1f) | 0x80;P2 &= 0x1f;
	}

	if(L4_flag)
	{
		P0 &= ~0x08;P2 = (P2 & 0x1f) | 0x80;P2 &= 0x1f;
	}

	else
	{
		P0 |= 0x08;P2 = (P2 & 0x1f) | 0x80;P2 &= 0x1f;
	}
	EA = 1;
		
}



void table_convert()
{
	if(table_flag)
	{
	 	smg_temp_val[0] = smg_du[temp_low_new % 10];
		smg_temp_val[1] = smg_du[temp_low_new / 10];
		smg_temp_val[2] = 0xff;
		smg_temp_val[3] = smg_du[temp_up_new % 10];
		smg_temp_val[4] = smg_du[temp_up_new / 10];
		smg_temp_val[5] = 0xff;
		smg_temp_val[6] = 0xff;
		smg_temp_val[7] = 0x8c;
	}
	else
	{
		smg_temp[0] = smg_du[temperature % 10];
		smg_temp[1] = smg_du[temperature / 10];
	 	smg_temp[2] = 0xff;
		smg_temp[3] = 0xff;
		smg_temp[4] = 0xff;
		smg_temp[5] = 0xff;
		smg_temp[6] = 0xff;
		smg_temp[7] = 0xc6;
	}
}

void Timer0Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x40;		//设置定时初始值
	TH0 = 0xA2;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void Delay100ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 5;
	j = 144;
	k = 71;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
