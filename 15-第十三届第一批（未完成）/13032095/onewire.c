#include "onewire.h"

extern u8 temp_dot;

//�������ڲ���ʱ����
void Delay_OneWire(unsigned int t)  
{
	u8 i;
	while(t--)for(i = 0;i <= 12;i++);
}

//������д����
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//�����߶�����
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	return dat;
}

//DS18B20��ʼ��
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1;
  	Delay_OneWire(10); 
    initflag = DQ;     
  	Delay_OneWire(5);
  
  	return initflag;
}

unsigned char rd_temperature(void)
{
	u8 low,high,temp;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
    Delay_OneWire(200);

	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
    Delay_OneWire(200);

	low = Read_DS18B20();
	high = Read_DS18B20();
	temp_dot = low & 0x0f;
	low >>= 4;
	high <<= 4;
	temp = (low | high);

	if((temp & 0x80) == 0x80)
	{
	 	temp = ~(temp - 1);
		temp_dot = ~(temp_dot - 1) * 0.625;
	}
	else
	{
		temp = temp;
		temp_dot = temp_dot  * 0.625;
	}
	return temp;
}