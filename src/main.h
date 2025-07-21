//
//
#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <inttypes.h>

struct Signal 
{
   byte throttle;
   byte pitch;
   byte roll;
   byte yaw;
   byte aux1;
   byte aux2;
   
};
#define BATTX  90
#define BATTY  2
#define BATTH  44
#define BATTB  34

// defines for array pos
#define YAW         0
#define PITCH       1
#define ROLL        2
#define THROTTLE    3

#define NUM_SERVOS  4

#define  ANZAHLMODELLE        5
#define  KANALSETTINGBREITE   4
#define  MODELSETTINGBREITE   32 // nur Kanalsettings. Anschliessend MixingSettings
#define  EEPROM_MODELSETTINGBREITE 64 //Kanalsettings und MixingSettings

#define  STATUS_OFFSET     0 
#define  LEVEL_OFFSET      1 //0x20 // 32
#define  EXPO_OFFSET       2 //0x30 // 48
#define  FUNKTION_OFFSET   3 // 96


#define DEVICE_OFFSET      0x70 // 122
#define AUSGANG_OFFSET     0x80 // 128

#define SAVE_STATUS      0
#define SAVE_LEVEL      1
#define SAVE_MIX        2
#define SAVE_EXPO       3
#define SAVE_FUNKTION   4
#define SAVE_DEVICE     5
#define SAVE_AUSGANG    6

// Joystick

#define OLED 1

#define ANZ_SCREEN         3
#define TASTE_OK           1
#define AKTION_OK           2
#define UPDATE_OK           3
#define T5_WAIT              4 // Warten auf 3 Impulse
#define MENU_ON            5

#define HOMESCREEN      0
#define MODELLSCREEN    1
#define LEVELSCREEN     2
#define EXPOSCREEN      3
#define SAVESCREEN      7

#define JOYSTICKTASTE1 70
#define JOYSTICKTASTE2 121
#define JOYSTICKTASTE3 192
#define JOYSTICKTASTE4 246
#define JOYSTICKTASTE5 308
#define JOYSTICKTASTE6 368
#define JOYSTICKTASTE7 426
#define JOYSTICKTASTE8 510
#define JOYSTICKTASTE9 580

#define TAB0    2
#define TAB1    4
#define TAB3    8


#define TAB4    80
#define TAB5    100

#define OFFSET 4 // Grund-offset auf display

#define cursortab0 2
#define cursortab1 26
#define cursortab2 46
#define cursortab3 54
#define cursortab4 66
#define cursortab5 78
#define cursortab6 90
#define cursortab7 100
//  uint8_t cursortab[10] = {cursortab0,cursortab1,cursortab2,cursortab3,cursortab4,cursortab5,cursortab6,cursortab7,cursortab0,cursortab0};

#define itemtab0  10
#define itemtab1  34
#define itemtab2  50
#define itemtab3  62
#define itemtab4  74
#define itemtab5  88
#define itemtab6  110
#define itemtab7  118

//uint8_t itemtab[10] = {itemtab0,itemtab1,itemtab2,itemtab3,itemtab4,itemtab5,itemtab6,itemtab7,itemtab0,itemtab0};

extern  uint16_t      motorsekunde;

extern  uint8_t       curr_levelarray[8];
extern  uint8_t       curr_expoarray[8];
extern  uint8_t       curr_mixarray[8];
extern  uint8_t       curr_funktionarray[8];
extern  uint8_t       curr_statusarray[8];
extern  uint8_t       curr_ausgangarray[8];

extern  uint8_t       curr_devicearray[8];

extern  int8_t        curr_trimmungarray[8];

extern  uint8_t       curr_mixstatusarray[8];
extern  uint8_t       curr_mixkanalarray[8];

uint8_t       curr_screen;
  uint8_t       curr_page; // aktuelle page
  uint8_t       curr_col; // aktuelle colonne
  uint8_t       curr_model; // aktuelles modell

  uint8_t       curr_kanal; // aktueller kanal

  uint8_t       curr_richtung; // aktuelle richtung
  uint8_t       curr_impuls; // aktueller impuls

  uint8_t       eepromsavestatus;
  uint8_t       programmstatus;
  uint8_t       servostatus;

  uint8_t       curr_setting;
  uint8_t       curr_cursorzeile; // aktuelle zeile des cursors
  uint8_t       curr_cursorspalte; // aktuelle colonne des cursors

 uint8_t       last_cursorzeile; // letzte zeile des cursors
 uint8_t       last_cursorspalte; // letzte colonne des cursors
 uint16_t      blink_cursorpos;

  uint8_t        curr_trimmkanal; // aktueller  Kanal fuerTrimmung
  uint8_t        curr_trimmung; // aktuelle  Trimmung fuer Trimmkanal
  int8_t         vertikaltrimm;
  int8_t         horizontaltrimm;


//extern  uint16_t      motorsekunde;
extern  uint8_t       motorstunde;
extern  uint16_t      motorminute;
extern  uint16_t      motorsekunde;
extern  uint16_t      stopsekunde;
extern  uint16_t      motorminute;
extern  uint16_t      stopminute;
extern  uint16_t      batteriespannung;

extern  uint16_t sendesekunde;
extern  uint16_t sendeminute;
extern  uint8_t sendestunde;

extern  uint16_t      tastentransfer;

extern  uint16_t  posregister[8][8]; // Aktueller screen: werte fuer page und daraufliegende col fuer Menueintraege (hex). geladen aus progmem

extern  uint16_t  cursorpos[8][8]; // Aktueller screen: werte fuer page und daraufliegende col fuer cursor (hex). geladen aus progmem

// 
extern  uint16_t              updatecounter; // Zaehler fuer Einschalten


#endif


