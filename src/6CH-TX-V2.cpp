// 6 Channel Transmitter | 6 Kanal Verici
// KendinYap Channel

#include <SPI.h>

//#include <Adafruit_LiquidCrystal.h>
#include <U8g2lib.h>
//#include <U8x8lib.h>
//#include <Wire.h>
#include "display.h"
#include "expo.h"
#include <nRF24L01.h>
#include <RF24.h>
const uint64_t pipeOut = 0xABCDABCD71LL;         // NOTE: The address in the Transmitter and Receiver code must be the same "0xABCDABCD71LL" | Verici ve Alıcı kodundaki adres aynı olmalıdır


//U8G2_SSD1327_WS_128X128_HW_I2C u8g2(U8G2_R0,U8X8_PIN_NONE);

// https://github.com/olikraus/u8g2/discussions/1865
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
#define TEST 1
#define CE_PIN 9
#define CSN_PIN 10
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

#define LOOPLED 4

#define BLINKRATE 0x00fF

// defines for PINS
// links
#define PITCH_PIN     A6
#define YAW_PIN       A3

// rechts
#define ROLL_PIN      A1
#define THROTTLE_PIN  A0  


// defines for array pos
#define YAW         0
#define PITCH       1
#define ROLL        2
#define THROTTLE    3

#define NUM_SERVOS  4

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


uint16_t schritt = 32;



volatile uint16_t          impulstimearray[NUM_SERVOS] = {};
const int           adcpinarray[NUM_SERVOS] = {A3,A6,A1,A0};    // pins der Pots

uint8_t kanalsettingarray[ANZAHLMODELLE][NUM_SERVOS][KANALSETTINGBREITE] = {};

uint16_t          servomittearray[NUM_SERVOS] = {}; // Werte fuer Mitte

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






uint16_t intdiff = 0;
uint16_t intdiffpitch = 0;



uint16_t potgrenzearray[NUM_SERVOS][2]; // obere und untere Grenze von adc

volatile float quot = (ppmhi - ppmlo)/(pothi - potlo);

volatile float expoquot = (ppmhi - ppmlo)/2/0x200; // umrechnen der max expo (512) auf PPM  

//volatile float quotarray[NUM_SERVOS] = {}; // Umrechnungsfaktor pro Pot

uint8_t curr_model = 0;

// OLED > in display.cpp
volatile uint16_t pot0 = 0;

uint16_t potwert = 0;

uint16_t errcounter = 0;
uint16_t radiocounter = 0;

struct Signal 
{
   byte throttle;
   byte pitch;
   byte roll;
   byte yaw;
   byte aux1;
   byte aux2;
   
};
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

void setup()
{
   Serial.begin(9600);
   pinMode(LOOPLED,OUTPUT);
   
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
   u8g2.clearDisplay(); 
   //u8g2.setFont(u8g2_font_helvR14_tr); // https://github.com/olikraus/u8g2/wiki/fntlist12
   u8g2.setFont(u8g2_font_t0_15_mr);  
   u8g2.setCursor(0, 14);
   u8g2.print(F("nRF24 T"));
   //u8g2.setFont(u8g2_font_ncenB10_tr);
   u8g2.setFontMode(0);
   oled_vertikalbalken(100,10,balkenvb,balkenvh);
   
   oled_horizontalbalken(10,50,balkenhb,balkenhh);
   
   
   
   
   
   
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
   for (uint8_t i=0;i<NUM_SERVOS;i++)
   {
      uint16_t wert = 500 + i * 50;
      wert = 750;
      impulstimearray[i] = wert; // mittelwert
      
      potgrenzearray[i][0] = potlo;
      potgrenzearray[i][1] = pothi;
      
      servomittearray[i] = analogRead(adcpinarray[i]);
      
   }
   Serial.print("\n"); 
   for (uint8_t i=0;i<NUM_SERVOS;i++)
   {
      Serial.print(adcpinarray[i]);
      Serial.print("\t");
      Serial.print(servomittearray[i]);
      Serial.print("\t");
      
      kanalsettingarray[0][i][1] = 0x00; // level
      kanalsettingarray[0][i][2] = 0x22; // expo
   }
   
   Serial.print("\n");
   kanalsettingarray[0][PITCH][1] = 0x00; // level
   kanalsettingarray[0][PITCH][2] = 0x00; // level
   
   potwert = servomittearray[0];  
   
   
   
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
      //levelint = map(levelint,0,127,0,(middle - lower));
      //levelint = middle - levelint;
      //levelintpitcha = levelint;
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
      //levelint = map(levelint,0,127,0,(upper - middle));     
      //levelintpitchb = levelint;
   }
      
   return ( reverse ? 255 - levelintcheck : levelintcheck );
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
   loopcounter++;
   
   if(loopcounter >= 2*BLINKRATE)
   {
      loopcounter = 0;
      blinkcounter++;
      impulscounter+=16;
      digitalWrite(LOOPLED, ! digitalRead(LOOPLED));
      
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
      oled_delete(0,44,72);
      
      //u8g2.drawGlyph(32,44,'A'+(charindex));
      // Yaw
      u8g2.setCursor(0,46);
      u8g2.print(data.yaw);
      
      // Pitch
      u8g2.setCursor(32,46);
      u8g2.print(data.pitch);
     
      
      uint8_t wertv = map(data.pitch,85,251,2,balkenvh-2); // Platz fuer 3 pixel dicke
      oled_vertikalbalken_setwert(100,10,balkenvb,balkenvh,wertv);
      
      uint8_t werth = map(data.yaw,85,251,2,balkenhb-2); // Platz fuer 3 pixel dicke
      
      oled_horizontalbalken_setwert(10,50,balkenhb,balkenhh,werth);
      
      
      u8g2.sendBuffer();
      if(loopcounter1 > 25)
      {
         loopcounter1 = 0;
      }
      if (TEST)
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
      } // if TEST
      
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

      if(i == YAW) 
      {
         potwertarray[YAW] = potwert;
      }


      //if((i == YAW) || (i == PITCH) || (i == ROLL))
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
      
      //potwertarray[i] = potwert;
      
      //
   } // for i
   
   // Border_Map(val, lower, middle, upper, reverse)
   
   
   //data.roll = Border_Map( impulscounter, 0, 512, 1023, true );  
   
   //data.yaw = Border_Map(potwertarray[YAW], 0, 512, 1023, true );        // CH4
   //data.yaw = map(potwertarray[YAW], 0, 512, 0,254);        // CH4
   //250703
   data.yaw = Border_Mapvar255(potwertarray[YAW],potgrenzearray[YAW][1],servomittearray[YAW],potgrenzearray[YAW][0],false);

   
   data.pitch = Border_Map(potwertarray[PITCH], 0, 512, 1023, true );    // CH2    
   
   data.roll = Border_Map(potwertarray[ROLL], 0, 512, 1023, true );      // CH1   Note: "true" or "false" for signal direction 
   
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
      errcounter++;
   }
}
