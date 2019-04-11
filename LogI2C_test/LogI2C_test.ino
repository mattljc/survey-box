/*
 * LogI2C_test: Test scripts to generate and test AltLogI2C messages.
 */

#include <Wire.h>
#include <CRC32.h>

#define Serial SerialUSB
#define TARGET 0x42
#define BUFFER_SIZE 32
#define SYNC_UP 0x5A
#define SYNC_LO 0xA5

struct{
  uint8_t sync1;
  uint8_t sync2;
  uint8_t type;
  uint8_t len;
  union{
    uint32_t u32;
    byte u8[4];
  }crc;
  uint8_t pld[BUFFER_SIZE];
}outbound;

CRC32 crc;

//Timers
unsigned long newtime = 0;
unsigned long oldtime = 0;


void setup() {
  Wire.begin();
}

void loop() {
  newtime = millis();

  if(newtime-oldtime >= 5000){ //1Hz Tasks

    oldtime = newtime;
    Serial.println("Send message");

    outbound.type = 0x00;
    outbound.len = 0x08;

    outbound.pld[0] = 0x55
    outbound.pld[1] = 0xAA
    outbound.pld[2] = 0x55
    outbound.pld[3] = 0xAA
    outbound.pld[4] = (newtime&0x000000ff)
    outbound.pld[5] = (newtime&0x0000ff00)>>2
    outbound.pld[6] = (newtime&0x00ff0000)>>4
    outbound.pld[7] = (newtime&0xff000000)>>6

    outbound.crc.u32 = crc.calculate(outbound.pld, outbound.len);

    Wire.beginTransmission(TARGET);
    Wire.write(outbound.sync1);
    Wire.write(outbound.sync2);
    Wire.write(outbound.type);
    Wire.write(outbound.len);
    Wire.write(outbound.crc.u32);
    for(uint8_t ct=0; ct<=outbound.len; ct++){
        Wire.write(outbound.pld[ct]);
    }
    Wire.endTransmission();

  }
}
