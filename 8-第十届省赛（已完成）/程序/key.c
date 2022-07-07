#include"key.h"

extern u8 trg,cont;
extern bit smg_flag,led_flag,fre_flag,dac_mode;

void Keyscanf()
{
	u8 ReadData = P3 ^ 0xff;
	trg = ReadData & (ReadData ^ cont);
	cont = ReadData;
}

void Keyproc()
{
	Keyscanf();

	if(trg & 0x01)
	{
		smg_flag = ~smg_flag;
	}

	if(trg & 0x02)
	{
		led_flag = ~led_flag;
	}

	if(trg & 0x04)
	{
		dac_mode = ~dac_mode;
	}

	if(trg & 0x08)
	{
		fre_flag = ~fre_flag;
	}
}
