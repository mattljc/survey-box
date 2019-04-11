#define SYNC_UP 0x5A
#define SYNC_LO 0xA5


//Initialize a data structure for incoming messages.
struct{
  uint8_t sync1;
  uint8_t sync2;
  uint8_t type;
  uint8_t len;
  uint8_t pld[BUFFER_SIZE];
  union{
    uint32_t u32;
    uint8_t u8[4];
  }crc_rx;
  uint32_t crc_calc;
  bool valid_sync;
  bool valid_length;
  bool valid_crc;
  bool new_msg;
}msg;

/*
 * Data recieve interupt for I2C data.
 * Pull the incoming data and look for sync bytes. Once the bytes have been
 * found, start populating the msg structure.
 */
void receive(int countBytes){

  //Reset msg flags.
  msg.valid_sync = false;
  msg.valid_length = false;
  msg.valid_crc = false;

  #ifdef VERBOSE
  Serial.println("Message Incoming");
  Serial.println("Size: "+String(countBytes));
  #endif

  //Check size won't overflow buffer
  if(countBytes > BUFFER_SIZE+8){
    #ifdef VERBOSE
    Serial.println("Incoming exceeds buffer");
    #endif
    active.println("ERR Incoming exceeds buffer");
    active.flush();
    //What to do if buffer is full?
  }
  else{
    //Look for sync bytes
    while(Wire.available()){
      msg.sync1 = Wire.read();

      #ifdef VERBOSE
      Serial.println("~"+String(msg.sync1,HEX)+"~");
      #endif

      if(msg.sync1 == SYNC_UP){
         //Found sync MSB, look for the next one.
         msg.sync2 = Wire.read();

         #ifdef VERBOSE
         Serial.println("~~"+String(msg.sync2,HEX)+"~~");
         #endif

         if(msg.sync2 == SYNC_LO){
             //Found sync LSB, populate the msg structure
             msg.valid_sync = true;
             msg.type = Wire.read();
             msg.len = Wire.read();
             msg.crc_rx.u8[0]= Wire.read();
             msg.crc_rx.u8[1]= Wire.read();
             msg.crc_rx.u8[2]= Wire.read();
             msg.crc_rx.u8[3]= Wire.read();

             #ifdef VERBOSE
             Serial.println("Valid Header Found");
             Serial.println("TYPE="+String(msg.type,HEX)+" SIZE="+String(msg.len,HEX)+" CRC="+String(msg.crc_rx.u32,HEX));
             #endif
         }
      }
    }
  }

  //At this point we have a header. Now check that message received is as long
  //as promised in the header. If so, pull down the payload.
  if(msg.valid_sync){
    msg.valid_length = (msg.len == (countBytes - 8));

    if(msg.valid_length){
      #ifdef VERBOSE
      Serial.println("Valid Payload Length");
      #endif

      for(uint8_t ct=0; ct<=msg.len; ct++){
          msg.pld[ct] = Wire.read();
      }
    }
    else{
      #ifdef VERBOSE
      Serial.println("Incomplete message");
      #endif

      active.println("ERR Incomplete message");
      active.flush();
    }

    //We've read in the message. Now validate the CRC.
    if(msg.valid_length){
      msg.crc_calc = crc.calculate(msg.pld, msg.len);
      msg.valid_crc = (msg.crc_calc == msg.crc_rx.u32);

      if(msg.valid_crc){
        #ifdef VERBOSE
        Serial.println("Valid Payload CRC");
        #endif
      }
      else{
        #ifdef VERBOSE
        Serial.println("CRC mismatch");
        #endif
        active.print("ERR CRC mismatch");
        active.flush();
      }
    }
  }

  //Finally set the new message flag if the message has passed all the tests.
  msg.new_msg = msg.valid_sync && msg.valid_length && msg.valid_crc;
}



/*
 * Data request interupt for I2C data.
 * Check the last received command, and figure out what to do next.
 */
void request(){
  processTX();
}

/*
 * ProcessRX reads the validated msg structure, and processes the payload.
 */
void processRX(){
  //Only run if its a new message.
  if(msg.new_msg){
      #ifdef VERBOSE
      Serial.println("Message processing");
      #endif

      switch(msg.type){
      case 0x00: //Log data
        #ifdef VERBOSE
        Serial.println(">> TYPE 0x00 LOG MESSAGE <<");
        #endif
        //active.println(String(msg.pld, HEX));
        active.flush();
        break;
      case 0x01: //Create new file (safely)
        #ifdef VERBOSE
        Serial.println(">> TYPE 0x01 NEW FILE <<");
        #endif
        break;
      case 0x02: //Append to existing file
        #ifdef VERBOSE
        Serial.println(">> TYPE 0x02 APPEND FILE <<");
        #endif
        break;
      default:
        #ifdef VERBOSE
        Serial.println(">> DEFAULT <<");
        #endif
        break;
    }
    msg.new_msg = false;
  }
}

/*
 * ProcessTX looks at message flags set by processRX to determine what data is
 * being requested. It then builds a message to return that data to the
 */
void processTX(){

}
