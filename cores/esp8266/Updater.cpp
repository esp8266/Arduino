#include "Updater.h"
#include "Arduino.h"
#include "eboot_command.h"
#include <interrupts.h>
#include <esp8266_peri.h>

//#define DEBUG_UPDATER Serial

extern "C" {
    #include "c_types.h"
    #include "spi_flash.h"
    #include "user_interface.h"
}

extern "C" uint32_t _SPIFFS_start;

UpdaterClass::UpdaterClass()
: _async(false)
, _error(0)
, _buffer(0)
, _bufferLen(0)
, _size(0)
, _startAddress(0)
, _currentAddress(0)
, _command(U_FLASH)
{
}

void UpdaterClass::_reset() {
  if (_buffer)
    delete[] _buffer;
  _buffer = 0;
  _bufferLen = 0;
  _startAddress = 0;
  _currentAddress = 0;
  _size = 0;
  _command = U_FLASH;

  if(_ledPin != -1) {
    digitalWrite(_ledPin, !_ledOn); // off
  }
}

bool UpdaterClass::begin(size_t size, int command, int ledPin, uint8_t ledOn) {
  if(_size > 0){
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.println(F("[begin] already running"));
#endif
    return false;
  }

  _ledPin = ledPin;
  _ledOn = !!ledOn; // 0(LOW) or 1(HIGH)

  /* Check boot mode; if boot mode is 1 (UART download mode),
    we will not be able to reset into normal mode once update is done.
    Fail early to avoid frustration.
    https://github.com/esp8266/Arduino/issues/1017#issuecomment-200605576
  */
  int boot_mode = (GPI >> 16) & 0xf;
  if (boot_mode == 1) {
    _setError(UPDATE_ERROR_BOOTSTRAP);
    return false;
  }
  
#ifdef DEBUG_UPDATER
  if (command == U_SPIFFS) {
    DEBUG_UPDATER.println(F("[begin] Update SPIFFS."));
  }
#endif

  if(size == 0) {
    _setError(UPDATE_ERROR_SIZE);
    return false;
  }

  if(!ESP.checkFlashConfig(false)) {
    _setError(UPDATE_ERROR_FLASH_CONFIG);
    return false;
  }

  _reset();
  clearError(); //  _error = 0

  wifi_set_sleep_type(NONE_SLEEP_T);

  uintptr_t updateStartAddress = 0;
  if (command == U_FLASH) {
    //size of current sketch rounded to a sector
    size_t currentSketchSize = (ESP.getSketchSize() + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));
    //address of the end of the space available for sketch and update
    uintptr_t updateEndAddress = (uintptr_t)&_SPIFFS_start - 0x40200000;
    //size of the update rounded to a sector
    size_t roundedSize = (size + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));
    //address where we will start writing the update
    updateStartAddress = (updateEndAddress > roundedSize)? (updateEndAddress - roundedSize) : 0;

#ifdef DEBUG_UPDATER
        DEBUG_UPDATER.printf("[begin] roundedSize:       0x%08zX (%zd)\n", roundedSize, roundedSize);
        DEBUG_UPDATER.printf("[begin] updateEndAddress:  0x%08zX (%zd)\n", updateEndAddress, updateEndAddress);
        DEBUG_UPDATER.printf("[begin] currentSketchSize: 0x%08zX (%zd)\n", currentSketchSize, currentSketchSize);
#endif

    //make sure that the size of both sketches is less than the total space (updateEndAddress)
    if(updateStartAddress < currentSketchSize) {
      _setError(UPDATE_ERROR_SPACE);    
      return false;
    }
  }
  else if (command == U_SPIFFS) {
     updateStartAddress = (uintptr_t)&_SPIFFS_start - 0x40200000;
  }
  else {
    // unknown command
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.println(F("[begin] Unknown update command."));
#endif
    return false;
  }

  //initialize
  _startAddress = updateStartAddress;
  _currentAddress = _startAddress;
  _size = size;
  if (ESP.getFreeHeap() > 2 * FLASH_SECTOR_SIZE) {
    _bufferSize = FLASH_SECTOR_SIZE;
  } else {
    _bufferSize = 256;
  }
  _buffer = new uint8_t[_bufferSize];
  _command = command;

#ifdef DEBUG_UPDATER
  DEBUG_UPDATER.printf("[begin] _startAddress:     0x%08X (%d)\n", _startAddress, _startAddress);
  DEBUG_UPDATER.printf("[begin] _currentAddress:   0x%08X (%d)\n", _currentAddress, _currentAddress);
  DEBUG_UPDATER.printf("[begin] _size:             0x%08zX (%zd)\n", _size, _size);
#endif

  _md5.begin();
  return true;
}

bool UpdaterClass::setMD5(const char * expected_md5){
  if(strlen(expected_md5) != 32)
  {
    return false;
  }
  _target_md5 = expected_md5;
  return true;
}

