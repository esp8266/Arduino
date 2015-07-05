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

#include "ESP8266SSDP.h"

extern "C" {
  #include "ip_addr.h"
  #include "user_interface.h"
  #include "mem.h"
}
#include "lwip/igmp.h"

#define SSDP_INTERVAL     1200
#define SSDP_PORT         1900
#define SSDP_METHOD_SIZE  10
#define SSDP_URI_SIZE     2
#define SSDP_BUFFER_SIZE  64

#define SSDP_UUID_SIZE              37
#define SSDP_SCHEMA_URL_SIZE        64
#define SSDP_FRIENDLY_NAME_SIZE     64
#define SSDP_SERIAL_NUMBER_SIZE     32
#define SSDP_PRESENTATION_URL_SIZE  128
#define SSDP_MODEL_NAME_SIZE        64
#define SSDP_MODEL_URL_SIZE         128
#define SSDP_MODEL_VERSION_SIZE     32
#define SSDP_MANUFACTURER_SIZE      64
#define SSDP_MANUFACTURER_URL_SIZE  128

static const IPAddress SSDP_MULTICAST_ADDR(239, 255, 255, 250);

const char* _ssdp_response_template = 
  "HTTP/1.1 200 OK\r\n"
  "EXT:\r\n"
  "ST: upnp:rootdevice\r\n";

const char* _ssdp_notify_template = 
  "NOTIFY * HTTP/1.1\r\n"
  "HOST: 239.255.255.250:1900\r\n"
  "NT: upnp:rootdevice\r\n"
  "NTS: ssdp:alive\r\n";

const char* _ssdp_packet_template = 
  "%s" // _ssdp_response_template / _ssdp_notify_template
  "CACHE-CONTROL: max-age=%u\r\n" // SSDP_INTERVAL
  "SERVER: Arduino/1.0 UPNP/1.1 %s/%s\r\n" // _modelName, _modelNumber
  "USN: uuid:%s\r\n" // _uuid
  "LOCATION: http://%u.%u.%u.%u:%u/%s\r\n" // WiFi.localIP(), _port, _shemaURL
  "\r\n";

const char* _ssdp_schema_template = 
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
  "</root>\r\n"
  "\r\n";

SSDPClass::SSDPClass(){
  _uuid = (char*)os_malloc(SSDP_UUID_SIZE);
  _schemaURL = (char*)os_malloc(SSDP_SCHEMA_URL_SIZE);
  _friendlyName = (char*)os_malloc(SSDP_FRIENDLY_NAME_SIZE);
  _presentationURL = (char*)os_malloc(SSDP_PRESENTATION_URL_SIZE);
  _serialNumber = (char*)os_malloc(SSDP_SERIAL_NUMBER_SIZE);
  _modelName = (char*)os_malloc(SSDP_MODEL_NAME_SIZE);
  _modelNumber = (char*)os_malloc(SSDP_MODEL_VERSION_SIZE);
  _modelURL = (char*)os_malloc(SSDP_MODEL_URL_SIZE);
  _manufacturer = (char*)os_malloc(SSDP_MANUFACTURER_SIZE);
  _manufacturerURL = (char*)os_malloc(SSDP_MANUFACTURER_URL_SIZE);
  
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
	os_free(_uuid);
	os_free(_schemaURL);
  os_free(_friendlyName);
  os_free(_presentationURL);
  os_free(_serialNumber);
  os_free(_modelName);
  os_free(_modelNumber);
  os_free(_modelURL);
  os_free(_manufacturer);
  os_free(_manufacturerURL);
  _pending = false;
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
  sprintf(_uuid, "38323636-4558-%04X-%04X-%02X%02X%02X%02X%02X%02X",
    (chipId >> 16) & 0xFFFF, chipId & 0xFFFF,
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
  );
#ifdef DEBUG_SSDP
  DEBUG_SSDP.printf("SSDP UUID: %s\n", (char *)_uuid);
#endif
  _server.begin(SSDP_PORT);
}

void SSDPClass::send(ssdp_method_t method){
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
    (uint8_t)(ip & 0xFF), (uint8_t)((ip >> 8) & 0xFF), (uint8_t)((ip >> 16) & 0xFF), (uint8_t)((ip >> 24) & 0xFF), _port, _schemaURL
  );
    
  _server.endPacket();
}

void SSDPClass::schema(WiFiClient client){
  uint32_t ip = WiFi.localIP();
  client.printf(_ssdp_schema_template,
    (uint8_t)(ip & 0xFF), (uint8_t)((ip >> 8) & 0xFF), (uint8_t)((ip >> 16) & 0xFF), (uint8_t)((ip >> 24) & 0xFF), _port,
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

          }else if(cursor < SSDP_METHOD_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
          break;
        case URI:
          if(c == ' '){
            if(strcmp(buffer, "*")) state = ABORT;
            else state = PROTO; 
            cursor = 0; 
          }else if(cursor < SSDP_URI_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
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
          }else if(c != '\r' && c != '\n'){
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
    send(NONE);
  }else if(_notify_time == 0 || (millis() - _notify_time) > (SSDP_INTERVAL * 1000L)){
    _notify_time = millis();
    send(NOTIFY);
  }
}

void SSDPClass::setSchemaURL(char *url){
  strcpy(_schemaURL, url);
}

void SSDPClass::setHTTPPort(uint16_t port){
  _port = port;
}

void SSDPClass::setName(char *name){
  strcpy(_friendlyName, name);
}

void SSDPClass::setURL(char *url){
  strcpy(_presentationURL, url);
}

void SSDPClass::setSerialNumber(char *serialNumber){
  strcpy(_serialNumber, serialNumber);
}

void SSDPClass::setModelName(char *name){
  strcpy(_modelName, name);
}

void SSDPClass::setModelNumber(char *num){
  strcpy(_modelNumber, num);
}

void SSDPClass::setModelURL(char *url){
  strcpy(_modelURL, url);
}

void SSDPClass::setManufacturer(char *name){
  strcpy(_manufacturer, name);
}

void SSDPClass::setManufacturerURL(char *url){
  strcpy(_manufacturerURL, url);
}

SSDPClass SSDP;
