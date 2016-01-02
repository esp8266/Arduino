#define LWIP_OPEN_SRC
#include <functional>
#include <WiFiUdp.h>
#include "ArduinoOTA.h"
#include "MD5Builder.h"

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
#include <ESP8266mDNS.h>


#ifdef DEBUG_ESP_OTA
#ifdef DEBUG_ESP_PORT
#define OTA_DEBUG DEBUG_ESP_PORT
#endif
#endif

ArduinoOTAClass::ArduinoOTAClass()
: _port(0)
, _udp_ota(0)
, _initialized(false)
, _state(OTA_IDLE)
, _size(0)
, _cmd(0)
, _ota_port(0)
, _start_callback(NULL)
, _end_callback(NULL)
, _error_callback(NULL)
, _progress_callback(NULL)
{
}

ArduinoOTAClass::~ArduinoOTAClass(){
  if(_udp_ota){
    _udp_ota->unref();
    _udp_ota = 0;
  }
}

void ArduinoOTAClass::onStart(OTA_CALLBACK(fn)) {
    _start_callback = fn;
}

void ArduinoOTAClass::onEnd(OTA_CALLBACK(fn)) {
    _end_callback = fn;
}

void ArduinoOTAClass::onProgress(OTA_CALLBACK_PROGRESS(fn)) {
    _progress_callback = fn;
}

void ArduinoOTAClass::onError(OTA_CALLBACK_ERROR(fn)) {
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

void ArduinoOTAClass::setPassword(const char * password) {
  if (!_initialized && !_password.length() && password) {
    _password = password;
  }
}

void ArduinoOTAClass::begin() {
  if (_initialized)
    return;

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

  if(!_udp_ota->listen(*IP_ADDR_ANY, _port))
    return;
  _udp_ota->onRx(std::bind(&ArduinoOTAClass::_onRx, this));
  MDNS.begin(_hostname.c_str());

  if (_password.length()) {
    MDNS.enableArduino(_port, true);
  } else {
    MDNS.enableArduino(_port);
  }
  _initialized = true;
  _state = OTA_IDLE;
#ifdef OTA_DEBUG
  OTA_DEBUG.printf("OTA server at: %s.local:%u\n", _hostname.c_str(), _port);
#endif
}

int ArduinoOTAClass::parseInt(){
  char data[16];
  uint8_t index = 0;
  char value;
  while(_udp_ota->peek() == ' ') _udp_ota->read();
  while(true){
    value = _udp_ota->peek();
    if(value < '0' || value > '9'){
      data[index++] = '\0';
      return atoi(data);
    }
    data[index++] = _udp_ota->read();
  }
  return 0;
}

String ArduinoOTAClass::readStringUntil(char end){
  String res = "";
  char value;
  while(true){
    value = _udp_ota->read();
    if(value == '\0' || value == end){
      return res;
    }
    res += value;
  }
  return res;
}

void ArduinoOTAClass::_onRx(){
  if(!_udp_ota->next()) return;
  ip_addr_t ota_ip;

  if (_state == OTA_IDLE) {
    int cmd = parseInt();
    if (cmd != U_FLASH && cmd != U_SPIFFS)
      return;
    _ota_ip = _udp_ota->getRemoteAddress();
    _cmd  = cmd;
    _ota_port = parseInt();
    _size = parseInt();
    _udp_ota->read();
    _md5 = readStringUntil('\n');
    _md5.trim();
    if(_md5.length() != 32)
      return;

    ota_ip.addr = (uint32_t)_ota_ip;

    if (_password.length()){
      MD5Builder nonce_md5;
      nonce_md5.begin();
      nonce_md5.add(String(micros()));
      nonce_md5.calculate();
      _nonce = nonce_md5.toString();
      
      char auth_req[38];
      sprintf(auth_req, "AUTH %s", _nonce.c_str());
      _udp_ota->append((const char *)auth_req, strlen(auth_req));
      _udp_ota->send(&ota_ip, _udp_ota->getRemotePort());
      _state = OTA_WAITAUTH;
      return;
    } else {
      _udp_ota->append("OK", 2);
      _udp_ota->send(&ota_ip, _udp_ota->getRemotePort());
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

    MD5Builder _passmd5;
    _passmd5.begin();
    _passmd5.add(_password);
    _passmd5.calculate();
    String passmd5 = _passmd5.toString();

    String challenge = passmd5 + ":" + String(_nonce) + ":" + cnonce;
    MD5Builder _challengemd5;
    _challengemd5.begin();
    _challengemd5.add(challenge);
    _challengemd5.calculate();
    String result = _challengemd5.toString();

    ota_ip.addr = (uint32_t)_ota_ip;
    if(result.equals(response)){
      _udp_ota->append("OK", 2);
      _udp_ota->send(&ota_ip, _udp_ota->getRemotePort());
      _state = OTA_RUNUPDATE;
    } else {
      _udp_ota->append("Authentication Failed", 21);
      _udp_ota->send(&ota_ip, _udp_ota->getRemotePort());
      if (_error_callback) _error_callback(OTA_AUTH_ERROR);
      _state = OTA_IDLE;
    }
  }

  while(_udp_ota->next()) _udp_ota->flush();
}

void ArduinoOTAClass::_runUpdate() {
  if (!Update.begin(_size, _cmd)) {
#ifdef OTA_DEBUG
    OTA_DEBUG.println("Update Begin Error");
#endif
    if (_error_callback) {
      _error_callback(OTA_BEGIN_ERROR);
    }
    _udp_ota->listen(*IP_ADDR_ANY, _port);
    _state = OTA_IDLE;
    return;
  }
  Update.setMD5(_md5.c_str());
  WiFiUDP::stopAll();
  WiFiClient::stopAll();

  if (_start_callback) {
    _start_callback();
  }
  if (_progress_callback) {
    _progress_callback(0, _size);
  }

  WiFiClient client;
  if (!client.connect(_ota_ip, _ota_port)) {
#ifdef OTA_DEBUG
    OTA_DEBUG.printf("Connect Failed\n");
#endif
    _udp_ota->listen(*IP_ADDR_ANY, _port);
    if (_error_callback) {
      _error_callback(OTA_CONNECT_ERROR);
    }
    _state = OTA_IDLE;
  }

  uint32_t written, total = 0;
  while (!Update.isFinished() && client.connected()) {
    int waited = 1000;
    while (!client.available() && waited--)
      delay(1);
    if (!waited){
#ifdef OTA_DEBUG
      OTA_DEBUG.printf("Receive Failed\n");
#endif
      _udp_ota->listen(*IP_ADDR_ANY, _port);
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
    client.print("OK");
    client.stop();
    delay(10);
#ifdef OTA_DEBUG
    OTA_DEBUG.printf("Update Success\nRebooting...\n");
#endif
    if (_end_callback) {
      _end_callback();
    }
    ESP.restart();
  } else {
    _udp_ota->listen(*IP_ADDR_ANY, _port);
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

void ArduinoOTAClass::handle() {
  if (_state == OTA_RUNUPDATE) {
    _runUpdate();
    _state = OTA_IDLE;
  }
}

ArduinoOTAClass ArduinoOTA;
