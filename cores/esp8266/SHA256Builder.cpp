#include <SHA256Builder.h>

void SHA256Builder::_init(void){
  _resultVector = {};
  SHA256_Init(&_ctx);
  return;
}

void SHA256Builder::_update(uint8_t * data, uint16_t len){
    SHA256_Update(&_ctx, data, len);
    return;
}

void SHA256Builder::_final(void){
    _resultVector.resize(HEX_STR_LEN);
    SHA256_Final(_resultVector.data(), &_ctx);
    return;
}
