#ifndef __DS1302_H
#define __DS1302_H
#define u8 unsigned char

u8 DEC_BCD(u8 dec);
u8 BCD_DEC(u8 bcd);
void init_time();
void read_ds1302();


#endif
