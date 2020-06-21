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
#ifndef ESP8266MDNS_LEGACY_H
#define ESP8266MDNS_LEGACY_H

#include "LEAmDNS2Host.h"
#include "WiFiUdp.h"

//this should be defined at build time
#ifndef ARDUINO_BOARD
#define ARDUINO_BOARD "generic"
#endif

class UdpContext;


namespace Legacy_MDNSResponder
{


struct MDNSService;
struct MDNSTxt;
struct MDNSAnswer;

class MDNSResponder
{
public:

    MDNSResponder();
    ~MDNSResponder();
    bool begin(const char* hostName);
    bool begin(const String& hostName)
    {
        return begin(hostName.c_str());
    }
    //for compatibility
    bool begin(const char* hostName, IPAddress ip, uint32_t ttl = 120)
    {
        (void) ip;
        (void) ttl;
        return begin(hostName);
    }
    bool begin(const String& hostName, IPAddress ip, uint32_t ttl = 120)
    {
        return begin(hostName.c_str(), ip, ttl);
    }
    /* Application should call this whenever AP is configured/disabled */
    void notifyAPChange();
    void update();

    void addService(char *service, char *proto, uint16_t port);
    void addService(const char *service, const char *proto, uint16_t port)
    {
        addService((char *)service, (char *)proto, port);
    }
    void addService(const String& service, const String& proto, uint16_t port)
    {
        addService(service.c_str(), proto.c_str(), port);
    }

    bool addServiceTxt(char *name, char *proto, char * key, char * value);
    bool addServiceTxt(const char *name, const char *proto, const char *key, const char * value)
    {
        return addServiceTxt((char *)name, (char *)proto, (char *)key, (char *)value);
    }
    bool addServiceTxt(const String& name, const String& proto, const String& key, const String& value)
    {
        return addServiceTxt(name.c_str(), proto.c_str(), key.c_str(), value.c_str());
    }

    int queryService(char *service, char *proto);
    int queryService(const char *service, const char *proto)
    {
        return queryService((char *)service, (char *)proto);
    }
    int queryService(const String& service, const String& proto)
    {
        return queryService(service.c_str(), proto.c_str());
    }
    String hostname(int idx);
    IPAddress IP(int idx);
    uint16_t port(int idx);

    void enableArduino(uint16_t port, bool auth = false);

    void setInstanceName(String name);
    void setInstanceName(const char * name)
    {
        setInstanceName(String(name));
    }
    void setInstanceName(char * name)
    {
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


    uint16_t _getServicePort(char *service, char *proto);
    MDNSTxt * _getServiceTxt(char *name, char *proto);
    uint16_t _getServiceTxtLen(char *name, char *proto);
    IPAddress _getRequestMulticastInterface();
    void _parsePacket();
    void _replyToTypeEnumRequest(IPAddress multicastInterface);
    void _replyToInstanceRequest(uint8_t questionMask, uint8_t responseMask, char * service, char *proto, uint16_t port, IPAddress multicastInterface);
    MDNSAnswer* _getAnswerFromIdx(int idx);
    int _getNumAnswers();
    bool _listen();
    void _restart();
};

}	// namespace Legacy_MDNSResponder

#endif //ESP8266MDNS_H



