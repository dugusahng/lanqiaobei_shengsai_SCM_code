#include"key.h"

extern u8 trg,cont;
extern u8 face_mode,temp_par;
extern bit work_flag,time_flag;

void Keyscanf()
{
	u8 readdata;		 //setkbd��getkbd����key.h�����ˣ������Ǽ��ͻ�ȡ��ֵ
	setkbd(0xf0);
	readdata = getkbd;
	setkbd(0x0f);	
	readdata = (readdata | getkbd) ^ 0xff;
	trg = readdata & (readdata ^ cont);
	cont = readdata;
}

void Keyproc()
{
	Keyscanf();
	if(trg == 0x28)  //s12	�����л�
	{
		 face_mode++;
		 if(face_mode >= 3)face_mode = 0; 
	}

	if(trg == 0x24)  //s13	����ģʽ�л�
	{
		work_flag = ~work_flag; 
	}

	if(trg == 0x18)	 //s16 �¶Ȳ���++
	{
	 	if(face_mode == 2)temp_par++;
		if(temp_par >= 100)temp_par = 99;
	}

	if(trg == 0x14) //s17 �¶Ȳ���--
	{
		if(face_mode == 2)temp_par--;
		if(temp_par <= 9)temp_par = 10;
	}

	if(cont == 0x14)	 //�л�ʱ��
	{
		if(face_mode == 1)time_flag = 1;
	}
	else
	{
	 	if(face_mode == 1)time_flag = 0;
	}
} 
