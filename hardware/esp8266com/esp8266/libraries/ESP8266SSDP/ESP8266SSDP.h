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

#ifndef ESP8266SSDP_H
#define ESP8266SSDP_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

typedef enum {
  NONE,
  SEARCH,
  NOTIFY
} ssdp_method_t;

class SSDPClass{
  public:
    SSDPClass();
    ~SSDPClass();

    void begin();
    uint8_t update();
    void schema(WiFiClient client);

    void setName(char *name);
    void setURL(char *url);
    void setSchemaURL(char *url);
    void setHTTPPort(uint16_t port);
    void setSerialNumber(char *serialNumber);
    void setModelName(char *name);
    void setModelNumber(char *num);
    void setModelURL(char *url);
    void setManufacturer(char *name);
    void setManufacturerURL(char *url);
    
  private:
    WiFiUDP _server;
    bool _pending;
    unsigned short _delay;
    unsigned long _process_time;
    unsigned long _notify_time;
    
    uint16_t _port;
    char *_schemaURL;
    char *_uuid;
    char *_friendlyName;
    char *_serialNumber;
    char *_presentationURL;
    char *_manufacturer;
    char *_manufacturerURL;
    char *_modelName;
    char *_modelURL;
    char *_modelNumber;

    void _send(ssdp_method_t method);
};

extern SSDPClass SSDP;

#endif
