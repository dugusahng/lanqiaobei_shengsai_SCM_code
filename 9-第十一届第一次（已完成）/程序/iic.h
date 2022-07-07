#ifndef _IIC_H
#define _IIC_H
#define u8 unsigned char

void Write_byte_24c02(u8 adrr,u8 dat);
void Write_8591(u8 dac);
u8 Read_byte_24c02(u8 adrr);
u8 Read_8591(u8 AIN);

#endif