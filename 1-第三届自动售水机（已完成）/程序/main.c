#include"reg52.h"
#include"key.h"
#include"iic.h"
#define u8 unsigned char
#define u16 unsigned int
sfr AUXR = 0x8e;
u8 smg_duan[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
u8 smg_wei[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8 table_1[8]; 
 
u8 count_key,count_dac,count_water;
u16 feilv = 50,zongjia = 0,dac;
bit	key_flag,dac_flag,water_flag;
extern u8 trg = 0,cont = 0;
extern u16 shuiliang = 0;
extern bit s7_flag = 0,s6_flag = 0;
float r_dac;	   //定义光敏电阻阻值

void Timer0Init(void);
void display(void);
void table_convert(void);
	  	 
void main()
{

	P2 = (P2 & 0x1f) | 0xa0; P0 = 0x00; P2 &= 0x1f;	//关蜂鸣器，继电器	
	P2 = (P2 & 0x1f) | 0x80; P0 = 0xff; P2 &= 0x1f;//关led灯		
	Timer0Init();
	while(1)
	{
		if(dac_flag)		//可以在较暗环境下用手机闪光灯检验
		{
			dac_flag = 0;
			write_8591(1);				 //???
			r_dac = read_8591(1);		 //读光敏电阻阻值
			dac = (u16)(r_dac *100 / 51.0f);	 //我不理解，但我大受震撼！
			if(dac < 125)	//L1亮
			{
				P2 = (P2 & 0x1f) | 0x80; P0 = ~0x01; P2 &= 0x1f;	
			}
			if(dac > 125)	//L1灭
			{
				P2 = (P2 & 0x1f) | 0x80; P0 = 0xff; P2 &= 0x1f;
			}
		}
		if(s7_flag == 1)   //L10亮
		{
			P2 = (P2 & 0x1f) | 0xa0; P0 = 0x10; P2 &= 0x1f;
		}
		if(s7_flag == 0)	//L10灭
		{
			P2 = (P2 & 0x1f) | 0xa0; P0 = 0x00; P2 &= 0x1f;
		}
		if(water_flag)				  //每100ms出水量加100
		{
			water_flag = 0;
			shuiliang += 100;
		}
	
		if(shuiliang >= 9999)		  //当出水量大于9999时自动停水
		{
			 P2 = (P2 & 0x1f) | 0xa0; P0 = 0x00; P2 &= 0x1f;
			 s7_flag = 0;s6_flag = 1;
		}
	
		zongjia = shuiliang / 2;	 //？？不足之处是，没用到费率0.5
	
		if(key_flag){key_flag = 0;keyproc();}
		table_convert();
	}
}

void Timer0isr(void) interrupt 1
{
	if(++count_key >= 6)	   //12ms读一次独立按键
	{
		count_key = 0;
		key_flag = 1;
	}
	if(++count_dac >= 150)		 //0.3s读一次电压
	{
		count_dac = 0;
		dac_flag = 1;
	}
	if((++count_water >= 50)&&(s7_flag == 1)) //100ms，当s7按下时开始出水计时
	{
	   count_water = 0;
	   water_flag = 1;	
	}

	display();
}

void display(void)
{
	static u8 i;
	P2 = (P2 & 0x1f) | 0xe0; P0 = 0xff; P2 &= 0x1f;		//消隐
	P2 = (P2 & 0x1f) | 0xc0; P0 = smg_wei[i]; P2 &= 0x1f;		//位选
	if(s6_flag == 0)
	{P2 = (P2 & 0x1f) | 0xe0; P0 = table_1[i]; P2 &= 0x1f;} 	//段选
	if(s6_flag == 1)
	{P2 = (P2 & 0x1f) | 0xe0; P0 = table_1[i]; P2 &= 0x1f;} 	//段选
	i++;if(i >= 8)i = 0;	
}

void table_convert(void)
{

	table_1[4] = smg_duan[feilv % 10]; 
	table_1[5] = smg_duan[feilv / 10 % 10]; 
	table_1[6] = smg_duan[feilv / 100 % 10] & 0x7f;
	table_1[7] = 0xff;

	if(s6_flag == 0)		   //当s6未按下时显示出水量
	{
	table_1[0] = smg_duan[shuiliang % 10]; 
	table_1[1] = smg_duan[shuiliang / 10 % 10]; 
	table_1[2] = smg_duan[shuiliang / 100 % 10] & 0x7f; 
	table_1[3] = smg_duan[shuiliang / 1000 % 10]; 
	}

	if(s6_flag == 1)	   	//当s6按下时显示总价
	{
	table_1[0] = smg_duan[zongjia % 10]; 
	table_1[1] = smg_duan[zongjia / 10 % 10]; 
	table_1[2] = smg_duan[zongjia / 100 % 10] & 0x7f; 
	table_1[3] = smg_duan[zongjia / 1000 % 10]; 
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
