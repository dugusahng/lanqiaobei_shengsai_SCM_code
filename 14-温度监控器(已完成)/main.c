#include"reg52.h"
#include"key.h"
#include"iic.h"
#include"onewire.h"
#include "intrins.h"
sfr AUXR = 0x8e;
#define u8 unsigned char
#define u16 unsigned int

u8 code smg_du[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
u8 code smg_we[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8 temp_table[8] = {0xC0,0xC0,0xFF,0xFF,0xC0,0xC0,0xC0,0xC0};

u8 temperature;
u8 count_key,count_temp;
bit key_flag,temp_flag;
extern u8 trg = 0,cont = 0;
extern char temp_up = 30,temp_down = 15;

void HC573(u8 n,u8 value);
void Timer0Init(void);
void Delay50ms();
void table_convert();
void Relayproc();

void main()
{
	u8 x;
	HC573(5,0x00);
	HC573(4,0xff);
	temp_down = read_24c02(0x01);
	temp_up = read_24c02(0x00);
	for(x = 0;x <= 10;x++){temperature = rd_temperature();Delay50ms();}
	Timer0Init();
	while(1)
	{
		table_convert();
		Relayproc();	
		if(key_flag){key_flag = 0;Keyproc();}

		if(temp_flag){temp_flag = 0;temperature = rd_temperature();}
	}
}

void display();
void Timer0isr() interrupt 1
{	
	count_key++;count_temp++;
	if(count_key >= 6){count_key = 0;key_flag = 1;}
	if(count_temp >= 250){count_temp = 0;temp_flag = 1;}
	display();
}

void Relayproc()
{
	if(temperature < temp_down)
	{
	 	HC573(5,0x10);
	}
	else
	{
		HC573(5,0x00);
	}
}

void display()
{
	static u8 i;
	HC573(7,0xff);
	HC573(6,smg_we[i]);
	HC573(7,temp_table[i]);
	i++; if(i >= 8)i = 0;	
}

void table_convert()
{
	temp_table[0] = smg_du[temperature % 10];
	temp_table[1] = smg_du[temperature / 10];
	temp_table[4] = smg_du[temp_down % 10];
	temp_table[5] = smg_du[temp_down / 10];
	temp_table[6] = smg_du[temp_up % 10];
	temp_table[7] = smg_du[temp_up / 10];		
}

void HC573(u8 n,u8 value)
{
	switch(n)
	{
		case 4: P2 = (P2 & 0x1f) | 0x80; P0 = value; P2 &= 0x1f; break;
		case 5: P2 = (P2 & 0x1f) | 0xa0; P0 = value; P2 &= 0x1f; break;
		case 6: P2 = (P2 & 0x1f) | 0xc0; P0 = value; P2 &= 0x1f; break;
		case 7: P2 = (P2 & 0x1f) | 0xe0; P0 = value; P2 &= 0x1f; break;
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

void Delay50ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 3;
	j = 72;
	k = 161;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
