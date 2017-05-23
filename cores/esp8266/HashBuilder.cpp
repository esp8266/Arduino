#include <HashBuilder.h>
#include <Arduino.h>

uint8_t HashBuilder::_hexCharToByte(uint8_t c){
    return  (c >= 'a' && c <= 'f') ? (c - ((uint8_t)'a' - 0xa)) :
            (c >= 'A' && c <= 'F') ? (c - ((uint8_t)'A' - 0xA)) :
            (c >= '0' &&  c<= '9') ? (c - (uint8_t)'0') : 0;
}

void HashBuilder::begin(void){
    _init();
}

void HashBuilder::add(uint8_t * data, uint16_t len){
    _update(data, len);
}

void HashBuilder::calculate(void){
    _final();
}


void HashBuilder::addHexString(const char * data){
    uint16_t i, len = strlen(data);
    std::unique_ptr<uint8_t[]> tmp(new uint8_t[len/2]);
    if(tmp == NULL) {
        return;
    }
    for(i=0; i<len; i+=2) {
        uint8_t high = _hexCharToByte(data[i]);
        uint8_t low = _hexCharToByte(data[i+1]);
        tmp[i/2] = (high & 0x0F) << 4 | (low & 0x0F);
    }
    add(tmp.get(), len/2);
    return;
}

bool HashBuilder::addStream(Stream & stream, const size_t totalLen){
    const int BUFFER_SIZE = 512;
    std::unique_ptr<uint8_t[]> buf(new uint8_t[BUFFER_SIZE]);
    if(!buf){
       return false;
    }

    int maxLeft = totalLen;
    int bytesAvailable = stream.available();
    while((bytesAvailable > 0) && (maxLeft > 0)) {

        int bytesToRead = bytesAvailable;
        if(bytesToRead > maxLeft) bytesToRead = maxLeft ;
        if(bytesToRead > BUFFER_SIZE) bytesToRead = BUFFER_SIZE;

        int numBytesRead = stream.readBytes(buf.get(), bytesToRead);
        if(numBytesRead<1) {
            return false;
        }
        _update(buf.get(), numBytesRead);
        yield();

        maxLeft -= numBytesRead;
        bytesAvailable = stream.available();
    }
    return true;
}

// RESULTS
String HashBuilder::_byteVecToString( std::vector<uint8_t>& res){
  String resString;
  resString.reserve(res.size()*2);
  static constexpr char hex[] = "0123456789abcdef";
  for (uint8_t & c : res){
      resString += String(hex[c / 16]);
      resString += String(hex[c % 16]);
  }
  return resString;
}

String HashBuilder::toString(){
  return _byteVecToString(_result());
}

void HashBuilder::getBytes(uint8_t * output){
    auto res = _result();
    for(uint8_t i = 0; i < res.size(); i++){
        output[i] =  res[i];
    }
}

void HashBuilder::getChars(char * output){
  auto res = _result();
  for(uint8_t i = 0; i < res.size(); i++){
    sprintf(output + (i * 2), "%02x", res[i]);
  }
}
