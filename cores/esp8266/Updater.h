#ifndef ESP8266UPDATER_H
#define ESP8266UPDATER_H

#include <Arduino.h>
#include <flash_utils.h>
#include <MD5Builder.h>
#include <functional>
#include <erase_config.h>

#define UPDATE_ERROR_OK                 (0)
#define UPDATE_ERROR_WRITE              (1)
#define UPDATE_ERROR_ERASE              (2)
#define UPDATE_ERROR_READ               (3)
#define UPDATE_ERROR_SPACE              (4)
#define UPDATE_ERROR_SIZE               (5)
#define UPDATE_ERROR_STREAM             (6)
#define UPDATE_ERROR_MD5                (7)
#define UPDATE_ERROR_FLASH_CONFIG       (8)
#define UPDATE_ERROR_NEW_FLASH_CONFIG   (9)
#define UPDATE_ERROR_MAGIC_BYTE         (10)
#define UPDATE_ERROR_BOOTSTRAP          (11)
#define UPDATE_ERROR_SIGN               (12)

#define U_FLASH   0
#define U_FS      100
#define U_AUTH    200

#ifdef DEBUG_ESP_UPDATER
#ifdef DEBUG_ESP_PORT
#define DEBUG_UPDATER DEBUG_ESP_PORT
#endif
#endif

// Abstract class to implement whatever signing hash desired
class UpdaterHashClass {
  public:
    virtual void begin() = 0;
    virtual void add(const void *data, uint32_t len) = 0;
    virtual void end() = 0;
    virtual int len() = 0;
    virtual const void *hash() = 0;
    virtual const unsigned char *oid() = 0;
};

// Abstract class to implement a signature verifier
class UpdaterVerifyClass {
  public:
    virtual uint32_t length() = 0; // How many bytes of signature are expected
    virtual bool verify(UpdaterHashClass *hash, const void *signature, uint32_t signatureLen) = 0; // Verify, return "true" on success
};

class UpdaterClass {
  public:
    typedef std::function<void(size_t, size_t)> THandlerFunction_Progress;

    UpdaterClass();
    ~UpdaterClass();

    /* Optionally add a cryptographic signature verification hash and method */
    void installSignature(UpdaterHashClass *hash, UpdaterVerifyClass *verify) {  _hash = hash;  _verify = verify; }

    /*
      Call this to check the space needed for the update
      Will return false if there is not enough space
    */
    bool begin(size_t size, int command = U_FLASH, int ledPin = -1, uint8_t ledOn = LOW);

#ifdef ERASE_CONFIG_H
    /*
    */
    inline void setEraseConfigOption(ERASE_CONFIG_MASK_t eraseOption) {
      _eraseConfigOption = eraseOption;
    }
#endif
    /*
      Run Updater from asynchronous callbacs
    */
    void runAsync(bool async){ _async = async; }

    /*
      Writes a buffer to the flash and increments the address
      Returns the amount written
    */
    size_t write(uint8_t *data, size_t len);

    /*
      Writes the remaining bytes from the Stream to the flash
      Uses readBytes() and sets UPDATE_ERROR_STREAM on timeout
      Returns the bytes written
      Should be equal to the remaining bytes when called
      Usable for slow streams like Serial
    */
    size_t writeStream(Stream &data);

    /*
      If all bytes are written
      this call will write the config to eboot
      and return true
      If there is already an update running but is not finished and !evenIfRemaining
      or there is an error
      this will clear everything and return false
      the last error is available through getError()
      evenIfRemaining is helpful when you update without knowing the final size first
    */
    bool end(bool evenIfRemaining = false);

    /*
      Prints the last error to an output stream
    */
    void printError(Print &out);

    /*
      sets the expected MD5 for the firmware (hexString)
    */
    bool setMD5(const char * expected_md5);

    /*
      returns the MD5 String of the sucessfully ended firmware
    */
    String md5String(void){ return _md5.toString(); }

    /*
      populated the result with the md5 bytes of the sucessfully ended firmware
    */
    void md5(uint8_t * result){ return _md5.getBytes(result); }

    /*
      This callback will be called when Updater is receiving data
    */
    UpdaterClass& onProgress(THandlerFunction_Progress fn);

    //Helpers
    uint8_t getError(){ return _error; }
    void clearError(){ _error = UPDATE_ERROR_OK; }
    bool hasError(){ return _error != UPDATE_ERROR_OK; }
    bool isRunning(){ return _size > 0; }
    bool isFinished(){ return _currentAddress == (_startAddress + _size); }
    size_t size(){ return _size; }
    size_t progress(){ return _currentAddress - _startAddress; }
    size_t remaining(){ return _size - (_currentAddress - _startAddress); }

    /*
      Template to write from objects that expose
      available() and read(uint8_t*, size_t) methods
      faster than the writeStream method
      writes only what is available
    */
    template<typename T>
    size_t write(T &data){
      size_t written = 0;
      if (hasError() || !isRunning())
        return 0;

      size_t available = data.available();
      while(available) {
        if(_bufferLen + available > remaining()){
          available = remaining() - _bufferLen;
        }
        if(_bufferLen + available > _bufferSize) {
          size_t toBuff = _bufferSize - _bufferLen;
          data.read(_buffer + _bufferLen, toBuff);
          _bufferLen += toBuff;
          if(!_writeBuffer())
            return written;
          written += toBuff;
        } else {
          data.read(_buffer + _bufferLen, available);
          _bufferLen += available;
          written += available;
          if(_bufferLen == remaining()) {
            if(!_writeBuffer()) {
              return written;
            }
          }
        }
        if(remaining() == 0)
          return written;
        delay(1);
        available = data.available();
      }
      return written;
    }

  private:
    void _reset();
    bool _writeBuffer();

    bool _verifyHeader(uint8_t data);
    bool _verifyEnd();

    void _setError(int error);

    bool _async = false;
    uint8_t _error = 0;
    uint8_t *_buffer = nullptr;
    size_t _bufferLen = 0; // amount of data written into _buffer
    size_t _bufferSize = 0; // total size of _buffer
    size_t _size = 0;
    uint32_t _startAddress = 0;
    uint32_t _currentAddress = 0;
    uint32_t _command = U_FLASH;

    String _target_md5;
    MD5Builder _md5;

    int _ledPin = -1;
    uint8_t _ledOn;

#ifdef ERASE_CONFIG_H
    uint32_t _eraseConfigOption = ERASE_CONFIG_BLANK_BIN;
#endif
    // Optional signed binary verification
    UpdaterHashClass *_hash = nullptr;
    UpdaterVerifyClass *_verify = nullptr;
    // Optional progress callback function
    THandlerFunction_Progress _progress_callback = nullptr;
};

extern UpdaterClass Update;

#endif
