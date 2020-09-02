#include "Updater.h"
#include "eboot_command.h"
#include <esp8266_peri.h>
#include <erase_config.h>
#include "StackThunk.h"

//#define DEBUG_UPDATER Serial

#include <Updater_Signing.h>
#ifndef ARDUINO_SIGNING
  #define ARDUINO_SIGNING 0
#endif

#if ARDUINO_SIGNING
namespace esp8266 {
  extern UpdaterHashClass& updaterSigningHash;
  extern UpdaterVerifyClass& updaterSigningVerifier;
}
#endif

extern "C" {
    #include "c_types.h"
    #include "spi_flash.h"
    #include "user_interface.h"
}

extern "C" uint32_t _FS_start;
extern "C" uint32_t _FS_end;

UpdaterClass::UpdaterClass()
: _async(false)
, _error(0)
, _buffer(0)
, _bufferLen(0)
, _size(0)
, _startAddress(0)
, _currentAddress(0)
, _command(U_FLASH)
#ifdef ERASE_CONFIG_H
, _eraseConfigOption(ERASE_CONFIG_BLANK_BIN)
#endif
, _ledPin(-1)
, _hash(nullptr)
, _verify(nullptr)
, _progress_callback(nullptr)
{
#if ARDUINO_SIGNING
  installSignature(&esp8266::updaterSigningHash, &esp8266::updaterSigningVerifier);
  stack_thunk_add_ref();
#endif
}

UpdaterClass::~UpdaterClass()
{
#if ARDUINO_SIGNING
    stack_thunk_del_ref();
#endif
}

