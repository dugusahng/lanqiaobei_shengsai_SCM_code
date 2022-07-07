#include"reg52.h"
#include"key.h"
#include"iic.h"
#define u8 unsigned char
#define u16 unsigned int
sfr AUXR = 0x8e;

u8 smg_duan[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
u8 smg_wei[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8 table_1[8],table_2[8],table_3[8];

u8 count_key,count_dac,count_table_vol;
u16 count_L1,dac;
bit	key_flag,dac_flag,count_flag,table_vol_flag,L1_flag_1,L1_flag_2,L2_flag,L3_flag;
extern u8 face_mode = 0,inv_key_val = 0;
extern u16 count_val = 0;
extern bit  key_val_flag = 0;
extern int dac_thr_val = 0,count_H = 0,count_L = 0;
																					     
void Timer0Init(void);
void LEDworking(void);
void HC573(u8 n, u8 value);
void table_convert(void);

void main()
{

	HC573(5,0x00);	  //灭蜂鸣器继电器
	HC573(4,0xff);	  //led全灭
	dac_thr_val = Read_byte_24c02(0x02)*10; 
	Timer0Init();		  //定时器初始化
	while(1)
	{	LEDworking();	//实现led功能
		if(dac_flag)
		{
			Write_8591(3);                        //????
			dac = Read_8591(3)* 1.96 + 0.5;		
		}	
		if(key_flag){key_flag = 0;keyproc();}	 //12ms按键

		if(table_vol_flag){table_vol_flag = 0;table_convert();}	  //0.3s界面刷新
	 						   
	}
}

void display(void);
void Timer0isr() interrupt 1
{	 
	if(++count_key == 6)	   //按键12ms
	{
		count_key = 0;
		key_flag = 1;
	}	
	if(++count_dac >= 50)	  //读电压0.1s
	{
		dac_flag = 1;
	}
		  
	if(++count_L1 > 2500)	 //5s
	{		
		count_L1 = 0;
		L1_flag_1 = 1;
	}
	if(++count_table_vol >= 150)	 //界面刷新时间小于等于0.3s
	{
		count_table_vol = 0;
		table_vol_flag = 1;
	}
	display();
}

void LEDworking(void)
{
	if((dac < dac_thr_val)&&(L1_flag_1 == 1))L1_flag_2 = 1;
	else{L1_flag_1 = 0;L1_flag_2 = 0;}
			
	if((count_L % 2) != 0)L2_flag = 1;
	else L2_flag = 0;
	
	if(inv_key_val == 3)key_val_flag = 1;			
	if(key_val_flag == 1)L3_flag = 1;		
	else L3_flag = 0;

	if(face_mode != 1)	//如果在设置dac_thr_val界面接着判断得话 可能回有计数误差
	{
	 	if(dac > dac_thr_val)	  //只有在电压大于阈值时才能判断 计数
			{
				count_flag = 1;	 //可以计数的标志
			}
		  if((count_flag == 1)&&(dac <= dac_thr_val))	 //一定要小于等于满足 只等于满足的话很难检测道等于的情况
				{
					count_flag = 0;		
					count_L++;			 //用两个int变量才能将范围扩大到7位 用long的话 会影响运行速度
					if((count_H >= 999)&&(count_L >= 9999))count_L = 9999;
					if(count_L >= 10000)
					{
						count_L = 0;
						count_H++;
						if(count_H >= 999)count_H = 999;
					}
				}
	  }
	EA = 0;		//关中断 不然LED会出现微量
	if((L1_flag_2 == 1)&&(L2_flag == 0)&&(L3_flag == 0))	 //1亮  23不亮
		HC573(4,~0x01);
	else if((L1_flag_2 == 0)&&(L2_flag == 1)&&(L3_flag == 0))	 //2亮  13不亮
		HC573(4,~0x02);
	else if((L1_flag_2 == 1)&&(L2_flag == 1)&&(L3_flag == 0))	 //12亮  3不亮
		HC573(4,~0x03);
	else if((L1_flag_2 == 0)&&(L2_flag == 0)&&(L3_flag == 1))	 //3亮  12不亮
		HC573(4,~0x04);
	else if((L1_flag_2 == 1)&&(L2_flag == 0)&&(L3_flag == 1))	 //13亮  2不亮
		HC573(4,~0x05);
	else if((L1_flag_2 == 0)&&(L2_flag == 1)&&(L3_flag == 1))	 //23亮  1不亮
		HC573(4,~0x06);
	else if((L1_flag_2 == 1)&&(L2_flag == 1)&&(L3_flag == 1))	 //123都亮
		HC573(4,~0x07);
	else if((L1_flag_2 == 0)&&(L2_flag == 0)&&(L3_flag == 0))	 //123不都亮
		HC573(4,0xff);
	EA = 1; 		
}

void display(void)
{
	static u8 i;
	HC573(7,0xff);
	HC573(6,smg_wei[i]);
	if(face_mode == 0)HC573(7,table_1[i]);	
	if(face_mode == 1)HC573(7,table_2[i]);
	if(face_mode == 2)HC573(7,table_3[i]);
	i++;if(i >= 8)i = 0;		
}

void table_convert(void)
{
	if(face_mode == 0)
	{
		table_1[0] = smg_duan[dac % 10];
		table_1[1] = smg_duan[dac / 10 % 10];
		table_1[2] = smg_duan[dac / 100] & 0x7f;
		table_1[3] = 0xff;
		table_1[4] = 0xff;
		table_1[5] = 0xff;
		table_1[6] = 0xff;
		table_1[7] = 0xc1;		//U
	}
	if(face_mode == 1)
	{
		table_2[0] = smg_duan[dac_thr_val%10];
		table_2[1] = smg_duan[dac_thr_val / 10 % 10];
		table_2[2] = smg_duan[dac_thr_val / 100] & 0x7f;
		table_2[3] = 0xff;
		table_2[4] = 0xff;
		table_2[5] = 0xff;
		table_2[6] = 0xff;
		table_2[7] = 0x8c;	  	//P
	}
	if(face_mode == 2)
	{
		table_3[7] = 0xc8;		 //N
		table_3[0]  = smg_duan[count_L % 10];

		if((count_H <= 0)&&(count_L < 10))
		table_3[1]  = 0xff;	 //按位显示 低位一定要考虑高位Count_H是否有值 不然会出现高位亮的时候 低位为0灭的情况
		else
		table_3[1]  = smg_duan[count_L / 10 % 10];

		if((count_H <= 0)&&(count_L < 100))
		table_3[2]  = 0xff;
		else
		table_3[2]  = smg_duan[count_L / 100 % 10];

		if((count_H <= 0)&&(count_L < 1000))
		table_3[3]  = 0xff;
		else
		table_3[3]  = smg_duan[count_L / 1000 % 10];

		if(count_H <= 0)
		table_3[4]  = 0xff;
		else
		table_3[4]  = smg_duan[count_H % 10];

		if(count_H < 10)
		table_3[5]  = 0xff;
		else
		table_3[5]  = smg_duan[count_H / 10 % 10];

		if(count_H < 100)
		table_3[6]  = 0xff;
		else
		table_3[6]  = smg_duan[count_H / 100 % 10];  	
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