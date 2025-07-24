// 6 Channel Transmitter | 6 Kanal Verici
// KendinYap Channel

#include "main.h"
#include <SPI.h>
#include <EEPROM.h>
//#include <Adafruit_LiquidCrystal.h>
#include <U8g2lib.h>
//#include <U8x8lib.h>
//#include <Wire.h>
#include "display.h"
#include "expo.h"
#include <nRF24L01.h>
#include <RF24.h>
#include <Bounce2.h> // github.com/thomasfredericks/Bounce2
const uint64_t pipeOut = 0xABCDABCD71LL;         // NOTE: The address in the Transmitter and Receiver code must be the same "0xABCDABCD71LL" | Verici ve Alıcı kodundaki adres aynı olmalıdır

extern "C" 

//U8G2_SSD1327_WS_128X128_HW_I2C u8g2(U8G2_R0,U8X8_PIN_NONE);

// github.com/olikraus/u8g2/discussions/1865
//U8X8_SSD1327_WS_128X128_HW_I2C u8x8(U8X8_PIN_NONE);

// 0.96"
// >> code in display.h

//U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
uint16_t loopcounter0 = 0;
uint16_t loopcounter1 = 0;
//uint8_t charh = 0;
//uint8_t balkenh = 50;
//uint8_t balkenb = 5;
//U8X8_SSD1327_WS_128X128_HW_I2C u8g2(A4,A5);
#define TEST 0
#define CE_PIN 9
#define CSN_PIN 10
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

#define LOOPLED 4

#define BUZZPIN 6

#define EEPROMTASTE  5

#define EEPROM_WRITE 0
#define EEPROM_READ  1

#define EEPROMINDEX_U 0x10
#define EEPROMINDEX_O 0x20
#define EEPROMINDEX_M 0x30

#define EEPROMLEVELSETTINGS  0x32
#define EEPROMEXPOSETTINGS  0x36


#define BLINKRATE 0x00fF

// defines for PINS
// links
#define PITCH_PIN     A6
#define YAW_PIN       A3

// rechts
#define ROLL_PIN      A1
#define THROTTLE_PIN  A0  

#define TASTATUR_PIN A7
#define TASTE_OFF  0
#define TASTE_ON  1


#define BATT_PIN         A2



uint16_t loopcounter = 0;
uint8_t blinkcounter = 0;
uint8_t impulscounter = 0;

// RC_22
//#define POT0LO 620  // Min wert vom ADC Pot 0
//#define POT0HI 3400 // Max wert vom ADC Pot 0

#define POTLO   0
#define POTHI  710

//Impulslaenge, ms
#define PPMLO  850  // Minwert ms fuer Impulslaenge
#define PPMHI  2150 // Maxwert ms fur Impulslaenge

#define MINDIFF 4



uint16_t schritt = 32;



volatile uint16_t          impulstimearray[NUM_SERVOS] = {};
const int                  adcpinarray[NUM_SERVOS] = {A3,A6,A1,A0};    // pins der Pots

uint8_t                    kanalsettingarray[ANZAHLMODELLE][NUM_SERVOS][KANALSETTINGBREITE] = {};

uint16_t                   servomittearray[NUM_SERVOS] = {}; // Werte fuer Mitte

uint8_t levelwert= 0;
uint8_t levelwerta = 0;
uint8_t levelwertb = 0;

uint8_t expowert = 0;
uint8_t expowerta = 0;
uint8_t expowertb = 0;
uint16_t          potwertarray[NUM_SERVOS] = {}; // Werte fuer Mitte
uint16_t          externpotwertarray[NUM_SERVOS] = {}; // Werte von extern  pro servo

uint16_t currentexpoarray[5][513] = {};


float potlo = POTLO; // min pot
float pothi = POTHI; // max pot
float ppmlo = PPMLO; // min ppm
float ppmhi = PPMHI; // max ppm

uint16_t diffa = 0;

uint16_t potwertpitch = 0;
uint16_t diffapitch = 0;
uint16_t diffbpitch = 0;

uint16_t diffb = 0;
float expofloat = 0;
uint16_t expoint = 0;
uint16_t levelint = 0;

uint16_t levelintraw = 0;

uint16_t levelintcheck = 0;

uint16_t expointpitch = 0;
uint16_t levelintpitch = 0;

uint16_t levelintpitcha = 0;

uint16_t levelintpitchb = 0;

uint16_t batteriespannung = 0;
uint16_t batteriearray[8] = {};
uint16_t batteriemittel = 0;
uint8_t batteriemittelwertcounter = 0;
uint16_t batterieanzeige = 0;
float UBatt = 0;
uint8_t eepromstatus = 0;
uint16_t eepromprelltimer = 0;
Bounce2::Button eepromtaste = Bounce2::Button();
uint16_t intdiff = 0;
uint16_t intdiffpitch = 0;

uint16_t tastaturwert = 0;
uint8_t tastencounter = 0;
uint8_t tastaturstatus = 0;
uint8_t Taste = 0;
uint8_t taste5counter = 0;

// balken
#define VBX   64
#define VBY    12
#define HBX    6
#define HBY    54

uint8_t taskarray[4] = {'Y', 'P', 'R', 'T'};

uint16_t potgrenzearray[NUM_SERVOS][2]; // obere und untere Grenze von adc

volatile float quot = (ppmhi - ppmlo)/(pothi - potlo);

volatile float expoquot = (ppmhi - ppmlo)/2/0x200; // umrechnen der max expo (512) auf PPM  

//volatile float quotarray[NUM_SERVOS] = {}; // Umrechnungsfaktor pro Pot



// OLED > in display.cpp
volatile uint16_t pot0 = 0;

uint16_t potwert = 0;

uint16_t errcounter = 0;
uint16_t radiocounter = 0;

volatile uint16_t                posregister[8][8]={}; // Aktueller screen: werte fuer page und daraufliegende col fuer Menueintraege (hex). geladen aus progmem

volatile uint16_t                cursorpos[8][8]={}; // Aktueller screen: werte fuer page und darauf liegende col fuer den cursor

unsigned char char_x = 0;
unsigned char char_y = 0;
uint8_t blink_cursorpos;

// Menu
volatile uint8_t                 curr_model=0; // aktuelles modell
volatile uint8_t                 speichermodel=0;
volatile uint8_t                 curr_funktion=0; // aktuelle funktion
volatile uint8_t                 curr_aktion=0; // aktuelle aktion
volatile uint8_t                 curr_impuls=0; // aktueller impuls

volatile uint8_t                 curr_setting=0; // aktuelles Setting fuer Modell
uint8_t                          speichersetting=0;

