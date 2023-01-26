#ifndef LWIP_OPEN_SRC
#define LWIP_OPEN_SRC
#endif
#include <functional>
#include <WiFiUdp.h>
#include "ArduinoOTA.h"
#include "MD5Builder.h"
#include "StreamString.h"

extern "C" {
  #include "osapi.h"
  #include "ets_sys.h"
  #include "user_interface.h"
}

#include "lwip/opt.h"
#include "lwip/udp.h"
#include "lwip/inet.h"
#include "lwip/igmp.h"
#include "lwip/mem.h"
#include "include/UdpContext.h"

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MDNS)
#include <ESP8266mDNS.h>
#endif

#if defined(DEBUG_ESP_OTA) && defined(DEBUG_ESP_PORT)
#define OTA_DEBUG DEBUG_ESP_PORT
#define OTA_DEBUG_PRINTF(fmt, ...) OTA_DEBUG.printf_P(PSTR(fmt),  ##__VA_ARGS__)
#else
#define OTA_DEBUG_PRINTF(...)
#endif

ArduinoOTAClass::ArduinoOTAClass()
{
}

ArduinoOTAClass::~ArduinoOTAClass(){
  if(_udp_ota){
    _udp_ota->unref();
    _udp_ota = 0;
  }
}

void ArduinoOTAClass::onStart(THandlerFunction fn) {
    _start_callback = fn;
}

void ArduinoOTAClass::onEnd(THandlerFunction fn) {
    _end_callback = fn;
}

void ArduinoOTAClass::onProgress(THandlerFunction_Progress fn) {
    _progress_callback = fn;
}

void ArduinoOTAClass::onError(THandlerFunction_Error fn) {
    _error_callback = fn;
}

void ArduinoOTAClass::setPort(uint16_t port) {
  if (!_initialized && !_port && port) {
    _port = port;
  }
}

void ArduinoOTAClass::setHostname(const char * hostname) {
  if (!_initialized && !_hostname.length() && hostname) {
    _hostname = hostname;
  }
}

String ArduinoOTAClass::getHostname() {
  return _hostname;
}

void ArduinoOTAClass::setPassword(const char * password) {
  if (!_initialized && !_password.length() && password) {
    MD5Builder passmd5;
    passmd5.begin();
    passmd5.add(password);
    passmd5.calculate();
    _password = passmd5.toString();
  }
}

void ArduinoOTAClass::setPasswordHash(const char * password) {
  if (!_initialized && !_password.length() && password) {
    _password = password;
  }
}

void ArduinoOTAClass::setRebootOnSuccess(bool reboot, bool eraseConfig){
  _rebootOnSuccess = reboot;
  _eraseConfig = eraseConfig;
}

void ArduinoOTAClass::begin(bool useMDNS) {
  if (_initialized)
    return;

  _useMDNS = useMDNS;

  if (!_hostname.length()) {
    char tmp[15];
    sprintf(tmp, "esp8266-%06x", ESP.getChipId());
    _hostname = tmp;
  }
  if (!_port) {
    _port = 8266;
  }

  if(_udp_ota){
    _udp_ota->unref();
    _udp_ota = 0;
  }

  _udp_ota = new UdpContext;
  _udp_ota->ref();

  if(!_udp_ota->listen(IP_ADDR_ANY, _port))
    return;
  _udp_ota->onRx(std::bind(&ArduinoOTAClass::_onRx, this));

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MDNS)
  if(_useMDNS) {
    MDNS.begin(_hostname.c_str());

    if (_password.length()) {
      MDNS.enableArduino(_port, true);
    } else {
      MDNS.enableArduino(_port);
    }
  }
#endif
  _initialized = true;
  _state = OTA_IDLE;
  OTA_DEBUG_PRINTF("OTA server at: %s.local:%u\n", _hostname.c_str(), _port);
}

