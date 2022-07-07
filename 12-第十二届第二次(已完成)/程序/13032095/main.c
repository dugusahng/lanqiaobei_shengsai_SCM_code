#include"reg52.h"
#include"key.h"
#include"iic.h"
sfr AUXR = 0x8e;
#define u8 unsigned char
#define u16 unsigned int

u8 code smg_du[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
u8 code smg_we[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8 table_f[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x8e};//F 0x8E
u8 table_t[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc8};//N 0xC8
u8 table_v[8] = {0xc0,0xc0,0xc0,0xff,0xff,0xc0,0xbf,0xc1};//U 0xC1

u8	count_key;
u16 count_f,count_t,count_s7,cycle;
bit	key_flag,flag;
extern u8 trg = 0,cont = 0,work_mode = 0,channel = 0;
extern u16 frency = 0,dac1 = 0,dac3 = 0,frency_temp = 0,dac_temp_3 = 0;
extern bit s7_flag = 0,cont_flag = 0,led_flag = 0;
float r_dac; 

void Timer0Init(void);
void Timer1Init(void);
void HC573(u8 n,u8 value);
void table_convert();
void LEDproc();
void Delay5ms();

void main()
{
	HC573(5,0x00);
	HC573(4,0xff);
	Timer0Init();
	Timer1Init();
	while(1)
	{
		cycle = 1000000 / frency; //��õ�λΪus������
		if(key_flag){key_flag = 0;Keyproc();}  //��������
		if(flag)
		{
			flag = 0;
			r_dac = read_8591(1);		   //��ͨ��3��ѹ
			dac1 = (u16)r_dac *100 /51.0f;
			Delay5ms();						 //��ʱ5ms
			r_dac = read_8591(3);		  //��ͨ��3��ѹ
			dac3 = (u16)r_dac *100 /51.0f;
		}
		LEDproc();
		table_convert();

	}
}

void Timer0isr() interrupt 1
{
	count_f++;
}

void display();
void Timer1isr() interrupt 3
{
	count_key++;count_t++;
	if(count_key >= 6){count_key = 0;key_flag = 1;}

	if(count_t >= 500){count_t = 0;frency = count_f;count_f = 0;flag = 1;}
	
	if(s7_flag)count_s7++; //����ʱ������ʱ
	else count_s7 = 0;	   //�ɿ�ʱ��ʱ����
	if(count_s7 >= 500){count_s7 = 0;led_flag = ~led_flag;}	//��ʱ1s��������ر�led����
			
	display();
} 

void LEDproc()
{
	u8 led = 0xff;
	
	if(dac3 < dac_temp_3)led &= ~0x01;
	else led |= 0x01;

	if(frency > frency_temp)led &= ~0x02;
	else led |= 0x02;

	if(work_mode == 0)led &= ~0x04;
	else led |= 0x04;

	if(work_mode == 1)led &= ~0x08;
	else led |= 0x08;

	if(work_mode == 2)led &= ~0x10;
	else led |= 0x10;

	if(s7_flag == 0)  //����־λΪ0�ǽ����жϣ��Ӷ�ʵ���ɿ���Ч
	{
		if(led_flag)HC573(4,0xff);	//Ĭ�ϴ򿪣��ϵ��һ��Ϊ�ر�
		else HC573(4,led);
	}
}

void display()
{
	static u8 i;
	HC573(7,0xff);
	HC573(6,smg_we[i]);
	if(work_mode == 0)HC573(7,table_f[i]);
	if(work_mode == 1)HC573(7,table_t[i]);
	if(work_mode == 2)HC573(7,table_v[i]);
	i++;if(i >= 8)i = 0;
}

void table_convert()
{
	if(work_mode == 0)
	{
		table_f[0] = smg_du[frency % 10];

		if(frency < 10)table_f[1] = 0xff;
		else table_f[1] = smg_du[frency /10 % 10];

		if(frency < 100)table_f[2] = 0xff;
		else table_f[2] = smg_du[frency /100 % 10];

		if(frency < 1000)table_f[3] = 0xff;
		else table_f[3] = smg_du[frency /1000 % 10];

		if(frency < 10000)table_f[4] = 0xff;
		else table_f[4] = smg_du[frency /10000 % 10];

		if(frency < 100000)table_f[5] = 0xff;
		else table_f[5] = smg_du[frency /100000 % 10];

		if(frency < 1000000)table_f[6] = 0xff;
		else table_f[6] = smg_du[frency /1000000 % 10];
	}
	if(work_mode == 1)
	{
		table_t[0] = smg_du[cycle % 10];

		if(cycle < 10)table_t[1] = 0xff;
		else table_t[1] = smg_du[cycle /10 % 10];

		if(cycle < 100)table_t[2] = 0xff;
		else table_t[2] = smg_du[cycle /100 % 10];

		if(cycle < 1000)table_t[3] = 0xff;
		else table_t[3] = smg_du[cycle /1000 % 10];

		if(cycle < 10000)table_t[4] = 0xff;
		else table_t[4] = smg_du[cycle /10000 % 10];

		if(cycle < 100000)table_t[5] = 0xff;
		else table_t[5] = smg_du[cycle /100000 % 10];

		if(cycle < 1000000)table_t[6] = 0xff;
		else table_t[6] = smg_du[cycle /1000000 % 10];	
	}
	if(work_mode == 2)
	{
		if(channel == 1)
		{
		table_v[0] = smg_du[dac1 % 10];
		table_v[1] = smg_du[dac1 /10 % 10];
		table_v[2] = smg_du[dac1 /100 % 10] & 0x7f;
		table_v[5] = smg_du[1];
		}
		if(channel == 3)
		{
			table_v[0] = smg_du[dac3 % 10];
			table_v[1] = smg_du[dac3 /10 % 10];
			table_v[2] = smg_du[dac3 /100 % 10] & 0x7f;
			table_v[5] = smg_du[3];
		}
	}
}

void HC573(u8 n,u8 value)
{
	switch(n)
	{
		case 4:  P2 = (P2 & 0x1f) | 0x80; P0 = value; P2 &= 0x1f; break;
		case 5:  P2 = (P2 & 0x1f) | 0xa0; P0 = value; P2 &= 0x1f; break;
		case 6:  P2 = (P2 & 0x1f) | 0xc0; P0 = value; P2 &= 0x1f; break;
		case 7:  P2 = (P2 & 0x1f) | 0xe0; P0 = value; P2 &= 0x1f; break;
	}
}

void Timer0Init(void)		//@12.000MHz��ʱ��0��������
{
	AUXR |= 0x80;	
	TMOD |= 0x04;	
	TL0 = 0xff;		
	TH0 = 0xff;		
	TF0 = 0;	
	TR0 = 1;		
	ET0 = 1;
	EA = 1;
}

void Timer1Init(void)		//2����@12.000MHz  ��ʱ��0������ʱ
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x40;		//���ö�ʱ��ʼֵ
	TH1 = 0xA2;		//���ö�ʱ��ʼֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;
	EA = 1;
}

void Delay5ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 59;
	j = 90;
	do
	{
		while (--j);
	} while (--i);
}
