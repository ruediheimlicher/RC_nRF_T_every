// 6 Channel Transmitter | 6 Kanal Verici
// KendinYap Channel

  #include <SPI.h>

  #include <Adafruit_LiquidCrystal.h>

  #include <nRF24L01.h>
  #include <RF24.h>
  const uint64_t pipeOut = 0xABCDABCD71LL;         // NOTE: The address in the Transmitter and Receiver code must be the same "0xABCDABCD71LL" | Verici ve Alıcı kodundaki adres aynı olmalıdır

  #define CE_PIN 9
  #define CSN_PIN 10
  // instantiate an object for the nRF24L01 transceiver
  RF24 radio(CE_PIN, CSN_PIN);
  
  //RF24 radio(9, 10);                               // select CE,CSN pin | CE ve CSN pinlerin seçimi
  #define LOOPLED 4

  #define BLINKRATE 0x00FF

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
//Impulslaenge, ms
#define PPMLO  850  // Minwert ms fuer Impulslaenge
#define PPMHI  2150 // Maxwert ms fur Impulslaenge
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



#define POTLO   1300
#define POTHI  2900


volatile uint16_t          impulstimearray[NUM_SERVOS] = {};
const int           adcpinarray[NUM_SERVOS] = {A3,A6,A1,A0};    // pins der Pots

uint8_t kanalsettingarray[ANZAHLMODELLE][NUM_SERVOS][KANALSETTINGBREITE] = {};

volatile uint16_t          servomittearray[NUM_SERVOS] = {}; // Werte fuer Mitte

volatile uint16_t          potwertarray[NUM_SERVOS] = {}; // Werte fuer Mitte
volatile uint16_t          externpotwertarray[NUM_SERVOS] = {}; // Werte von extern  pro servo



float potlo = POTLO; // min pot
float pothi = POTHI; // max pot
float ppmlo = PPMLO; // min ppm
float ppmhi = PPMHI; // max ppm

uint16_t potgrenzearray[NUM_SERVOS][2]; // obere und untere Grenze von adc
volatile float quot = (ppmhi - ppmlo)/(pothi - potlo);

volatile float expoquot = (ppmhi - ppmlo)/2/0x200; // umrechnen der max expo (512) auf PPM  

volatile float quotarray[NUM_SERVOS] = {}; // Umrechnungsfaktor pro Pot

uint8_t curr_model = 0;



volatile uint16_t pot0 = 0;



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
  data.yaw_data = 127;
  data.aux1 = 0;                       
  data.aux2 = 0;

}

// initialize the library with the numbers of the interface pins
//Adafruit_LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Adafruit_LiquidCrystal lcd(0);

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
  for (int i=0;i<NUM_SERVOS;i++)
   {
      //adcpinarray[i] = 0xFF;
   }
  /*
  adcpinarray[0] = PITCH_PIN;
  adcpinarray[1] = YAW_PIN;
  adcpinarray[2] = ROLL_PIN;
  adcpinarray[3] = THROTTLE_PIN;
  */
   for (int i=0;i<NUM_SERVOS;i++)
   {
    int wert = 500 + i * 50;
      wert = 750;
       impulstimearray[i] = wert; // mittelwert

      potgrenzearray[i][0] = potlo;
      potgrenzearray[i][1] = pothi;
      quotarray[i] = quot;
      servomittearray[i] = analogRead(adcpinarray[i]);
      
   }
    Serial.print("\n"); 
   for (int i=0;i<NUM_SERVOS;i++)
   {
      Serial.print(adcpinarray[i]);
      Serial.print("\t");
      Serial.print(servomittearray[i]);
      Serial.print("\t");
   }
  Serial.print("\n");
   kanalsettingarray[0][0][1] = 0x11;
   kanalsettingarray[0][0][2] = 0x22;


 
} // setup

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
  void loop()
{                                  
  loopcounter++;
  
  if(loopcounter >= BLINKRATE)
  {
    loopcounter = 0;
    blinkcounter++;
    impulscounter+=16;
    digitalWrite(LOOPLED, ! digitalRead(LOOPLED));
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
   
    Serial.print(" A0: ");
    Serial.print(analogRead(A0));
    Serial.print(" A1: ");
    Serial.print(analogRead(A1));
    Serial.print(" A3: ");
    Serial.print(analogRead(A3));
    Serial.print(" A6: ");
    Serial.print(analogRead(A6));

 
    Serial.print(" *\n");
  }
  // pot lesen
  for (uint8_t i=0;i<NUM_SERVOS;i++)
  {
    uint16_t potwert=analogRead(adcpinarray[i]);
    potwertarray[i] = potwert;

    uint16_t mitte = servomittearray[i];
  
    uint8_t levelwert = kanalsettingarray[curr_model][i][1]; // element 1, levelarray
    
    // levelwert   faktor
    //    0             8/8
    //    1             7/8
    //    2             6/8
    //    3             5/8
    //    4             4/8
    
    // eventuell ungleiche werte 
    
    uint8_t levelwerta = levelwert & 0x07;
    uint8_t levelwertb = (levelwert & 0x70)>>4;

      // expowert ev. ungleich fuer richtung
    uint8_t expowert = kanalsettingarray[curr_model][i][2]; // element2, expoarray

    uint8_t expowerta = expowert & 0x07;
    uint8_t expowertb = (expowert & 0x70)>>4;



  } // for i

  // Border_Map(val, lower, middle, upper, reverse)
    
    
  data.yaw_data = Border_Map( impulscounter, 0, 512, 1023, true );  

  //data.yaw_data = Border_Map(potwertarray[YAW], 0, 512, 1023, false );        // CH4
  data.roll_data = Border_Map(potwertarray[ROLL], 0, 512, 1023, true );        // CH1   Note: "true" or "false" for signal direction 
  data.pitch_data = Border_Map(potwertarray[PITCH], 0, 512, 1023, true );       // CH2    
  
  //data.throttle_data = Border_Map(potwertarray[THROTTLE],0, 340, 570, true );      // CH3   Note: For Single side ESC
  data.throttle_data = Border_Map(potwertarray[THROTTLE],0, 10, 500, true ); 
  
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
