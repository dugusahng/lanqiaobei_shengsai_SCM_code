#include"key.h"

extern u8 trg,cont;
extern char temp_up_old,temp_low_old,temp_up_new,temp_low_new;
extern bit table_flag,temp_val_mode,L4_flag;

void Keyscanf()
{
	u8 readdata = P3 ^ 0xff;
	trg = readdata & (readdata ^ cont);
	cont = readdata;
}

void Keyproc()
{
	Keyscanf();
	if(trg & 0x01)
	{
		if(table_flag)
		{
			if(temp_val_mode == 0)
			{				
				temp_up_new--;if(temp_up_new <= 0)temp_up_new = 0;
			}
			if(temp_val_mode == 1)
			{
				temp_low_new--;if(temp_low_new <= 0)temp_low_new = 0;
			}
		}	
	}
	if(trg & 0x02)
	{
		if(table_flag)
		{	
			if(temp_val_mode == 0)
			{				
				temp_up_new++;if(temp_up_new > 99)temp_up_new = 99;
			}
			if(temp_val_mode == 1)
			{				
				temp_low_new++;if(temp_low_new > 99)temp_low_new = 99;
			}
		}
	}
	if(trg & 0x04)
	{
		temp_val_mode = ~temp_val_mode;
		
	}
	if(trg & 0x08)
	{	
		
		if(temp_up_new >= temp_low_new)
		{
			temp_up_old = temp_up_new;
			temp_low_old = temp_low_new;
			table_flag = ~table_flag;
			if(temp_up_new >= temp_low_new)L4_flag = 0;
		}
		else
		{
			temp_up_new = temp_up_old;
			temp_low_new = temp_low_old;			
			table_flag = ~table_flag;
			L4_flag = 1;	
		}
	}
}
