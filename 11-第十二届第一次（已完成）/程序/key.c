#include"key.h"

extern u8 trg,cont,table_mode,temp_par_1;
extern char temp_par;
extern bit dac_mode;

void Keyscanf()
{
	u8 readdata;
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
	if(trg == 0x88)	 //s4
	{
		table_mode++;if(table_mode >= 3)table_mode = 0;
		if(table_mode == 2)temp_par_1 = temp_par;		
	}
	if(trg == 0x48)	  //s8
	{
		if(table_mode == 1)
		{
			temp_par--;if(temp_par <= 0)temp_par = 0;
		}
	}
	if(trg == 0x44)	 //s9
	{
		if(table_mode == 1)
		{
			temp_par++;if(temp_par >= 99)temp_par = 99;
		}
	}
	if(trg == 0x84)	//s5
	{
		dac_mode = ~dac_mode;
	}
		
}