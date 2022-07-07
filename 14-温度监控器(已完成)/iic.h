#ifndef _IIC_H
#define _IIC_H

#include "reg52.h"
#include "intrins.h"
#define u8 unsigned char

sbit SDA = P2^1;
sbit SCL = P2^0;

void write_24c02(u8 adrr,u8 dat);
u8 read_24c02(u8 adrr);

#endif
