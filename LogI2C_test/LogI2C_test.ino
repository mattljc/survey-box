/*
 * LogI2C_test: Test scripts to generate and test AltLogI2C messages.
 */

#include <Wire.h>
#include <CRC32.h>

#define Serial SerialUSB
#define TARGET 0x42

byte msg[] = {0x5A, 0xA5, 0xFF, 0x00, 0xAA, 0xFF, 0x00, 0xAA};
CRC32 crc;

//Timers
unsigned long newtime = 0;
unsigned long oldtime = 0;


void setup() {
  Wire.begin();
}

void loop() {
  newtime = millis();

  if(newtime-oldtime >= 1000){ //1Hz Tasks
    
    oldtime = newtime;
    Serial.println("Send message");

    Wire.beginTransmission(TARGET);
    Wire.write(0xC3);
    Wire.write(0xAA);
    Wire.write(msg, 8);
    Wire.endTransmission();
    
  }
}
