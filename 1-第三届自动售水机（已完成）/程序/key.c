#include"key.h"

extern u8 trg,cont;
extern u16 shuiliang;
extern bit s7_flag,s6_flag;

void keyscanf(void)
{
	u8 readdata = P3 ^ 0xff;
	trg  = readdata &(readdata ^ cont);
	cont = readdata;
}

void keyproc(void)
{
	keyscanf();
	if(trg & 0x01)	//S7 控制出水，
	{
		s7_flag = 1;
		shuiliang = 0;
	}
	if(trg & 0x02)	//S6  控制停水
	{
		s7_flag = 0;
		s6_flag = ~s6_flag;
	}
//	if(trg & 0x04)	//S5		没用到s5,s4
//	{
//
//	}
//	if(trg & 0x08)	//S4
//	{
//
//	}
}





