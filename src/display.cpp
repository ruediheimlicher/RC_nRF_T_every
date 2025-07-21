
#include <inttypes.h>
#include "main.h"
#include "display.h"

extern uint16_t currentexpoarray[5][513];

extern  uint8_t                 curr_model; // aktuelles modell
extern  uint8_t                 speichermodel;
extern  uint8_t                 curr_kanal; // aktueller kanal
extern  uint8_t                 curr_setting; // aktuelles Setting fuer Modell
extern  uint8_t                 curr_screen; // aktueller screen
extern  uint8_t                 last_screen; // letzter screen

extern  uint8_t                 curr_page; // aktuelle page
extern  uint8_t                 curr_col; // aktuelle colonne

extern  uint8_t                 curr_cursorzeile; // aktuelle zeile des cursors
extern  uint8_t                 curr_cursorspalte; // aktuelle colonne des cursors
extern  uint8_t                 last_cursorzeile; // letzte zeile des cursors
extern  uint8_t                 last_cursorspalte; // letzte colonne des cursors

extern float UBatt;
extern uint16_t batterieanzeige;
extern Signal data;


uint8_t charh = 0;
uint8_t balkenh = 50;
uint8_t balkenb = 5;
uint8_t balkenvh = 40;
 uint8_t balkenvb = 5;
 uint8_t balkenhh = 3;
 uint8_t balkenhb = 40;

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

void oled_batteriebalken_setwert(uint8_t x,uint8_t y, uint8_t b, uint8_t h,uint16_t wert)
{
 uint8_t anzeige = map(wert-30,0,12,0,h); // Bereich 3-4.2V, 1.2V
 uint8_t min = map(2,0,12,0,h);
  u8g2.setDrawColor(0);
  u8g2.drawBox(x+1,y+1,b-2,h-2);
  u8g2.setDrawColor(1);
  //u8g2.drawBox(x+7,y+1,b-2,h-2);
  //u8g2.drawHLine(x,y+h-wert,b);
  //u8g2.drawHLine(x,y+h-wert-1,b);
  //u8g2.drawHLine(x,y+h-wert+1,b);
  u8g2.drawBox(x+1,y+h-anzeige,b-2,anzeige);
  u8g2.setDrawColor(0);
  u8g2.drawHLine(x,y+h-min,b);
  u8g2.drawHLine(x,y+h-min-1,b);
  u8g2.setDrawColor(1);

       // Batt
      //sprintf(buf1, "%1.1f", UBatt);



}

void oled_setBatterieWert(uint8_t x,uint8_t y, uint8_t b, uint8_t h,float wert)
{
      u8g2.setCursor(x,y);
      u8g2.setDrawColor(0);
      u8g2.print(wert,1);
      u8g2.setDrawColor(1);
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




void setHomeScreen()
{
   u8g2.clear();
   u8g2.setFont(u8g2_font_t0_15_mr);  
   u8g2.setCursor(TAB0, 14);
   u8g2.print(F("nRF24 T"));

    oled_vertikalbalken(BATTX,BATTY,BATTB,BATTH);


}

void updateHomeScreen()
{

   char buf0[4];

      // Yaw
      //u8g2.setCursor(4,30);
      //u8g2.print(data.yaw);
      sprintf(buf0, "%3d", data.yaw);
      u8g2.drawStr(TAB0,30,buf0);
      
      // Pitch
     // u8g2.setCursor(36,30);
     // u8g2.print(data.pitch);
      sprintf(buf0, "%3d", data.pitch);
      u8g2.drawStr(32,30,buf0);

      // Roll
      //u8g2.setCursor(4,46);
      //u8g2.print(data.roll);
      sprintf(buf0, "%3d", data.roll);
      u8g2.drawStr(TAB0,42,buf0);
      
      // Throttle
      //u8g2.setCursor(36,46);
      //u8g2.print(data.throttle);
      sprintf(buf0, "%3d", data.throttle);
      u8g2.drawStr(32,42,buf0);
     
   uint8_t p = curr_model;
   oled_batteriebalken_setwert(BATTX,BATTY,BATTB,BATTH,batterieanzeige);
   oled_setBatterieWert(BATTX,BATTY+BATTH+16,BATTB,24,UBatt);
}

