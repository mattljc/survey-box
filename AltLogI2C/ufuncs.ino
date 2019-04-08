/*
 * ufuncs.ino
 * 
 * Utility functions for the AltLogI2C package.
 * 
 */

String hexString(uint8_t * bytes, uint8_t len){
  String output;
  String temp;
  for(int ct = 0; ct<=len; ct++){
    temp = String(bytes[ct]);
    output = output + temp;
  }
  return output;
}

//LED toggle function?
