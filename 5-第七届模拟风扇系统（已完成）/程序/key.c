#include"reg52.h"
#define u8 unsigned char
#define u16 unsigned int

u8 trg,cont;
extern bit s7_flag;
extern  u8 work_mode,count_sec,pwm_duty;
extern u16 sec;

void HC573(u8 n,u8 value);
void Keyscanf()
{
	u8 ReadData = P3 ^ 0xff;
	trg = ReadData & (ReadData ^ cont);
	cont = ReadData;
}

void Keyproc()
{
	Keyscanf();
	if(trg & 0x01) //s7
	{
		s7_flag = ~s7_flag;	
	}	
	if(trg & 0x02)	  //s6
	{
		sec = 0	;
	}
	if(trg & 0x04)	  //s5
	{
		if(sec == 0)
	 	{
	   	 sec = 60;
	 	}
	 	else if(sec <= 60)
	 	{
	 	   sec = 120;
	 	}
	 	else if(sec <= 120)
		{
	   		sec = 0;
	 	} 
	}
	if(trg & 0x08)	 //s4
	{		
		work_mode++;
		if(work_mode >= 4)work_mode = 1;				
	}
}
