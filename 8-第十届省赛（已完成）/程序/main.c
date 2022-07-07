#include"reg52.h"
#include"key.h"
#include"iic.h"
#define u8 unsigned char
#define u16 unsigned int
sfr AUXR = 0x8e;

u8 code smg_du[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
u8 code smg_we[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8 table_v[8] = {0xc0,0xc0,0xc0,0xff,0xff,0xff,0xff,0xc1};
u8 table_f[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x8e};

u8 count_key,count_dac,led;
u16 frequency,count_t,count_f,dac_rb2,dac,v3;
bit key_flag,dac_flag;
extern u8 trg = 0,cont = 0;
extern bit smg_flag = 0,led_flag = 0,fre_flag = 0,dac_mode = 0;

void Timer0Init(void);
void Timer1Init(void);
void table_convert();
void LEDproc();

void main()
{
	P0 = 0x00; P2 = (P2 & 0x1f) | 0xa0; P2 &= 0x1f;
	P0 = 0xff; P2 = (P2 & 0x1f) | 0x80; P2 &= 0x1f;
	Timer0Init();
	Timer1Init();
	write_8591(102);		//2.0V	 0-255		 102/2=51  51*5= 255
	while(1)
	{
	   	if(dac_flag)
		{		
		 	v3 = Read_8591(3); 		  //Vrb2��ֵ��0-255
		 	dac_rb2 = v3 * 100 / 51;	 //dac_rb2��ֵ��0-500
		
		 	if(dac_mode)
			{
				dac = dac_rb2;
				write_8591(v3);
			}				
		}
			if(dac_mode == 0)
			{
				dac = 200;
				write_8591(102);
			}

		EA = 0;
	 	LEDproc();
		EA = 1;
		if(key_flag){key_flag = 0; Keyproc();}
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
	count_key++;count_dac++;count_t++;

	if(count_dac >= 250){count_dac = 0;dac_flag = 1;}	  //0.5s

	if(count_t >= 500)		   //1s
	{
		count_t = 0;			
		frequency = count_f;		//�� ����ֵ �� �� Ƶ��
		count_f = 0;				//����ֵ����
	}	 

	if(count_key >= 6){count_key = 0;key_flag = 1;}			   //12ms

	display();
}

void LEDproc()	 //0x01,0x02,0x04,0x08
{										
	static u8 x1,x2,x3,x4,x5;			
	if(led_flag)				  
	{										
		if(fre_flag)
		{
			x1 = 0x00;x2 = 0x02;
		}
		else	
		{
			x1 = 0x01;x2 = 0x00;
		}
	
		if(dac_rb2 < 150)
		{
			x3 = 0x00;
		}
		if((dac_rb2 >= 150)&&(dac_rb2 < 250))
		{
			x3 = 0x04;
		}

		if((dac_rb2 >= 250)&&(dac_rb2 < 350))
		{
			x3 = 0x00;
		}
		if(dac_rb2 >= 350)
		{
			x3 = 0x04;
		}
	
		if(frequency < 1000)		
		{						
			x4 = 0x00;
		}
		if((frequency >= 1000)&&(frequency < 5000))
		{	
			x4 = 0x08;
		}
		if((frequency >= 5000)&&(frequency < 10000))
		{	
			x4 = 0x00;	
		}
		if(frequency >= 10000)
		{	
			x4 = 0x08;
		}
	
		if(dac_mode == 0)
		{
			x5 = 0x00;
		}
		if(dac_mode == 1)
		{
			x5 = 0x10;
		}
		P0 = ~(x1 | x2 | x3 | x4 | x5);P2 = (P2 & 0x1f) | 0x80; P2 &= 0x1f;		
	}
	else
	{
		P0 = 0xff;P2 = (P2 & 0x1f) | 0x80; P2 &= 0x1f;
	} 
}

void display()
{
	static u8 i;
	P0 = 0xff; P2 = (P2 & 0x1f) | 0xe0; P2 &= 0x1f;  
	P0 = smg_we[i]; P2 = (P2 & 0x1f) | 0xc0; P2 &= 0x1f;
	if(smg_flag)
	{		
		if(fre_flag)	//f��Ƶ��
		{	
			P0 = table_f[i]; P2 = (P2 & 0x1f) | 0xe0; P2 &= 0x1f;
		}
		else		  //v�ǵ�ѹ
		{
			P0 = table_v[i]; P2 = (P2 & 0x1f) | 0xe0; P2 &= 0x1f;
		}
	}
	else
	{
		P0 = 0xff; P2 = (P2 & 0x1f) | 0xe0; P2 &= 0x1f;
	}
	i++;if(i >= 8)i = 0;
}

void table_convert()
{
	if(smg_flag)
	{										 //����û�зֳɸ�3λ�͵�4λ	
		if(fre_flag)						//���������ηֳ�2���� ��ʱ
													//��û�ֿ���Ҫ�ֿ��Ļ���
		{											//����˵num�Ƶ�4λcount�Ƹ�3λ
													//num++;											
			table_f[0] = smg_du[frequency % 10];	//if(num >= 9999��{count ++;num = 0 ;}��������
			if(frequency < 10)
			{
				table_f[1]  = 0xff;	
			}
			else 
			{
				table_f[1] = smg_du[frequency / 10 % 10];
			}

			if(frequency < 100)
			{
				table_f[2] = 0xff;					
			}
			else
			{
				table_f[2] = smg_du[frequency / 100 % 10];				
			}

			if(frequency < 1000)
			{			
				table_f[3] = 0xff;	
			}
			else
			{			
				table_f[3] = smg_du[frequency / 1000 % 10];				
			}

			if(frequency < 10000)
			{			
				table_f[4] = 0xff;
			}
			else
			{
				table_f[4] = smg_du[frequency / 10000 % 10];
			}

			if(frequency <= 100000)
			{			
				table_f[5] = 0xff;	
			}
			else
			{
				table_f[5] = smg_du[frequency / 100000 % 10];
			}		
		}

		else
		{
			table_v[0] = smg_du[dac % 10];
			table_v[1] = smg_du[dac / 10 % 10];
			table_v[2] = smg_du[dac / 100 % 10] & 0x7f;	
		}
	}
}

void Timer0Init(void) //����������ʱ����ʼ��û�кϲ�,�ϲ����AUXR��TMODӦ�Ǻ�ֵ������
{
	AUXR |= 0x80;						  //�ҿ�ѧ���Ǹ����򣬶�ʱ���ϲ�AUXR���ù���Ϊ�� |=	��		
	TMOD |= 0x04;						  //�Ҳ�֪��ΪɶTMOD |= 0x04;�Ҵ�STC�ϸ��Ƶ���TMOD |= 0x02;
	TH0 = 0xff;							  //���ұ������ 0x04����Ҫ��TOMD = 0xF0;��ɾ�˲�ɾ������
	TL0 = 0xff;	
	TR0 = 1;
	ET0 = 1;
	EA = 1;		
}

void Timer1Init(void)		//2����@12.000MHz
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
