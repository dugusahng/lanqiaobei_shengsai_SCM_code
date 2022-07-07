#include"key.h"

extern u8 trg,cont,count,time,n,t,mode;
extern bit L1_flag;
extern u16 count_time;

void Keyscanf()
{
	u8 readdata = P3 ^ 0xff;
	trg = readdata & (cont^readdata);
	cont = readdata;
}

void Keyproc()
{
	Keyscanf();
	if(trg & 0x01)	//s7		回到mode1，初始化变量
	{
		if(mode == 3)
		{
			mode = 1;
			t = 0;
			n = 0;
			L1_flag = 0;
		}
	}

	if(trg & 0x02)		//s6
	{
		if(mode == 3)L1_flag = 1;			 //L1灭
	}

	if(trg & 0x04)		//s5
	{	
		if(mode == 1)mode = 2;			//切换模式2					
	}

	if(trg & 0x08)	   //s4					设置温度读取间隔
	{
		if(mode == 1)
		{
			  count++;if(count == 5)count = 1;
			  if(count == 1){count_time = 500;time = 1;}
			  if(count == 2){count_time = 2500;time = 5;}
			  if(count == 3){count_time = 15000;time = 30;}
			  if(count == 4){count_time = 30000;time = 60;}
		}
	}
}
