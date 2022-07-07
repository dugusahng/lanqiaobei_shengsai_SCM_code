#ifndef __KEY_H
#define __KEY_H

#include "reg52.h"
sfr P4 = 0xc0;
#define u8 unsigned char
#define u16 unsigned int
  
#define setkbd(x) P4 = ((x & 0x80) >> 3) | ((x & 0x40) >> 4); P3 = x
#define getkbd ((P4 & 0x10) << 3) | ((P4 & 0x04) << 4) | (P3 & 0x3f)

void Keyscanf();
void Keyproc();

#endif
