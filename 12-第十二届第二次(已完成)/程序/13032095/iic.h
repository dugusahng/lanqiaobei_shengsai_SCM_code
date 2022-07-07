#ifndef _IIC_H
#define _IIC_H

#include "reg52.h"
#include "intrins.h"

#define u8 unsigned char

sbit SDA = P2^1;
sbit SCL = P2^0;

u8 read_8591(u8 n); 

#endif
