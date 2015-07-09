#ifndef ESP8266UPDATER_H
#define ESP8266UPDATER_H

#include "Arduino.h"
#include "flash_utils.h"

#define UPDATE_ERROR_OK     0
#define UPDATE_ERROR_WRITE  1
#define UPDATE_ERROR_ERASE  2
#define UPDATE_ERROR_SPACE  3
#define UPDATE_ERROR_SIZE   4
#define UPDATE_ERROR_STREAM 5

class UpdaterClass {
  public:
    UpdaterClass();
    /*
      Call this to check the space needed for the update
      Will return false if there is not enough space
    */
    bool begin(size_t size);
    
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
      If there is already an update running but is not finished and !evenIfRemainanig
      or there is an error
      this will clear everything and return false
      the last error is available through getError()
      evenIfRemaining is helpfull when you update without knowing the final size first
    */
    bool end(bool evenIfRemaining = false);
    
    /*
      Prints the last error to an output stream
    */
    void printError(Stream &out);
    
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
        if(_bufferLen + available > FLASH_SECTOR_SIZE) {
          size_t toBuff = FLASH_SECTOR_SIZE - _bufferLen;
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
        yield();
        available = data.available();
      }
      return written;
    }
    
  private:
    void _reset();
    bool _writeBuffer();

    uint8_t *_buffer;
    size_t _bufferLen;
    size_t _size;
    uint32_t _startAddress;
    uint32_t _currentAddress;
    uint8_t _error;
};

extern UpdaterClass Update;

#endif
