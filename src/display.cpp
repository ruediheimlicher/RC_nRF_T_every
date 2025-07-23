
#include <inttypes.h>
#include "main.h"
#include "display.h"
#include "text.h"

extern uint16_t currentexpoarray[5][513];

extern volatile uint8_t                 curr_model; // aktuelles modell
extern volatile uint8_t                 speichermodel;
extern volatile uint8_t                 curr_funktion; // aktueller kanal
extern volatile uint8_t                 curr_setting; // aktuelles Setting fuer Modell
extern volatile uint8_t                 curr_screen; // aktueller screen
extern volatile uint8_t                 last_screen; // letzter screen

extern volatile uint8_t                 curr_page; // aktuelle page
extern volatile uint8_t                 curr_col; // aktuelle colonne

extern volatile uint8_t                 curr_cursorzeile; // aktuelle zeile des cursors
extern volatile uint8_t                 curr_cursorspalte; // aktuelle colonne des cursors
extern volatile uint8_t                 last_cursorzeile; // letzte zeile des cursors
extern volatile uint8_t                 last_cursorspalte; // letzte colonne des cursors

extern uint8_t blink_cursorpos;

#define cursortab0 2
#define cursortab1 26
#define cursortab2 46
#define cursortab3 54
#define cursortab4 66
#define cursortab5 78
#define cursortab6 90
#define cursortab7 100
extern  volatile uint8_t cursortab[10] = {cursortab0,cursortab1,cursortab2,cursortab3,cursortab4,cursortab5,cursortab6,cursortab7,cursortab0,cursortab0};
extern volatile uint16_t  cursorpos[8][8]; // Aktueller screen: werte fuer page und daraufliegende col fuer cursor (hex). geladen aus progmem

#define itemtab0  10
#define itemtab1  34
#define itemtab2  50
#define itemtab3  62
#define itemtab4  74
#define itemtab5  88
#define itemtab6  110
#define itemtab7  118

extern volatile uint8_t itemtab[10] = {itemtab0,itemtab1,itemtab2,itemtab3,itemtab4,itemtab5,itemtab6,itemtab7,itemtab0,itemtab0};

extern volatile uint16_t  posregister[8][8]; // Aktueller screen: werte fuer page und daraufliegende col fuer Menueintraege (hex). geladen aus progmem

#define  taby0    16
#define  taby1    24
#define  taby2    32
#define  taby3    40
#define  taby4    48
#define  taby5    56
#define  taby6    56
#define  taby7    56

extern volatile uint8_t taby[8] = {taby0,taby1,taby2,taby3,taby4,taby5,taby6,taby7};

#define menu0  24
#define menu1  40
#define menu2  56

#define menuh 20

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

char menubuffer[20];
char titelbuffer[20];


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

void resetRegister(void)
{
   uint8_t i=0,k=0;
   for(i=0;i<8;i++)
   {
      for (k=0;k<8;k++)
      {
         posregister[i][k]=0xFFFF;
      }
   }
}


void setHomeScreen()
{
   u8g2.clear();
   u8g2.setFont(u8g2_font_t0_15_mr);  
   u8g2.setCursor(TAB0, 14);
   u8g2.print(F("nRF24 T"));

    oled_vertikalbalken(BATTX,BATTY,BATTB,BATTH);
   u8g2.sendBuffer();

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
   //u8g2.sendBuffer();
}

void setMenuScreen()
{
   u8g2.clear();
   resetRegister();
   blink_cursorpos=0xFFFF;
   charh = u8g2.getMaxCharHeight()-1;

   char_x = 30;
   char_y = 45;
   u8g2.setFontDirection(3);
   u8g2.drawStr(char_x,char_y +charh,SettingTable[0]);
   u8g2.setFontDirection(0);
   updateMenuScreen();
   return;
   char_y = 2;
   uint8_t i = 0;
   while (char_y < 64)
   {
      if(i==curr_model)
      {
         //u8g2.drawButtonUTF8(char_x,char_y, U8G2_BTN_INV, 50, 1, 1, ModelTable[i]);
           u8g2.setDrawColor(1);
         u8g2.drawFrame(char_x,char_y,54,18);
         u8g2.setDrawColor(1);
         u8g2.drawStr(char_x,char_y + charh, ModelTable[i]);

      }
      else
      {
         //u8g2.drawButtonUTF8(char_x,char_y, U8G2_BTN_BW0, 50, 1, 1, ModelTable[i]);
           u8g2.setDrawColor(0);
         u8g2.drawFrame(char_x,char_y,54,18);
         u8g2.setDrawColor(1);
         u8g2.drawStr(char_x,char_y + charh, ModelTable[i]);

      }
      //u8g2.drawFrame(char_x,char_y,50,24);
      //u8g2.drawStr(char_x+2,char_y,ModelTable[i]);
      char_y += menuh;
      i++;
   }

}


