#include"reg52.h"
#include"key.h"
#include"iic.h"
sfr AUXR = 0x8e;
#define u8 unsigned char
#define u16 unsigned int

u8  smg_du[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0XF8,0x80,0x90};//数码管段码
u8  smg_we[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};//数码管位选
u8 table_VAIN[8];	   //电压
u8 table_VAIN_par[8];  //电压参数
  
u8 count_key,count_dac;
u16 VAIN1,VAIN3;
bit key_flag,dac_flag;
extern u8 trg = 0,cont = 0;
extern u16 VP1 = 250,VP3 = 300;
extern bit VIN_mode = 0,table_flag = 0;
float dac1,dac3;

void Timer0Init(void);
void Delay10ms();
void HC573(u8 n,u8 value);
void table_convert();
void LEDproc();

void main()
{
	HC573(5,0x00);
	HC573(4,0xff);	
	Wirte_8591(102);
	Timer0Init();
	while(1)
	{
		table_convert();
	
		if(dac_flag)
		{
//		 	EA = 0;
			dac1 = Read_8591(1);
			VAIN1 = (u16)dac1 * 100 / 51.0f;

			dac3 = Read_8591(3);
			VAIN3 = (u16)dac3 * 100 / 51.0f;
//			EA = 1;
		}
		LEDproc();
		if(key_flag){key_flag = 0;Keyproc();}		
	}
}

void display();
void Timer0isr() interrupt 1 
{
	count_key++;count_dac++;
	if(count_key >= 6){count_key = 0;key_flag = 1;}
	if(count_dac >= 45){count_dac = 0;dac_flag = 1;}	
	display();
}

void LEDproc()
{
	EA = 0;
	P0 = 0xff;
	if(VAIN1 > VP1)
	{
		P2 = (P2 & 0x1f) | 0x80; P0 &= ~0x01; P2 &= 0x1f; 
	}
	else
	{
		P2 = (P2 & 0x1f) | 0x80; P0 |= ~0x01; P2 &= 0x1f;
	}

	if(VAIN3 > VP3)
	{
		P2 = (P2 & 0x1f) | 0x80; P0 &= ~0x02; P2 &= 0x1f;
	}
	else
	{
		P2 = (P2 & 0x1f) | 0x80; P0 |= 0x02; P2 &= 0x1f;
	}

	if(VIN_mode)
	{
		P2 = (P2 & 0x1f) | 0x80; P0 |= 0x04; P2 &= 0x1f;
	}
	else
	{
	 	P2 = (P2 & 0x1f) | 0x80; P0 &= ~0x04; P2 &= 0x1f;
	}

	if(table_flag)
	{
		P2 = (P2 & 0x1f) | 0x80; P0 |= 0x08; P2 &= 0x1f;
	}
	else
	{
		P2 = (P2 & 0x1f) | 0x80; P0 &= ~0x08; P2 &= 0x1f;
	}
	if(VAIN1 > VAIN3)
	{
		P2 = (P2 & 0x1f) | 0x80; P0 &= ~0x10; P2 &= 0x1f;
	}
	else
	{
	 	P2 = (P2 & 0x1f) | 0x80; P0 |= 0x10; P2 &= 0x1f;
	} 
	EA = 1;		
}



void display()
{
	static u8 i;
	HC573(7,0xff);
	HC573(6,smg_we[i]);
	if(table_flag)HC573(7,table_VAIN_par[i]);
	else HC573(7,table_VAIN[i]);
	
	i++;if(i >= 8)i = 0;
}

void table_convert()
{
	if(table_flag == 0)
	{
		if(VIN_mode == 0)
		{
			table_VAIN[0] = smg_du[VAIN1 % 10];
			table_VAIN[1] = smg_du[VAIN1 / 10 % 10];
			table_VAIN[2] = smg_du[VAIN1 / 100] & 0x7f;
			table_VAIN[3] = 0xff;
			table_VAIN[4] = 0xff;
			table_VAIN[5] = 0xff;
			table_VAIN[6] = smg_du[1];
			table_VAIN[7] = 0xc1;
		}
		else
		{
			table_VAIN[0] = smg_du[VAIN3 % 10];
			table_VAIN[1] = smg_du[VAIN3 / 10 % 10];
			table_VAIN[2] = smg_du[VAIN3 / 100] & 0x7f;
			table_VAIN[3] = 0xff;
			table_VAIN[4] = 0xff;
			table_VAIN[5] = 0xff;
			table_VAIN[6] = smg_du[3];
			table_VAIN[7] = 0xc1;	
		}
	}
	else
	{
		if(VIN_mode == 0)
		{
			table_VAIN_par[0] = smg_du[VP1 % 10];
			table_VAIN_par[1] = smg_du[VP1 /10 % 10];
		   	table_VAIN_par[2] = smg_du[VP1 / 100] & 0x7f;
			table_VAIN_par[3] = 0xff;
		   	table_VAIN_par[4] = 0xff;
		   	table_VAIN_par[5] = 0xff;
		  	table_VAIN_par[6] = smg_du[1];
		   	table_VAIN_par[7] = 0x8c;
		}
		else
		{
			table_VAIN_par[0] = smg_du[VP3 % 10];
			table_VAIN_par[1] = smg_du[VP3 /10 % 10];
		   	table_VAIN_par[2] = smg_du[VP3 / 100] & 0x7f;
			table_VAIN_par[3] = 0xff;
		   	table_VAIN_par[4] = 0xff;
		   	table_VAIN_par[5] = 0xff;
		  	table_VAIN_par[6] = smg_du[3];
		   	table_VAIN_par[7] = 0x8c;
		}
	}
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

//void Delay10ms()		//@12.000MHz
//{
//	unsigned char i, j;
//
//	i = 117;
//	j = 184;
//	do
//	{
//		while (--j);
//	} while (--i);
//}