UpdaterClass& UpdaterClass::onProgress(THandlerFunction_Progress fn) {
    _progress_callback = fn;
    return *this;
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
#if defined(ERASE_CONFIG_H) && !defined(HOST_MOCK)
  // Empty call so erase_config.cpp module to ensure it is built and linked in.
  enable_erase_config_at_link_time();
#endif

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
  if (command == U_FS) {
    DEBUG_UPDATER.println(F("[begin] Update Filesystem."));
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
  _target_md5 = emptyString;
  _md5 = MD5Builder();

#ifndef HOST_MOCK
  wifi_set_sleep_type(NONE_SLEEP_T);
#endif

  //address where we will start writing the update
  uintptr_t updateStartAddress = 0;
  //size of current sketch rounded to a sector
  size_t currentSketchSize = (ESP.getSketchSize() + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));
  //size of the update rounded to a sector
  size_t roundedSize = (size + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));

  if (command == U_FLASH) {
    //address of the end of the space available for sketch and update
    uintptr_t updateEndAddress = (uintptr_t)&_FS_start - 0x40200000;

    updateStartAddress = (updateEndAddress > roundedSize)? (updateEndAddress - roundedSize) : 0;

#ifdef DEBUG_UPDATER
        DEBUG_UPDATER.printf_P(PSTR("[begin] roundedSize:       0x%08zX (%zd)\n"), roundedSize, roundedSize);
        DEBUG_UPDATER.printf_P(PSTR("[begin] updateEndAddress:  0x%08zX (%zd)\n"), updateEndAddress, updateEndAddress);
        DEBUG_UPDATER.printf_P(PSTR("[begin] currentSketchSize: 0x%08zX (%zd)\n"), currentSketchSize, currentSketchSize);
#endif

    //make sure that the size of both sketches is less than the total space (updateEndAddress)
    if(updateStartAddress < currentSketchSize) {
      _setError(UPDATE_ERROR_SPACE);
      return false;
    }
  }
  else if (command == U_FS) {
    if((uintptr_t)&_FS_start + roundedSize > (uintptr_t)&_FS_end) {
      _setError(UPDATE_ERROR_SPACE);
      return false;
    }

#ifdef ATOMIC_FS_UPDATE
    //address of the end of the space available for update
    uintptr_t updateEndAddress = (uintptr_t)&_FS_start - 0x40200000;

    updateStartAddress = (updateEndAddress > roundedSize)? (updateEndAddress - roundedSize) : 0;

    if(updateStartAddress < currentSketchSize) {
      _setError(UPDATE_ERROR_SPACE);
      return false;
    }
#else
    updateStartAddress = (uintptr_t)&_FS_start - 0x40200000;
#endif
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
  DEBUG_UPDATER.printf_P(PSTR("[begin] _startAddress:     0x%08X (%d)\n"), _startAddress, _startAddress);
  DEBUG_UPDATER.printf_P(PSTR("[begin] _currentAddress:   0x%08X (%d)\n"), _currentAddress, _currentAddress);
  DEBUG_UPDATER.printf_P(PSTR("[begin] _size:             0x%08zX (%zd)\n"), _size, _size);
#endif

  if (!_verify) {
    _md5.begin();
  }
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
    _reset();
    return false;
  }

  if(hasError() || (!isFinished() && !evenIfRemaining)){
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.printf_P(PSTR("premature end: res:%u, pos:%zu/%zu\n"), getError(), progress(), _size);
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

  uint32_t sigLen = 0;
  if (_verify) {
    ESP.flashRead(_startAddress + _size - sizeof(uint32_t), &sigLen, sizeof(uint32_t));
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.printf_P(PSTR("[Updater] sigLen: %d\n"), sigLen);
#endif
    if (sigLen != _verify->length()) {
      _setError(UPDATE_ERROR_SIGN);
      _reset();
      return false;
    }

    int binSize = _size - sigLen - sizeof(uint32_t) /* The siglen word */;
    _hash->begin();
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.printf_P(PSTR("[Updater] Adjusted binsize: %d\n"), binSize);
#endif
      // Calculate the MD5 and hash using proper size
    uint8_t buff[128];
    for(int i = 0; i < binSize; i += sizeof(buff)) {
      ESP.flashRead(_startAddress + i, (uint32_t *)buff, sizeof(buff));
      size_t read = std::min((int)sizeof(buff), binSize - i);
      _hash->add(buff, read);
    }
    _hash->end();
#ifdef DEBUG_UPDATER
    unsigned char *ret = (unsigned char *)_hash->hash();
    DEBUG_UPDATER.printf_P(PSTR("[Updater] Computed Hash:"));
    for (int i=0; i<_hash->len(); i++) DEBUG_UPDATER.printf(" %02x", ret[i]);
    DEBUG_UPDATER.printf("\n");
#endif
    uint8_t *sig = (uint8_t*)malloc(sigLen);
    if (!sig) {
      _setError(UPDATE_ERROR_SIGN);
      _reset();
      return false;
    }
    ESP.flashRead(_startAddress + binSize, (uint32_t *)sig, sigLen);
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.printf_P(PSTR("[Updater] Received Signature:"));
    for (size_t i=0; i<sigLen; i++) {
      DEBUG_UPDATER.printf(" %02x", sig[i]);
    }
    DEBUG_UPDATER.printf("\n");
#endif
    if (!_verify->verify(_hash, (void *)sig, sigLen)) {
      free(sig);
      _setError(UPDATE_ERROR_SIGN);
      _reset();
      return false;
    }
    free(sig);
#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.printf_P(PSTR("[Updater] Signature matches\n"));
#endif
  } else if (_target_md5.length()) {
    _md5.calculate();
    if (strcasecmp(_target_md5.c_str(), _md5.toString().c_str())) {
      _setError(UPDATE_ERROR_MD5);
      return false;
    }
#ifdef DEBUG_UPDATER
    else DEBUG_UPDATER.printf_P(PSTR("MD5 Success: %s\n"), _target_md5.c_str());
#endif
  }

  if(!_verifyEnd()) {
    _reset();
    return false;
  }

  if (_command == U_FLASH) {
    eboot_command ebcmd;
    memset(&ebcmd, 0, sizeof(ebcmd));
    ebcmd.action = ACTION_COPY_RAW;
    ebcmd.args[0] = _startAddress;
    ebcmd.args[1] = 0x00000;
    ebcmd.args[2] = _size;
#ifdef ERASE_CONFIG_H
    ebcmd.args[4] =  _eraseConfigOption;
    ebcmd.args[5] = ~_eraseConfigOption;
    ebcmd.args[6] =  _eraseConfigOption;
    ebcmd.args[7] = ~_eraseConfigOption;
#endif
    eboot_command_write(&ebcmd);

#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.printf_P(PSTR("Staged: address:0x%08X, size:0x%08zX\n"), _startAddress, _size);
#endif
  }
  else if (_command == U_FS) {
#ifdef ATOMIC_FS_UPDATE
    eboot_command ebcmd;
    memset(&ebcmd, 0, sizeof(ebcmd));
    ebcmd.action = ACTION_COPY_RAW;
    ebcmd.args[0] = _startAddress;
    ebcmd.args[1] = (uintptr_t)&_FS_start - 0x40200000;
    ebcmd.args[2] = _size;
    eboot_command_write(&ebcmd);
#endif

#ifdef DEBUG_UPDATER
    DEBUG_UPDATER.printf_P(PSTR("Filesystem: address:0x%08X, size:0x%08zX\n"), _startAddress, _size);
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
  //TODO - GZIP can't do this
  if ((_currentAddress == _startAddress + FLASH_MODE_PAGE) && (_buffer[0] != 0x1f) && (_command == U_FLASH)) {
    flashMode = ESP.getFlashChipMode();
    #ifdef DEBUG_UPDATER
      DEBUG_UPDATER.printf_P(PSTR("Header: 0x%1X %1X %1X %1X\n"), _buffer[0], _buffer[1], _buffer[2], _buffer[3]);
    #endif
    bufferFlashMode = ESP.magicFlashChipMode(_buffer[FLASH_MODE_OFFSET]);
    if (bufferFlashMode != flashMode) {
      #ifdef DEBUG_UPDATER
        DEBUG_UPDATER.printf_P(PSTR("Set flash mode from 0x%1X to 0x%1X\n"), bufferFlashMode, flashMode);
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
  if (!_verify) {
    _md5.add(_buffer, _bufferLen);
  }
  _currentAddress += _bufferLen;
  _bufferLen = 0;
  return true;
}

size_t UpdaterClass::write(uint8_t *data, size_t len) {
  if(hasError() || !isRunning())
    return 0;

  if(progress() + _bufferLen + len > _size) {
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
        if ((data != 0xE9) && (data != 0x1f)) {
            _currentAddress = (_startAddress + _size);
            _setError(UPDATE_ERROR_MAGIC_BYTE);
            return false;
        }
        return true;
    } else if(_command == U_FS) {
        // no check of FS possible with first byte.
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
	//
	// TODO: GZIP compresses the chipsize flags, so can't do check here
	if ((buf[0] == 0x1f) && (buf[1] == 0x8b)) {
            // GZIP, just assume OK
            return true;
        } else if (buf[0] != 0xE9) {
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
    } else if(_command == U_FS) {
        // FS is already over written checks make no sense any more.
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
    if (_progress_callback) {
        _progress_callback(0, _size);
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
        if(_progress_callback) {
            _progress_callback(progress(), _size);
        }
        yield();
    }
    if(_progress_callback) {
        _progress_callback(progress(), _size);
    }
    return written;
}

void UpdaterClass::_setError(int error){
  _error = error;
#ifdef DEBUG_UPDATER
  printError(DEBUG_UPDATER);
#endif
  _reset(); // Any error condition invalidates the entire update, so clear partial status
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
    out.printf_P(PSTR("MD5 Failed: expected:%s, calculated:%s\n"), _target_md5.c_str(), _md5.toString().c_str());
  } else if(_error == UPDATE_ERROR_SIGN){
    out.println(F("Signature verification failed"));
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
