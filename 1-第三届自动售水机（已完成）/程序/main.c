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
float r_dac;	   //�������������ֵ

void Timer0Init(void);
void display(void);
void table_convert(void);
	  	 
void main()
{

	P2 = (P2 & 0x1f) | 0xa0; P0 = 0x00; P2 &= 0x1f;	//�ط��������̵���	
	P2 = (P2 & 0x1f) | 0x80; P0 = 0xff; P2 &= 0x1f;//��led��		
	Timer0Init();
	while(1)
	{
		if(dac_flag)		//�����ڽϰ����������ֻ�����Ƽ���
		{
			dac_flag = 0;
			write_8591(1);				 //???
			r_dac = read_8591(1);		 //������������ֵ
			dac = (u16)(r_dac *100 / 51.0f);	 //�Ҳ���⣬���Ҵ����𺳣�
			if(dac < 125)	//L1��
			{
				P2 = (P2 & 0x1f) | 0x80; P0 = ~0x01; P2 &= 0x1f;	
			}
			if(dac > 125)	//L1��
			{
				P2 = (P2 & 0x1f) | 0x80; P0 = 0xff; P2 &= 0x1f;
			}
		}
		if(s7_flag == 1)   //L10��
		{
			P2 = (P2 & 0x1f) | 0xa0; P0 = 0x10; P2 &= 0x1f;
		}
		if(s7_flag == 0)	//L10��
		{
			P2 = (P2 & 0x1f) | 0xa0; P0 = 0x00; P2 &= 0x1f;
		}
		if(water_flag)				  //ÿ100ms��ˮ����100
		{
			water_flag = 0;
			shuiliang += 100;
		}
	
		if(shuiliang >= 9999)		  //����ˮ������9999ʱ�Զ�ͣˮ
		{
			 P2 = (P2 & 0x1f) | 0xa0; P0 = 0x00; P2 &= 0x1f;
			 s7_flag = 0;s6_flag = 1;
		}
	
		zongjia = shuiliang / 2;	 //��������֮���ǣ�û�õ�����0.5
	
		if(key_flag){key_flag = 0;keyproc();}
		table_convert();
	}
}

void Timer0isr(void) interrupt 1
{
	if(++count_key >= 6)	   //12ms��һ�ζ�������
	{
		count_key = 0;
		key_flag = 1;
	}
	if(++count_dac >= 150)		 //0.3s��һ�ε�ѹ
	{
		count_dac = 0;
		dac_flag = 1;
	}
	if((++count_water >= 50)&&(s7_flag == 1)) //100ms����s7����ʱ��ʼ��ˮ��ʱ
	{
	   count_water = 0;
	   water_flag = 1;	
	}

	display();
}

void display(void)
{
	static u8 i;
	P2 = (P2 & 0x1f) | 0xe0; P0 = 0xff; P2 &= 0x1f;		//����
	P2 = (P2 & 0x1f) | 0xc0; P0 = smg_wei[i]; P2 &= 0x1f;		//λѡ
	if(s6_flag == 0)
	{P2 = (P2 & 0x1f) | 0xe0; P0 = table_1[i]; P2 &= 0x1f;} 	//��ѡ
	if(s6_flag == 1)
	{P2 = (P2 & 0x1f) | 0xe0; P0 = table_1[i]; P2 &= 0x1f;} 	//��ѡ
	i++;if(i >= 8)i = 0;	
}

void table_convert(void)
{

	table_1[4] = smg_duan[feilv % 10]; 
	table_1[5] = smg_duan[feilv / 10 % 10]; 
	table_1[6] = smg_duan[feilv / 100 % 10] & 0x7f;
	table_1[7] = 0xff;

	if(s6_flag == 0)		   //��s6δ����ʱ��ʾ��ˮ��
	{
	table_1[0] = smg_duan[shuiliang % 10]; 
	table_1[1] = smg_duan[shuiliang / 10 % 10]; 
	table_1[2] = smg_duan[shuiliang / 100 % 10] & 0x7f; 
	table_1[3] = smg_duan[shuiliang / 1000 % 10]; 
	}

	if(s6_flag == 1)	   	//��s6����ʱ��ʾ�ܼ�
	{
	table_1[0] = smg_duan[zongjia % 10]; 
	table_1[1] = smg_duan[zongjia / 10 % 10]; 
	table_1[2] = smg_duan[zongjia / 100 % 10] & 0x7f; 
	table_1[3] = smg_duan[zongjia / 1000 % 10]; 
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
