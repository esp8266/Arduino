#ifndef __ESP8266_SHA256_BUILDER__
#define __ESP8266_SHA256_BUILDER__

#include <WString.h>
#include <Stream.h>
#include "sha256.h"

class SHA256Builder {
  private:
    SHA256_CTX _ctx;
    uint8_t _buf[32];
    uint8_t hex_char_to_byte(uint8_t c);
  public:
    void begin(void);
    void add(const uint8_t * data, const uint16_t len);
    void add(const char * data){ add((const uint8_t*)data, strlen(data)); }
    void add(char * data){ add((const char*)data); }
    void add(const String data){ add(data.c_str()); }
    void addHexString(const char * data);
    void addHexString(char * data){ addHexString((const char*)data); }
    void addHexString(const String data){ addHexString(data.c_str()); }
    bool addStream(Stream & stream, const size_t maxLen);
    void calculate(void);
    void getBytes(uint8_t * output);
    void getChars(char * output);
    String toString(void);
};


#endif
