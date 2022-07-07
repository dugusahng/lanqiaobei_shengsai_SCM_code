#include"reg52.h"
#include"key.h"
#include"onewire.h"
#include "intrins.h"
#define u8 unsigned char
#define u16 unsigned int
sfr AUXR = 0x8e;

extern u8 smg_duan[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
extern u8 down_table[8] = {0xbf,0xff,0xff,0xff,0xff,0xbf,0xff,0xff};
u8 smg_wei[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}; //从0x01开始是为方便温度阈值计数
u8 table_1[8] = {0xbf,0xc0,0xbf,0xff,0xff,0xff,0xc0,0xc0};

u8 temperature,count_temp,count_key,mode;
u16 blink_time,blink_count; 
bit	temp_flag,key_flag,blink_flag;
extern u8 trg = 0,cont = 0,Tmin = 20,Tmax = 30;
extern bit error_flag = 0,set_flag = 0 ;
																				     					
void Delay100ms();			 //函数声明
void Timer0Init(void);
void HC573(u8 n, u8 value);
void table_convert(void);

void main()
{
	u8 x;
	HC573(5,0x00);HC573(4,0xff);

	for(x = 0;x < 10;x++){temperature = rd_temperature(); Delay100ms();}

	if(temperature < Tmin)mode = 0;
	if((temperature >= Tmin)&&(temperature <= Tmax))mode = 1;
	if(temperature > Tmax)mode = 2; 

	Timer0Init();	
	while(1)
	{

		if(temp_flag)
		{
			temp_flag = 0;
			temperature = rd_temperature();		//读温度

			table_convert();			   //数码管数组

			if(temperature < Tmin)			//温度小于温度下限
			{
				mode = 0;					//开启模式0
				blink_time = 400;
				HC573(5,0x00);			   //关闭继电器
			}

			if((temperature >= Tmin)&&(temperature <= Tmax))
			{						//温度大于等于温度下限，小于等于温度上限
				mode = 1;			  //开启模式1
				blink_time = 200;
				HC573(5,0x00);		  //关闭继电器
			}

			if(temperature > Tmax)
			{
				mode = 2;				  //开启模式2
				blink_time = 100;
				HC573(5,0x10);			  //开启继电器
			}		
		}

		if(error_flag == 0)
		{
			if(blink_flag)	 HC573(4,~0x01);	//当闪烁计时等于给定模式的时间 时 开启L1		
			else  	HC573(4,0xff);			//关闭L1		
		}

		if(key_flag){key_flag = 0;keyproc();}  //按键

	}
}

void display(void);			 //函数声明
void Timer0isr() interrupt 1
{
	count_temp++; count_key++; blink_count++;
	if(count_temp == 250){count_temp = 0;temp_flag = 1;}	//0.5s

	if(count_key == 6){count_key = 0;key_flag = 1;}	  //12ms

	if(blink_count > blink_time){blink_count = 0;blink_flag = ~blink_flag;}	   //控制闪烁
		
//	if(error_flag == 0)				   //与主函数中重复
//	{
//		if(blink_flag) HC573(4,~0x01);		
//		else HC573(4,0xff);				
//	}

	display();				//smg显示函数
}

void display(void)
{
	static u8 i;				//定义为静态变量，下次调用时值不变
	HC573(7,0xff);				//消隐
	HC573(6,smg_wei[i]);			  //位选
	if(set_flag == 0) HC573(7,table_1[i]);	//段选
	else HC573(7,down_table[i]);		  //段选
	i++;if(i >= 8)i = 0;		
}

void table_convert(void)
{
	if(set_flag == 0)
	{
		table_1[1] = smg_duan[mode];
		table_1[6] = smg_duan[temperature / 10];
		table_1[7] = smg_duan[temperature % 10];	
	}			
}

void HC573(u8 n,u8 value)
{
	switch(n)
	{
		case 4:P2 = (P2 & 0x1f) | 0x80; P0 = value; P2 &= 0x1f; break;
		case 5:P2 = (P2 & 0x1f) | 0xa0; P0 = value; P2 &= 0x1f; break;
		case 6:P2 = (P2 & 0x1f) | 0xc0; P0 = value; P2 &= 0x1f; break;
		case 7:P2 = (P2 & 0x1f) | 0xe0; P0 = value; P2 &= 0x1f; break;
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
