#include "reg52.h"
#include "iic.h"
#include "intrins.h"
sfr AUXR = 0x8e;
#define uchar unsigned char
#define uint unsigned int

sbit relay = P0^4;
sbit buzz = P0^6;

bit turn_flag = 0;
bit set_flag = 0;
bit blink_flag = 0;
bit led_flag = 0;
bit ad_flag = 0;
bit open_flag = 1;
bit pwm_buff = 0;

uchar trg,cont;//按键
uchar mode = 1;
uchar set_count;
uchar pwm;
uint dianya;
uint turn_time;
uchar duan[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00,0x40,0xBF}; //0-9,black,-
uchar disbuff[8];

void clzbuzz()
{
	P2 = P2&0x1f|0xa0;
	buzz = 0;
	relay = 0;
	P2 = P2&0x1f;
}

void display()
{
	static uchar index;

	P2 = P2&0x1f|0xe0;//选择所有段码
	P0 = 0xff;//消影
	P2 = P2&0x1f;

	P2 = P2&0x1f|0xc0;
	P0 = 1<<index;
	P2 = P2&0x1f;

	P2 = P2&0x1f|0xe0;
	P0 = ~duan[disbuff[index]];
	P2 = P2&0x1f;

	index++;
	index &= 0x07;
}

void keyscan()
{
	uchar readdate = P3^0xff;
	trg = readdate&(readdate^cont);
	cont = readdate; 
}

void keyfun()
{
	if(trg == 0x01)
	{
		open_flag = ~open_flag;
	}
	if(trg == 0x02)
	{
		set_flag = 1;


		if(++set_count == 3)
		{
			set_flag = 0;
			set_count = 0;

			switch(mode)
			{
				case 1:write_24c02(0x05,turn_time);break;
				case 2:write_24c02(0x02,turn_time);break;				
				case 3:write_24c02(0x03,turn_time);break;
				case 4:write_24c02(0x04,turn_time);break;	
			}
		}
	}

	if((trg == 0x04)&&(set_count == 1))
	{
		 if(++mode == 5)
		 mode = 4;
		switch(mode)
			{
				case 1 :turn_time = read_24c02(0x05);break;
				case 2 :turn_time = read_24c02(0x02);break;
				case 3 :turn_time = read_24c02(0x03);break;
				case 4 :turn_time = read_24c02(0x04);break;
			}
	}

	if((trg == 0x08)&&(set_count == 1))
	{
		 if(--mode == 0)
		 mode = 1;
		switch(mode)
			{
				case 1 :turn_time = read_24c02(0x05);break;
				case 2 :turn_time = read_24c02(0x02);break;
				case 3 :turn_time = read_24c02(0x03);break;
				case 4 :turn_time = read_24c02(0x04);break;
			}
	}

	if((trg == 0x04)&&(set_count == 2))
	{
		 if(++turn_time == 13)
		 turn_time = 12;
	}

	if((trg == 0x08)&&(set_count == 2))
	{		
		 if(--turn_time == 3)
		 turn_time = 4;
	}

	if(cont == 0x08)
	{
		pwm_buff = 1;
	}
	else  pwm_buff = 0;
}

void show_num()
{
	if(set_flag == 1)
	{
		if((blink_flag == 1)&&(set_count == 1))
		{
			disbuff[0] = 11;
			disbuff[1] = mode;
			disbuff[2] = 11;
		 	disbuff[3] = 10;
			if(turn_time/10 == 0)
			disbuff[4] =10;
			else
			disbuff[4] = turn_time/10;
			disbuff[5] = turn_time%10;
			disbuff[6] = 0;
			disbuff[7] = 0;				
		}

		if((blink_flag == 0)&&(set_count == 1))
		{
			disbuff[0] = 10;
			disbuff[1] = 10;
			disbuff[2] = 10;
			disbuff[3] = 10;
			if(turn_time/10 == 0)
			disbuff[4] =10;
			else
			disbuff[4] = turn_time/10;
			disbuff[5] = turn_time%10;
			disbuff[6] = 0;
			disbuff[7] = 0;				
		}

		if((blink_flag == 1)&&(set_count == 2))
		{
			disbuff[0] = 11;
			disbuff[1] = mode;
			disbuff[2] = 11;
			disbuff[3] = 10;
			if(turn_time/10 == 0)
			disbuff[4] =10;
			else
			disbuff[4] = turn_time/10;
			disbuff[5] = turn_time%10;
			disbuff[6] = 0;
			disbuff[7] = 0;			
		}

		if((blink_flag == 0)&&(set_count == 2))
		{
			disbuff[0] = 11;
			disbuff[1] = mode;
			disbuff[2] = 11;
			disbuff[3] = 10;
			disbuff[4] = 10;
			disbuff[5] = 10;
			disbuff[6] = 10;
			disbuff[7] = 10;			
		}
	}
	if(pwm_buff == 1&&set_flag == 0)
	{
		disbuff[0] = 10;
		disbuff[1] = 10;
		disbuff[2] = 10;
		disbuff[3] = 10;
		disbuff[4] = 10;
		disbuff[5] = 10;
		disbuff[6] = 11;
		disbuff[7] = pwm;		
	}
	else if(set_flag == 0&&pwm_buff == 0)
	{
		disbuff[0] = 10;
		disbuff[1] = 10;
		disbuff[2] = 10;
		disbuff[3] = 10;
		disbuff[4] = 10;
		disbuff[5] = 10;
		disbuff[6] = 10;
		disbuff[7] = 10;	
	}
}

