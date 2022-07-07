#include"reg52.h"
#include"key.h"
#include"onewire.h"

sfr AUXR = 0x8e;

#define u8 unsigned char
#define u16 unsigned int

u8 smg_duan[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
u8 smg_wei[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8 temp_table[8];
u8 time_table[8];

extern u16 sec = 10;
u8 temperature,count,count_temp,count_key,count_sec = 0;
extern  u8 work_mode = 1,pwm_duty = 0,count_pwm = 0; 
extern bit temp_flag = 0,key_flag = 0,s7_flag = 0;

void Delay1ms();
void Delayms(u16 ms);
void HC573(u8 n,u8 value);
void display(void);
void table_convert(void);
void Working(void);
void Timer0Init(void);

void main()
{  	
	u8 i;
	HC573(5,0x00);
	HC573(4,0xff);
	for(i = 0;i < 10;i++){temperature = rd_temperature();Delayms(100);}	  
	Timer0Init();
	while(1)
	{	
		EA = 0;	
		Working();
		EA = 1;		 
		if(temp_flag)
		{
	 		temp_flag = 0;
	 		temperature = rd_temperature();
		}

		if(key_flag)
		{
	 		key_flag = 0;
	 		Keyproc();
		}

		table_convert();
	}
}

void Working(void)
{
	if(sec != 0)
	{
		
		if(work_mode == 1)
		{
			pwm_duty = 2;
			
		}

		else if(work_mode == 2)
		{	
			pwm_duty = 3;
			
		}

		else if(work_mode == 3)
		{
			pwm_duty = 7;				 
		}			
	}	
	if(sec == 0)
	{
	    count_sec = 0;
		HC573(4,0xff);
	}	
	
}

void Timer0isr(void) interrupt 1
{	
	count++;count_pwm++;
	if(count == 20)		  //2ms
	{
		count = 0;
		count_key++;
		count_temp++;
	}  
		 
	if(count_key == 6) //每12ms读一次按键
	{
		count_key = 0;
		key_flag = 1;	  
	}
	
	if(count_temp == 250)   //每0.5s读一次温度
	{
		count_temp = 0;
		temp_flag = 1;

	  	count_sec++;		  //
	}

	if(sec != 0)
	{
		if(count_sec == 2)	            
		{	
		count_sec = 0;		
		sec--;
		}
	}

	if(sec > 0)
	{
		if(work_mode == 1)
	 	{
	    	if(count_pwm >= pwm_duty)
			{		    	
				HC573(4,0xff);				
	 		}
			   	
	 		if(count_pwm == 10)
			{
				count_pwm = 0;		   		
				HC573(4,~0x01);					
	 		}
	 	}
	  	else if(work_mode==2)
	  	{
	     	if(count_pwm >= pwm_duty)
		 	{		   	 
				HC573(4,0xff);				
			}
		 	if(count_pwm == 10)
		 	{				
			   	count_pwm = 0;
			   	HC573(4,~0x02);						   	 	
			}
	  	}
	  	else if(work_mode==3)
	  	{
	     	if(count_pwm >= pwm_duty)
			{
				 HC573(4,0xff);
			}
		 	if(count_pwm == 10)
		 	{
				 count_pwm = 0;
				 HC573(4,~0x04);		   		 
	 		}
  		}
 	} 
 	display();		   //数码管显示
}

void display(void)
{
	static u8 i; 
	HC573(7,0xff);
	HC573(6,smg_wei[i]);	
	if(s7_flag)
	{HC573(7,temp_table[i]);}
	else	
	{HC573(7,time_table[i]);}	
	i++;if(i >= 8)i = 0;	
}

void table_convert(void)
{
   	if(s7_flag)		//读温度模式
	{
		temp_table[0] = 0xc6;
		temp_table[1] = smg_duan[temperature % 10];
		temp_table[2] = smg_duan[temperature / 10];
		temp_table[3] = 0xff;
		temp_table[4] = 0xff;
		temp_table[5] = 0xbf;
		temp_table[6] = smg_duan[4];
		temp_table[7] = 0xbf;
	}
	else
	{		
		time_table[0] = smg_duan[sec % 10];
		time_table[1] = smg_duan[sec / 10 % 10];
		time_table[2] = smg_duan[sec / 100 % 10];	
		time_table[3] = smg_duan[sec / 1000];
		time_table[4] = 0xff;
		time_table[5] = 0xbf;
		time_table[6] = smg_duan[work_mode];	
		time_table[7] = 0xbf;
	}
}

void HC573(u8 n,u8 value)
{
	switch(n)
	{
		case 4: P0 = value; P2 = (P2 & 0x1f) | 0x80;  P2 &= 0x1f; break;
		case 5: P0 = value; P2 = (P2 & 0x1f) | 0xa0;  P2 &= 0x1f; break;
		case 6: P0 = value; P2 = (P2 & 0x1f) | 0xc0;  P2 &= 0x1f; break;
		case 7: P0 = value; P2 = (P2 & 0x1f) | 0xe0;  P2 &= 0x1f; break;
	}
}

void Timer0Init(void)		//1ms@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x50;		//设置定时初始值
	TH0 = 0xFB;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void Delay1ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 12;
	j = 169;
	do
	{
		while (--j);
	} while (--i);
}

void Delayms(u16 ms)
{
	while(ms--)
	{
		Delay1ms();
	}
}
