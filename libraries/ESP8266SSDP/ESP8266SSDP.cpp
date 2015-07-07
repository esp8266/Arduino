/*
ESP8266 Simple Service Discovery
Copyright (c) 2015 Hristo Gochkov

Original (Arduino) version by Filippo Sallemi, July 23, 2014.
Can be found at: https://github.com/nomadnt/uSSDP

License (MIT license):
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

*/
#define LWIP_OPEN_SRC
#include "ESP8266SSDP.h"

extern "C" {
  #include "user_interface.h"
  #include "mem.h"
}
#include "lwip/ip_addr.h"
#include "lwip/igmp.h"

//#define DEBUG_SSDP  Serial

#define SSDP_INTERVAL     1200
#define SSDP_PORT         1900
#define SSDP_METHOD_SIZE  10
#define SSDP_URI_SIZE     2
#define SSDP_BUFFER_SIZE  64

static const IPAddress SSDP_MULTICAST_ADDR(239, 255, 255, 250);

static const char* _ssdp_response_template = 
  "HTTP/1.1 200 OK\r\n"
  "EXT:\r\n"
  "ST: upnp:rootdevice\r\n";

static const char* _ssdp_notify_template = 
  "NOTIFY * HTTP/1.1\r\n"
  "HOST: 239.255.255.250:1900\r\n"
  "NT: upnp:rootdevice\r\n"
  "NTS: ssdp:alive\r\n";

static const char* _ssdp_packet_template = 
  "%s" // _ssdp_response_template / _ssdp_notify_template
  "CACHE-CONTROL: max-age=%u\r\n" // SSDP_INTERVAL
  "SERVER: Arduino/1.0 UPNP/1.1 %s/%s\r\n" // _modelName, _modelNumber
  "USN: uuid:%s\r\n" // _uuid
  "LOCATION: http://%u.%u.%u.%u:%u/%s\r\n" // WiFi.localIP(), _port, _schemaURL
  "\r\n";

static const char* _ssdp_schema_template = 
  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/xml\r\n"
  "Connection: close\r\n"
  "Access-Control-Allow-Origin: *\r\n"
  "\r\n"
  "<?xml version=\"1.0\"?>"
  "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
    "<specVersion>"
      "<major>1</major>"
      "<minor>0</minor>"
    "</specVersion>"
    "<URLBase>http://%u.%u.%u.%u:%u/</URLBase>" // WiFi.localIP(), _port
    "<device>"
      "<deviceType>urn:schemas-upnp-org:device:Basic:1</deviceType>"
      "<friendlyName>%s</friendlyName>"
      "<presentationURL>%s</presentationURL>"
      "<serialNumber>%s</serialNumber>"
      "<modelName>%s</modelName>"
      "<modelNumber>%s</modelNumber>"
      "<modelURL>%s</modelURL>"
      "<manufacturer>%s</manufacturer>"
      "<manufacturerURL>%s</manufacturerURL>"
      "<UDN>uuid:%s</UDN>"
    "</device>"
//    "<iconList>"
//      "<icon>"
//        "<mimetype>image/png</mimetype>"
//        "<height>48</height>"
//        "<width>48</width>"
//        "<depth>24</depth>"
//        "<url>icon48.png</url>"
//      "</icon>"
//      "<icon>"
//       "<mimetype>image/png</mimetype>"
//       "<height>120</height>"
//       "<width>120</width>"
//       "<depth>24</depth>"
//       "<url>icon120.png</url>"
//      "</icon>"
//    "</iconList>"
  "</root>\r\n"
  "\r\n";

SSDPClass::SSDPClass(){
  _uuid[0] = '\0';
  _modelNumber[0] = '\0';
  _friendlyName[0] = '\0';
  _presentationURL[0] = '\0';
  _serialNumber[0] = '\0';
  _modelName[0] = '\0';
  _modelURL[0] = '\0';
  _manufacturer[0] = '\0';
  _manufacturerURL[0] = '\0';
  sprintf(_schemaURL, "ssdp/schema.xml");
  _port = 80;
  _pending = false;
}

SSDPClass::~SSDPClass(){
}

void SSDPClass::begin(){
  ip_addr_t ifaddr;
  ip_addr_t multicast_addr;

  _pending = false;

  ifaddr.addr = WiFi.localIP();
  multicast_addr.addr = (uint32_t) SSDP_MULTICAST_ADDR;
  igmp_joingroup(&ifaddr, &multicast_addr);

  uint8_t mac[6];
  WiFi.macAddress(mac);
  uint32_t chipId = ESP.getChipId();
  sprintf(_uuid, "38323636-4558-%04x-%04x-%02x%02x%02x%02x%02x%02x",
    (chipId >> 16) & 0xFFFF, chipId & 0xFFFF,
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
  );
#ifdef DEBUG_SSDP
  DEBUG_SSDP.printf("SSDP UUID: %s\n", (char *)_uuid);
#endif
  _server.begin(SSDP_PORT);
}

