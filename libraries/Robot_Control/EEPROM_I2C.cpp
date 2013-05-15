#include "EEPROM_I2C.h"
#include <Wire.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

void EEPROM_I2C::begin(){
  Wire.begin();
}

void EEPROM_I2C::writeByte(unsigned int eeaddress, byte data){
  int rdata = data;
  this->_beginTransmission(eeaddress);
  Wire.write(rdata);
  this->_endTransmission();
}

byte EEPROM_I2C::readByte(unsigned int eeaddress){
  int rdata;
  this->_beginTransmission(eeaddress);
  this->_endTransmission();
  
  Wire.requestFrom(DEVICEADDRESS,1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

void EEPROM_I2C::writePage(unsigned int eeaddress, byte* data, byte length ){
  this->_beginTransmission(eeaddress);
  
  byte c;
  
  for ( c = 0; c < length; c++)
    Wire.write(data[c]);
	
  this->_endTransmission();
  
  delay(10);                           // need some delay
}

void EEPROM_I2C::readBuffer(unsigned int eeaddress, byte *buffer, int length ){
  this->_beginTransmission(eeaddress);
  this->_endTransmission();
  Wire.requestFrom(DEVICEADDRESS,length);

  for ( int c = 0; c < length; c++ )
    if (Wire.available()) buffer[c] = Wire.read();
}



void EEPROM_I2C::_beginTransmission(unsigned int eeaddress){
  Wire.beginTransmission(DEVICEADDRESS);
  Wire.write((eeaddress >> 8));    // Address High Byte
  Wire.write((eeaddress & 0xFF));  // Address Low Byte
}
void EEPROM_I2C::_endTransmission(){
  Wire.endTransmission();
}