volatile uint8_t                 curr_trimmkanal=0; // aktueller  Kanal fuerTrimmung
volatile uint8_t                 curr_trimmung=0; // aktuelle  Trimmung fuer Trimmkanal


volatile uint8_t                 curr_screen = 0; // aktueller screen
volatile uint8_t                 last_screen=0; // letzter screen

volatile uint8_t                 curr_page=7; // aktuelle page
volatile uint8_t                 curr_col=0; // aktuelle colonne

volatile uint8_t                 curr_cursorzeile=0; // aktuelle zeile des cursors
volatile uint8_t                 curr_cursorspalte=0; // aktuelle colonne des cursors
volatile uint8_t                 last_cursorzeile=0; // letzte zeile des cursors
volatile uint8_t                 last_cursorspalte=0; // letzte colonne des cursors

// Tastatur
volatile uint8_t                 Tastenindex=0;
volatile uint16_t                Tastenwert=0;
volatile uint8_t                 adcswitch=0;
volatile uint16_t                lastTastenwert=0;
volatile int16_t                 Tastenwertdiff=0;
volatile uint16_t                tastaturcounter=0;
volatile uint16_t                tastaturdelaycounter=0;






Signal data;
void ResetData() 
{
   data.throttle = 0;                  
   data.pitch = 127;
   data.roll = 127;
   data.yaw = 127;
   data.aux1 = 0;                       
   data.aux2 = 0;
   
}

// initialize the library with the numbers of the interface pins
//Adafruit_LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//Adafruit_LiquidCrystal lcd(0);

void updatemitte(void)
{
   
   
}// updatemitte

void printgrenzen()
{
   Serial.print("\nprintgrenzen\n");  
   for (uint8_t i = 0;i<NUM_SERVOS;i++)
   {
      Serial.print("grenzen i:\t");
      Serial.print(i);
      Serial.print("\t");
      Serial.write(taskarray[i]);
      Serial.print("\t");
      Serial.print("potgrenze HI:\t");
      Serial.print(potgrenzearray[i][0]);
      Serial.print("\t");
      Serial.print("potgrenze LO:\t");
      Serial.print(potgrenzearray[i][1]);
      Serial.print("\t");
      Serial.print("servomitte:\t");
      Serial.print(servomittearray[i]);
      Serial.print("\n");
   }
   Serial.print("end printgrenzen\n");  
}

void printeeprom(uint8_t zeilen)
{
   Serial.print("printeeprom\n");
   for (uint8_t i=0;i<zeilen;i++)
   {
      //Serial.write(taskarray[i]);
      //Serial.print("\t");
      uint8_t f = EEPROM.read(i);
      
      if ((i+1)%8==0 )
      {
         //Serial.print(i);
         //Serial.print(": ");
   
         Serial.print(f);
         Serial.print("\n");
      }
      else
      {
         //Serial.print(i);
         //Serial.print(": ");
   
         Serial.print(f);
         Serial.print("\t");
      }
      
   }
   Serial.print("\n");
   uint8_t eepromyawlo = EEPROM.read(2*(0 + EEPROMINDEX_U));
   uint8_t eepromyawhi = EEPROM.read(2*(0 + EEPROMINDEX_U)+1);
   uint16_t eepromyaw = (eepromyawhi << 8) | eepromyawlo;

   Serial.print("eeprompitch U: \t");
   Serial.print(eepromyawlo);
   Serial.print("\t");
   Serial.print(eepromyawhi);
   Serial.print("\t");
   Serial.print(eepromyaw);
   Serial.print("\n");

   eepromyawlo = EEPROM.read(2*(0 + EEPROMINDEX_O));
   eepromyawhi = EEPROM.read(2*(0 + EEPROMINDEX_O)+1);
   eepromyaw = (eepromyawhi << 8) | eepromyawlo;

   Serial.print("eeprompitch O: \t");
   Serial.print(eepromyawlo);
   Serial.print("\t");
   Serial.print(eepromyawhi);
   Serial.print("\t");
   Serial.print(eepromyaw);
   Serial.print("\n");
}

void eepromread()
{
   Serial.print("eepromread \t");
   for (uint8_t i = 0;i<NUM_SERVOS;i++)
         {
             Serial.write(taskarray[i]);
            Serial.print("\t");
            uint8_t l = (potgrenzearray[i][0] & 0x00FF); // lo byte
            uint8_t h = (potgrenzearray[i][0] & 0xFF00)>>8; // hi byte
            Serial.print("potgrenzearray 0\t");
            Serial.print(potgrenzearray[i][0]);
            Serial.print("\t");
            uint16_t grenzeU = (h << 8) | l;
            Serial.print("grenzeU\t");

            uint8_t el = 0;
            uint8_t  eh = 0;
            el = EEPROM.read(2*(i + EEPROMINDEX_U)); // lo byte
            eh = EEPROM.read(2*(i + EEPROMINDEX_U)+1); // hi byte
            Serial.print(el);
            Serial.print("\t");
            Serial.print(eh);
            Serial.print("\t");

            potgrenzearray[i][1] = (eh << 8) | el;

            el = EEPROM.read(2*(i + EEPROMINDEX_O)); // lo byte
            eh = EEPROM.read(2*(i + EEPROMINDEX_O)+1); // hi byte
            Serial.print(el);
            Serial.print("\t");
            Serial.print(eh);
            Serial.print("\t");

            potgrenzearray[i][0] = (eh << 8) | el;


            
         } // for i
       Serial.print("\n");  
}

