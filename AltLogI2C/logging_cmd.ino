#define SYNC_UP 0xc3
#define SYNC_LO 0xaa


//Initialize a data structure for incoming messages.
struct{
  uint8_t sync1;
  uint8_t sync2;
  uint8_t type, len;
  uint8_t pld[BUFFER_SIZE-8];
  union{
    uint32_t u32;
    uint8_t u8arr[4];
  }crc_rx;
  uint32_t crc_calc;
  bool valid_sync;
  bool valid_length;
  bool valid_crc;
}msg;

/*
 * Data recieve interupt for I2C data.
 * Pull the incoming data and push onto the circular buffer.
 */
void receive(int countBytes){
  uint8_t in = 0;
  
  Serial.println("Message Incoming");
  
  if ((countBytes+buffer.size()) > BUFFER_SIZE){
    Serial.println("Incoming exceeds buffer");
    active.println("ERR Incoming exceeds buffer");
    active.flush();
    //What to do if buffer is full?
  }
  else{
    while(Wire.available()){
      in = Wire.read();
      if (!buffer.isFull()){
        buffer.push(in);
      }
      else{
        Serial.println("Buffer full");
        active.println("ERR Buffer full");
        active.flush();
        //What to do if buffer is full?
      }
    }
  }
}

/*
 * Data request interupt for I2C data.
 * Check the last received command, and figure out what to do next.
 */
void request(){
  processTX();
}

/*
 * ProcessRX reads the incoming message buffer, validates the messages, and
 * determines the appropriate action based on the message type.
 */
void processRX(){
  Serial.println("Message processing");
  //Reinitialize values
  msg.valid_sync = false;
  msg.valid_length = true;
  msg.valid_crc = false;

  //Read bytes until valid message sync bytes are detected.
  while(!buffer.isEmpty()){
    msg.sync1 = buffer.pop();
    Serial.println(String(msg.sync1));
    if (msg.sync1 == SYNC_UP){
      msg.sync2 = buffer.pop();
      if (msg.sync2 == SYNC_LO){
        msg.valid_sync = true;
      }
    }
  }

  //Valid sync bytes detected, populate the rest of the message but be careful
  //of the case of an incomplete message.
  if(msg.valid_sync && (buffer.size()>=6)){
    msg.type = buffer.pop();
    msg.len = buffer.pop();
    msg.crc_rx.u8arr[0] = buffer.pop();
    msg.crc_rx.u8arr[1] = buffer.pop();
    msg.crc_rx.u8arr[2] = buffer.pop();
    msg.crc_rx.u8arr[3] = buffer.pop();

    if(buffer.size() >= msg.len){
      for(int ct=0; ct <= msg.len; ct++){
        msg.pld[ct] = buffer.pop();
      }
    }
    else{
      Serial.println("Incomplete message");
      active.println("ERR Incomplete message");
      active.flush();
    }
    msg.valid_length = true;
  }
  else{
    Serial.println("Incomplete message");
    active.println("ERR Incomplete message");
    active.flush();
  }

  //We've read in the message. Now validate the CRC.
  if(msg.valid_sync && msg.valid_length){
    msg.crc_calc = crc.calculate(msg.pld, msg.len);
    if(msg.crc_calc == msg.crc_rx.u32){
      msg.valid_crc == true;
    }
    else{
      Serial.println("CRC mismatch");
      active.print("ERR CRC mismatch >");
      active.println(hexString(msg.pld, msg.len));
      active.flush();
    }
  }

  //At this point, we're pretty sure we have a valid and complete message, lets
  //process the thing.
  switch(msg.type){
    case 0x00: //Log data
      active.println(hexString(msg.pld, msg.len));
      active.flush();
      break;
    case 0x01: //Create new file (safely)
      break;
    case 0x02: //Append to existing file
      break;
    default: 
      break;
  }


}

/*
 * ProcessTX looks at message flags set by processRX to determine what data is
 * being requested. It then builds a message to return that data to the
 */
void processTX(){

}
