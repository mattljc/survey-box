/*
 * AltLogI2C: Alternate SD Logger for use with SFE Quiic OpenLog
 * Command based data logging
 */

#include <Wire.h>
#include <SPI.h>
#include "SdFat.h"
#include "CRC32.h"

#define I2C_ADDR 0x42
#define SPI_CS 10
#define SPI_MISO
#define SPI_MOSI

//Initialize I2C stuff, including CRC32 generator
#define BUFFER_SIZE 32
CRC32 crc;

//Define SD variables
SdFat SD;
File active;

//Define special flags.
#define DEBUG
#define VERBOSE

//Useful varaibles
unsigned long oldtime_100Hz,oldtime_10Hz,oldtime_1Hz = 0;
unsigned long newtime = 0;
int ledState = LOW;
int led_pwr = 5;
int led_stat = 13;

bool all_go = false;

void setup() {
  //Setup I2C and attatch interupts.
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receive);
  Wire.onRequest(request);

  //Setup serial port for debug messages.
  //If debug mode specified, wait for the monitor to open.
  Serial.begin(9600);
  #ifdef DEBUG
  while(!Serial)
  #endif

  //Initialize the SD card
  while(!SD.begin(SPI_CS)){
    #ifdef VERBOSE
    Serial.println(F("Card initialize failed"));
    #endif
    while(true);
  }
  
  //Open a default file, create that file if it does not exist.
  active = SD.open("log.txt", O_WRONLY | O_CREAT | O_APPEND);
  if(!active){
    #ifdef VERBOSE
    Serial.println(F("File open failed"));
    //Serial.println(FreeRam());
    #endif
    while(true);
  }
  else{
    #ifdef VERBOSE
    Serial.println(F("File open. Awaiting data."));
    #endif
    active.println(F("File open. Awaiting data."));
    active.flush();
  }

  //Initialize the LED pin
  pinMode(led_pwr, OUTPUT);

  //Let the all_go
  all_go = true;
}

void loop() {
  /*
   * In the main loop process the incoming messages.
   * onRequest handles all the data requests, as it assumes that they will be
   * proceeded by a sent message.
   */
  processRX();

  /*
   * Heartbeat code to run periodic stuff.
   */
  newtime = millis();
  if(newtime-oldtime_1Hz >= 1000){ //1Hz Tasks
    oldtime_1Hz = newtime;

    #ifdef VERBOSE
    Serial.println(F("1Hz Heartbeat"));
    #endif

    if (ledState == LOW) {ledState = HIGH;}
    else {ledState = LOW;}
    digitalWrite(led_pwr, ledState);
  }
}