void eepromwrite()
{
Serial.print("eepromwrite\n");  
   for (uint8_t i = 0;i<NUM_SERVOS;i++)
   {
      Serial.print("potgrenzearray raw i:\t");
      Serial.print(i);
      Serial.print("\t");
       Serial.write(taskarray[i]);
      Serial.print("\t");
      Serial.print("potgrenze HI:\t");
      Serial.print(potgrenzearray[i][0]);
      Serial.print("\t");
      Serial.print("potgrenze LO:\t");
      Serial.print(potgrenzearray[i][1]);
      Serial.print("\t");
      Serial.print("servomitte:\t");
      Serial.print(servomittearray[i]);
       Serial.print("\t");
       Serial.print("adresse U:\t");
       uint8_t addresseU_LO = 2*(i + EEPROMINDEX_U);
       Serial.print(addresseU_LO);
       Serial.print("\t");
       Serial.print("adresse H:\t");
       uint8_t addresseU_HI = 2*(i + EEPROMINDEX_U)+1;
      Serial.print(addresseU_HI);
      
      
      Serial.print("\n");


      
      EEPROM.update(2*(i + EEPROMINDEX_U),(potgrenzearray[i][1] & 0x00FF)); // lo byte
      EEPROM.update(2*(i + EEPROMINDEX_U)+1,((potgrenzearray[i][1] & 0xFF00) >> 8)); // hi byte

      EEPROM.update(2*(i + EEPROMINDEX_O),(potgrenzearray[i][0] & 0x00FF)); // lo byte
      EEPROM.update(2*(i + EEPROMINDEX_O)+1,((potgrenzearray[i][0] & 0xFF00) >> 8)); // hi byte

      
      
     

      EEPROM.update(2*(i + EEPROMINDEX_M),(servomittearray[i] & 0x00FF)); // lo byte
      EEPROM.update(2*(i + EEPROMINDEX_M)+1,((servomittearray[i] & 0xFF00) >> 8)); // hi byte
      

      for (uint8_t i=0;i<8;i++)
      {
            EEPROM.write(2*(i + EEPROMLEVELSETTINGS)+i,255); // lo byte
            EEPROM.write(2*(i + EEPROMEXPOSETTINGS)+i,255);

      }

      EEPROM.update(2*(i + EEPROMLEVELSETTINGS),(kanalsettingarray[curr_model][i][1] )); // level
      EEPROM.update(2*(i + EEPROMEXPOSETTINGS),(kanalsettingarray[curr_model][i][2] )); // expo

      
      delay(20);
      /*
      Serial.print("kontrolle i: \t*");
      Serial.print(i);
      Serial.print("\t");
      uint8_t el = EEPROM.read(2*(i + EEPROMINDEX_U)); // lo byte
      uint8_t eh = EEPROM.read(2*(i + EEPROMINDEX_U)+1); // hi byte
      Serial.print(el);
      Serial.print("\t");
      Serial.print(eh);
      Serial.print("\t");
      uint16_t grenzeU = (eh << 8) | el;
      Serial.print("grenzeU eeprom:\t");
      Serial.print(grenzeU);
      Serial.print(" *\n");
     */
   }

   Serial.print("eepromwrite end\n");
}
uint8_t Joystick_Tastenwahl(uint16_t Tastaturwert)
{
   //return 0;
   if (Tastaturwert < JOYSTICKTASTE1) 
      return 2;
   if (Tastaturwert < JOYSTICKTASTE2)
      return 1;
   if (Tastaturwert < JOYSTICKTASTE3)
      return 4;
   if (Tastaturwert < JOYSTICKTASTE4)
      return 7;
   if (Tastaturwert < JOYSTICKTASTE5)
      return 8;
   if (Tastaturwert < JOYSTICKTASTE6)
      return 3;
   if (Tastaturwert < JOYSTICKTASTE7)
      return 6;
   if (Tastaturwert < JOYSTICKTASTE8)
      return 9;
   if (Tastaturwert < JOYSTICKTASTE9)
      return 5;
      /*
   if (Tastaturwert < JOYSTICKTASTEL)
      return 10;
   if (Tastaturwert < JOYSTICKTASTE0)
      return 0;
   if (Tastaturwert < JOYSTICKTASTER)
      return 12;
      */
   return 0;
}
 // tastenwahl

void tastenfunktion(uint16_t Tastenwert)
{  
   tastaturcounter++;   
   if (Tastenwert>10) // ca Minimalwert der Matrix
   {      
      //Serial.print(Tastenwert);
      //Serial.print("\t");
      //Serial.print(tastaturcounter);

      //Serial.print("\n");
               
      if (tastaturcounter>=400)   //   Prellen
      {        
         
         tastaturcounter=0x00;
         //Serial.println("Taste down");
         if (!(tastaturstatus & (1<<TASTE_OK))) // Taste noch nicht gedrueckt
         {
           
            //Serial.println(Tastenwert);
            //Taste = 0;
            
            //tastaturstatus |= (1<<TASTE_ON); // nur einmal   
            tastaturstatus |= (1<<TASTE_OK); // nur einmal   
            Taste= Joystick_Tastenwahl(Tastenwert);
            tastaturstatus |= (1<<AKTION_OK);
            if(OLED && Taste) // Taste und Tastenwert anzeigen
            {
               //oled_delete(0,62,20);
               //u8g2.setCursor(0,42);
               //u8g2.print(tastaturwert);
               //u8g2.print("T ");
               //u8g2.setCursor(0,62);
               //u8g2.print(Taste);
               
               //u8g2.sendBuffer(); 

            }


            //;
         }
         else // Taste neu gedrückt
         {
            /*
            Taste = 0;
            //tastaturstatus |= (1<<TASTE_ON); // nur einmal 

            Taste= Joystick_Tastenwahl(Tastenwert);
            tastaturstatus |= (1<<AKTION_OK);
            if(OLED && Taste) // Taste und Tastenwert anzeigen
            {
               oled_delete(0,62,40);
               u8g2.setCursor(0,62);
               //u8g2.print(tastaturwert);
               u8g2.print("T ");
               u8g2.print(Taste);
               
               u8g2.sendBuffer(); 

            }
            */

         }
      }


   }// if tastenwert
   else 
   {
      //if (tastaturstatus & (1<<TASTE_ON))
      {

         //tastaturstatus &= ~(1<<TASTE_OK);
      }
   }






}//tastenfunktion

