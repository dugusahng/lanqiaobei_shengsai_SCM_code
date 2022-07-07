#include "key.h"

u8 trg,cont;
extern u8 face_mode,inv_key_val;
extern u16 	count_H,count_L;	
extern bit key_val_flag;
extern int dac_thr_val;

void keyscanf()			 //setkbd和getkbd宏定义放在了 iic头文件里了	主要是输入和检测功能
{
	u8 readdata;
	setkbd(0xf0);
	readdata = getkbd;
	setkbd(0x0f);
	readdata = (readdata | getkbd) ^ 0xff;
	trg = readdata & (readdata ^ cont);
	cont = readdata;  
}

void keyproc()
{
	keyscanf();
	if(trg == 0x28)	  //s12
	{
		key_val_flag = 0;
		if((inv_key_val == 3)&&(key_val_flag == 0))inv_key_val = 0; 
		face_mode++;
		if(face_mode == 2)Write_byte_24c02(0x02,dac_thr_val/10);	//????	
		if(face_mode == 3)face_mode = 0;		   	
	}

	if(trg == 0x24)	  //s13
	{		
		if(face_mode == 2)
		{
			key_val_flag = 0;
			if((inv_key_val == 3)&&(key_val_flag == 0))inv_key_val = 0;  
			count_H = 0;
			count_L = 0; 
		}
		else
		{
			inv_key_val++;					
		}						
	}
	if(trg == 0x18)	 //s16		+
	{		
		if(face_mode == 1)
		{	
			key_val_flag = 0;
			if((inv_key_val == 3)&&(key_val_flag == 0))inv_key_val = 0; 						  
			dac_thr_val = dac_thr_val + 50;
			if(dac_thr_val >= 501)dac_thr_val = 0;
								
		}
		else
		{
			inv_key_val++;			
		}				
	}	
	
	if(trg == 0x14)	 //s17		-
	{			
		if(face_mode == 1) 
		{
			key_val_flag = 0;
			if((inv_key_val == 3)&&(key_val_flag == 0))inv_key_val = 0; 
			dac_thr_val = dac_thr_val - 50;
			if(dac_thr_val <= -1)dac_thr_val = 500;	
																			
		}
		else
		{
			inv_key_val++;			
		}		
	}
	if(trg == 0x12)inv_key_val++;			//无关操作
	if(trg == 0x11)inv_key_val++;
	if(trg == 0x22)inv_key_val++;
	if(trg == 0x21)inv_key_val++;
	if(trg == 0x41)inv_key_val++;
	if(trg == 0x42)inv_key_val++;
	if(trg == 0x44)inv_key_val++;
	if(trg == 0x48)inv_key_val++;
	if(trg == 0x81)inv_key_val++;
	if(trg == 0x82)inv_key_val++;
	if(trg == 0x84)inv_key_val++;
	if(trg == 0x88)inv_key_val++;
}