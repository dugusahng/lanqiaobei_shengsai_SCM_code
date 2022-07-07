#include"key.h"
#include "iic.h"

extern u8 trg,cont;
extern char temp_up,temp_down;

void Keyscanf()
{
	u8 readdata = P3 ^ 0xff;
	trg = readdata & (readdata ^ cont);
	cont = readdata;  
}

void Keyproc()
{
	Keyscanf();
	if(trg & 0x01)	   //temp_down -
	{
		temp_down--;if(temp_down <= 0)temp_down = 0;
		write_24c02(0x01,temp_down);	
	}
	if(trg & 0x02)	   //temp_down +
	{
		temp_down++;if(temp_down >= 99)temp_down = 99;
		write_24c02(0x01,temp_down);
	}
	if(trg & 0x04)	  //temp_up -
	{
		temp_up--;if(temp_up <= 0)temp_up = 0;
			write_24c02(0x00,temp_up);
	}
	if(trg & 0x08)	  //temp_up +
	{
		temp_up++;if(temp_up >= 99)temp_up = 99;
		write_24c02(0x00,temp_up);
	}
}
