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

	HC573(5,0x00);	  //��������̵���
	HC573(4,0xff);	  //ledȫ��
	dac_thr_val = Read_byte_24c02(0x02)*10; 
	Timer0Init();		  //��ʱ����ʼ��
	while(1)
	{	LEDworking();	//ʵ��led����
		if(dac_flag)
		{
			Write_8591(3);                        //????
			dac = Read_8591(3)* 1.96 + 0.5;		
		}	
		if(key_flag){key_flag = 0;keyproc();}	 //12ms����

		if(table_vol_flag){table_vol_flag = 0;table_convert();}	  //0.3s����ˢ��
	 						   
	}
}

void display(void);
void Timer0isr() interrupt 1
{	 
	if(++count_key == 6)	   //����12ms
	{
		count_key = 0;
		key_flag = 1;
	}	
	if(++count_dac >= 50)	  //����ѹ0.1s
	{
		dac_flag = 1;
	}
		  
	if(++count_L1 > 2500)	 //5s
	{		
		count_L1 = 0;
		L1_flag_1 = 1;
	}
	if(++count_table_vol >= 150)	 //����ˢ��ʱ��С�ڵ���0.3s
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

	if(face_mode != 1)	//���������dac_thr_val��������жϵû� ���ܻ��м������
	{
	 	if(dac > dac_thr_val)	  //ֻ���ڵ�ѹ������ֵʱ�����ж� ����
			{
				count_flag = 1;	 //���Լ����ı�־
			}
		  if((count_flag == 1)&&(dac <= dac_thr_val))	 //һ��ҪС�ڵ������� ֻ��������Ļ����Ѽ������ڵ����
				{
					count_flag = 0;		
					count_L++;			 //������int�������ܽ���Χ����7λ ��long�Ļ� ��Ӱ�������ٶ�
					if((count_H >= 999)&&(count_L >= 9999))count_L = 9999;
					if(count_L >= 10000)
					{
						count_L = 0;
						count_H++;
						if(count_H >= 999)count_H = 999;
					}
				}
	  }
	EA = 0;		//���ж� ��ȻLED�����΢��
	if((L1_flag_2 == 1)&&(L2_flag == 0)&&(L3_flag == 0))	 //1��  23����
		HC573(4,~0x01);
	else if((L1_flag_2 == 0)&&(L2_flag == 1)&&(L3_flag == 0))	 //2��  13����
		HC573(4,~0x02);
	else if((L1_flag_2 == 1)&&(L2_flag == 1)&&(L3_flag == 0))	 //12��  3����
		HC573(4,~0x03);
	else if((L1_flag_2 == 0)&&(L2_flag == 0)&&(L3_flag == 1))	 //3��  12����
		HC573(4,~0x04);
	else if((L1_flag_2 == 1)&&(L2_flag == 0)&&(L3_flag == 1))	 //13��  2����
		HC573(4,~0x05);
	else if((L1_flag_2 == 0)&&(L2_flag == 1)&&(L3_flag == 1))	 //23��  1����
		HC573(4,~0x06);
	else if((L1_flag_2 == 1)&&(L2_flag == 1)&&(L3_flag == 1))	 //123����
		HC573(4,~0x07);
	else if((L1_flag_2 == 0)&&(L2_flag == 0)&&(L3_flag == 0))	 //123������
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
		table_3[1]  = 0xff;	 //��λ��ʾ ��λһ��Ҫ���Ǹ�λCount_H�Ƿ���ֵ ��Ȼ����ָ�λ����ʱ�� ��λΪ0������
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