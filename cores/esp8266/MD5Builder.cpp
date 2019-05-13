#include <Arduino.h>
#include <MD5Builder.h>

uint8_t hex_char_to_byte(uint8_t c){
    return  (c >= 'a' && c <= 'f') ? (c - ((uint8_t)'a' - 0xa)) :
            (c >= 'A' && c <= 'F') ? (c - ((uint8_t)'A' - 0xA)) :
            (c >= '0' &&  c<= '9') ? (c - (uint8_t)'0') : 0;
}

void MD5Builder::begin(void){
    memset(_buf, 0x00, 16);
    MD5Init(&_ctx);
}

void MD5Builder::add(const uint8_t * data, const uint16_t len){
    MD5Update(&_ctx, data, len);
}

void MD5Builder::addHexString(const char * data){
    uint16_t i, len = strlen(data);
    uint8_t * tmp = (uint8_t*)malloc(len/2);
    if(tmp == NULL) {
        return;
    }
    for(i=0; i<len; i+=2) {
        uint8_t high = hex_char_to_byte(data[i]);
        uint8_t low = hex_char_to_byte(data[i+1]);
        tmp[i/2] = (high & 0x0F) << 4 | (low & 0x0F);
    }
    add(tmp, len/2);
    free(tmp);
}

bool MD5Builder::addStream(Stream & stream, const size_t maxLen){
    const int buf_size = 512;
    int maxLengthLeft = maxLen;
    uint8_t * buf = (uint8_t*) malloc(buf_size);

    if(!buf) {
        return false;
    }

    int bytesAvailable = stream.available();
    while((bytesAvailable > 0) && (maxLengthLeft > 0)) {

        // determine number of bytes to read
        int readBytes = bytesAvailable;
        if(readBytes > maxLengthLeft) {
            readBytes = maxLengthLeft ;    // read only until max_len
        }
        if(readBytes > buf_size) {
            readBytes = buf_size;    // not read more the buffer can handle
        }

        // read data and check if we got something
        int numBytesRead = stream.readBytes(buf, readBytes);
        if(numBytesRead< 1) {
            return false;
        }

        // Update MD5 with buffer payload
        MD5Update(&_ctx, buf, numBytesRead);

        yield();      // time for network streams

        // update available number of bytes
        maxLengthLeft -= numBytesRead;
        bytesAvailable = stream.available();
    }
    free(buf);
    return true;
}

void MD5Builder::calculate(void){
    MD5Final(_buf, &_ctx);
}

void MD5Builder::getBytes(uint8_t * output){
    memcpy(output, _buf, 16);
}

void MD5Builder::getChars(char * output){
    for(uint8_t i = 0; i < 16; i++) {
        sprintf(output + (i * 2), "%02x", _buf[i]);
    }
}

String MD5Builder::toString(void){
    char out[33];
    getChars(out);
    return String(out);
}