void setup()
{
   
   uint8_t ee[16];
   delay(50);
   for (uint8_t i=0;i<64;i++)
   {
      //ee[i] = EEPROM.read(i);
      //EEPROM.write(i,0);
      
   }

   delay(50);

   Serial.begin(9600);
   delay(500);
   /*
   for (uint8_t i=0;i<16;i++)
   {
      ee[i] = EEPROM.read(i);
      Serial.print(" i: ");
      Serial.print(i);
      Serial.print(" ee: *");
      Serial.print(ee[i]);
      
   }
   Serial.print("\n");
   */
   Serial.println(__DATE__);
   Serial.println(__TIME__);

   printeeprom(128);

   eepromread();


   pinMode(BUZZPIN,OUTPUT);

   pinMode(LOOPLED,OUTPUT);

   pinMode(BATT_PIN,INPUT);

   pinMode(TASTATUR_PIN,INPUT);


   //pinMode(EEPROMTASTE,INPUT_PULLUP);
   eepromtaste.attach( EEPROMTASTE ,  INPUT_PULLUP ); 
   eepromtaste.interval(5);
   eepromtaste.setPressedState(LOW);


   //digitalWrite(EEPROMTASTE, HIGH);
   
   // https://registry.platformio.org/libraries/adafruit/Adafruit%20LiquidCrystal/installation
   // set up the LCD's number of rows and columns: 
   //lcd.begin(20, 4);
   // Print a message to the LCD.
   //lcd.print("hello, world!");
   
   // OLED
   //u8x8.setBusClock(4000000);
   //u8x8.setI2CAddress(2*0x3D);
   //u8x8.begin();
   //u8g2.setBusClock(4000000);
   
   // 0.96"
   //u8g2.begin(); 
   initDisplay();

   /*
   u8g2.clearDisplay(); 
   //u8g2.setFont(u8g2_font_helvR14_tr); // https://github.com/olikraus/u8g2/wiki/fntlist12
   u8g2.setFont(u8g2_font_t0_15_mr);  
   u8g2.setCursor(4, 14);
   u8g2.print(F("nRF24 T"));
   //u8g2.setFont(u8g2_font_ncenB10_tr);
   u8g2.setFontMode(0);
   oled_vertikalbalken(VBX,VBY,balkenvb,balkenvh);
   
   oled_horizontalbalken(HBX,HBY,balkenhb,balkenhh);
   */
   oled_vertikalbalken(BATTX,BATTY,BATTB,BATTH);
   
   
   setHomeScreen();
   
   
   
   u8g2.sendBuffer(); 
   
   
   //                Configure the NRF24 module  | NRF24 modül konfigürasyonu
   radio.begin();
   radio.openWritingPipe(pipeOut);
   //radio.setChannel(100);
   radio.setChannel(124);
   radio.setAutoAck(false);
   //radio.setDataRate(RF24_250KBPS);    // The lowest data rate value for more stable communication  | Daha kararlı iletişim için en düşük veri hızı.
   radio.setDataRate(RF24_2MBPS); // Set the speed of the transmission to the quickest available
   
   
   radio.setPALevel(RF24_PA_MAX);      // Output power is set for maximum range  |  Çıkış gücü maksimum menzil için ayarlanıyor.
   
   radio.setPALevel(RF24_PA_MIN); 
   radio.setPALevel(RF24_PA_MAX); 
   
   radio.stopListening();              // Start the radio comunication for Transmitter | Verici için sinyal iletişimini başlatır.
   if (radio.failureDetected) 
   {
      radio.failureDetected = false;
      delay(250);
      Serial.println("Radio failure detected, restarting radio");
   }
   else
   {
      Serial.println("Radio OK");
   }
   ResetData();
   
   // RC_22
   for (uint16_t i=0;i<NUM_SERVOS;i++)
   {
      //adcpinarray[i] = 0xFF;
   }
   /*
    adcpinarray[0] = PITCH_PIN;
    adcpinarray[1] = YAW_PIN;
    adcpinarray[2] = ROLL_PIN;
    adcpinarray[3] = THROTTLE_PIN;
    */

   Serial.print("servomitte\n");
   for (uint8_t i=0;i<NUM_SERVOS;i++)
   {
      uint16_t wert = 500 + i * 50;
      wert = 750;
      impulstimearray[i] = wert; // mittelwert
      
      //potgrenzearray[i][0] = potlo;
      //potgrenzearray[i][1] = pothi;
      
      servomittearray[i] = analogRead(adcpinarray[i]);
      Serial.print("i:\t");
      Serial.print(i);
      Serial.print("\t");
      Serial.print(servomittearray[i]);
      Serial.print("\t lo: ");
      Serial.print(servomittearray[i] & 0x00FF);
      Serial.print("\t hi: ");
      Serial.print((servomittearray[i]& 0xFF00) >> 8);
      Serial.print("\n");


      //uint8_t n = i*i+1;
      //EEPROM.write(i,0 );
      
   }

   Serial.print("\n"); 
   for (uint8_t i=0;i<NUM_SERVOS;i++)
   {
      Serial.print(adcpinarray[i]);
      Serial.print("\t");
      Serial.print(servomittearray[i]);
      Serial.print("\t");
      
      kanalsettingarray[0][i][1] = 0x11; // level
      kanalsettingarray[0][i][2] = 0x33; // expo
   }
   
   Serial.print("\n");

   kanalsettingarray[0][YAW][1] = 0x11; // level
   kanalsettingarray[0][YAW][2] = 0x01; // expo

   kanalsettingarray[0][PITCH][1] = 0x23; // level
   kanalsettingarray[0][PITCH][2] = 0x32; // expo


   kanalsettingarray[0][ROLL][1] = 0x33; // level
   kanalsettingarray[0][ROLL][2] = 0x21; // expo

   kanalsettingarray[0][THROTTLE][1] = 0x22; // level
   kanalsettingarray[0][THROTTLE][2] = 0x02; // expo


  


   potwert = servomittearray[0];  

   Serial.print("setup EEPROM\n");
   printeeprom(128);
   

/*
for (uint8_t i = 0;i<NUM_SERVOS;i++)
   {
      EEPROM.update(2*(i + EEPROMINDEX_U),(potgrenzearray[i][1] & 0x00FF)); // lo byte
      EEPROM.update(2*(i + EEPROMINDEX_U)+1,((potgrenzearray[i][1] & 0xFF00) >> 8)); // hi byte
      EEPROM.update(2*(i + EEPROMINDEX_O),(potgrenzearray[i][0] & 0x00FF)); // lo byte
      EEPROM.update(2*(i + EEPROMINDEX_O)+1,((potgrenzearray[i][0] & 0xFF00) >> 8)); // hi byte

      EEPROM.update(2*(i + EEPROMINDEX_M),(servomittearray[i] & 0x00FF)); // lo byte
      EEPROM.update(2*(i + EEPROMINDEX_M)+1,((servomittearray[i] & 0xFF00) >> 8)); // hi byte
 
   }
   //EEPROM.update(127,13);
delay(4);
*/

//printeeprom(128);
   
} // setup

int Throttle_Map(int val, int fromlow, int fromhigh,int tolow, int tohigh, bool reverse)
{
   val = constrain(val, fromlow, fromhigh);
   val = map(val, fromlow,fromhigh, tolow, tohigh);
   return ( reverse ? 255 - val : val );
}

// Joystick center and its borders 
int Border_Map(int val, int lower, int middle, int upper, bool reverse)
{
   val = constrain(val, lower, upper);
   if ( val < middle )
      val = map(val, lower, middle, 0, 128);
   else
      val = map(val, middle, upper, 128, 255);
   return ( reverse ? 255 - val : val );
}

// Joystick center and its borders 
int Border_Map10(int val, int lower, int middle, int upper, bool reverse)
{
   val = constrain(val, lower, upper);
   if ( val < middle )
      val = map(val, lower, middle, 0, 254); // normieren auf 0-254

   else
      val = map(val, middle, upper, 255, 512); // normieren auf 255 - 512
   return ( reverse ? 512 - val : val );
}