int ArduinoOTAClass::parseInt(){
  char data[16];
  uint8_t index;
  char value;
  while(_udp_ota->peek() == ' ') _udp_ota->read();
  for(index = 0; index < sizeof(data); ++index){
    value = _udp_ota->peek();
    if(value < '0' || value > '9'){
      data[index] = '\0';
      return atoi(data);
    }
    data[index] = _udp_ota->read();
  }
  return 0;
}

String ArduinoOTAClass::readStringUntil(char end){
  String res;
  int value;
  while(true){
    value = _udp_ota->read();
    if(value < 0 || value == '\0' || value == end){
      return res;
    }
    res += static_cast<char>(value);
  }
  return res;
}

void ArduinoOTAClass::_onRx(){
  if(!_udp_ota->next()) return;
  IPAddress ota_ip;

  if (_state == OTA_IDLE) {
    int cmd = parseInt();
    if (cmd != U_FLASH && cmd != U_FS)
      return;
    _ota_ip = _udp_ota->getRemoteAddress();
    _cmd  = cmd;
    _ota_port = parseInt();
    _ota_udp_port = _udp_ota->getRemotePort();
    _size = parseInt();
    _udp_ota->read();
    _md5 = readStringUntil('\n');
    _md5.trim();
    if(_md5.length() != 32)
      return;

    ota_ip = _ota_ip;

    if (_password.length()){
      MD5Builder nonce_md5;
      nonce_md5.begin();
      nonce_md5.add(String(micros()));
      nonce_md5.calculate();
      _nonce = nonce_md5.toString();

      char auth_req[38];
      sprintf(auth_req, "AUTH %s", _nonce.c_str());
      _udp_ota->append((const char *)auth_req, strlen(auth_req));
      _udp_ota->send(ota_ip, _ota_udp_port);
      _state = OTA_WAITAUTH;
      return;
    } else {
      _state = OTA_RUNUPDATE;
    }
  } else if (_state == OTA_WAITAUTH) {
    int cmd = parseInt();
    if (cmd != U_AUTH) {
      _state = OTA_IDLE;
      return;
    }
    _udp_ota->read();
    String cnonce = readStringUntil(' ');
    String response = readStringUntil('\n');
    if (cnonce.length() != 32 || response.length() != 32) {
      _state = OTA_IDLE;
      return;
    }

    String challenge = _password + ':' + String(_nonce) + ':' + cnonce;
    MD5Builder _challengemd5;
    _challengemd5.begin();
    _challengemd5.add(challenge);
    _challengemd5.calculate();
    String result = _challengemd5.toString();

    ota_ip = _ota_ip;
    if(result.equalsConstantTime(response)) {
      _state = OTA_RUNUPDATE;
    } else {
      _udp_ota->append("Authentication Failed", 21);
      _udp_ota->send(ota_ip, _ota_udp_port);
      if (_error_callback) _error_callback(OTA_AUTH_ERROR);
      _state = OTA_IDLE;
    }
  }

  while(_udp_ota->next()) _udp_ota->flush();
}

