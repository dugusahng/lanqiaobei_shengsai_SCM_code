#include"key.h"

extern u8 trg,cont,set,set_n,l1,count_L1;
extern char sec ,min ,hour ,sec_n ,min_n ,hour_n ;

extern bit mode,L1_flag_1,temp_table;
extern char;

void Keyscanf()
{
	u8 readdata = P3 ^ 0xff;
	trg = readdata & (readdata ^ cont);
	cont = readdata;	
}

void Keyproc()
{
	Keyscanf();
	if(trg & 0x0f)
	{	
		if(L1_flag_1)
		{
		  	L1_flag_1 = 0;
			l1 = 0;
			count_L1 = 0;
			P0 = 0xff; P2 = (P2 & 0x1f) | 0x80;P2 &= 0x1f;
		} 
	  			
		if(trg & 0x01)
		{	
			if(mode == 0)
			{
				set++;
				if(set >= 4){set = 0;init_time();}
			}
			
		}
		if(trg & 0x02)
		{
			mode = 1;
			if(mode == 1)
			{
				set_n++;
				if(set_n >= 4){set_n = 0;mode = 0;}
			
			}
		}
	
		if(trg & 0x04)
		{
			if(mode == 0)
			{
				if(set == 1){hour++;if(hour > 23)hour = 23;}
				if(set == 2){min++;if(min > 59)min = 59;}
				if(set == 3){sec++;if(sec > 59)sec = 59;}					
			}
			if(mode == 1)
			{
				if(set_n == 1){hour_n++;if(hour_n > 23)hour_n = 23;}
				if(set_n == 2){min_n++;if(min_n > 59)min_n = 59;}
				if(set_n == 3){sec_n++;if(sec_n > 59)sec_n = 59;}					
			}
		}
	
		if(trg & 0x08)
		{
			if(mode == 0)
			{
				if(set == 1){hour--;if(hour < 0)hour = 0;}
				if(set == 2){min--;if(min < 0)min = 0;}
				if(set == 3){sec--;if(sec < 0)sec = 0;}					
			}
			if(mode == 1)
			{
				if(set_n == 1){hour_n--;if(hour_n < 0)hour_n = 0;}
				if(set_n == 2){min_n--;if(min_n < 0)min_n = 0;}
				if(set_n == 3){sec_n--;if(sec_n < 0)sec_n = 0;}					
			}	
		}
}
		if(cont & 0x08)
		{
			if(set == 0&&set_n == 0)temp_table = 1;	
		}
		else
		{
			if(set == 0&&set_n == 0)temp_table = 0;
		}	
	
		


}
