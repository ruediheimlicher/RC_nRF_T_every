//
//
#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <inttypes.h>
#include <U8g2lib.h>



extern uint8_t balkenh;
extern uint8_t balkenb;

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

void initDisplay();


#endif