bool UpdaterClass::end(bool evenIfRemaining){
  if(_size == 0){
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.println(F("no update"));
#endif
    return false;
  }

  if(hasError() || (!isFinished() && !evenIfRemaining)){
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.printf("premature end: res:%u, pos:%zu/%zu\n", getError(), progress(), _size);
#endif

    _reset();
    return false;
  }

  if(evenIfRemaining) {
    if(_bufferLen > 0) {
      _writeBuffer();
    }
    _size = progress();
  }

  _md5.calculate();
  if(_target_md5.length()) {
    if(strcasecmp(_target_md5.c_str(), _md5.toString().c_str()) != 0){
      _setError(UPDATE_ERROR_MD5);
      _reset();
      return false;
    }
#ifdef DEBUG_UPDATER
    else DEBUG_UPDATER.printf("MD5 Success: %s\n", _target_md5.c_str());
#endif
  }

  if(!_verifyEnd()) {
    _reset();
    return false;
  }

  if (_command == U_FLASH) {
    eboot_command ebcmd;
    ebcmd.action = ACTION_COPY_RAW;
    ebcmd.args[0] = _startAddress;
    ebcmd.args[1] = 0x00000;
    ebcmd.args[2] = _size;
    eboot_command_write(&ebcmd);

#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.printf("Staged: address:0x%08X, size:0x%08zX\n", _startAddress, _size);
  }
  else if (_command == U_SPIFFS) {
    DEBUG_UPDATER.printf("SPIFFS: address:0x%08X, size:0x%08zX\n", _startAddress, _size);
#endif
  }

  _reset();
  return true;
}

bool UpdaterClass::_writeBuffer(){
  #define FLASH_MODE_PAGE  0
  #define FLASH_MODE_OFFSET  2

  bool eraseResult = true, writeResult = true;
  if (_currentAddress % FLASH_SECTOR_SIZE == 0) {
    if(!_async) yield();
    eraseResult = ESP.flashEraseSector(_currentAddress/FLASH_SECTOR_SIZE);
  }

  // If the flash settings don't match what we already have, modify them.
  // But restore them after the modification, so the hash isn't affected.
  // This is analogous to what esptool.py does when it receives a --flash_mode argument.
  bool modifyFlashMode = false;
  FlashMode_t flashMode = FM_QIO;
  FlashMode_t bufferFlashMode = FM_QIO;
  if (_currentAddress == _startAddress + FLASH_MODE_PAGE) {
    flashMode = ESP.getFlashChipMode();
    #ifdef DEBUG_UPDATER
      DEBUG_UPDATER.printf("Header: 0x%1X %1X %1X %1X\n", _buffer[0], _buffer[1], _buffer[2], _buffer[3]);
    #endif
    bufferFlashMode = ESP.magicFlashChipMode(_buffer[FLASH_MODE_OFFSET]);
    if (bufferFlashMode != flashMode) {
      #ifdef DEBUG_UPDATER
        DEBUG_UPDATER.printf("Set flash mode from 0x%1X to 0x%1X\n", bufferFlashMode, flashMode);
      #endif

      _buffer[FLASH_MODE_OFFSET] = flashMode;
      modifyFlashMode = true;
    }
  }
  
  if (eraseResult) {
    if(!_async) yield();
    writeResult = ESP.flashWrite(_currentAddress, (uint32_t*) _buffer, _bufferLen);
  } else { // if erase was unsuccessful
    _currentAddress = (_startAddress + _size);
    _setError(UPDATE_ERROR_ERASE);
    return false;
  }

  // Restore the old flash mode, if we modified it.
  // Ensures that the MD5 hash will still match what was sent.
  if (modifyFlashMode) {
    _buffer[FLASH_MODE_OFFSET] = bufferFlashMode;
  }

  if (!writeResult) {
    _currentAddress = (_startAddress + _size);
    _setError(UPDATE_ERROR_WRITE);
    return false;
  }
  _md5.add(_buffer, _bufferLen);
  _currentAddress += _bufferLen;
  _bufferLen = 0;
  return true;
}

size_t UpdaterClass::write(uint8_t *data, size_t len) {
  if(hasError() || !isRunning())
    return 0;

  if(len > remaining()){
    //len = remaining();
    //fail instead
    _setError(UPDATE_ERROR_SPACE);
    return 0;
  }

  size_t left = len;

  while((_bufferLen + left) > _bufferSize) {
    size_t toBuff = _bufferSize - _bufferLen;
    memcpy(_buffer + _bufferLen, data + (len - left), toBuff);
    _bufferLen += toBuff;
    if(!_writeBuffer()){
      return len - left;
    }
    left -= toBuff;
    if(!_async) yield();
  }
  //lets see whats left
  memcpy(_buffer + _bufferLen, data + (len - left), left);
  _bufferLen += left;
  if(_bufferLen == remaining()){
    //we are at the end of the update, so should write what's left to flash
    if(!_writeBuffer()){
      return len - left;
    }
  }
  return len;
}

