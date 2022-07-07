#ifndef _IIC_H
#define _IIC_H

//void write_8591(unsigned char dac);
unsigned char read_8591_channel(unsigned char channel_n);
void write_byte_at24c02(unsigned char adrr, unsigned char dat);
unsigned char read_byte_at24c02(unsigned char adrr);

#endif