#ifndef __DS1302_H
#define __DS1302_H

#include <reg52.h>
#include <intrins.h>
#define u8 unsigned char

sbit SCK = P1^7;		
sbit SDA = P2^3;		
sbit RST = P1^3; 

u8 BCD_DEC(u8 bcd);	  
void init_time();
void read_ds1302();

#endif
