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

#define TASTE_OK           1
#define AKTION_OK           2
#define UPDATE_OK           3

#define JOYSTICKTASTE1 189
#define JOYSTICKTASTE2 273
#define JOYSTICKTASTE3 331
#define JOYSTICKTASTE4 391
#define JOYSTICKTASTE5 452
#define JOYSTICKTASTE6 506
#define JOYSTICKTASTE7 576
#define JOYSTICKTASTE8 627
#define JOYSTICKTASTE9 661

#define TAB0    2
#define TAB1    4
#define TAB3    8


#define TAB4    80
#define TAB5    100
#endif


