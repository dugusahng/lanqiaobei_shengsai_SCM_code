#include"key.h"

extern u8 trg,cont;
extern int VP1,VP3; 
extern bit VIN_mode,table_flag;

void Keyscnaf()
{
	u8 readdata = P3 ^ 0xff;
	trg = readdata &(readdata ^ cont);
	cont = readdata; 
}

void Keyproc()
{
	Keyscnaf();
	if(trg & 0x01)//s7 -
	{
		if(table_flag)
		{
			if(VIN_mode)
			{
				VP3 -= 20;if(VP3 <= 0)VP3 = 0;
			}
			else
			{
				VP1 -= 20;if(VP1 <= 0)VP1 = 0;
			}
		}
	}
	if(trg & 0x02)//s6 +
	{
		if(table_flag)
		{
			if(VIN_mode)
			{
				VP3 += 20;if(VP3 >= 500)VP3 = 500;
			}
			else
			{
				VP1 += 20;if(VP1 >= 500)VP1 = 500;
			}
		}
	}
	if(trg & 0x04)//s5
	{
		table_flag = ~table_flag;
	}
	if(trg & 0x08)//s4
	{
		VIN_mode = ~VIN_mode;
	}
}

 