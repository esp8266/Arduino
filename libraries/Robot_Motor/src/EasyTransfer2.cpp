/******************************************************************
*  EasyTransfer Arduino Library 
*		details and example sketch: 
*			http://www.billporter.info/easytransfer-arduino-library/
*
*		Brought to you by:
*              Bill Porter
*              www.billporter.info
*
*		See Readme for other info and version history
*	
*  
*This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
<http://www.gnu.org/licenses/>
*
*This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. 
*To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or
*send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
******************************************************************/

#include "EasyTransfer2.h"




//Captures address and size of struct
void EasyTransfer2::begin(HardwareSerial *theSerial){
	_serial = theSerial;
	
	//dynamic creation of rx parsing buffer in RAM
	//rx_buffer = (uint8_t*) malloc(size);
	
	resetData();
}

void EasyTransfer2::writeByte(uint8_t dat){
	if(position<20)
		data[position++]=dat;
		size++;
}
void EasyTransfer2::writeInt(int dat){
	if(position<19){
		data[position++]=dat>>8;
		data[position++]=dat;
		size+=2;
	}
}
uint8_t EasyTransfer2::readByte(){
	if(position>=size)return 0;
	return data[position++];
}
int EasyTransfer2::readInt(){
	if(position+1>=size)return 0;
	int dat_1=data[position++]<<8;
	int dat_2=data[position++];
	int dat= dat_1 | dat_2;
	return dat;
}

void EasyTransfer2::resetData(){
	for(int i=0;i<20;i++){
		data[i]=0;
	}
	size=0;
	position=0;
}

//Sends out struct in binary, with header, length info and checksum
void EasyTransfer2::sendData(){
  uint8_t CS = size;
  _serial->write(0x06);
  _serial->write(0x85);
  _serial->write(size);
  for(int i = 0; i<size; i++){
    CS^=*(data+i);
    _serial->write(*(data+i));
	//Serial.print(*(data+i));
	//Serial.print(",");
  }
  //Serial.println("");
  _serial->write(CS);
  
  resetData();
}

boolean EasyTransfer2::receiveData(){
  
  //start off by looking for the header bytes. If they were already found in a previous call, skip it.
  if(rx_len == 0){
  //this size check may be redundant due to the size check below, but for now I'll leave it the way it is.
    if(_serial->available() >= 3){
	//this will block until a 0x06 is found or buffer size becomes less then 3.
      while(_serial->read() != 0x06) {
		//This will trash any preamble junk in the serial buffer
		//but we need to make sure there is enough in the buffer to process while we trash the rest
		//if the buffer becomes too empty, we will escape and try again on the next call
		if(_serial->available() < 3)
			return false;
		}
		//Serial.println("head");
      if (_serial->read() == 0x85){
        rx_len = _serial->read();
		//Serial.print("rx_len:");
		//Serial.println(rx_len);
		resetData();

		//make sure the binary structs on both Arduinos are the same size.
        /*if(rx_len != size){
          rx_len = 0;
          return false;
        }*/
      }
    }
	//Serial.println("nothing");
  }
  
  //we get here if we already found the header bytes, the struct size matched what we know, and now we are byte aligned.
  if(rx_len != 0){
	
    while(_serial->available() && rx_array_inx <= rx_len){
      data[rx_array_inx++] = _serial->read();
    }
    
    if(rx_len == (rx_array_inx-1)){
      //seem to have got whole message
      //last uint8_t is CS
      calc_CS = rx_len;
	  //Serial.print("len:");
	  //Serial.println(rx_len);
      for (int i = 0; i<rx_len; i++){
        calc_CS^=data[i];
		//Serial.print("m");
		//Serial.print(data[i]);
		//Serial.print(",");
      } 
	  //Serial.println();
      //Serial.print(data[rx_array_inx-1]);
	  //Serial.print(" ");
	  //Serial.println(calc_CS);
	  
      if(calc_CS == data[rx_array_inx-1]){//CS good
		//resetData();
        //memcpy(data,d,rx_len);
		for(int i=0;i<20;i++){
			//Serial.print(data[i]);
			//Serial.print(",");
		}
		//Serial.println("");
		size=rx_len;
		rx_len = 0;
		rx_array_inx = 0;
		return true;
		}
		
	  else{
  		//Serial.println("CS");
		resetData();
	  //failed checksum, need to clear this out anyway
		rx_len = 0;
		rx_array_inx = 0;
		return false;
	  }
        
    }
  }
  //Serial.print(rx_len);
  //Serial.print(" ");
  //Serial.print(rx_array_inx);
  //Serial.print(" ");
  //Serial.println("Short");
  return false;
}