bool UpdaterClass::_verifyHeader(uint8_t data) {
    if(_command == U_FLASH) {
        // check for valid first magic byte (is always 0xE9)
        if(data != 0xE9) {
            _currentAddress = (_startAddress + _size);
            _setError(UPDATE_ERROR_MAGIC_BYTE);
            return false;
        }
        return true;
    } else if(_command == U_SPIFFS) {
        // no check of SPIFFS possible with first byte.
        return true;
    }
    return false;
}

bool UpdaterClass::_verifyEnd() {
    if(_command == U_FLASH) {

        uint8_t buf[4];
        if(!ESP.flashRead(_startAddress, (uint32_t *) &buf[0], 4)) {
            _currentAddress = (_startAddress);
            _setError(UPDATE_ERROR_READ);            
            return false;
        }

        // check for valid first magic byte
        if(buf[0] != 0xE9) {
            _currentAddress = (_startAddress);
            _setError(UPDATE_ERROR_MAGIC_BYTE);            
            return false;
        }

        uint32_t bin_flash_size = ESP.magicFlashChipSize((buf[3] & 0xf0) >> 4);

        // check if new bin fits to SPI flash
        if(bin_flash_size > ESP.getFlashChipRealSize()) {
            _currentAddress = (_startAddress);
            _setError(UPDATE_ERROR_NEW_FLASH_CONFIG);            
            return false;
        }

        return true;
    } else if(_command == U_SPIFFS) {
        // SPIFFS is already over written checks make no sense any more.
        return true;
    }
    return false;
}

size_t UpdaterClass::writeStream(Stream &data) {
    size_t written = 0;
    size_t toRead = 0;
    if(hasError() || !isRunning())
        return 0;

    if(!_verifyHeader(data.peek())) {
#ifdef DEBUG_UPDATER
        printError(DEBUG_UPDATER);
#endif
        _reset();
        return 0;
    }

    if(_ledPin != -1) {
        pinMode(_ledPin, OUTPUT);
    }

    while(remaining()) {
        if(_ledPin != -1) {
            digitalWrite(_ledPin, _ledOn); // Switch LED on
        }
        size_t bytesToRead = _bufferSize - _bufferLen;
        if(bytesToRead > remaining()) {
            bytesToRead = remaining();
        }
        toRead = data.readBytes(_buffer + _bufferLen,  bytesToRead);
        if(toRead == 0) { //Timeout
            delay(100);
            toRead = data.readBytes(_buffer + _bufferLen, bytesToRead);
            if(toRead == 0) { //Timeout
                _currentAddress = (_startAddress + _size);
                _setError(UPDATE_ERROR_STREAM);
                _reset();
                return written;
            }
        }
        if(_ledPin != -1) {
            digitalWrite(_ledPin, !_ledOn); // Switch LED off
        }
        _bufferLen += toRead;
        if((_bufferLen == remaining() || _bufferLen == _bufferSize) && !_writeBuffer())
            return written;
        written += toRead;
        yield();
    }
    return written;
}

void UpdaterClass::_setError(int error){
  _error = error;
#ifdef DEBUG_UPDATER
  printError(DEBUG_UPDATER);
#endif
}

void UpdaterClass::printError(Print &out){
  out.printf_P(PSTR("ERROR[%u]: "), _error);
  if(_error == UPDATE_ERROR_OK){
    out.println(F("No Error"));
  } else if(_error == UPDATE_ERROR_WRITE){
    out.println(F("Flash Write Failed"));
  } else if(_error == UPDATE_ERROR_ERASE){
    out.println(F("Flash Erase Failed"));
  } else if(_error == UPDATE_ERROR_READ){
    out.println(F("Flash Read Failed"));
  } else if(_error == UPDATE_ERROR_SPACE){
    out.println(F("Not Enough Space"));
  } else if(_error == UPDATE_ERROR_SIZE){
    out.println(F("Bad Size Given"));
  } else if(_error == UPDATE_ERROR_STREAM){
    out.println(F("Stream Read Timeout"));
  } else if(_error == UPDATE_ERROR_MD5){
    //out.println(F("MD5 Check Failed"));
    out.printf("MD5 Failed: expected:%s, calculated:%s\n", _target_md5.c_str(), _md5.toString().c_str());
  } else if(_error == UPDATE_ERROR_FLASH_CONFIG){
    out.printf_P(PSTR("Flash config wrong real: %d IDE: %d\n"), ESP.getFlashChipRealSize(), ESP.getFlashChipSize());
  } else if(_error == UPDATE_ERROR_NEW_FLASH_CONFIG){
    out.printf_P(PSTR("new Flash config wrong real: %d\n"), ESP.getFlashChipRealSize());
  } else if(_error == UPDATE_ERROR_MAGIC_BYTE){
    out.println(F("Magic byte is wrong, not 0xE9"));
  } else if (_error == UPDATE_ERROR_BOOTSTRAP){
    out.println(F("Invalid bootstrapping state, reset ESP8266 before updating"));
  } else {
    out.println(F("UNKNOWN"));
  }
}

UpdaterClass Update;
