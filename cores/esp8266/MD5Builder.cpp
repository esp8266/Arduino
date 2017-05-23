#include <Arduino.h>
#include <MD5Builder.h>

void MD5Builder::_init(void){
  _resultVector = {};
  MD5Init(&_ctx);
  return;
}

void MD5Builder::_update(uint8_t * data, uint16_t len){
    MD5Update(&_ctx, data, len);
    return;
}

void MD5Builder::_final(void){
    _resultVector.resize(HEX_STR_LEN);
    MD5Final(_resultVector.data(), &_ctx);
    return;
}