void updateMenuScreen()
{
   uint8_t z =  curr_cursorzeile;
   
   char_y = 2;
   uint8_t i = 0;
   charh = u8g2.getMaxCharHeight()-1;
   char_x = 48;
   while (char_y < 64)
   {
      if(i==curr_model)
      {
         u8g2.setDrawColor(1);
         u8g2.drawFrame(char_x,char_y,64,18);
         //u8g2.setDrawColor(1);
         //u8g2.drawStr(char_x+2,char_y + charh, ModelTable[i]);
         //u8g2.drawButtonUTF8(char_x,char_y, U8G2_BTN_BW1, 50, 1, 1, ModelTable[i]);

      }
      else
      {
         u8g2.setDrawColor(0);
         u8g2.drawFrame(char_x,char_y,64,18);
         //u8g2.setDrawColor(1);
         //u8g2.drawStr(char_x+2,char_y + charh, ModelTable[i]);
         //8g2.drawButtonUTF8(char_x,char_y, U8G2_BTN_BW0, 50, 1, 1, ModelTable[i]);

      }
      u8g2.setDrawColor(1);
      u8g2.drawStr(char_x+2,char_y + charh-1, ModelTable[i]);
      //u8g2.drawFrame(char_x,char_y,50,24);
      //u8g2.drawStr(char_x+2,char_y,ModelTable[i]);
      char_y += menuh;
      i++;
   }
}

void setModellScreen()
{
   u8g2.clear();
   resetRegister();
   blink_cursorpos=0xFFFF;
   char_x = 30;
   char_y = 45;
   //u8g2.drawFrame(char_y,char_y,64,18);
   u8g2.setDrawColor(1);
   u8g2.setFontDirection(3);
   u8g2.drawStr(char_x,char_y + charh,ModelTable[curr_model]);
   u8g2.setFontDirection(0);
   char_y = taby[3];
   updateModellScreen();
   //u8g2.drawStr(2,char_y,SettingTable[1]);

}

void updateModellScreen(void)
{
   char_y = 4;
   uint8_t i = 0;
   u8g2.setFont(u8g2_font_t0_14_mr);  
   charh = u8g2.getMaxCharHeight()-1;
   char_x = 48;
   while (char_y < 64)
   {
       u8g2.setDrawColor(1);
      u8g2.drawStr(char_x+2,char_y + charh, FunktionTable[i]);
      if(i==curr_funktion)
      {
         u8g2.setDrawColor(1);
         u8g2.drawFrame(char_x,char_y,64,14);
         //u8g2.setDrawColor(1);
         //u8g2.drawStr(char_x+2,char_y + charh, FunktionTable[i]);
         //u8g2.setDrawColor(1);
         //u8g2.drawButtonUTF8(char_x,char_y, U8G2_BTN_BW1, 50, 1, 1, ModelTable[i]);

      }
      else
      {
         u8g2.setDrawColor(0);
         u8g2.drawFrame(char_x,char_y,64,14);
         
         //u8g2.drawStr(char_x+2,char_y + charh, FunktionTable[i]);


      }
      
     
      //u8g2.drawFrame(char_x,char_y,50,24);
      //u8g2.drawStr(char_x+2,char_y,ModelTable[i]);
      char_y += 16;
      i++;
   }
   u8g2.setFont(u8g2_font_t0_15_mr);  

}

void setFunktionScreen()
{

}// setFunktionScreen

void updateFunktionScreen()
{

}// updateFunktionScreen