int Border_Mapvar512(int val, int lower, int middle, int upper, bool reverse)
{
   val = constrain(val, lower, upper); // Grenzen einhalten

   if ( val < middle )
   {
      val = map(val, lower, middle, 0, 254); // normieren auf 0-512
      intdiff =  (middle - val);// Abweichung von mitte, 
      levelintraw = intdiff;
      diffa = map(intdiff,0,(middle - lower), 0,512);
      expoint = expoarray[expowerta][diffa];
      levelint = expoint * (8-levelwerta);
      levelint /= 8;
      //levelintraw = levelint;
      levelint = map(levelint,0,512,0,(middle - lower));
      levelint = middle - levelint;
      levelintpitcha = levelint;
   }  
   else
   {
      val = map(val, middle, upper, 255, 512); // normieren auf 0 - 512
      intdiff =  (val - middle);// Abweichung von mitte, 
      diffb = map(intdiff,0,(upper - middle),0,512);
      if(diffb >= 512 )
      {
         diffb = 512;
      }
      expoint = expoarray[expowertb][diffb];
      levelint = expoint * (8-levelwertb) ;     
      levelint /= 8;
      levelintraw = levelint;
      levelint = map(levelint,0,512,0,(upper - middle));     
      levelintpitchb = levelint;
   }
      
   return ( reverse ? 512 - levelint : levelint );
}

int Border_Mapvar255(int val, int lower, int middle, int upper, bool reverse)
{
   val = constrain(val, lower, upper); // Grenzen einhalten
   
   if ( val < middle )
   {

      val = map(val, lower, middle, 0, 127); // normieren auf 0-127
      //intdiff = val;
      intdiff =  (127 - val);// Abweichung von mitte, 
      //levelintraw = intdiff;
      //diffa = map(intdiff,0,(middle - lower), 0,512);
      diffa = intdiff;
      expoint = expoarray[expowerta][4*diffa]/4;
      levelint = expoint * (8-levelwerta);
      levelint /= 8;
      levelintcheck = 127 + levelint;
      levelint = 127 + levelint;
   }  
   else
   {
      val = map(val, middle, upper, 128, 255); // normieren auf 128 - 255
      //intdiff = val;
      
     intdiff =  (val - 127);// Abweichung von mitte, 
      //diffb = map(intdiff,0,(upper - middle),0,512);
      diffb = intdiff;
      if(diffb >= 127 )
      {
         diffb = 127;
      }
      expoint = expoarray[expowertb][4*diffb]/4;
      levelint = expoint * (8-levelwertb) ;     
      levelint /= 8;
      levelintcheck= 127 - levelint;
      levelint= 127 - levelint;
   }
      
   return ( reverse ? 255 - levelint : levelint );
}



uint16_t map_uint16(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) 
{
   if (in_max == in_min) return out_min; // prevent division by zero
   return (uint16_t)(((uint32_t)(x - in_min) * (out_max - out_min)) / (in_max - in_min) + out_min);
}

