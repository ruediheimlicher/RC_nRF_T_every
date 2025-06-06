// 6 Channel Transmitter | 6 Kanal Verici
// KendinYap Channel

  #include <SPI.h>

  //#include <Adafruit_LiquidCrystal.h>
  #include <U8g2lib.h>
  #include <U8x8lib.h>
  #include <Wire.h>

  #include "expo.h"
  #include <nRF24L01.h>
  #include <RF24.h>
  const uint64_t pipeOut = 0xABCDABCD71LL;         // NOTE: The address in the Transmitter and Receiver code must be the same "0xABCDABCD71LL" | Verici ve Alıcı kodundaki adres aynı olmalıdır

  //U8G2_SSD1327_MIDAS_128X128_F_HW_I2C  u8g2(U8G2_R0,U8X8_PIN_NONE);
  //U8G2_SSD1327_WS_128X128_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ A5, /* data=*/ A4, /* reset=*/ U8X8_PIN_NONE);
  
  //U8G2_SSD1327_WS_128X128_HW_I2C u8g2(U8G2_R0,U8X8_PIN_NONE);

// https://github.com/olikraus/u8g2/discussions/1865
//  U8G2_SSD1327_WS_128X128_F_HW_I2C u8g2(0, U8X8_PIN_NONE, A5, A4);

U8X8_SSD1327_WS_128X128_HW_I2C u8x8(U8X8_PIN_NONE);

  //U8X8_SSD1327_WS_128X128_HW_I2C u8g2(A4,A5);
  #define TEST 1
  #define CE_PIN 9
  #define CSN_PIN 10
  // instantiate an object for the nRF24L01 transceiver
  RF24 radio(CE_PIN, CSN_PIN);
  
  //RF24 radio(9, 10);                               // select CE,CSN pin | CE ve CSN pinlerin seçimi
  #define LOOPLED 4

  #define BLINKRATE 0x01FF

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
#define POT0LO 620  // Min wert vom ADC Pot 0
#define POT0HI 3400 // Max wert vom ADC Pot 0

#define POTLO   0
#define POTHI  688

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

uint16_t diffapitch = 0;
uint16_t diffbpitch = 0;

uint16_t diffb = 0;
float expofloat = 0;
uint16_t expoint = 0;
uint16_t levelint = 0;

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



volatile uint16_t pot0 = 0;

uint16_t potwert = 0;

uint16_t errcounter = 0;
uint16_t radiocounter = 0;
  
  struct Signal 
  {
  byte throttle_data;
  byte pitch_data;
  byte roll_data;
  byte yaw_data;
  byte aux1;
  byte aux2;
  
  };
  Signal data;
  void ResetData() 
{
  data.throttle_data = 0;                  
  data.pitch_data = 127;
  data.roll_data = 127;
  data.yaw_data = 110;
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
u8x8.setBusClock(4000000);
u8x8.setI2CAddress(2*0x3D);
u8x8.begin();
  //u8g2.setBusClock(4000000);

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
      //[i] = quot;
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
    kanalsettingarray[0][i][2] = 0x33; // expo
  }
  Serial.print("\n");
  
