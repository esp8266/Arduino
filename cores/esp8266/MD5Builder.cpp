#include "Arduino.h"
#include "md5.h"
#include "MD5Builder.h"

#define hex_char_to_byte(c) (((c)>='a'&&(c)<='f')?((c)-87):((c)>='A'&&(c)<='F')?((c)-55):((c)>='0'&&(c)<='9')?((c)-48):0)

void MD5Builder::begin(void){
  memset(_buf, 0x00, 16);
  MD5Init(&_ctx);
}

void MD5Builder::add(uint8_t * data, uint16_t len){
  MD5Update(&_ctx, data, len);
}

void MD5Builder::addHexString(const char * data){
  uint16_t i, len = strlen(data);
  uint8_t * tmp = (uint8_t*)malloc(len/2);
  if(tmp == NULL)
    return;
  for(i=0; i<len; i+=2) tmp[i/2] = (hex_char_to_byte(data[i]) & 0x0F) << 4 | (hex_char_to_byte(data[i+1]) & 0x0F);
  add(tmp, len/2);
  free(tmp);
}

bool MD5Builder::addStream(Stream & stream, const size_t total_len) {
  const int buf_size = 512;
  int bytesleft = total_len;
  uint8_t * buf = (uint8_t*) malloc(buf_size);
  if(buf) {
    while((stream.available() > -1) && (bytesleft > 0)) {
      // get available data size
      int sizeAvailable = stream.available();
      if(sizeAvailable) {
        int readBytes = sizeAvailable;

        // read only the asked bytes
        if(readBytes > bytesleft) {
          readBytes = bytesleft ;
        }

        // not read more the buffer can handle
        if(readBytes > buf_size) {
          readBytes = buf_size;
        }

        // read data
        int bytesread = stream.readBytes(buf, readBytes);
        bytesleft -= bytesread;
        if(bytesread > 0) {
          MD5Update(&_ctx, buf, bytesread);
        }
      }
      // time for network streams
      delay(0);
    }
    // guaranteed not null
    free(buf);
    return (bytesleft == 0);
  } else {
    return false;
  }
}

void MD5Builder::calculate(void){
  MD5Final(_buf, &_ctx);
}

void MD5Builder::getBytes(uint8_t * output){
  memcpy(output, _buf, 16);
}

void MD5Builder::getChars(char * output){
  for(uint8_t i = 0; i < 16; i++)
    sprintf(output + (i * 2), "%02x", _buf[i]);
}

String MD5Builder::toString(void){
  char out[33];
  getChars(out);
  return String(out);
}