#ifndef __ESP8266_HASH_BUILDER__
#define __ESP8266_HASH_BUILDER__

#include <WString.h>
#include <Stream.h>
#include <memory>
#include <vector>

class HashBuilder{
  public:
    void begin(void);
    void add(uint8_t * data, uint16_t len);
    void calculate(void);

    void addHexString(const char * data);
    bool addStream(Stream & stream, const size_t maxLen);
    String toString(void);

    // deprecated
    void getBytes(uint8_t * output );
    void getChars(char * output);

    // INLINE
    void add(char * data){ add((const char*)data); }
    void add(String data){ add(data.c_str()); }
    void addHexString(char * data){ addHexString((const char*)data); }
    void addHexString(String data){ addHexString(data.c_str()); }
    void add(const char * data){ add((uint8_t*)data, strlen(data)); }

  protected:
    virtual void _init(void) = 0;
    virtual void _update(uint8_t * data, uint16_t len) = 0;
    virtual void _final() = 0;
    virtual std::vector<uint8_t> &_result() = 0;

    uint8_t _hexCharToByte(uint8_t c);
    String _byteVecToString( std::vector<uint8_t>& v);
};

// might be renamed to cryptoBuilder, cause same baseclass
// might be reused in CrpytoSubmodule
typedef HashBuilder CryptoBuilder;

#endif
