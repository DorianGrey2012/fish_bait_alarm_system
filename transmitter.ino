/**
 * 
 * Transmitter for fishing-bait system.
 *
 * Using LowPower library to put Arduino in sleep mode.
 * See: https://github.com/rocketscream/Low-Power
 *
 * Using nRF24L01 library for RF interface.
 * See: https://github.com/aaronds/arduino-nrf24l01
 *
 * Pins:
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13
 *
 * Configurable:
 * CE -> 9
 * CSN -> 8
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <LowPower.h>

volatile bool do_transmission;
byte buttonNr;

void setup(){
  // Setup button nr
  setButtonNr();
  char receiverAddr[] = "button0";
  receiverAddr[6] = '1' + buttonNr;

  // Setup MIRF board
  Mirf.cePin = 8;
  Mirf.csnPin = 9;

  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();

  Mirf.setRADDR((byte *) receiverAddr);

  Mirf.payload = sizeof(byte);

  // we use channel 90 as it is outside of WLAN bands
  // or channels used by wireless surveillance cameras
  Mirf.channel = 90;

  Mirf.config();

  // Setup button
  pinMode(2, INPUT);           // set pin to input
  digitalWrite(2, HIGH);       // turn on pullup resistors

  // Setup other variables
  do_transmission = false;

  // Reduce power
  pinsLow();
}

void loop()
{
  if (do_transmission == true)
  {
    // Clear flag
    do_transmission = false;

    // Setup packet data
    Mirf.setTADDR((byte *) "receiver");

    // Send data
    Mirf.send((byte *) & buttonNr);

    // Wait for sending to finish
    while (Mirf.isSending())
    {
    }

    // Power down the MIRF module
    Mirf.powerDown();


    delay(50);
  } else
  {
    // Wait for pin 2 to go high
    while (!digitalRead(2))
    {
      delay(50);
    }

    // Go to sleep
    sleepNow();
  }
}

void wakeUpNow ()
{
  // Don't put detach here, seems to lock up chip,
  // i.e. on button bounce while trying to go into sleep.
  // Detach the interrupt so it doesn't fire again
  //detachInterrupt(0);

  // Set flag
  do_transmission = true;
}

void setButtonNr ()
{

  buttonNr = 0;
 
}

void pinsLow ()
{
  // Make all unused pins low outputs to save power
  uint8_t const pinNrs[] = {0, 1, 6, 7, 10, A0, A1, A2, A3, A4, A5, A6, A7};

  for (int i = 0; i < sizeof(pinNrs); ++i)
  {
    pinMode(pinNrs[i], OUTPUT);
    digitalWrite(pinNrs[i], LOW);
  }
}

void sleepNow ()         // here we put the arduino to sleep
{
  // Make sure the nRF24L01+ module is powered down
  Mirf.powerDown();

  // Make sure we don't go sleeping with pin low!
  while (!digitalRead(2))
  {
  }

 
   *
   * In all but the IDLE sleep modes only LOW can be used.
   */
  attachInterrupt(0, wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function

  // Sleep with ADC module and BOD module off
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP

  // Detach the interrupt so it doesn't fire again
  detachInterrupt(0);
}
