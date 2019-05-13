/*
 * ESP8266 LLMNR responder
 * Copyright (C) 2017 Stephen Warren <swarren@wwwdotorg.org>
 *
 * Based on:
 * ESP8266 Multicast DNS (port of CC3000 Multicast DNS library)
 * Version 1.1
 * Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
 * ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)
 * MDNS-SD Suport 2015 Hristo Gochkov
 * Extended MDNS-SD support 2016 Lars Englund (lars.englund@gmail.com)
 *
 * License (MIT license):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef ESP8266LLMNR_H
#define ESP8266LLMNR_H

#include <ESP8266WiFi.h>

class UdpContext;

class LLMNRResponder {
public:
    LLMNRResponder();
    ~LLMNRResponder();

    /* Initialize and start responding to LLMNR requests on all interfaces */
    bool begin(const char* hostname);

    /* Application should call this whenever AP is configured/disabled */
    void notify_ap_change();

private:
    String _hostname;
    UdpContext *_conn;
    WiFiEventHandler _sta_got_ip_handler;
    WiFiEventHandler _sta_disconnected_handler;

    bool _restart();
    void _process_packet();
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LLMNR)
extern LLMNRResponder LLMNR;
#endif

#endif
