#include "Updater.h"
#include "Arduino.h"
#include "eboot_command.h"
#include "interrupts.h"

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
}

bool UpdaterClass::begin(size_t size, int command) {
  if(_size > 0){
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.println("[begin] already running");
#endif
    return false;
  }

#ifdef DEBUG_UPDATER
  if (command == U_SPIFFS) {
    DEBUG_UPDATER.println("[begin] Update SPIFFS.");
  }
#endif

  if(size == 0) {
    _error = UPDATE_ERROR_SIZE;
#ifdef DEBUG_UPDATER
    printError(DEBUG_UPDATER);
#endif
    return false;
  }

  if(!ESP.checkFlashConfig(false)) {
    _error = UPDATE_ERROR_FLASH_CONFIG;
#ifdef DEBUG_UPDATER
    printError(DEBUG_UPDATER);
#endif
    return false;
  }

  _reset();
  _error = 0;

  wifi_set_sleep_type(NONE_SLEEP_T);

  uint32_t updateStartAddress = 0;
  if (command == U_FLASH) {
    //size of current sketch rounded to a sector
    uint32_t currentSketchSize = (ESP.getSketchSize() + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));
    //address of the end of the space available for sketch and update
    uint32_t updateEndAddress = (uint32_t)&_SPIFFS_start - 0x40200000;
    //size of the update rounded to a sector
    uint32_t roundedSize = (size + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));
    //address where we will start writing the update
    updateStartAddress = updateEndAddress - roundedSize;

#ifdef DEBUG_UPDATER
        DEBUG_UPDATER.printf("[begin] roundedSize:       0x%08X (%d)\n", roundedSize, roundedSize);
        DEBUG_UPDATER.printf("[begin] updateEndAddress:  0x%08X (%d)\n", updateEndAddress, updateEndAddress);
        DEBUG_UPDATER.printf("[begin] currentSketchSize: 0x%08X (%d)\n", currentSketchSize, currentSketchSize);
#endif

    //make sure that the size of both sketches is less than the total space (updateEndAddress)
    if(updateStartAddress < currentSketchSize) {
      _error = UPDATE_ERROR_SPACE;
#ifdef DEBUG_UPDATER
      printError(DEBUG_UPDATER);
#endif
      return false;
    }
  }
  else if (command == U_SPIFFS) {
     updateStartAddress = (uint32_t)&_SPIFFS_start - 0x40200000;
  }
  else {
    // unknown command
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.println("[begin] Unknown update command.");
#endif
    return false;
  }

  //initialize
  _startAddress = updateStartAddress;
  _currentAddress = _startAddress;
  _size = size;
  _buffer = new uint8_t[FLASH_SECTOR_SIZE];
  _command = command;

#ifdef DEBUG_UPDATER
  DEBUG_UPDATER.printf("[begin] _startAddress:     0x%08X (%d)\n", _startAddress, _startAddress);
  DEBUG_UPDATER.printf("[begin] _currentAddress:   0x%08X (%d)\n", _currentAddress, _currentAddress);
  DEBUG_UPDATER.printf("[begin] _size:             0x%08X (%d)\n", _size, _size);
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
    DEBUG_UPDATER.println("no update");
#endif
    return false;
  }

  if(hasError() || (!isFinished() && !evenIfRemaining)){
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.printf("premature end: res:%u, pos:%u/%u\n", getError(), progress(), _size);
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
    if(_target_md5 != _md5.toString()){
      _error = UPDATE_ERROR_MD5;
#ifdef DEBUG_UPDATER
      DEBUG_UPDATER.printf("MD5 Failed: expected:%s, calculated:%s\n", _target_md5.c_str(), _md5.toString().c_str());
#endif
      _reset();
      return false;
    }
#ifdef DEBUG_UPDATER
    else DEBUG_UPDATER.printf("MD5 Success: %s\n", _target_md5.c_str());
#endif
  }

  if(!_verifyEnd()) {
#ifdef DEBUG_UPDATER
    printError(DEBUG_UPDATER);
#endif
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
    DEBUG_UPDATER.printf("Staged: address:0x%08X, size:0x%08X\n", _startAddress, _size);
  }
  else if (_command == U_SPIFFS) {
    DEBUG_UPDATER.printf("SPIFFS: address:0x%08X, size:0x%08X\n", _startAddress, _size);
#endif
  }

  _reset();
  return true;
}