void ArduinoOTAClass::_runUpdate() {
  IPAddress ota_ip = _ota_ip;

  if (!Update.begin(_size, _cmd)) {
    OTA_DEBUG_PRINTF("Update Begin Error\n");
    if (_error_callback) {
      _error_callback(OTA_BEGIN_ERROR);
    }

    StreamString ss;
    Update.printError(ss);
    _udp_ota->append("ERR: ", 5);
    _udp_ota->append(ss.c_str(), ss.length());
    _udp_ota->send(ota_ip, _ota_udp_port);
    delay(100);
    _udp_ota->listen(IP_ADDR_ANY, _port);
    _state = OTA_IDLE;
    return;
  }
  _udp_ota->append("OK", 2);
  _udp_ota->send(ota_ip, _ota_udp_port);
  delay(100);

  Update.setMD5(_md5.c_str());

  if (_start_callback) {
    _start_callback();
  }
  if (_progress_callback) {
    _progress_callback(0, _size);
  }

  WiFiClient client;
  if (!client.connect(_ota_ip, _ota_port)) {
    OTA_DEBUG_PRINTF("Connect Failed\n");
    _udp_ota->listen(IP_ADDR_ANY, _port);
    if (_error_callback) {
      _error_callback(OTA_CONNECT_ERROR);
    }
    _state = OTA_IDLE;
  }
  // OTA sends little packets
  client.setNoDelay(true);

  uint32_t written, total = 0;
  while (!Update.isFinished() && (client.connected() || client.available())) {
    int waited = 1000;
    while (!client.available() && waited--)
      delay(1);
    if (!waited){
      OTA_DEBUG_PRINTF("Receive Failed\n");
      _udp_ota->listen(IP_ADDR_ANY, _port);
      if (_error_callback) {
        _error_callback(OTA_RECEIVE_ERROR);
      }
      _state = OTA_IDLE;
    }
    written = Update.write(client);
    if (written > 0) {
      client.print(written, DEC);
      total += written;
      if(_progress_callback) {
        _progress_callback(total, _size);
      }
    }
  }

  if (Update.end()) {
    // Ensure last count packet has been sent out and not combined with the final OK
    client.flush();
    delay(1000);
    client.print("OK");
    client.flush();
    delay(1000);
    client.stop();
    OTA_DEBUG_PRINTF("Update Success\n");
    if (_end_callback) {
      _end_callback();
    }
    if(_rebootOnSuccess){
      OTA_DEBUG_PRINTF("Rebooting...\n");
      //let serial/network finish tasks that might be given in _end_callback
      delay(100);
      if (_eraseConfig) {
        eraseConfigAndReset();  // returns on failure
        //C What is the best action to take on failure?
        //C  1) On failure, we could invalidate eboot_command buffer -
        //C     aborting the flash update.
        //C  2) Just ignore it and restart.
        //C  3) Retry forever
        if (_error_callback) {
          _error_callback(OTA_ERASE_SETTINGS_ERROR);
        }
        _state = OTA_ERASEWIFI;
        return;
      }
      ESP.restart();
    }
  } else {
    _udp_ota->listen(IP_ADDR_ANY, _port);
    if (_error_callback) {
      _error_callback(OTA_END_ERROR);
    }
    Update.printError(client);
#ifdef OTA_DEBUG
    Update.printError(OTA_DEBUG);
#endif
    _state = OTA_IDLE;
  }
}

void ArduinoOTAClass::end() {
    _initialized = false;
    _udp_ota->unref();
    _udp_ota = 0;
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MDNS)
    if(_useMDNS){
        MDNS.end();
    }
#endif
    _state = OTA_IDLE;
    OTA_DEBUG_PRINTF("OTA server stopped.\n");
}

void ArduinoOTAClass::eraseConfigAndReset() {
  OTA_DEBUG_PRINTF("Erase Config and Hard Reset ...\n");
  if (WiFi.mode(WIFI_OFF)) {
    ESP.eraseConfig(true);    // No return testing - Only returns on failure
    OTA_DEBUG_PRINTF("  ESP.eraseConfig(true) failed!\n");
  } else {
    OTA_DEBUG_PRINTF("  WiFi.mode(WIFI_OFF) Timeout!\n");
  }

  delay(2000);  // force a gap between retries
}

//this needs to be called in the loop()
void ArduinoOTAClass::handle() {
  if (_state == OTA_RUNUPDATE) {
    _runUpdate();
    _state = OTA_IDLE;
  }

  if (_state == OTA_ERASEWIFI) {
    eraseConfigAndReset();
    return;
  }

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MDNS)
  if(_useMDNS)
    MDNS.update(); //handle MDNS update as well, given that ArduinoOTA relies on it anyways
#endif
}

int ArduinoOTAClass::getCommand() {
  return _cmd;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ARDUINOOTA)
ArduinoOTAClass ArduinoOTA;
#endif
