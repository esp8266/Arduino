/*
ESP8266 Multicast DNS (port of CC3000 Multicast DNS library)
Version 1.1
Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)
Extended MDNS-SD support 2016 Lars Englund (lars.englund@gmail.com)

This is a simple implementation of multicast DNS query support for an Arduino
running on ESP8266 chip. Only support for resolving address queries is currently
implemented.

Requirements:
- ESP8266WiFi library

Usage:
- Include the ESP8266 Multicast DNS library in the sketch.
- Call the begin method in the sketch's setup and provide a domain name (without
  the '.local' suffix, i.e. just provide 'foo' to resolve 'foo.local'), and the
  Adafruit CC3000 class instance.  Optionally provide a time to live (in seconds)
  for the DNS record--the default is 1 hour.
- Call the update method in each iteration of the sketch's loop function.

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
#ifndef ESP8266MDNS_H
#define ESP8266MDNS_H

#include "ESP8266WiFi.h"
#include "WiFiUdp.h"

//this should be defined at build time
#ifndef ARDUINO_BOARD
#define ARDUINO_BOARD "generic"
#endif

class UdpContext;

struct MDNSService;
struct MDNSTxt;
struct MDNSAnswer;

class MDNSResponder {
public:
  MDNSResponder();
  ~MDNSResponder();
  bool begin(const char* hostName);
  //for compatibility
  bool begin(const char* hostName, IPAddress ip, uint32_t ttl=120);
  void update();

  void addService(char *service, char *proto, uint16_t port);
  void addService(const char *service, const char *proto, uint16_t port){
    addService((char *)service, (char *)proto, port);
  }
  void addService(String service, String proto, uint16_t port){
    addService(service.c_str(), proto.c_str(), port);
  }
  
  bool addServiceTxt(char *name, char *proto, char * key, char * value);
  void addServiceTxt(const char *name, const char *proto, const char *key,const char * value){
    addServiceTxt((char *)name, (char *)proto, (char *)key, (char *)value);
  }
  void addServiceTxt(String name, String proto, String key, String value){
    addServiceTxt(name.c_str(), proto.c_str(), key.c_str(), value.c_str());
  }
  
  int queryService(char *service, char *proto);
  int queryService(const char *service, const char *proto){
    return queryService((char *)service, (char *)proto);
  }
  int queryService(String service, String proto){
    return queryService(service.c_str(), proto.c_str());
  }
  String hostname(int idx);
  IPAddress IP(int idx);
  uint16_t port(int idx);
  
  void enableArduino(uint16_t port, bool auth=false);

  void setInstanceName(String name);
  void setInstanceName(const char * name){
    setInstanceName(String(name));
  }
  void setInstanceName(char * name){
    setInstanceName(String(name));
  }

private:
  struct MDNSService * _services;
  UdpContext* _conn;
  String _hostName;
  String _instanceName;
  struct MDNSAnswer * _answers;
  struct MDNSQuery * _query;
  bool _newQuery;
  bool _waitingForAnswers;
  WiFiEventHandler _disconnectedHandler;
  WiFiEventHandler _gotIPHandler;
  uint32_t _ip;

  bool _begin(const char* hostName, uint32_t ip, uint32_t ttl);
  uint32_t _getOurIp();
  uint16_t _getServicePort(char *service, char *proto);
  MDNSTxt * _getServiceTxt(char *name, char *proto);
  uint16_t _getServiceTxtLen(char *name, char *proto);
  void _parsePacket();
  void _reply(uint8_t replyMask, char * service, char *proto, uint16_t port);
  size_t advertiseServices(); // advertise all hosted services
  MDNSAnswer* _getAnswerFromIdx(int idx);
  int _getNumAnswers();
  bool _listen();
  void _restart();
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MDNS)
extern MDNSResponder MDNS;
#endif

#endif //ESP8266MDNS_H