double mapd(double x, double in_min, double in_max, double out_min, double out_max) 
{
   if (in_max == in_min) return out_min;
   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void loop()
{                
  //            
   loopcounter++;
   //digitalWrite(BUZZPIN,!(digitalRead(BUZZPIN)));
      tastaturwert = analogRead(TASTATUR_PIN);
      tastenfunktion(tastaturwert);
      

   if (tastaturstatus & (1<<TASTE_OK) && Taste) // Menu ansteuern
   {
      tastaturcounter = 0;
      switch (Taste)
      {
         case 0: // null-pos, nichts tun
         {

         }break;

         case 1:
         {
            Serial.print("T 1");
           
         }break;

         case 2:
         {
            //Serial.print("T 2");
            if (tastaturstatus & (1<<AKTION_OK))
            {
               Serial.print("T 2 up*");

               tastaturstatus &=  ~(1<<AKTION_OK);
               tastaturstatus |= (1<<UPDATE_OK);
               switch (curr_screen)
               {
                  case 0: // HOMESCREEN
                  {

                  }break;
                  case 1: // MENUSCREEN
                  {
                     if(curr_model)
                     {
                        curr_model--;
                        updateMenuScreen();
                        u8g2.sendBuffer();
                     }
                  }
                  case 2: //MODELLSCREEN
                  {
                      if(curr_funktion)
                     {
                        curr_funktion--;
                        updateModellScreen();
                        u8g2.sendBuffer();
                     }
                  }break;

                  case 3: //FUNKTIONSCREEN
                  {
                     switch (curr_cursorspalte)
                     {
                        case 0:
                        {
                            if(curr_aktion)
                           {
                              curr_aktion--;
                              updateFunktionScreen();
                              u8g2.sendBuffer();
                           }
                        }break;
                        case 1: // Level, expo up, down
                        {
                           Serial.print("curr_aktion: ");
                           Serial.print(curr_aktion) ;


                        }break;
                     }// switch curr_cursorspalte
                     
                  }break;
                  

               }// switch (curr_screen)
            }
           
         }break;

         case 3:
         {
            Serial.print("T 3");
           
         }break;

         case 4:
         {
            //Serial.print("T 4");
            
            if (tastaturstatus & (1<<AKTION_OK))
            {
               Serial.print("T 4 left");
               tastaturstatus &=  ~(1<<AKTION_OK);
               tastaturstatus |= (1<<UPDATE_OK);
               
            }
           
         }break;

         case 5: // Ebene tiefer
         {
            if (tastaturstatus & (1<<AKTION_OK))
            {

               Serial.print("T 5 in ");

               if ((curr_screen == 0) ) //&& (taste5counter < 3))
               {
                  tastaturstatus |= (1<<T5_WAIT); // Warten auf 3 Impulse
                  {
                  
                  taste5counter++;
                  Serial.print("taste5counter: ");
                  Serial.println(taste5counter);
                  tastaturcounter = 300; // Mehrfachklick ermoeglichen
                  if (taste5counter == 3)
                  {
                     curr_screen = 1;
                     taste5counter = 0;
                     tastaturstatus |= ~(1<<T5_WAIT); // Warten beendet
                     Serial.print("T5 setMenuScreen ");
                     //u8g2.clear();
                     setMenuScreen();
                     u8g2.sendBuffer();
                  }
                  //
                  }
               }
               else //if (!(tastaturstatus & (1<<T5_WAIT))) // kein Warten
               {
                  taste5counter = 0;

                  if(curr_screen < 5)
                  {
                     Serial.print("T 5 klick ");
                     Serial.println(curr_screen);
                     switch (curr_screen)
                     {
                        case 1: // MODELLSCREEN
                        {
                           Serial.print("> Modellscreen curr_model: ");
                           Serial.println(curr_model);
                           setModellScreen();
                           curr_screen = 2;
                           u8g2.sendBuffer();
                        }break;

                        case 2: // FUNKTIONSCREEN
                        {
                           Serial.print("> FunktionScreen curr_funktion: " );
                           Serial.println(curr_funktion);
                           setFunktionScreen();
                           curr_screen = 3;
                           u8g2.sendBuffer();
                        }break;

                        case 3: // AKTIONSCREEN
                        {
                           Serial.print("> AktionScreen curr_aktion: " );
                           Serial.println(curr_aktion);
                           setAktionScreen();
                           curr_screen = 4;
                           u8g2.sendBuffer();
                        }break;

                        
                     }// switch (curr_screen)


                  }
               }
               Serial.print("curr_screen: ");
               Serial.println(curr_screen);              
               tastaturstatus &=  ~(1<<AKTION_OK);
               tastaturstatus |= (1<<UPDATE_OK);
               
            }
           
         }break;

         case 6:
         {
            Serial.print("T 6");
            if (tastaturstatus & (1<<AKTION_OK))
            {
               Serial.print("T 6 right");
               tastaturstatus &=  ~(1<<AKTION_OK);
               tastaturstatus |= (1<<UPDATE_OK);
               switch (curr_screen)
               {
                  case 0: // HOMESCREEN
                  {

                  }break;

                  case 1: // MENUSCREEN
                  {

                  }break;
                  case 2: // MODELLSCREEN
                  {

                  }break;
                  case 3: // FUNKTIONSCREEN
                  {
                     
                     switch (curr_cursorspalte)
                     {
                        case 0:
                        {
                           curr_cursorspalte++;

                           updateFunktionScreen();
                           u8g2.sendBuffer();

                           
                        }break;
                        case 1: // up, down enabled
                        {

                        }break;
                     }// switch curr_cursorspalte
                  }break;

               }// swich curr_screen
               
            }
           
         }break;

         case 7:
         {
            
            if (tastaturstatus & (1<<AKTION_OK))
            {
               Serial.print("T 7 back ");
               if(curr_screen )
                  {
                     curr_screen--;
                     u8g2.clear();
                     switch (curr_screen)
                     {
                        case 0: // HOMESCREEN
                        {
                           setHomeScreen();

                        }break;
                        case 1: // MENUSCREEN
                        {
                           setMenuScreen();
                        }break;
                        case 2: // MODELLSCREEN
                        {
                           setModellScreen();
                        }break;
                        case 3: // FUNKTIONSCREEN
                        {
                           setFunktionScreen();
                        }break;
                     }// switch curr_screen
                  }
               Serial.print("T7 curr_screen: ");
               Serial.println(curr_screen);              
               u8g2.sendBuffer();
               tastaturstatus &=  ~(1<<AKTION_OK);
               tastaturstatus |= (1<<UPDATE_OK);
               
            }
           
         }break;

         case 8:
         {
            Serial.print("T 8");
            if (tastaturstatus & (1<<AKTION_OK))
            {
               Serial.print("T 8 down");
               tastaturstatus &=  ~(1<<AKTION_OK);
               tastaturstatus |= (1<<UPDATE_OK);
               switch (curr_screen)
               {
                  case 0: // HOMESCREEN
                  {

                  }break;
                  case 1: // MENUSCREEN Modelle
                  {
                     if(curr_model < 5)
                     {
                        curr_model++;
                        updateMenuScreen();
                        u8g2.sendBuffer();

                     }
                     
                  }break;

                  case 2:  // MODELLSCREEN  Funktionen
                  {
                      if(curr_funktion < 4)
                     {
                        curr_funktion++;
                        updateModellScreen();
                        u8g2.sendBuffer();

                     }
                  }break;

                  case 3:  // FUNKTIONSCREEN 
                  {
                      if(curr_aktion < 5)
                     {
                        curr_aktion++;
                        updateFunktionScreen();
                        u8g2.sendBuffer();

                     }
                  }break;

               }// switch (curr_screen)
            }
           
         }break;

         case 9:
         {
            Serial.print("T 9");
           
         }break;



      }//switch (Taste)
      if(Taste)
      {
         Serial.print("\n");
         Taste = 0;
         tastaturstatus &= ~(1<<TASTE_OK);
      }
      

      
   }// if TASTE_OK

   if(loopcounter >= 2*BLINKRATE)
   {
      if(Taste)
      {
         Serial.print(tastaturwert);
         Serial.print(" Taste: ");
         Serial.print(Taste);
         Serial.print("\n");
         
            
      }
      loopcounter = 0;
      blinkcounter++;
      impulscounter+=16;
      digitalWrite(LOOPLED, ! digitalRead(LOOPLED));

      

      batteriespannung = analogRead(BATT_PIN);
      batteriearray[batteriemittelwertcounter] = batteriespannung;
      batteriemittelwertcounter++;
      batteriemittelwertcounter &= 0x07;
      batteriemittel = 0;
      for(uint8_t i=0;i<8;i++)
      {
         batteriemittel += batteriearray[i];
      }
      batteriemittel /= 8;
      //Serial.println(batteriemittel);

      
      UBatt = float(batteriespannung) / 107;

      

      //eepromread();
      ///*
      //u8g2.clearBuffer();                   // Clear display.
      //u8x8.setFont(u8g2_font_ncenB08_tr);    // choose a suitable font
      //u8x8.drawString(0, 24, "Hello OLED!");    // write something to the buffer
      //u8x8.sendBuffer();   
      //u8g2.sendBuffer(); // Transfer buffer to screen.
      //*/
      /*
       Serial.print("blinkcounter: ");
       Serial.print(blinkcounter);
       
       Serial.print(" radiocounter: ");
       Serial.print(radiocounter);
       Serial.print(" errcounter: ");
       Serial.print(errcounter);
       //Serial.print(" impulscounter: ");
       //Serial.print(impulscounter);
       */
      /*
       potwert += schritt;
       if (potwert >= POTHI)
       {
       potwert = POTLO;
       }
       */
      //Serial.print(" M: ");
      //if(abs(servomittearray[ROLL] - potwertarray[ROLL]) > 2)
      

      // 0.96
      loopcounter1++;
      uint8_t charindex = loopcounter1  & 0x7F;
      //u8g2.setDrawColor(0);
      charh = u8g2.getMaxCharHeight() ;
      //oled_delete(4,44,64);
      
      //u8g2.drawGlyph(32,44,'A'+(charindex));
      char buf0[4];
/*
      // Yaw
      //u8g2.setCursor(4,30);
      //u8g2.print(data.yaw);
      sprintf(buf0, "%3d", data.yaw);
      u8g2.drawStr(4,30,buf0);
      
      // Pitch
     // u8g2.setCursor(36,30);
     // u8g2.print(data.pitch);
      sprintf(buf0, "%3d", data.pitch);
      u8g2.drawStr(32,30,buf0);

      // Roll
      //u8g2.setCursor(4,46);
      //u8g2.print(data.roll);
      sprintf(buf0, "%3d", data.roll);
      u8g2.drawStr(4,42,buf0);
      
      // Throttle
      //u8g2.setCursor(36,46);
      //u8g2.print(data.throttle);
      sprintf(buf0, "%3d", data.throttle);
      u8g2.drawStr(32,42,buf0);
     */
      
      uint8_t wertv = map(data.pitch,0,255,2,balkenvh-2); // Platz fuer 3 pixel dicke
      //oled_vertikalbalken_setwert(VBX,VBY,balkenvb,balkenvh,wertv);
      
      uint8_t werth = map(data.yaw,0,255,2,balkenhb-2); // Platz fuer 3 pixel dicke
      
      //oled_horizontalbalken_setwert(HBX,HBY,balkenhb,balkenhh,werth);
      
      batterieanzeige = (0x50*batteriespannung)/0x6B/8; // resp. /107
      /*
      Serial.print(batteriespannung);
      Serial.print("\t");
      Serial.print(batterieanzeige);
      Serial.print("\t");
      Serial.println(UBatt);
      */
      //oled_batteriebalken_setwert(BATTX,BATTY,BATTB,BATTH,batterieanzeige);
      //oled_setBatterieWert(BATTX,BATTY+BATTH+16,BATTB,24,UBatt);
      
      
      /*
      //char buf1[4];
       // Batt
      //sprintf(buf1, "%1.2f", UBatt);
      u8g2.setCursor(90,62);
      u8g2.setDrawColor(0);
      u8g2.print(UBatt,2);
      u8g2.setDrawColor(1);
      */
     if(curr_screen == 0)
     {
         updateHomeScreen();
         u8g2.sendBuffer();
     }
      
      if(loopcounter1 > 25)
      {
         loopcounter1 = 0;
      }

      

      if (TEST == 1)
      {
         Serial.print("YAW\t "); 
         Serial.print(potgrenzearray[YAW][0]);
         Serial.print("\t ");   
         Serial.print(potgrenzearray[YAW][1]);
         Serial.print("\t* ");  
         Serial.print("pw2: ");
         Serial.print("\t ");
         Serial.print(potwertarray[YAW]);
         Serial.print("\t ");

         // Serial.print("map: ");
         //Serial.print("\t ");
         //uint8_t yawmap = map(potwertarray[YAW],0,680,0,254);
         //Serial.print(yawmap);
         Serial.print(" *255*\t ");
         uint16_t yawmap2 = Border_Map(potwertarray[YAW],potgrenzearray[YAW][1],servomittearray[YAW],potgrenzearray[YAW][0],true);
         Serial.print("\t ");
         Serial.print(yawmap2);
         
         Serial.print(" *512*\t ");
         uint16_t yawmap3 = Border_Map10(potwertarray[YAW],potgrenzearray[YAW][1],servomittearray[YAW],potgrenzearray[YAW][0],true);
         Serial.print("\t ");
         Serial.print(yawmap3);

         Serial.print("\t ");
         Serial.print("pwarray: ");
         Serial.print("\t ");
         Serial.print(potwertarray[YAW]);


       //  int var = Border_Mapvar255(potwertarray[YAW],potgrenzearray[YAW][1],servomittearray[YAW],potgrenzearray[YAW][0],true);
         
         
         Serial.print("\t ");
         Serial.print("intdiff: ");
         Serial.print("\t ");
         Serial.print(intdiff);

         Serial.print("\t ");
         Serial.print("levelintcheck: ");
         Serial.print("\t ");
         Serial.print(levelintcheck);


         Serial.print("\t ");
         Serial.print("data.yaw: ");
         Serial.print("\t ");
         Serial.print(data.yaw);
         Serial.print("\t ");
         Serial.print(batteriespannung);

         Serial.print("\t ");
         
         Serial.print(UBatt,2);



         //Serial.print("\t ");
         //Serial.print("levelintpitcha: ");
         //Serial.print(levelintpitcha);



         /*
         Serial.print("PITCH\t "); 
         Serial.print(potgrenzearray[PITCH][0]);
         Serial.print("\t ");   
         Serial.print(potgrenzearray[PITCH][1]);
         Serial.print("\t* ");   
         
         
         Serial.print(servomittearray[PITCH]);
         Serial.print("\t ");    
         Serial.print(" potwertpitch: ");
         
         Serial.print(potwertpitch);
         
         Serial.print("\t potwertarray: ");
         Serial.print(potwertarray[PITCH]);
         Serial.print("\t");
         
         
         Serial.print("\t");    
         //Serial.print(" levelwerta: ");
         // Serial.print(levelwerta);
         //Serial.print("\t");    //Serial.print(" expoint: ");
         //uint8_t ea = kanalsettingarray[curr_model][PITCH][2];
         //Serial.print(ea);
         
         
         Serial.print("\t levelintraw: ");
         
         Serial.print(levelintraw);
         
         Serial.print("\t intdiffpitch: ");
         
         Serial.print(intdiffpitch);
         Serial.print("\t intdiff: ");
         
         Serial.print(intdiff);
         
         Serial.print("\tdiffa ");    
         Serial.print(diffa);
         Serial.print("\tlevelintpitcha ");    //Serial.print(" expoint: ");
         Serial.print(levelintpitcha);
         Serial.print("\texpoint ");    //Serial.print(" expoint: ");
         Serial.print(expoint);
         
         
         Serial.print("\t");    //Serial.print(" diffb: ");
         Serial.print("\tdiffb ");
         Serial.print(diffb);
         Serial.print("\t levelintpitchb ");    //Serial.print(" expoint: ");
         Serial.print(levelintpitchb);
         
         Serial.print("\t  data.pitch ");   
         Serial.print(data.pitch);
         */
         
         
         /*
          Serial.print("\tadc "); 
          Serial.print(potwertarray[THROTTLE]);
          Serial.print("\tthrottle ");    //Serial.print(" expoint: ");
          Serial.print(data.throttle);
          */
         
         Serial.print(" *\n");
      } // if TEST 1
      
      /*
       Serial.print(" \t");
       Serial.print(" * ");
       Serial.print(" \t");
       Serial.print(" potwert: ");
       Serial.print(potwertarray[YAW]);
       
       Serial.print(" yaw: ");
       Serial.print(data.yaw);
       
       Serial.print(" \t");
       Serial.print(" PITCH: ");
       Serial.print(potwertarray[PITCH]);
       
       Serial.print(" pitch: ");
       Serial.print(data.pitch);     
       Serial.print(" ROLL: ");
       Serial.print(potwertarray[ROLL]);
       
       Serial.print(" roll: ");
       Serial.print(data.roll);
       
       */
      /*
       Serial.print(" A1: ");
       Serial.print(potwertarray[PITCH]);
       Serial.print(" A3: ");
       Serial.print(potwertarray[ROLL]);
       Serial.print(" A6: ");
       Serial.print(potwertarray[THROTTLE]);
       */
      
      //Serial.print(" *\n");
   }
   // EEPROM
   eepromtaste.update();

   if(eepromtaste.pressed())
   {
      {
         Serial.print("\nEEPROM update\n");
         printgrenzen();
         void setHomeScreen();
         u8g2.sendBuffer();
         //analogWrite(6,0x0F);
         eepromwrite();
         //tone(6,440,250);
         Serial.println("EEPROM update end\n");
         eepromstatus &= ~(1<<EEPROM_WRITE) ;
      }

      delay(4);
      printeeprom(128);

   
   }

   if(UBatt < 4.07)
      {
         //digitalWrite(BUZZPIN,!(digitalRead(BUZZPIN)));

      }
   
   // pot lesen
   for (uint8_t i=0;i<NUM_SERVOS;i++)
   {
      potwert=analogRead(adcpinarray[i]);
      if(potwert > potgrenzearray[i][0])
      {
         potgrenzearray[i][0] = potwert; // pothi
      }
      if(potwert < potgrenzearray[i][1])
      {
         potgrenzearray[i][1] = potwert; // potlo
      }
      //potgrenzearray[0][0] = 17;
      //potgrenzearray[0][1] = 33;
      
      uint16_t mitte = servomittearray[i];
      uint8_t levelwert = kanalsettingarray[curr_model][i][1]; // element 1, levelarray
      
      // levelwert   faktor
      //    0             8/8
      //    1             7/8
      //    2             6/8
      //    3             5/8
      //    4             4/8
      
      // eventuell ungleiche werte 
      
      levelwerta = levelwert & 0x07;
      levelwertb = (levelwert & 0x70)>>4;
      
      // expowert ev. ungleich fuer richtung
      expowert = kanalsettingarray[curr_model][i][2]; // element2, expoarray
      
      expowerta = expowert & 0x07;
      expowertb = (expowert & 0x70)>>4;
      
      
      
      // map(value, fromLow, fromHigh, toLow, toHigh)

      if((i == YAW) || (i == PITCH) || (i == ROLL))
      {
         potwertarray[i] = potwert;
      }
      else if(i == THROTTLE)
      {
         potwertarray[i] = potwert;
      }


      //if((i == YAW) || (i == PITCH) || (i == ROLL))
      /*
      if((i == PITCH) || (i == ROLL))
  
      {      
         potwertpitch = potwert;
         if((potwert) < mitte) // Seite A, Ziehen
         {
            intdiff =  (mitte - potwert); // Abweichung von mitte
            //constrain(intdiff, 0,mitte);
            intdiffpitch = intdiff;
            //if (intdiff > MINDIFF)
            {
               diffa = map(intdiff,0,(mitte - potgrenzearray[i][1]), 0,512); 
               
               expoint = expoarray[expowerta][diffa];
               //expoint umrechnen mit level
               levelint = expoint * (8-levelwerta);
               
               levelint /= 8;
               levelintraw = levelint;
               levelint = map(levelint,0,512,0,(mitte - potgrenzearray[i][1]));
               //constrain(levelint, 0,mitte);
               levelint = mitte - levelint;
               
               //levelint = mitte - diffa;
               levelintpitcha = levelint;
               potwertarray[i] = levelint;
            }
            
         }
         else // Seite B potwert > mitte Stossen
         {
            intdiff = (potwert - mitte);
            //constrain(intdiff, 0,mitte);
            intdiffpitch = intdiff;
            
            //if (intdiff > MINDIFF)
            {
               diffb = map(intdiff,0,(potgrenzearray[i][0] - mitte),0,512);
               if(diffb >= 512 )
               {
                  diffb = 512;
               }
               //constrain(diffb, 0, 512);
               expoint = expoarray[expowertb][diffb];
               levelint = expoint * (8-levelwertb) ;
               
               levelint /= 8;
               levelintraw = levelint;
               
               levelint = map(levelint,0,512,0,(potgrenzearray[i][0] - mitte));
               
               levelint = mitte + levelint;
               levelintpitchb = levelint;
               potwertarray[i] = levelint;
               
               //potwertarray[i] = potwert;
            }
         }
         
      }
      else
      {
         potwertarray[i] = potwert;
      }
      */
      //potwertarray[i] = potwert;
      
      //
   } // for i
   
   // Border_Map(val, lower, middle, upper, reverse)
   
   
   //data.roll = Border_Map( impulscounter, 0, 512, 1023, true );  
   
   //data.yaw = Border_Map(potwertarray[YAW], 0, 512, 1023, true );        // CH4
   //data.yaw = map(potwertarray[YAW], 0, 512, 0,254);        // CH4
   //250703
   data.yaw = Border_Mapvar255(potwertarray[YAW],potgrenzearray[YAW][1],servomittearray[YAW],potgrenzearray[YAW][0],false);

   
   //data.pitch = Border_Map(potwertarray[PITCH], 0, 512, 1023, true );    // CH2    
   data.pitch = Border_Mapvar255(potwertarray[PITCH],potgrenzearray[PITCH][1],servomittearray[PITCH],potgrenzearray[PITCH][0],false);

   //data.roll = Border_Map(potwertarray[ROLL], 0, 512, 1023, true );      // CH1   Note: "true" or "false" for signal direction 
   
   potgrenzearray[ROLL][0] = servomittearray[ROLL];
   potgrenzearray[ROLL][1] = servomittearray[ROLL];

   data.roll = Border_Mapvar255(potwertarray[ROLL],potgrenzearray[ROLL][1],servomittearray[ROLL],potgrenzearray[ROLL][0],false);
     

   //data.throttle = Border_Map(potwertarray[THROTTLE],0, 30, 800, false );      // Stick
   //data.throttle = Border_Map(potwertarray[THROTTLE],0, 5, 1200, false ); 
   
   uint16_t throttlemitte = servomittearray[THROTTLE];
   data.throttle = Throttle_Map(potwertarray[THROTTLE],throttlemitte, POTHI,0,255, false );   
   
   //data.throttle = Border_Map(potwertarray[THROTTLE],0, 340,570, false );      // Potentiometer
   
   data.aux1 = digitalRead(5);                                          // CH5
   data.aux2 = digitalRead(7);                                          // CH6
   radiocounter++;
   
   if (radio.write(&data, sizeof(Signal)))
   {
      radiocounter++; 
   }
   else
   {
      Serial.print("radio error\n");
      digitalWrite(BUZZPIN,!(digitalRead(BUZZPIN)));
      errcounter++;
   }
}
