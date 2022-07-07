#include"reg52.h"
#include"key.h"
#include"ds1302.h"
#include"onewire.h"
sfr AUXR = 0x8e;
#define u8 unsigned char
#define u16 unsigned int

u8 code smg_du[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};	 //����
u8 code smg_we[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};		   //λѡ
u8 table_temp[] = {0xc0,0xc0,0xc0,0xff,0xff,0xff,0xf9,0xc1};	//�¶���ʾ
u8 table_time[] = {0xc0,0xc0,0xbf,0xc0,0xc0,0xff,0xa4,0xc1};	//ʱ����ʾ
u8 table_temp_par[] = {0xc0,0xc0,0xff,0xff,0xff,0xff,0xb0,0xc1};//������ʾ

u8 temperature;
u8 count_key,count_tt;
u16 count_temp,count_t,count_ts;
bit temp_flag,key_flag,relay_flag,L1_flag,L3_flash;
extern u8 trg = 0,cont = 0;
extern u8 face_mode = 0,temp_par = 23;
extern u8 temp_dot = 0;
extern u8 sec = 0x50,min = 0x59,hour = 0x23;
extern bit work_flag = 0,time_flag = 0;

void Timer0Init(void);	  //��������
void HC573(u8 n,u8 value);
void Delay100ms();
void table_convert();
void relayproc();
void ledproc();

void main()
{
	u8 x;
	HC573(5,0x00);	//�ر�led���̵���
	HC573(4,0xff);
	for(x = 0;x <= 10;x++){temperature = rd_temperature();Delay100ms();}  //������ֵ85
	init_time();
	Timer0Init();	
	while(1)
	{			
		if(temp_flag){temp_flag = 0; temperature = rd_temperature();}	//����
		if(key_flag){key_flag = 0;Keyproc();}	//����
		ledproc();			  //led����
		read_ds1302();
		table_convert();	 //��������飬������������һֱ��
	}
}



void display();
void Timer0isr() interrupt 1
{
	count_temp++;count_key++;
	if(count_temp >= 450){count_temp = 0;temp_flag = 1;}	//0.9s
	if(count_key >= 6){count_key = 0;key_flag = 1;}		  //12ms

	if((min == 0)&&(sec == 0))			  //������ʱ����L1�ͼ̵�����־λ
	{
		if(work_flag){relay_flag = 1;} 
		L1_flag = 1;
	}
	
	if(relay_flag)
	{
		count_t++;
		count_tt++;
		if(count_t >= 2500)	 //5s
		{
			count_t = 0;
			relay_flag = 0;
		}
		if(count_tt >= 50)	   //0.1s
		{
		 	count_tt = 0;
			L3_flash = ~L3_flash;	 //ȡ������˸
		}
	}

	if(L1_flag)				  //L1 5s����
	{	 
		count_ts++;
		if(count_ts >= 2500)	//5s
		{
			count_ts = 0;		
			L1_flag = 0;			
		}
	}

	if(work_flag == 1)
	{
		if(relay_flag)
		{P2 = (P2 & 0x1f) | 0xa0;P0 &= 0x10;P2 &= 0x1f;}// �򿪼̵���	
		else 	
		{P2 = (P2 & 0x1f) | 0xa0;P0 &= 0x00;P2 &= 0x1f;}//�رռ̵���	
	}

	if(work_flag == 0)
	{	 
		if(temperature > temp_par)
		{P2 = (P2 & 0x1f) | 0xa0;P0 &= 0x10;P2 &= 0x1f;}//�򿪼̵���
		else 
		{P2 = (P2 & 0x1f) | 0xa0;P0 &= 0x00;P2 &= 0x1f;}//�رռ̵���
	}

	display();	  //�������ʾ����
}

void ledproc()		 //led����
{
	u8 led = 0xff;
	if(L1_flag)led = ~0x01;
	else led |= 0x01; 
	
	if(work_flag)led |= 0x02;
	else led &= ~0x02;
	
	if((relay_flag)&&(L3_flash))led &= ~0x04;
	else led |= 0x04; 
	EA = 0;
	HC573(4,led);
	EA = 1;
}

void display()	   //�������ʾ����
{
	static u8 i;
	HC573(7,0xff);	 //����
	HC573(6,smg_we[i]);	 //λѡ
	if(face_mode == 0)HC573(7,table_temp[i]);
	if(face_mode == 1)HC573(7,table_time[i]);
	if(face_mode == 2)HC573(7,table_temp_par[i]);
	i++; if(i >= 8)i = 0;	
}

void table_convert()	//���������
{
	if(face_mode == 0)
	{
	table_temp[0] = smg_du[temp_dot % 10];
	table_temp[1] = smg_du[temperature % 10] & 0x7f;
	table_temp[2] = smg_du[temperature / 10 % 10];
	}

	if(face_mode == 1)
	{
		if(time_flag)
		{
			table_time[0] = smg_du[sec % 10];
			table_time[1] = smg_du[sec  /10 % 10];
			table_time[3] = smg_du[min % 10];
			table_time[4] = smg_du[min / 10 % 10];
		}
		else
		{
			table_time[0] = smg_du[min % 10];
			table_time[1] = smg_du[min  /10 % 10];
			table_time[3] = smg_du[hour % 10];
			table_time[4] = smg_du[hour / 10 % 10];
		}
	}

	if(face_mode == 2)
	{
		table_temp_par[0] = smg_du[temp_par % 10];
		table_temp_par[1] = smg_du[temp_par / 10];
	}
}

void HC573(u8 n,u8 value)
{
	switch(n)
	{
		case 4:  P0 = value; P2 = (P2 & 0x1f) | 0x80;  P2 &= 0x1f; break;
		case 5:  P0 = value; P2 = (P2 & 0x1f) | 0xa0;  P2 &= 0x1f; break;
		case 6:  P0 = value; P2 = (P2 & 0x1f) | 0xc0;   P2 &= 0x1f; break;
		case 7:  P0 = value; P2 = (P2 & 0x1f) | 0xe0;   P2 &= 0x1f; break;  	
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
