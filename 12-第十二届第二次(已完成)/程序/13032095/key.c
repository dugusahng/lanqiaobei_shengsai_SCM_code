#include"key.h"

extern u8 trg,cont,work_mode,channel;
extern u16 frency,dac1,dac3,frency_temp,dac_temp_3;
extern bit s7_flag,cont_flag,led_flag;

void Keyscanf()
{
	u8 readdata = P3 ^ 0xff;
	trg = readdata & (readdata ^ cont);
	cont = readdata;
}

void Keyproc()
{
	Keyscanf();
	if(trg & 0x01)		   //����Ƶ��
	{
		frency_temp = frency;
	}

	if(cont & 0x01)			 
	{
		s7_flag = 1;		 //��ʼ����
	}
	else
	{
		s7_flag = 0;	    //�ɿ�����
	}

	if(trg & 0x02)			 //����ͨ��3��ѹ
	{
	 	dac_temp_3 = dac3;
		
	}

	if(trg & 0x04)			//�л�ͨ��
	{		

		if(channel == 1)channel = 3;
		else if(channel == 3)channel = 1;
	}

	if(trg & 0x08)//s4 �����ģʽ�л�
	{
	 	work_mode++;if(work_mode >= 3)work_mode = 0;
		if(work_mode == 2)channel = 1;		//���������ڽ���ʱ��ͨ����Ϊ1
	}

}
