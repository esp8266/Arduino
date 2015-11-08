#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "ArduinoOTA.h"
#include "MD5Builder.h"

//#define OTA_DEBUG 1

ArduinoOTAClass::ArduinoOTAClass()
{
    _udp_ota = new WiFiUDP();
    _password = 0;
    _hostname = 0;
    _port = 0;
    _nonce = 0;
    _state = OTA_IDLE;
    
    _size = 0;
    _cmd = 0;
    _ota_port = 0;
    _ota_ip = (uint32_t)0;
    _md5 = new char[33];
    
    _start_callback    = NULL;
    _end_callback      = NULL;
    _progress_callback = NULL;
    _error_callback    = NULL;
}

void ArduinoOTAClass::onStart(OTA_CALLBACK(fn)){
    _start_callback = fn;
}

void ArduinoOTAClass::onEnd(OTA_CALLBACK(fn)){
    _end_callback = fn;
}

void ArduinoOTAClass::onProgress(OTA_CALLBACK_PROGRESS(fn)){
    _progress_callback = fn;
}

void ArduinoOTAClass::onError(OTA_CALLBACK_ERROR(fn)){
    _error_callback = fn;
}

ArduinoOTAClass::~ArduinoOTAClass(){
    delete _udp_ota;
}

void ArduinoOTAClass::setPort(uint16_t port){
  if(!_initialized && !_port && port){
    _port = port;
  }
}

void ArduinoOTAClass::setHostname(const char * hostname){
  if(!_initialized && !_hostname && hostname){
    _hostname = new char[strlen(hostname)];
    sprintf(_hostname, "%s", hostname);
  }
}

void ArduinoOTAClass::setPassword(const char * password){
  if(!_initialized && !_password && password){
    _password = new char[strlen(password)];
    sprintf(_password, "%s", password);
  }
}

void ArduinoOTAClass::begin() {
  if(_initialized)
    return;
  _initialized = true;
  if(!_hostname){
    _hostname = new char[15];
    sprintf(_hostname, "esp8266-%02x", ESP.getChipId());
  }
  if(!_port)
    _port = 8266;
  
  _udp_ota->begin(_port);
  MDNS.begin(_hostname);
  if(_password){
    _nonce = new char[33];
    MDNS.enableArduino(_port, true);
  } else 
    MDNS.enableArduino(_port);
  _state = OTA_IDLE;
#if OTA_DEBUG
  Serial.printf("OTA server at: %s.local:%u\n", _hostname, _port);
#endif
}

void ArduinoOTAClass::_runUpdate(){
  if(!Update.begin(_size, _cmd)){
#if OTA_DEBUG
    Serial.println("Update Begin Error");
#endif
    if (_error_callback) _error_callback(OTA_BEGIN_ERROR);
    _udp_ota->begin(_port);
    _state = OTA_IDLE;
    return;
  }
  Update.setMD5(_md5);
  WiFiUDP::stopAll();
  WiFiClient::stopAll();
  
  
  if (_start_callback) _start_callback();
  if (_progress_callback) _progress_callback(0, _size);

  WiFiClient client;
  if (!client.connect(_ota_ip, _ota_port)) {
#if OTA_DEBUG
    Serial.printf("Connect Failed\n");
#endif
    _udp_ota->begin(_port);
    if (_error_callback) _error_callback(OTA_CONNECT_ERROR);
    _state = OTA_IDLE;
  }

  uint32_t written, total = 0;
  while(!Update.isFinished() && client.connected()){
    int waited = 1000;
    while(!client.available() && waited--)
      delay(1);
    if(!waited){
#if OTA_DEBUG
      Serial.printf("Recieve Failed\n");
#endif
      _udp_ota->begin(_port);
      if (_error_callback) _error_callback(OTA_RECIEVE_ERROR);
      _state = OTA_IDLE;
    }
    written = Update.write(client); 
    if(written > 0){
      client.print(written, DEC);
      total += written;
      if(_progress_callback) _progress_callback(total, _size);
    }
  }

  if(Update.end()){
    client.print("OK");
#if OTA_DEBUG
    Serial.printf("Update Success\nRebooting...\n");
#endif
    if(_end_callback) _end_callback();
    ESP.restart();
  } else {
    _udp_ota->begin(_port);
    if (_error_callback) _error_callback(OTA_END_ERROR);
    Update.printError(client);
#if OTA_DEBUG
    Update.printError(Serial);
#endif
    _state = OTA_IDLE;
  }
}

void ArduinoOTAClass::handle() {
  if (!*_udp_ota) {
    _udp_ota->begin(_port); 
#if OTA_DEBUG
    Serial.println("OTA restarted"); 
#endif
  }

  if (!_udp_ota->parsePacket()) return;

  if(_state == OTA_IDLE){
    _ota_ip = _udp_ota->remoteIP();
    _cmd  = _udp_ota->parseInt();
    _ota_port = _udp_ota->parseInt();
    _size = _udp_ota->parseInt();
    _udp_ota->read();
    sprintf(_md5, "%s", _udp_ota->readStringUntil('\n').c_str());

#if OTA_DEBUG
    Serial.print("Update Start: ip:");
    Serial.print(_ota_ip);
    Serial.printf(", port:%d, size:%d, md5:%s\n", _ota_port, _size, _md5);
#endif
    
    _udp_ota->beginPacket(_ota_ip, _udp_ota->remotePort());
    if(_password){
      MD5Builder nonce_md5;
      nonce_md5.begin();
      nonce_md5.add(String(micros()));
      nonce_md5.calculate();
      nonce_md5.getChars(_nonce);
      _udp_ota->printf("AUTH %s", _nonce);
      _udp_ota->endPacket();
      _state = OTA_WAITAUTH;
      return;
    } else {
      _udp_ota->print("OK");
      _udp_ota->endPacket();
      _state = OTA_RUNUPDATE;
    }
  } else if(_state == OTA_WAITAUTH){
    String cnonce = _udp_ota->readStringUntil(' ');
    String response = _udp_ota->readStringUntil('\n');
    
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

    if(result.equals(response)){
      _udp_ota->beginPacket(_ota_ip, _udp_ota->remotePort());
      _udp_ota->print("OK");
      _udp_ota->endPacket();
      _state = OTA_RUNUPDATE;
    } else {
      _udp_ota->beginPacket(_ota_ip, _udp_ota->remotePort());
      _udp_ota->print("Authentication Failed");
      _udp_ota->endPacket();
      if (_error_callback) _error_callback(OTA_AUTH_ERROR);
      _state = OTA_IDLE;
    }
  }
  
  if(_state == OTA_RUNUPDATE)
    _runUpdate();
}

ArduinoOTAClass ArduinoOTA;