void SSDPClass::_send(ssdp_method_t method){
#ifdef DEBUG_SSDP
  if(method == NONE){
    DEBUG_SSDP.print("Sending Response to ");
    DEBUG_SSDP.print(_server.remoteIP());
    DEBUG_SSDP.print(":");
    DEBUG_SSDP.println(_server.remotePort());
  }else if(method == NOTIFY){
    DEBUG_SSDP.println("Sending Notify to 239.255.255.250:1900");
  }
#endif
  
  if(method == NONE){
    _server.beginPacket(_server.remoteIP(), _server.remotePort());
  } else {
    _server.beginPacket(SSDP_MULTICAST_ADDR, SSDP_PORT);
  }

  uint32_t ip = WiFi.localIP();
  
  _server.printf(_ssdp_packet_template,
    (method == NONE)?_ssdp_response_template:_ssdp_notify_template,
    SSDP_INTERVAL,
    _modelName, _modelNumber,
    _uuid,
    IP2STR(&ip), _port, _schemaURL
  );
    
  _server.endPacket();
}

void SSDPClass::schema(WiFiClient client){
  uint32_t ip = WiFi.localIP();
  client.printf(_ssdp_schema_template,
    IP2STR(&ip), _port,
    _friendlyName,
    _presentationURL,
    _serialNumber,
    _modelName,
    _modelNumber,
    _modelURL,
    _manufacturer,
    _manufacturerURL,
    _uuid
  );
}

uint8_t SSDPClass::update(){
  if(!_pending && _server.parsePacket() > 0){
    ssdp_method_t method = NONE;

    typedef enum {METHOD, URI, PROTO, KEY, VALUE, ABORT} states;
    states state = METHOD;

    typedef enum {START, MAN, ST, MX} headers;
    headers header = START;

    uint8_t cursor = 0;
    uint8_t cr = 0;

    char buffer[SSDP_BUFFER_SIZE] = {0};
    
    while(_server.available() > 0){
      char c = _server.read();

      (c == '\r' || c == '\n') ? cr++ : cr = 0;

      switch(state){
        case METHOD:
          if(c == ' '){
            if(strcmp(buffer, "M-SEARCH") == 0) method = SEARCH;
            else if(strcmp(buffer, "NOTIFY") == 0) method = NOTIFY;
            
            if(method == NONE) state = ABORT;
            else state = URI; 
            cursor = 0;

          } else if(cursor < SSDP_METHOD_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
          break;
        case URI:
          if(c == ' '){
            if(strcmp(buffer, "*")) state = ABORT;
            else state = PROTO; 
            cursor = 0; 
          } else if(cursor < SSDP_URI_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
          break;
        case PROTO:
          if(cr == 2){ state = KEY; cursor = 0; }
          break;
        case KEY:
          if(cr == 4){ _pending = true; _process_time = millis(); }
          else if(c == ' '){ cursor = 0; state = VALUE; }
          else if(c != '\r' && c != '\n' && c != ':' && cursor < SSDP_BUFFER_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
          break;
        case VALUE:
          if(cr == 2){
            switch(header){
              case MAN:
#ifdef DEBUG_SSDP
                DEBUG_SSDP.printf("MAN: %s\n", (char *)buffer);
#endif
                break;
              case ST:
                if(strcmp(buffer, "ssdp:all")){
                  state = ABORT;
#ifdef DEBUG_SSDP
                  DEBUG_SSDP.printf("REJECT: %s\n", (char *)buffer);
#endif
                }
                break;
              case MX:
                _delay = random(0, atoi(buffer)) * 1000L;
                break;
            }

            if(state != ABORT){ state = KEY; header = START; cursor = 0; }
          } else if(c != '\r' && c != '\n'){
            if(header == START){
              if(strncmp(buffer, "MA", 2) == 0) header = MAN;
              else if(strcmp(buffer, "ST") == 0) header = ST;
              else if(strcmp(buffer, "MX") == 0) header = MX;
            }
            
            if(cursor < SSDP_BUFFER_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
          }
          break;
        case ABORT:
          _pending = false; _delay = 0;
          break;
      }
    }
    
    _server.flush();
  }

  if(_pending && (millis() - _process_time) > _delay){
    _pending = false; _delay = 0;
    _send(NONE);
  } else if(_notify_time == 0 || (millis() - _notify_time) > (SSDP_INTERVAL * 1000L)){
    _notify_time = millis();
    _send(NOTIFY);
  }
}

void SSDPClass::setSchemaURL(const char *url){
  strlcpy(_schemaURL, url, sizeof(_schemaURL));
}

void SSDPClass::setHTTPPort(uint16_t port){
  _port = port;
}

void SSDPClass::setName(const char *name){
  strlcpy(_friendlyName, name, sizeof(_friendlyName));
}

void SSDPClass::setURL(const char *url){
  strlcpy(_presentationURL, url, sizeof(_presentationURL));
}

void SSDPClass::setSerialNumber(const char *serialNumber){
  strlcpy(_serialNumber, serialNumber, sizeof(_serialNumber));
}

void SSDPClass::setModelName(const char *name){
  strlcpy(_modelName, name, sizeof(_modelName));
}

void SSDPClass::setModelNumber(const char *num){
  strlcpy(_modelNumber, num, sizeof(_modelNumber));
}

void SSDPClass::setModelURL(const char *url){
  strlcpy(_modelURL, url, sizeof(_modelURL));
}

void SSDPClass::setManufacturer(const char *name){
  strlcpy(_manufacturer, name, sizeof(_manufacturer));
}

void SSDPClass::setManufacturerURL(const char *url){
  strlcpy(_manufacturerURL, url, sizeof(_manufacturerURL));
}

SSDPClass SSDP;
