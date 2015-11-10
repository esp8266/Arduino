#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "ArduinoOTA.h"
#include "MD5Builder.h"

//#define OTA_DEBUG 1

ArduinoOTAClass::ArduinoOTAClass()
: _port(0)
, _state(OTA_IDLE)
, _size(0)
, _cmd(0)
, _ota_port(0)
, _start_callback(NULL)
, _end_callback(NULL)
, _progress_callback(NULL)
, _error_callback(NULL)
{
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

ArduinoOTAClass::~ArduinoOTAClass() {
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
  _initialized = true;

  if (!_hostname.length()) {
    char tmp[15];
    sprintf(tmp, "esp8266-%02x", ESP.getChipId());
    _hostname = tmp;
  }
  if (!_port) {
    _port = 8266;
  }

  _udp_ota.begin(_port);
  MDNS.begin(_hostname.c_str());

  if (_password.length()) {
    MDNS.enableArduino(_port, true);
  } else {
    MDNS.enableArduino(_port);
  }
  _state = OTA_IDLE;
#if OTA_DEBUG
  Serial.printf("OTA server at: %s.local:%u\n", _hostname.c_str(), _port);
#endif
}

void ArduinoOTAClass::_runUpdate() {
  if (!Update.begin(_size, _cmd)) {
#if OTA_DEBUG
    Serial.println("Update Begin Error");
#endif
    if (_error_callback) {
      _error_callback(OTA_BEGIN_ERROR);
    }
    _udp_ota.begin(_port);
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
#if OTA_DEBUG
    Serial.printf("Connect Failed\n");
#endif
    _udp_ota.begin(_port);
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
#if OTA_DEBUG
      Serial.printf("Recieve Failed\n");
#endif
      _udp_ota.begin(_port);
      if (_error_callback) {
        _error_callback(OTA_RECIEVE_ERROR);
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
#if OTA_DEBUG
    Serial.printf("Update Success\nRebooting...\n");
#endif
    if (_end_callback) {
      _end_callback();
    }
    ESP.restart();
  } else {
    _udp_ota.begin(_port);
    if (_error_callback) {
      _error_callback(OTA_END_ERROR);
    }
    Update.printError(client);
#if OTA_DEBUG
    Update.printError(Serial);
#endif
    _state = OTA_IDLE;
  }
}

void ArduinoOTAClass::handle() {
  if (!_udp_ota) {
    _udp_ota.begin(_port);
#if OTA_DEBUG
    Serial.println("OTA restarted");
#endif
  }

  if (!_udp_ota.parsePacket()) return;

  if (_state == OTA_IDLE) {
    int cmd = _udp_ota.parseInt();
    if (cmd != U_FLASH && cmd != U_SPIFFS)
      return;
    _ota_ip = _udp_ota.remoteIP();
    _cmd  = cmd;
    _ota_port = _udp_ota.parseInt();
    _size = _udp_ota.parseInt();
    _udp_ota.read();
    _md5 = _udp_ota.readStringUntil('\n');
    _md5.trim();
    if(_md5.length() != 32)
      return;

#if OTA_DEBUG
    Serial.print("Update Start: ip:");
    Serial.print(_ota_ip);
    Serial.printf(", port:%d, size:%d, md5:%s\n", _ota_port, _size, _md5.c_str());
#endif

    _udp_ota.beginPacket(_ota_ip, _udp_ota.remotePort());
    if (_password){
      MD5Builder nonce_md5;
      nonce_md5.begin();
      nonce_md5.add(String(micros()));
      nonce_md5.calculate();
      _nonce = nonce_md5.toString();
      _udp_ota.printf("AUTH %s", _nonce.c_str());
      _udp_ota.endPacket();
      _state = OTA_WAITAUTH;
      return;
    } else {
      _udp_ota.print("OK");
      _udp_ota.endPacket();
      _state = OTA_RUNUPDATE;
    }
  } else if (_state == OTA_WAITAUTH) {
    int cmd = _udp_ota.parseInt();
    if (cmd != U_AUTH) {
      _state = OTA_IDLE;
      return;
    }
    _udp_ota.read();
    String cnonce = _udp_ota.readStringUntil(' ');
    String response = _udp_ota.readStringUntil('\n');
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

    if(result.equals(response)){
      _udp_ota.beginPacket(_ota_ip, _udp_ota.remotePort());
      _udp_ota.print("OK");
      _udp_ota.endPacket();
      _state = OTA_RUNUPDATE;
    } else {
      _udp_ota.beginPacket(_ota_ip, _udp_ota.remotePort());
      _udp_ota.print("Authentication Failed");
      _udp_ota.endPacket();
      if (_error_callback) _error_callback(OTA_AUTH_ERROR);
      _state = OTA_IDLE;
    }
  }

  if (_state == OTA_RUNUPDATE) {
    _runUpdate();
  }
}

ArduinoOTAClass ArduinoOTA;
