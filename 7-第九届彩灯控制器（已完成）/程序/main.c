#include"reg52.h"
#include"key.h"
#include"iic.h"
#include "intrins.h"
#define u8 unsigned char
#define u16 unsigned int
sfr AUXR = 0x8e;

u8 smg_duan[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
u8 smg_wei[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

u8 work_mode_1[] = {~0x01,~0x02,~0x04,~0x08,~0x10,~0x20,~0x40,~0x80};
u8 work_mode_2[] = {~0x80,~0x40,~0x20,~0x10,~0x08,~0x04,~0x02,~0x01};
u8 work_mode_3[] = {~0x81,~0x42,~0x24,~0x18};
u8 work_mode_4[] = {~0x18,~0x24,~0x42,~0x81};

u8 table1[8];
u8 table2[8];
extern u16 time_f[4] = 0;

u8 key_count,pwm,dac;
extern u16	time = 400;
static u16 count,flash_count,pwm_count;
bit key_flag,flash_flag,time_flag,pwm_flag,ad_flag,turn_flag,led_flag ;
extern u8  work_mode = 1,s6_mode = 0;
extern bit s6_flag = 0,open_flag = 1,pwm_buff = 0;

void HC573(u8 n,u8 value);
void Timer0Init(void);
void LEDworking();
void display(void);
void table_convert(void);
void initsystem();

void main()
{	
	initsystem();
	while(1)
	{
		  	EA = 0;
		 	dac = read_8591_channel(3);
		 	EA = 1;						
			ad_flag = 0;
			if(dac<63)pwm = 2;
			
			else if((dac>=63)&&(dac<127))pwm = 4;
			
			else if((dac>=127)&&(dac<191))pwm = 6;
			
			else if((dac>=191)&&(dac<255))pwm = 8;
			EA = 0;
			LEDworking();
			EA = 1;
		if(key_flag == 1){key_flag = 0;Keyproc();}
	 	table_convert();
	}
}

void Timer0isr() interrupt 1
{
   	if(++key_count == 12){key_count = 0;key_flag = 1;}
	if(++flash_count == 800)
	{
		flash_count = 0;
		flash_flag = ~flash_flag;

	}
		 
		if(++count >= time_f[work_mode - 1]&&open_flag == 1)
		{
			turn_flag = 1;	//流转间隔标志
			count = 0;	   
		}
	
	if(++pwm_count == 10) 
	{
	  
		pwm_count = 0; 
	}
	
 	if(pwm_count <= pwm)led_flag = 1; //pwm标志
	if(pwm_count > pwm)led_flag = 0;
	display();
}

void Delay10ms();

void initsystem()
{
	HC573(5,0x00);
	HC573(4,0xff);
	Timer0Init();
 	time_f[0] = read_byte_at24c02(0x00) * 10;
	Delay10ms();
	time_f[1] = read_byte_at24c02(0x01) * 10;
	Delay10ms();
	time_f[2] = read_byte_at24c02(0x02) * 10;
	Delay10ms();
	time_f[3] = read_byte_at24c02(0x03) * 10;
	Delay10ms();
}

void LEDworking()
{
	static a = 0x01;
	static b = 0x80;
	static count_led;

	if(led_flag == 1)
	{
		if(work_mode == 1)
		{		
			HC573(4,~a);
			if(turn_flag == 1)
			{
				a=_crol_(a,1);
				turn_flag = 0;
			}
		}

		if(work_mode == 2)
		{
			HC573(4,~b);
			if(turn_flag == 1)
			{
				b=_cror_(b,1);
				turn_flag = 0;
			}
		}

		if(work_mode == 3)
		{
			if(turn_flag == 1)
			{
				turn_flag = 0;
				if(++count_led == 5){count_led = 1;}
			}	
			if(count_led == 1){HC573(4,~0x81);}
			if(count_led == 2){HC573(4,~0x42);}
			if(count_led == 3){HC573(4,~0x24);}
			if(count_led == 4){HC573(4,~0x18);}				
		}

		if(work_mode == 4)
		{
			if(turn_flag == 1)
			{
				turn_flag = 0;
				if(++count_led == 5){count_led = 1;}
			}		
			if(count_led == 1){HC573(4,~0x18);}	
			if(count_led == 2){HC573(4,~0x24);}					
			if(count_led == 3){HC573(4,~0x42);}	
			if(count_led == 4){HC573(4,~0x81);}						
	    }
	}
	else if(led_flag == 0){HC573(4,0xff);}	
}

void display(void)
{
	static u8 i;
	HC573(7,0xff);
	HC573(6,smg_wei[i]);
	if(s6_flag == 1){HC573(7,table1[i]);}
	if((pwm_buff == 1)&&(s6_flag == 0)){HC573(7,table2[i]);}	
	i++;
	if(i >= 8) i = 0;
}

void table_convert(void)
{
	if((flash_flag == 1)&&(s6_mode == 1))
	{
		table1[0] = smg_duan[time_f[work_mode - 1] % 10];
		table1[1] = smg_duan[time_f[work_mode - 1] / 10 % 10];
		table1[2] = smg_duan[time_f[work_mode - 1] / 100 %10];
		table1[3] = smg_duan[time_f[work_mode - 1] / 1000];
		table1[4] = 0xff;
		table1[5] = 0xbf;
		table1[6] = smg_duan[work_mode];		
		table1[7] = 0xbf;
	}
		if((flash_flag == 0)&&(s6_mode == 1))
	{
		table1[0] = smg_duan[time_f[work_mode - 1] % 10];
		table1[1] = smg_duan[time_f[work_mode - 1] / 10 % 10];
		table1[2] = smg_duan[time_f[work_mode - 1] / 100 %10];
		table1[3] = smg_duan[time_f[work_mode - 1] / 1000];
		table1[4] = 0xff;
		table1[5] = 0xbf;
		table1[6] = 0xff;		
		table1[7] = 0xbf;
	}
	if((flash_flag == 1)&&(s6_mode == 2))
	{
		table1[0] = smg_duan[time_f[work_mode - 1] % 10];
		table1[1] = smg_duan[time_f[work_mode - 1] / 10 % 10];
		table1[2] = smg_duan[time_f[work_mode - 1] / 100 %10];
		table1[3] = smg_duan[time_f[work_mode - 1] / 1000];
		table1[4] = 0xff;
		table1[5] = 0xbf;
		table1[6] = smg_duan[work_mode];		
		table1[7] = 0xbf;
	}
	if((flash_flag == 0)&&(s6_mode == 2))
	{
		table1[0] = 0xff;
		table1[1] = 0xff;
		table1[2] =	0xff;
		table1[3] = 0xff;
		table1[4] = 0xff;
		table1[5] = 0xbf;
		table1[6] = smg_duan[work_mode];		
		table1[7] = 0xbf;
	}

	if((pwm_buff == 1)&&(s6_flag == 0))
	{
		table2[0] = smg_duan[pwm];
		table2[1] = 0xbf;
		table2[2] = 0xff;
		table2[3] = 0xff;
		table2[4] = 0xff;
		table2[5] = 0xff;
		table2[6] = 0xff;
		table2[7] = 0xff;
	}
	else if((s6_flag == 0)&&(pwm_buff == 0))
	{
		HC573(7,0xff);
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

void Timer0Init(void)		//T0 1ms     @12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x20;		//设置定时0初始值
	TH0 = 0xD1;		//设置定时0初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;  
	EA = 1;
}

void Delay10ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 117;
	j = 184;
	do
	{
		while (--j);
	} while (--i);
}
