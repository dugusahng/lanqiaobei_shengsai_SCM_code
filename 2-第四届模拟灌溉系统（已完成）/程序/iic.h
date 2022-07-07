#ifndef _IIC_H
#define _IIC_H
#define u8 unsigned char
#define u16 unsigned int

u8 read_8591_channel(unsigned char channel_n);
void write_byte_at24c02(unsigned char adrr, unsigned char dat);
u8 read_byte_at24c02(unsigned char adrr);
#endif