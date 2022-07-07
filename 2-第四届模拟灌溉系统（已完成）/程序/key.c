#include"reg52.h"
#include"iic.h"

u8 trg,cont;
extern bit work_mode,buzz_flag,hum_val_flag,s6_flag,irr_flag;
extern u8 hum;
extern char hum_val;

void Keyscanf()				 //按键扫描
{
	u8 ReadData = P3^0xff;
	trg = ReadData &(ReadData ^ cont);
	cont = ReadData;
}

void Keyproc()			   
{	
	Keyscanf();

	if(trg & 0x01)	     //S7
	{
		work_mode = ~work_mode;	 //改变工作状态	
	}
			 	
	if(trg & 0x02)	       //s6
	{

		if(work_mode == 0)
		{
			s6_flag = ~s6_flag;					 
			hum_val_flag = ~hum_val_flag;	    //使阈值改变的开关

		}
		
		if(work_mode == 1)			 
		{
			buzz_flag = ~buzz_flag;		  //关闭,打开蜂鸣器的标志																		 										                     
		}
				  									 										                   	
	}

	if(trg & 0x04)   //s5
	{
			
	   	irr_flag = 1;			           //开启灌溉,打开继电器
									
		if((hum_val_flag == 1)&&(work_mode == 0))		//湿度阈值增
		{			
			hum_val++;
        	if(hum_val >= 99) hum_val = 99;
			write_byte_at24c02(0x00,hum_val); 
		}
			
	}
		
	if(trg & 0x08)	   //s4
	{	
		
		irr_flag = 0;		          //关闭灌溉，关闭继电器
					
	   	if((hum_val_flag == 1)&&(work_mode == 0))	//湿度阈值减
		{			
			hum_val--;
			if(hum_val <= 0)	hum_val = 0; 
		    write_byte_at24c02(0x00, hum_val);     
		}			
	}	
}	