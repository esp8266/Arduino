#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "ArduinoOTA.h"

ArduinoOTA::ArduinoOTA(const char *mdns_host_prefix, int port, bool serial_debug)
{
    _port = port;
    _mdns_host = new String(mdns_host_prefix);
    *_mdns_host += String(ESP.getChipId(), HEX);
    _udp_ota = new WiFiUDP();
    _serial_debug = serial_debug;

    _start_callback    = NULL;
    _end_callback      = NULL;
    _progress_callback = NULL;
    _error_callback    = NULL;
}

void ArduinoOTA::onStart(OTA_CALLBACK(fn)){
    _start_callback = fn;
}

void ArduinoOTA::onEnd(OTA_CALLBACK(fn)){
    _end_callback = fn;
}

void ArduinoOTA::onProgress(OTA_CALLBACK_PROGRESS(fn)){
    _progress_callback = fn;
}

void ArduinoOTA::onError(OTA_CALLBACK(fn)){
    _error_callback = fn;
}

ArduinoOTA::~ArduinoOTA(){
    delete _udp_ota;
    delete _mdns_host;
}

void ArduinoOTA::setup() {
  _udp_ota->begin(_port);
  if (_mdns_host) {
    if (_serial_debug)
      Serial.printf("OTA server at: %s:%u\n",
                    _mdns_host->c_str(),
                    _port);
    MDNS.begin(_mdns_host->c_str());
    MDNS.addService("arduino", "tcp", _port);
  }
}

void ArduinoOTA::handle() {
    
  if (!*_udp_ota) {
    _udp_ota->begin(_port); 
    Serial.println(F("OTA restarted")); 
  }

  if (!_udp_ota->parsePacket()) return;

  IPAddress remote = _udp_ota->remoteIP();
  int cmd  = _udp_ota->parseInt();
  int port = _udp_ota->parseInt();
  int size = _udp_ota->parseInt();

  if (_serial_debug){
      Serial.print("Update Start: ip:");
      Serial.print(remote);
      Serial.printf(", port:%d, size:%d\n", port, size);
  }

  WiFiUDP::stopAll();

  if(!Update.begin(size, cmd)){
    if (_serial_debug)
        Serial.println("Update Begin Error");
    if (_error_callback) _error_callback();
    _udp_ota->begin(_port);
    return;
  }
  if (_start_callback) _start_callback();
  if (_progress_callback) _progress_callback(0, size);

  WiFiClient client;
  if (!client.connect(remote, port)) {
    if (_serial_debug)
        Serial.printf("Connect Failed\n");
    _udp_ota->begin(_port);
    if (_error_callback) _error_callback();
  }

  uint32_t written;
  while(!Update.isFinished() && client.connected()){
    // TODO(mangelajo): enhance the Update.write(client) to 
    // accept a progress callback
    written = Update.write(client); 
    if(written > 0) client.print(written, DEC);
    if(_progress_callback) _progress_callback(written, size);
  }

  Serial.setDebugOutput(false);

  if(Update.end()){
    client.println("OK");
    if (_serial_debug)
        Serial.printf("Update Success\nRebooting...\n");
    if(_end_callback) _end_callback();
    ESP.restart();
  } else {
    // Update failed: listen UDP again, callback and print
    _udp_ota->begin(_port);
    if (_error_callback) _error_callback();
    Update.printError(client);
    if (_serial_debug)
        Update.printError(Serial);
  }
}
