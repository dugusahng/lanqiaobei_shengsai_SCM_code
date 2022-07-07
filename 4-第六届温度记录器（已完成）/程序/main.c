#include"reg52.h"
#include"key.h"
#include"onewire.h"
#include"ds1302.h"
#include "intrins.h"
#define u8 unsigned char
#define u16 unsigned int
sfr AUXR = 0x8e;

u8 smg_duan[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
u8 smg_wei[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8 table_1[8],table_2[8],table_3[8];
u8 temp[10];

u8 temperature,count_key;
u16 count_temp,count_flash,count_1;
bit temp_flag,key_flag,flash_flag;
extern u8 trg = 0,cont = 0,count = 1,time = 1,n = 0,mode = 1,t = 0;
extern u8 sec = 0x50,min = 0x59,hour = 0x23;	 //定义时间23点59分50秒
extern u16 count_time = 500;
extern bit L1_flag = 0,s6_flag = 0;

void Delay100ms();		   //函数声明
void Timer0Init(void);
void table_convert();

void main()
{	
	u8 x;
	P2 = (P2 & 0x1f) | 0xa0; P0 = 0x00; P2 &= 0x1f;	  //灭蜂鸣器继电器
	P2 = (P2 & 0x1f) | 0x80; P0 = 0xff; P2 &= 0x1f;  //led全灭
	for(x = 0;x < 10; x++){temperature = rd_temperature();Delay100ms();} //把温度初始值85读掉
	init_time();			//时间初始化
	Timer0Init();			//定时器初始化
	while(1)
	{	
		if(mode == 2)
		{
		if(temp_flag)
		{
			temp_flag = 0;
			t++;if(t >= 11)t = 10;					
			temp[t-1] = rd_temperature();						
		}		
		}

		if(t == 10)
		{	
			mode = 3;					
					
			if(L1_flag == 1)	 //按下s6时，灯灭
			{				
			  	P2 = (P2 & 0x1f) | 0x80; P0 = 0xff; P2 &= 0x1f;			
			}
			else	 //否则，灯亮
			{
				P2 = (P2 & 0x1f) | 0x80; P0 = ~0x01; P2 &= 0x1f;
			}
		}

		if(key_flag){key_flag = 0;Keyproc();}	  //执行按键函数

		table_convert(); 		   //数组要放到主函数里一直读

		read_ds1302();			   //读时钟
	}

}

void display();             //函数声明
void Timer0isr() interrupt 1
{
	count_flash++;count_key++;

	if(mode == 2)
	{
		count_temp++;
		if(count_temp >= count_time)	   //当计时 等于 设定的延时
		{		
			count_temp = 0;
			temp_flag = 1;
		}
	}
		
	if(count_flash == 500){count_flash = 0;flash_flag =~ flash_flag;} //1s 闪烁
			   	 
	if(mode == 3)	  
	{
		if((temp[9] != 0)&&(L1_flag == 1))  //当温度数组读完时，并按下s6时，启动计时
		{
			if(++count_1 == 500)		   //1s
			{				
				n++;if(n >= 10)n = 9;		//加到9停止		
				count_1 = 0;
			}
		}
	}
		
	if(count_key == 6){count_key = 0;key_flag = 1;}				//12ms，独立按键
	
	display();				//2ms，数码管显示函数
}

void display()				//数码管显示函数
{
	static u8 i;			//定义成静态变量，下次调用值不变
	P2 = (P2 & 0x1f) | 0xe0; P0 = 0xff; P2 &= 0x1f;	 //数码管消隐
	P2 = (P2 & 0x1f) | 0xc0; P0 = smg_wei[i]; P2 &= 0x1f;	//位选

	if(mode == 1)					 //mode1段选
	{
		P2 = (P2 & 0x1f) | 0xe0; P0 = table_1[i]; P2 &= 0x1f;
	}
	if(mode == 2)			  //mode2段选
	{
		P2 = (P2 & 0x1f) | 0xe0; P0 = table_2[i]; P2 &= 0x1f;
	}
	if(mode == 3)		   //mode2段选
	{
		P2 = (P2 & 0x1f) | 0xe0; P0 = table_3[i]; P2 &= 0x1f; 
	}
	i++;if(i >= 8)i = 0;		//能使i在0-8内循环
}

void table_convert()		   //将数码管显示的内容写到数组里
{
	if(mode == 1)
	{
	table_1[0] = smg_duan[time % 10];
	table_1[1] = smg_duan[time / 10];
  	table_1[2] = 0xbf;
	table_1[3] = 0xff;
	table_1[4] = 0xff;
	table_1[5] = 0xff;
	table_1[6] = 0xff;
	table_1[7] = 0xff;
	}

   	if(mode == 2)
	{
		if(flash_flag)
		{
			table_2[0] = smg_duan[sec%10];			
			table_2[1] = smg_duan[sec/10];
			table_2[2] = 0xff;
			table_2[3] = smg_duan[min%10];
			table_2[4] = smg_duan[min/10];
			table_2[5] = 0xff;
			table_2[6] = smg_duan[hour%10];
			table_2[7] = smg_duan[hour/10];			
		}
		else
		{	
			table_2[0] = smg_duan[sec%10];
			table_2[1] = smg_duan[sec/10];
			table_2[2] = 0xbf;
			table_2[3] = smg_duan[min%10];
			table_2[4] = smg_duan[min/10];
			table_2[5] = 0xbf;	
			table_2[6] = smg_duan[hour%10];
			table_2[7] = smg_duan[hour/10];		
		
		}
	}
	if(mode == 3)
	{
		table_3[0] = smg_duan[temp[n]%10];
		table_3[1] = smg_duan[temp[n]/10];	
		table_3[2] = 0xbf;
		table_3[3] = 0xff;
		table_3[4] = 0xff;
		table_3[5] = smg_duan[n];
		table_3[6] = smg_duan[0];
		table_3[7] = 0xbf;
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