potwert = POTLO;


 
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
  
  if(loopcounter >= BLINKRATE)
  {
    loopcounter = 0;
    blinkcounter++;
    impulscounter+=16;
    digitalWrite(LOOPLED, ! digitalRead(LOOPLED));

    ///*
    //u8g2.clearBuffer();                   // Clear display.
    u8x8.setFont(u8g2_font_ncenB08_tr);    // choose a suitable font
    u8x8.drawString(0, 24, "Hello OLED!");    // write something to the buffer
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
    if (TEST)
    {
    Serial.print(servomittearray[PITCH]);
    Serial.print("\t");    //Serial.print(" potwert: ");
    Serial.print(potwertarray[PITCH]);
    Serial.print("\t");
    //Serial.print(" intdiff: ");
   // Serial.print("\t");    //Serial.print(" levelwerta: ");
   // Serial.print(levelwerta);
    Serial.print("\t");    //Serial.print(" expoint: ");
    //uint8_t ea = kanalsettingarray[curr_model][PITCH][2];
    //Serial.print(ea);
    /*
    Serial.print("\tintdiff "); 
    Serial.print(intdiffpitch);
    Serial.print("\tdiffa ");    //Serial.print(" diffa: ");
    Serial.print(diffa);
    Serial.print("\tlevelintpitcha ");    //Serial.print(" expoint: ");
    Serial.print(levelintpitcha);
    Serial.print("\texpoint ");    //Serial.print(" expoint: ");
    Serial.print(expoint);


    Serial.print("\t");    //Serial.print(" diffb: ");
    Serial.print("\tdiffb ");
    Serial.print(diffb);
    Serial.print("\tlevelintpitchb ");    //Serial.print(" expoint: ");
    Serial.print(levelintpitchb);
    */
    Serial.print("\tadc "); 
    Serial.print(potwertarray[THROTTLE]);
     Serial.print("\tthrottle ");    //Serial.print(" expoint: ");
    Serial.print(data.throttle_data);


    Serial.print(" *\n");
    } // if TEST
    
/*
    Serial.print(" \t");
    Serial.print(" * ");
    Serial.print(" \t");
    Serial.print(" potwert: ");
    Serial.print(potwertarray[YAW]);

    Serial.print(" yaw_data: ");
    Serial.print(data.yaw_data);

    Serial.print(" \t");
    Serial.print(" PITCH: ");
    Serial.print(potwertarray[PITCH]);

    Serial.print(" pitch_data: ");
    Serial.print(data.pitch_data);     
    Serial.print(" ROLL: ");
    Serial.print(potwertarray[ROLL]);

    Serial.print(" roll_data: ");
    Serial.print(data.roll_data);

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


    
    //float expofloat = 0;
    //float diff = 0;
    
    // map(value, fromLow, fromHigh, toLow, toHigh)
    
    if((i == YAW) || (i == PITCH) || (i == ROLL))
    //if(i == PITCH)
    {      
      if((potwert) < mitte) // Seite A, Ziehen
      {
        intdiff =  (mitte - potwert); // Abweichung von mitte
        constrain(intdiff, 0,mitte);
        intdiffpitch = intdiff;
        //if (intdiff > MINDIFF)
        {
          diffa = map(intdiff,0,(mitte - POTLO), 0,512);

          expoint = expoarray[expowerta][diffa];
          //expoint umrechnen mit level
          levelint = expoint * (8-levelwerta);
          levelint /= 8;
          levelint = map(levelint,0,512,0,(mitte - POTLO));
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
        constrain(intdiff, 0,mitte);
        intdiffpitch = intdiff;

        //if (intdiff > MINDIFF)
        {
          diffb = map(intdiff,0,(POTHI - mitte),0,512);
          if(diffb >= 512 )
          {
            diffb = 512;
          }
          //constrain(diffb, 0, 512);
          expoint = expoarray[expowertb][diffb];
          levelint = expoint * (8-levelwertb) ;
          levelint /= 8;
          levelint = map(levelint,0,512,0,(POTHI - mitte));

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
    
    
  //data.roll_data = Border_Map( impulscounter, 0, 512, 1023, true );  

  data.yaw_data = Border_Map(potwertarray[YAW], 0, 512, 1023, true );        // CH4
  
  data.roll_data = Border_Map(potwertarray[ROLL], 0, 512, 1023, true );        // CH1   Note: "true" or "false" for signal direction 
  data.pitch_data = Border_Map(potwertarray[PITCH], 0, 512, 1023, true );       // CH2    
  
  //data.throttle_data = Border_Map(potwertarray[THROTTLE],0, 30, 800, false );      // Stick
  //data.throttle_data = Border_Map(potwertarray[THROTTLE],0, 5, 1200, false ); 
  
  uint16_t throttlemitte = servomittearray[THROTTLE];
  data.throttle_data = Throttle_Map(potwertarray[THROTTLE],throttlemitte, POTHI,0,255, false );   
  
  //data.throttle_data = Border_Map(potwertarray[THROTTLE],0, 340,570, false );      // Potentiometer

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
