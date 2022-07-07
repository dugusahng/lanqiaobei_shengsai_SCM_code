#include"reg52.h"
#include"key.h"
#include"iic.h"

u8 trg,cont;
extern u16 time;
extern u8 s6_mode,work_mode;
extern bit s6_flag,open_flag,pwm_buff;
extern u16 time_f[4];

void Keyscanf()
{
	u8 readdata = P3^0xff;
	trg = readdata & (readdata ^ cont);
	cont = readdata; 	
}

void Keyproc()
{
	Keyscanf();

	if(trg & 0x01);	                       	//s7
	{
	  open_flag = ~open_flag;
	}

	if(trg & 0x02)	                    	//s6
	{	
		s6_flag = 1;			
		if(++s6_mode == 3)
		 {
		 	s6_mode = 0;
		 	s6_flag = 0;		
		 }
	}

	if(trg & 0x04)		                    //s5 +
	{
		if(s6_mode == 1)
		{					
			if(++work_mode == 5) work_mode = 4; //只能加到4	 			
		}
		if(s6_mode == 2)
		{
			time_f[work_mode - 1] = time_f[work_mode - 1] + 100;
			if(time_f[work_mode - 1] >= 1200)
		 	time_f[work_mode - 1] = 1200;
			time = time_f[work_mode - 1]; 
			switch(work_mode)
			{
				case 1: write_byte_at24c02(0x00,time / 10);break;					   						
				case 2:	write_byte_at24c02(0x01,time / 10);	break;					  															
				case 3:	write_byte_at24c02(0x02,time / 10);break;										
				case 4:	write_byte_at24c02(0x03,time / 10);break;							
			}
		}		
	}

	if(trg & 0x08)		                   //s4	 -
	{
		if(s6_mode == 1)
		{
		 	if(--work_mode == 0)work_mode = 1;	//只能减到1		 
					if(work_mode == 1)time = time_f[0];
  					else if(work_mode == 2)time = time_f[1];
					else if(work_mode == 3)time = time_f[2];
					else if(work_mode == 4)time = time_f[3];
		}
		if(s6_mode == 2)
			{
				time_f[work_mode - 1] = time_f[work_mode - 1] - 100;
			 	if(time_f[work_mode - 1] <= 400)
		 		time_f[work_mode - 1] = 400;
				time = time_f[work_mode - 1];	
				switch(work_mode)
				{
					case 1: write_byte_at24c02(0x00,time / 10);break;					   						
					case 2:	write_byte_at24c02(0x01,time / 10);	break;					  															
					case 3:	write_byte_at24c02(0x02,time / 10);break;										
					case 4:	write_byte_at24c02(0x03,time / 10);break;
				}							
			}

	}

	if(cont & 0x08)			 //s4长按
	{			
		pwm_buff = 1;		
	}		
	else
	{
		pwm_buff = 0;
	}	
	
}
