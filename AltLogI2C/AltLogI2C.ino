/*
 * AltLogI2C: Alternate SD Logger for use with SFE Quiic OpenLog
 * Command based data logging
 */

#include <Wire.h>
#include <CircularBuffer.h>
#include <SPI.h>
#include <SD.h>
#include <CRC32.h>

#define I2C_ADDR 0x42
#define SPI_CS 4
#define SPI_MISO
#define SPI_MOSI

//Initialize the circular buffer of chars for I2C data, and the CRC32 generator
#define BUFFER_SIZE 256 //TODO: Test if this is too big
CircularBuffer<uint8_t, BUFFER_SIZE> buffer;
CRC32 crc;

//Define special flags.
#define DEBUG 1

void setup() {
  //Setup I2C and attatch interupts.
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receive);
  Wire.onRequest(request);

  //Setup serial port for debug messages.
  //If debug mode specified, wait for the monitor to open.
  Serial.begin(9600);
  if(DEBUG){
    while(!Serial);
  }

  //Initialize the SD card
  if(!SD.begin(SPI_CS)){
    Serial.println("Card initialize failed");
    while(true);
  }
  //Open a default file, create that file if it does not exist.
  File active = SD.open("default.txt")
}

void loop() {

  //In the main loop just process the incoming messages. The onReceive interupt
  //will simply dump everything into the buffer as fast as possible.
  //onRequest handles all the data requests, as it assumes that they will be
  //proceeded by a sent message.
  if (!buffer.isEmpty()){
    processRX();
  }
}
