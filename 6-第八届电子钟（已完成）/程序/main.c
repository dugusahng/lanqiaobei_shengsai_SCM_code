#include"reg52.h"
#include"key.h"
#include"onewire.h"
#include"ds1302.h"
#include "intrins.h"
sfr AUXR = 0x8e;
#define u8 unsigned char
#define u16 unsigned int

u8 smg_duan[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
u8 smg_wei[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8 time[8],temp[8];
 
u8 temperature;
u8 count_temp,count_key,flash;
u16 count_flash;
bit temp_flag,key_flag,flash_L1; 
extern char sec = 50,min = 59,hour = 23,sec_n = 0,min_n = 0,hour_n = 0;
extern u8 trg = 0,cont = 0,set = 0,set_n = 0,count_L1 = 0,l1 = 0; 
extern bit mode = 0,L1_flag_1 = 0,temp_table = 0;

void Timer0Init(void);
void Delay100ms();
void table_convert();

void main()
{
	u8 x;

	P0 = 0x00; P2 = (P2 & 0x1f) | 0xa0; P2 &= 0x1f;
	P0 = 0xff; P2 = (P2 & 0x1f) | 0x80; P2 &= 0x1f;

	for(x = 0;x < 10;x++)
	{
		temperature = rd_temperature();
		Delay100ms();
	}

	init_time();
	Timer0Init();
	while(1)
	{

		if(temp_flag){temp_flag = 0;temperature = rd_temperature();}

		if(key_flag){key_flag = 0;Keyproc();}

		if(L1_flag_1)
		{
		
			if(flash_L1)
			{
				EA = 0;	
				P0 = ~0x01 ; P2 = (P2 & 0x1f) | 0x80; P2 &= 0x1f;
				EA = 1;	
			}	
			else 
			{	EA = 0;	
				P0 = 0xff; P2 = (P2 & 0x1f) | 0x80; P2 &= 0x1f;
				EA = 1;	
			}
		
		}
					
		table_convert();
		if((set == 0)&&(set_n == 0)){read_ds1302();}			 
	}
}

void display();
void Timer0isr() interrupt 1
{
	count_temp++;count_key++;count_flash++;count_L1++;	
	if(count_temp >= 250){count_temp = 0;temp_flag = 1;}

	if(count_key >= 6){count_key = 0;key_flag = 1;}

	if(count_flash >= 500){count_flash = 0;flash = ~flash;}
	if((mode == 0)&&(sec == sec_n)&&(min == min_n)&&(hour == hour_n))L1_flag_1 = 1;
	
	if(count_L1 >= 100)		   //0.2s
	{
		count_L1 = 0;
		flash_L1 = ~flash_L1;
		l1++;
			
		if(l1 >= 25)		//5s
		{		
				L1_flag_1 = 0;
				l1 = 0;
				P2 = (P2 & 0x1f) | 0x80;P0 = 0xff;P2 &= 0x1f;
		}
	}


	display();	
}

void display()
{
	static u8 i;
	P0 = 0xff; P2 = (P2 & 0x1f) | 0xe0; P2 &= 0x1f;
	P0 = smg_wei[i]; P2 = (P2 & 0x1f) | 0xc0; P2 &= 0x1f;
	if(temp_table)
	{
		P0 = temp[i]; P2 = (P2 & 0x1f) | 0xe0; P2 &= 0x1f;
	}
	else
	{
	P0 = time[i]; P2 = (P2 & 0x1f) | 0xe0; P2 &= 0x1f;
	}
	 
	i++;if(i >= 8)i = 0;
}

void table_convert()
{

	if((mode == 0)&&(temp_table == 0))
	{
		if(set == 0)
		{
			time[0] = smg_duan[sec % 10];
			time[1] = smg_duan[sec / 10];
			time[2] = 0xbf;
			time[3] = smg_duan[min % 10];
			time[4] = smg_duan[min / 10];
			time[5] = 0xbf;
			time[6] = smg_duan[hour % 10];
			time[7] = smg_duan[hour / 10];
		}

		if(set == 1)
		{
			time[0] = smg_duan[sec % 10];
			time[1] = smg_duan[sec / 10];
			time[2] = 0xbf;
			time[3] = smg_duan[min % 10];
			time[4] = smg_duan[min / 10];
			time[5] = 0xbf;
			time[6] = smg_duan[hour % 10] | flash;
			time[7] = smg_duan[hour / 10] | flash;
		}

		if(set == 2)
		{
			time[0] = smg_duan[sec % 10];
			time[1] = smg_duan[sec / 10];
			time[2] = 0xbf;
			time[3] = smg_duan[min % 10] | flash;
			time[4] = smg_duan[min / 10] | flash;
			time[5] = 0xbf;
			time[6] = smg_duan[hour % 10];
			time[7] = smg_duan[hour / 10];
		}

		if(set == 3)
		{
			time[0] = smg_duan[sec % 10] | flash;
			time[1] = smg_duan[sec / 10] | flash;
			time[2] = 0xbf;
			time[3] = smg_duan[min % 10];
			time[4] = smg_duan[min / 10];
			time[5] = 0xbf;
			time[6] = smg_duan[hour % 10];
			time[7] = smg_duan[hour / 10];
		}

	}

	if((mode == 1)&&(temp_table == 0))
	{
		time[0] = smg_duan[sec_n % 10];
		time[1] = smg_duan[sec_n / 10];
		time[2] = 0xbf;
		time[3] = smg_duan[min_n % 10];
		time[4] = smg_duan[min_n / 10];
		time[5] = 0xbf;
		time[6] = smg_duan[hour_n % 10];
		time[7] = smg_duan[hour_n / 10];	
	}							

	if(temp_table)
	{
		temp[0] = 0xc6;
		temp[1] = smg_duan[temperature % 10];
		temp[2] = smg_duan[temperature / 10];
		temp[3] = 0xff;
		temp[4] = 0xff;
		temp[5] = 0xff;
		temp[6] = 0xff;
		temp[7] = 0xff;
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
