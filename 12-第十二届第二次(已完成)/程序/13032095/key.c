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
	if(trg & 0x01)		   //缓存频率
	{
		frency_temp = frency;
	}

	if(cont & 0x01)			 
	{
		s7_flag = 1;		 //开始长按
	}
	else
	{
		s7_flag = 0;	    //松开长按
	}

	if(trg & 0x02)			 //缓存通道3电压
	{
	 	dac_temp_3 = dac3;
		
	}

	if(trg & 0x04)			//切换通道
	{		

		if(channel == 1)channel = 3;
		else if(channel == 3)channel = 1;
	}

	if(trg & 0x08)//s4 数码管模式切换
	{
	 	work_mode++;if(work_mode >= 3)work_mode = 0;
		if(work_mode == 2)channel = 1;		//当进入周期界面时将通道调为1
	}

}
