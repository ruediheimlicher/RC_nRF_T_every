// 6 Channel Transmitter | 6 Kanal Verici
// KendinYap Channel

  #include <SPI.h>
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

uint16_t loopcounter = 0;
uint8_t blinkcounter = 0;
uint8_t impulscounter = 0;

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
  void setup()
{
  Serial.begin(115200);
  pinMode(LOOPLED,OUTPUT);
                                 // Configure the NRF24 module  | NRF24 modül konfigürasyonu
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
 
}
                                      // Joystick center and its borders | Joystick merkez ve sınırları
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

 
  

    
    Serial.print("*\n");
  }
    //data.roll = Border_Map( 4*impulscounter, 0, 512, 1023, true );  


  data.roll = Border_Map( analogRead(A6), 0, 512, 1023, true );        // CH1   Note: "true" or "false" for signal direction | "true" veya "false" sinyal yönünü belirler
  data.pitch = Border_Map( analogRead(A0), 0, 512, 1023, true );       // CH2    
  data.throttle = Border_Map( analogRead(A3),0, 340, 570, true );      // CH3   Note: For Single side ESC | Tek yönlü ESC için
  data.yaw = Border_Map( analogRead(A1), 0, 512, 1023, false );        // CH4
  data.aux1 = digitalRead(8);                                          // CH5
  data.aux2 = digitalRead(2);                                          // CH6
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
