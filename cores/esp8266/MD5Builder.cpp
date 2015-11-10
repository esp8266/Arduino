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
  char out[32];
  getChars(out);
  return String(out);
}