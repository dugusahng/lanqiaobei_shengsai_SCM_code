#include"reg52.h"
#include"key.h"
#include"onewire.h"
#include"iic.h"
sfr AUXR = 0x8e;
#define u8 unsigned char
#define u16 unsigned int

u8 code smg_du[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};	   //段码
u8 code smg_we[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};  		 //位选
u8 table_temp[] = {0xc0,0xc0,0xc0,0xc0,0xff,0xff,0xff,0xc6};   //温度数组
u8 table_par[] = {0xc0,0xc0,0xff,0xff,0xff,0xff,0xff,0x8c};	  //参数
u8 table_dac[] = {0xc0,0xc0,0xc0,0xff,0xff,0xff,0xff,0x88};	  //电压
		 
u8 temperature;
u16 dac;
u8 count_key,count_temp,count_dac;
bit key_flag,temp_flag,dac_flag;
extern u8 trg = 0,cont = 0,table_mode = 0,temp_par_1 = 25;
extern char temp_par = 25;
extern u8 temp_dot = 0;
extern bit dac_mode = 1;

void Timer0Init(void);			  //函数声明
void Delay100ms();
void HC573(u8 n,u8 value);
void table_convert();
void DACproc();
void LEDproc();

void main()
{
	u8 x;
	HC573(5,0x00);
	HC573(4,0xff);
	for(x = 0;x <= 10;x++){temperature = rd_temperature();Delay100ms();}  //把初值85读掉
	Timer0Init();
	while(1)
	{
		table_convert();
		if(temp_flag){temp_flag = 0;temperature = rd_temperature();}
	 	if(key_flag){key_flag = 0;Keyproc();}
		if(dac_flag)
		{
			DACproc();		//dac输出函数
		}
		EA = 0;
		LEDproc();	//led功能函数
		EA = 1;
	}
}

void DACproc()
{
	if(dac_mode)
	{
		if(temperature < temp_par_1)	//温度小于温度参数1时
		{
			dac = 0;
			wirte_8591(dac);			 				
		}
		else
		{
			dac = 500;
			wirte_8591(dac / 100 * 51);
		}
	}
	else
	{
		if(temperature <= 20)
		{
			dac = 100;
			wirte_8591(dac / 100 * 51);		
		}
		else if((temperature > 20)&&(temperature < 40))
		{
			dac = (temperature * 0.15) * 100;
			wirte_8591((temperature * 0.15) * 51);			
		}
		else if(temperature >= 40)
		{
		   	dac = 400;
			wirte_8591(dac / 100 * 51);
		}
	}
}

void LEDproc()
{
	u8 led = 0xff;
	if(dac_mode)led &= ~0x01;		 	
	else led |= 0x01;	
		
	if(table_mode == 0)led &= ~0x02;	
	else led |= 0x02;
		
	if(table_mode == 1)led &= ~0x04;
	else led |= 0x04;
		
	if(table_mode == 2)led &= ~0x08;
	else led |= 0x08;

	P2 = (P2 & 0x1f) | 0x80; P0 = led; P2 &= 0x1f;
}

void display();
void Timer0isr() interrupt 1
{
	count_key++;count_temp++;count_dac++;
	if(count_key >= 6){count_key = 0;key_flag = 1;}
	if(count_temp >= 20){count_temp = 0;temp_flag = 1;}
	if(count_dac >= 200){count_dac = 0;dac_flag = 1;}
	display();
}

void display()
{
	static u8 i;
	HC573(7,0xff);
	HC573(6,smg_we[i]);
	if(table_mode == 0)HC573(7,table_temp[i]);
	else if(table_mode == 1)HC573(7,table_par[i]);
	else if(table_mode == 2)HC573(7,table_dac[i]);
	i++;if(i >= 8)i = 0;
}

void table_convert()
{
	if(table_mode == 0)
	{
		table_temp[0] = smg_du[temp_dot % 10];
		table_temp[1] = smg_du[temp_dot / 10];
		table_temp[2] = smg_du[temperature % 10] & 0x7f;
		table_temp[3] = smg_du[temperature / 10];
	}
	if(table_mode == 1)
	{
		table_par[0] = smg_du[temp_par % 10];
		table_par[1] = smg_du[temp_par / 10];
	}
	if(table_mode == 2)
	{
		table_dac[0] = smg_du[dac % 10];
		table_dac[1] = smg_du[dac / 10 % 10];
		table_dac[2] = smg_du[dac / 100] & 0x7f;
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