bool UpdaterClass::_writeBuffer(){

  if(!_async) yield();
  bool result = ESP.flashEraseSector(_currentAddress/FLASH_SECTOR_SIZE);
  if(!_async) yield();
  if (result) {
      result = ESP.flashWrite(_currentAddress, (uint32_t*) _buffer, _bufferLen);
  }
  if(!_async) yield();

  if (!result) {
    _error = UPDATE_ERROR_WRITE;
    _currentAddress = (_startAddress + _size);
#ifdef DEBUG_UPDATER
    printError(DEBUG_UPDATER);
#endif
    return false;
  }
  _md5.add(_buffer, _bufferLen);
  _currentAddress += _bufferLen;
  _bufferLen = 0;
  return true;
}

size_t UpdaterClass::write(uint8_t *data, size_t len) {
  size_t left = len;
  if(hasError() || !isRunning())
    return 0;

  if(len > remaining())
    len = remaining();

  while((_bufferLen + left) > FLASH_SECTOR_SIZE) {
    size_t toBuff = FLASH_SECTOR_SIZE - _bufferLen;
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
            _error = UPDATE_ERROR_MAGIC_BYTE;
            _currentAddress = (_startAddress + _size);
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
            _error = UPDATE_ERROR_READ;
            _currentAddress = (_startAddress);
            return false;
        }

        // check for valid first magic byte
        if(buf[0] != 0xE9) {
            _error = UPDATE_ERROR_MAGIC_BYTE;
            _currentAddress = (_startAddress);
            return false;
        }

        uint32_t bin_flash_size = ESP.magicFlashChipSize((buf[3] & 0xf0) >> 4);

        // check if new bin fits to SPI flash
        if(bin_flash_size > ESP.getFlashChipRealSize()) {
            _error = UPDATE_ERROR_NEW_FLASH_CONFIG;
            _currentAddress = (_startAddress);
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

    while(remaining()) {
        toRead = data.readBytes(_buffer + _bufferLen,  (FLASH_SECTOR_SIZE - _bufferLen));
        if(toRead == 0) { //Timeout
            delay(100);
            toRead = data.readBytes(_buffer + _bufferLen, (FLASH_SECTOR_SIZE - _bufferLen));
            if(toRead == 0) { //Timeout
                _error = UPDATE_ERROR_STREAM;
                _currentAddress = (_startAddress + _size);
#ifdef DEBUG_UPDATER
                printError(DEBUG_UPDATER);
#endif
                _reset();
                return written;
            }
        }
        _bufferLen += toRead;
        if((_bufferLen == remaining() || _bufferLen == FLASH_SECTOR_SIZE) && !_writeBuffer())
            return written;
        written += toRead;
        yield();
    }
    return written;
}

void UpdaterClass::printError(Stream &out){
  out.printf("ERROR[%u]: ", _error);
  if(_error == UPDATE_ERROR_OK){
    out.println("No Error");
  } else if(_error == UPDATE_ERROR_WRITE){
    out.println("Flash Write Failed");
  } else if(_error == UPDATE_ERROR_ERASE){
    out.println("Flash Erase Failed");
  } else if(_error == UPDATE_ERROR_READ){
    out.println("Flash Read Failed");
  } else if(_error == UPDATE_ERROR_SPACE){
    out.println("Not Enough Space");
  } else if(_error == UPDATE_ERROR_SIZE){
    out.println("Bad Size Given");
  } else if(_error == UPDATE_ERROR_STREAM){
    out.println("Stream Read Timeout");
  } else if(_error == UPDATE_ERROR_MD5){
    out.println("MD5 Check Failed");
  } else if(_error == UPDATE_ERROR_FLASH_CONFIG){
    out.printf("Flash config wrong real: %d IDE: %d\n", ESP.getFlashChipRealSize(), ESP.getFlashChipSize());
  } else if(_error == UPDATE_ERROR_NEW_FLASH_CONFIG){
    out.printf("new Flash config wrong real: %d\n", ESP.getFlashChipRealSize());
  } else if(_error == UPDATE_ERROR_MAGIC_BYTE){
    out.println("Magic byte is wrong, not 0xE9");
  } else {
    out.println("UNKNOWN");
  }
}

UpdaterClass Update;
