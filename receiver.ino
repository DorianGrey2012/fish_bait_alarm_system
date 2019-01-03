/*
 * 
 * Using nRF24L01 library for RF interface.
 
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#define speakerPin  10

// Buffer to store data.
byte data[sizeof(byte)];
int note = 262;
void setup()
{
  // Give bootloader some time to run.
  delay(1500);
  int led_mas[]={3,4,5,6,7};
  for (int i;i<6;i++)
  {
  pinMode(led_mas[i],OUTPUT);
  digitalWrite(led_mas[i], HIGH);
  }
  // Setup pins / SPI.
  Mirf.cePin = 9;
  Mirf.csnPin = 8;

  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();

  // Configure receiving address.
  Mirf.setRADDR((byte *) "receiver");

  // Set the payload length to sizeof(byte)
  Mirf.payload = sizeof(byte);

  
  Mirf.channel = 90;

  Mirf.config();

  Serial.begin(115200);
  //Serial.println("[Receiver] Started");
}

void loop()
{
  /*
   * If a packet has been received.
   *
   * isSending also restores listening mode when it
   * transitions from true to false.
   */

  if(!Mirf.isSending() && Mirf.dataReady())
  {
    // Load the packet into the buffer.
    Mirf.getData(data);


    char receivedID = '0' + data[0] + 1;
      switch(receivedID)
{
    case '1':
        Serial.write(receivedID);
        Serial.write('\r');
        Serial.write('\n');
        digitalWrite(data[0] + 3, LOW);
        tone(speakerPin,note);
    case '2':
        Serial.write(receivedID);
        Serial.write('\r');
        Serial.write('\n');
        digitalWrite(data[0] + 3, LOW);
        tone(speakerPin, note);
    case '3':
        Serial.write(receivedID);
        Serial.write('\r');
        Serial.write('\n');
        digitalWrite(data[0] + 3, LOW);
        tone(speakerPin, note);
     case '4':
        Serial.write(receivedID);
        Serial.write('\r');
        Serial.write('\n');
        digitalWrite(data[0] + 3, LOW);
        tone(speakerPin, note);
     case '5':
        Serial.write(receivedID);
        Serial.write('\r');
        Serial.write('\n');
        digitalWrite(data[0] + 3, LOW);
        tone(speakerPin, note);

}
    
    
  }
}
