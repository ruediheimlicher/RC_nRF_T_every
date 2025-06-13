//
//
#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <inttypes.h>
#include <U8g2lib.h>


extern uint8_t charh;
extern uint8_t balkenh;
extern uint8_t balkenb;
extern uint8_t balkenvh;
extern uint8_t balkenvb;
extern uint8_t balkenhh;
extern uint8_t balkenhb;

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

void initDisplay();
void oled_fill(uint8_t x,uint8_t y,uint8_t l);
void oled_setInt(uint8_t x,uint8_t y, uint16_t data);
void oled_delete(uint8_t x,uint8_t y,uint8_t l);
void oled_frame(uint8_t x,uint8_t y,uint8_t l);
void oled_vertikalbalken(uint8_t x,uint8_t y, uint8_t b, uint8_t h);
void oled_vertikalbalken_setwert(uint8_t x,uint8_t y, uint8_t b, uint8_t h,uint8_t wert);
void oled_horizontalbalken(uint8_t x,uint8_t y, uint8_t b, uint8_t h);
void oled_horizontalbalken_setwert(uint8_t x,uint8_t y, uint8_t b, uint8_t h,uint8_t wert);



#endif


