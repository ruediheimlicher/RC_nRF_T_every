
#include <inttypes.h>
#include "display.h"

uint8_t charh = 0;
uint8_t balkenh = 50;
uint8_t balkenb = 5;
uint8_t balkenvh = 50;
 uint8_t balkenvb = 5;
 uint8_t balkenhh = 5;
 uint8_t balkenhb = 60;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

void initDisplay()
{
  u8g2.begin();
  uint8_t c = 0;
}

 void oled_fill(uint8_t x,uint8_t y,uint8_t l)
{
   //u8g2.setDrawColor(0);
   u8g2.drawBox(x,y-charh,l,charh+4);
   //u8g2.setDrawColor(1);
   u8g2.sendBuffer();
}



void oled_setInt(uint8_t x,uint8_t y, uint16_t data)
{
   u8g2.setCursor(x,y);
   u8g2.print(data);
   u8g2.sendBuffer();

}

void oled_delete(uint8_t x,uint8_t y,uint8_t l)
{
   u8g2.setDrawColor(0);
   u8g2.drawBox(x,y-charh,l,charh+4);
   u8g2.setDrawColor(1);
   //u8g2.sendBuffer();
}

void oled_frame(uint8_t x,uint8_t y,uint8_t l)
{
   //u8g2.setDrawColor(0);
   u8g2.drawFrame(x,y-charh,l,charh+4);
   //u8g2.setDrawColor(1);
   u8g2.sendBuffer();
}
void oled_vertikalbalken(uint8_t x,uint8_t y, uint8_t b, uint8_t h)
{
   u8g2.drawFrame(x,y,b,h);


}

void oled_vertikalbalken_setwert(uint8_t x,uint8_t y, uint8_t b, uint8_t h,uint8_t wert)
{
  //Serial.print(h);
  //Serial.print("\t");
  //Serial.print(wert);
  
  //Serial.print("\n");
  
  
  u8g2.setDrawColor(0);
  u8g2.drawBox(x+1,y+1,b-2,h-2);
  u8g2.setDrawColor(1);
  //u8g2.drawBox(x+7,y+1,b-2,h-2);
  u8g2.drawHLine(x,y+h-wert,b);
  u8g2.drawHLine(x,y+h-wert-1,b);
  u8g2.drawHLine(x,y+h-wert+1,b);

}

void oled_horizontalbalken(uint8_t x,uint8_t y, uint8_t b, uint8_t h)
{
   u8g2.drawFrame(x,y,b,h);
}
void oled_horizontalbalken_setwert(uint8_t x,uint8_t y, uint8_t b, uint8_t h,uint8_t wert)
{
  //Serial.print(b);
  //Serial.print("\t");
  //Serial.print(wert);
  
  //Serial.print("\n");
  

  u8g2.setDrawColor(0);
  u8g2.drawBox(x+1,y+1,b-2,h-2);

 

  u8g2.setDrawColor(1);
  //u8g2.drawBox(x+7,y+1,b-2,h-2);
  //return;
  u8g2.drawVLine(x+b-wert,y,h);
  //u8g2.drawHLine(x+b-wert-1,y,h);
  //u8g2.drawHLine(x+b-wert+1,y,h);

}
