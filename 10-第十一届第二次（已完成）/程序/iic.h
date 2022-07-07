#ifndef __IIC_H_
#define __IIC_H_

#include "reg52.h"
#include "intrins.h"
#define u8 unsigned char

sbit SDA = P2^1;
sbit SCL = P2^0;

void write_8591(u8 dac);
u8 read_8591(u8 n);

#endif
