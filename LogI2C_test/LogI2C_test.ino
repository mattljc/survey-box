/*
 * LogI2C_test: Test scripts to generate and test AltLogI2C messages.
 */

#include <Wire.h>
#include <CRC32.h>

#define Serial SerialUSB
#define TARGET 0x42

String msg,pld;
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
    msg = "MESSAGE";

    Wire.beginTransmission(TARGET);
    Wire.write('A');
    Wire.endTransmission();
    
  }
}
