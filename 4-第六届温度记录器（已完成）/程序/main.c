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
extern u8 sec = 0x50,min = 0x59,hour = 0x23;	 //����ʱ��23��59��50��
extern u16 count_time = 500;
extern bit L1_flag = 0,s6_flag = 0;

void Delay100ms();		   //��������
void Timer0Init(void);
void table_convert();

void main()
{	
	u8 x;
	P2 = (P2 & 0x1f) | 0xa0; P0 = 0x00; P2 &= 0x1f;	  //��������̵���
	P2 = (P2 & 0x1f) | 0x80; P0 = 0xff; P2 &= 0x1f;  //ledȫ��
	for(x = 0;x < 10; x++){temperature = rd_temperature();Delay100ms();} //���¶ȳ�ʼֵ85����
	init_time();			//ʱ���ʼ��
	Timer0Init();			//��ʱ����ʼ��
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
					
			if(L1_flag == 1)	 //����s6ʱ������
			{				
			  	P2 = (P2 & 0x1f) | 0x80; P0 = 0xff; P2 &= 0x1f;			
			}
			else	 //���򣬵���
			{
				P2 = (P2 & 0x1f) | 0x80; P0 = ~0x01; P2 &= 0x1f;
			}
		}

		if(key_flag){key_flag = 0;Keyproc();}	  //ִ�а�������

		table_convert(); 		   //����Ҫ�ŵ���������һֱ��

		read_ds1302();			   //��ʱ��
	}

}

void display();             //��������
void Timer0isr() interrupt 1
{
	count_flash++;count_key++;

	if(mode == 2)
	{
		count_temp++;
		if(count_temp >= count_time)	   //����ʱ ���� �趨����ʱ
		{		
			count_temp = 0;
			temp_flag = 1;
		}
	}
		
	if(count_flash == 500){count_flash = 0;flash_flag =~ flash_flag;} //1s ��˸
			   	 
	if(mode == 3)	  
	{
		if((temp[9] != 0)&&(L1_flag == 1))  //���¶��������ʱ��������s6ʱ��������ʱ
		{
			if(++count_1 == 500)		   //1s
			{				
				n++;if(n >= 10)n = 9;		//�ӵ�9ֹͣ		
				count_1 = 0;
			}
		}
	}
		
	if(count_key == 6){count_key = 0;key_flag = 1;}				//12ms����������
	
	display();				//2ms���������ʾ����
}

void display()				//�������ʾ����
{
	static u8 i;			//����ɾ�̬�������´ε���ֵ����
	P2 = (P2 & 0x1f) | 0xe0; P0 = 0xff; P2 &= 0x1f;	 //���������
	P2 = (P2 & 0x1f) | 0xc0; P0 = smg_wei[i]; P2 &= 0x1f;	//λѡ

	if(mode == 1)					 //mode1��ѡ
	{
		P2 = (P2 & 0x1f) | 0xe0; P0 = table_1[i]; P2 &= 0x1f;
	}
	if(mode == 2)			  //mode2��ѡ
	{
		P2 = (P2 & 0x1f) | 0xe0; P0 = table_2[i]; P2 &= 0x1f;
	}
	if(mode == 3)		   //mode2��ѡ
	{
		P2 = (P2 & 0x1f) | 0xe0; P0 = table_3[i]; P2 &= 0x1f; 
	}
	i++;if(i >= 8)i = 0;		//��ʹi��0-8��ѭ��
}

void table_convert()		   //���������ʾ������д��������
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

void Timer0Init(void)		//2����@12.000MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x40;		//���ö�ʱ��ʼֵ
	TH0 = 0xA2;		//���ö�ʱ��ʼֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
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