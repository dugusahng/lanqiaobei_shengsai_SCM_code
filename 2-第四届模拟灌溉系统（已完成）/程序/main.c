//===================================//
//项目名称：模拟智能灌溉系统
//作者：电气204刘佳波
//时间：2022.03.08
//===================================//
#include"reg52.h"
#include"key.h"
#include"ds1302.h"
#include"iic.h"

#define u8 unsigned char
#define u16 unsigned int

sfr AUXR = 0x8e;

u8 smg_duan[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90}; 
u8 smg_wei[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8 time_table1[8];
u8 time_table2[8];

u8 key_count,dac,adc_count;  
extern u8 min = 0x30,hour = 0x08,sec = 0x00,hum = 0; 			
extern bit work_mode = 0,buzz_flag = 0,hum_val_flag = 0,s6_flag = 0,irr_flag = 0;
extern char hum_val = 50;
bit adc_flag = 0,key_flag = 0;

void initsystem();
void HC573(u8 n,u8 value);
void table_convert();
void Timer0Init(void);
void display();
void LEDproc();

void main()
{
	initsystem();	
	Timer0Init();
	hum_val = read_byte_at24c02(0x00);									
	init_time();									             	//0x10打开继电器
	while(1)										             	//0x40打开蜂鸣器
	{														   
        LEDproc();          	
		if(adc_flag)
		{	
			adc_flag = 0;		
			EA = 0;							 //断开中断，防止影响IIC总线时序
			dac = read_8591_channel(3);
			EA = 1;
			hum = BCD_DEC(dac) * 9 / 15;	//0x00-0xff转化为00-99			  
		}		
		EA = 0; 						 //断开中断，防止影响ds1302时序
		read_ds1302(); 
		EA = 1;
		
		if(key_flag){key_flag = 0;Keyproc();}	 //独立按键

		table_convert();		 				//数码管数组
	}
}

void Timer0isr() interrupt 1
{
	key_count++;adc_count++;
	if(adc_count >= 150)	//0.3s计时
	{
	 	adc_count = 0;
		adc_flag = 1;
	}
	 if(key_count > 6)	    //12ms
	{
		key_count = 0;
		key_flag = 1;
	}  	  		
	display();
}

void LEDproc()
{
	EA = 0;
	if(work_mode == 0)	                                        //自动模式
	{
		HC573(4,~0x01);	                 							//打开L1					
		if(hum < hum_val){HC573(5,0x10);}  					    //打开继电器
		else{HC573(5,0x00);}	           						//关闭继电器
	}
	else			                                            //手动模式								 
	{
		HC573(4,~0x02);	  	                                    //打开L2  																				           
		if(irr_flag == 1)
		{	
			if((hum < hum_val)&&(buzz_flag == 0))        //buzz_flag为0时准许蜂鸣器工作	
			{
				HC573(5,0xff);							            //打开蜂鸣器继电器
			}
			else
			{										     	    	
				HC573(5,0x10);										//关闭蜂鸣器打开继电器
			}	
		}
		if(irr_flag == 0)
		{
			if((hum < hum_val)&&(buzz_flag == 0))
			{
				HC573(5,0x40);									    //打开蜂鸣器关闭继电器
			}
			else
			{
				HC573(5,0x00);									    //关闭蜂鸣器继电器
			}
		}
	}
	EA = 1;
}

void initsystem()					//系统初始化
{
	HC573(5,0x00);
	HC573(4,0xff);
}

void HC573(u8 n,u8 value)
{
	switch(n)
	{
		case 4:  P0 = value; P2 = (P2 & 0x1f) | 0x80;  P2 &= 0x1f; break;
		case 5:  P0 = value; P2 = (P2 & 0x1f) | 0xa0;  P2 &= 0x1f; break;
		case 6:  P0 = value; P2 = (P2 & 0x1f) | 0xc0;  P2 &= 0x1f; break;
		case 7:  P0 = value; P2 = (P2 & 0x1f) | 0xe0;  P2 &= 0x1f; break;
	}
}

void display()			 //SMG显示
{
	static u8 i;
	HC573(7,0xff);
	HC573(6,smg_wei[i]);

	
	if(s6_flag == 0)
	{		
		HC573(7,time_table1[i]);	
	}
	else
	{	
		HC573(7,time_table2[i]);		
	}
	
	i++;if(i >= 8)i = 0;
}

void table_convert()
{		

  	if(s6_flag == 0)
	{
	time_table1[0] = smg_duan[hum % 10];
	time_table1[1] = smg_duan[hum / 10];
	time_table1[2] = 0xff;
	time_table1[3] = smg_duan[min % 10];
	time_table1[4] = smg_duan[min / 10];
	time_table1[5] = 0xbf;
	time_table1[6] = smg_duan[hour % 10];
	time_table1[7] = smg_duan[hour / 10];
	}
	else
	{
	time_table2[0] = smg_duan[hum_val % 10];
	time_table2[1] = smg_duan[hum_val / 10];
	time_table2[2] = 0xff;
	time_table2[3] = 0xff;
	time_table2[4] = 0xff;
	time_table2[5] = 0xff;
	time_table2[6] = 0xbf;
	time_table2[7] = 0xbf;
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