void LED_set()
{
	static a = 0x01;
	static b = 0x80;
	static count_led;

	if(led_flag == 1)
	{
	if(mode == 1)
	{		
		P2 = P2&0x1f|0x80;
		P0 = ~a;
		P2 = P2&0x1f;

		if(turn_flag == 1)
		{
			a=_crol_(a,1);		 //只能左移
			turn_flag = 0;
		}
	}

	if(mode == 2)
	{
		P2 = P2&0x1f|0x80;
		P0 = ~b;
		P2 = P2&0x1f;

		if(turn_flag == 1)
		{
			b=_cror_(b,1);
			turn_flag = 0;
		}
	}

	if(mode == 3)
	{
		if(turn_flag == 1)
		{
			turn_flag = 0;
			if(++count_led == 5)
			{
				 count_led = 1;
			}
		}

		if(count_led == 1)
		{
			P2 = P2&0x1f|0x80;
			P0 = ~0x81;
			P2 = P2&0x1f;
		}

		if(count_led == 2)
		{
			P2 = P2&0x1f|0x80;
			P0 = ~0x42;
			P2 = P2&0x1f;
		}

		if(count_led == 3)
		{
			P2 = P2&0x1f|0x80;
			P0 = ~0x24;
			P2 = P2&0x1f;
		}

		if(count_led == 4)
		{
			P2 = P2&0x1f|0x80;
			P0 = ~0x18;
			P2 = P2&0x1f;
		}	
	}

	if(mode == 4)
	{
		if(turn_flag == 1)
		{
			turn_flag = 0;
			if(++count_led == 5)
			{
				 count_led = 1;
			}
		}

		if(count_led == 1)
		{
			P2 = P2&0x1f|0x80;
			P0 = ~0x18;
			P2 = P2&0x1f;
		}

		if(count_led == 2)
		{
			P2 = P2&0x1f|0x80;
			P0 = ~0x24;
			P2 = P2&0x1f;
		}

		if(count_led == 3)
		{
			P2 = P2&0x1f|0x80;
			P0 = ~0x42;
			P2 = P2&0x1f;
		}

		if(count_led == 4)
		{
			P2 = P2&0x1f|0x80;
			P0 = ~0x81;
			P2 = P2&0x1f;
		}	
	}
	}

	else if(led_flag == 0)
	{
		P2 = P2&0x1f|0x80;
		P0 = 0xff;
		P2 = P2&0x1f;
	}
}
void Timer0Init()		//1毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x20;		//设置定时初值
	TH0 = 0xD1;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void Timer0() interrupt 1
{
	static uint count;
	static uint blink_count;
	static turn_time_count,pwm_count;
	keyscan();
	keyfun();
	display();

	if(++blink_count == 800)
	{
		blink_flag = ~blink_flag;
		blink_count = 0;
		ad_flag = 1;
	}

	if(++turn_time_count == 100)
	{
		 turn_time_count = 0;
		 
		if(++count >= turn_time&&open_flag == 1)
		{
			turn_flag = 1;
			count = 0;	   
		}
	}

	if(++pwm_count == 10)
	{
		pwm_count = 0; 
	}

	if(pwm_count < pwm)
	led_flag = 1;
	else if(pwm_count > pwm)led_flag = 0;
}

void main()
{
	clzbuzz();
	Timer0Init();
	init_adc(0x03); //初始化adc
	turn_time = read_24c02(0x05);
	while(1)
	{
		show_num();
		LED_set();

		if(ad_flag == 1)
		{
			dianya = (uchar)read_adc(0x03);
			dianya = dianya/255.0f*99;
			ad_flag = 0;
			if(dianya<25)
			pwm = 1;
			if(dianya>=25&&dianya<50)
			pwm = 2;
			if(dianya>=50&&dianya<75)
			pwm = 3;
			if(dianya>=75&&dianya<100)
			pwm = 4;
		}
	}
}
