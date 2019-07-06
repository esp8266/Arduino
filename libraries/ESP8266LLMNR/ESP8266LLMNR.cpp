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
 *
 * Reference:
 * https://tools.ietf.org/html/rfc4795 (LLMNR)
 * https://tools.ietf.org/html/rfc1035 (DNS)
 */

#include <debug.h>
#include <functional>
#include <ESP8266LLMNR.h>
#include <WiFiUdp.h>

extern "C" {
#include <user_interface.h>
}

#include <lwip/udp.h>
#include <lwip/igmp.h>
#include <include/UdpContext.h>

//#define LLMNR_DEBUG

//BIT(x) is defined in tools/sdk/c_types.h

#define FLAGS_QR                BIT(15)
#define FLAGS_OP_SHIFT          11
#define FLAGS_OP_MASK           0xf
#define FLAGS_C                 BIT(10)
#define FLAGS_TC                BIT(9)
#define FLAGS_T                 BIT(8)
#define FLAGS_RCODE_SHIFT       0
#define FLAGS_RCODE_MASK        0xf

#define _conn_read16() (((uint16_t)_conn->read() << 8) | _conn->read())
#define _conn_read8() _conn->read()
#define _conn_readS(b, l) _conn->read((b), (l));

// llmnr ipv6 is FF02:0:0:0:0:0:1:3
// lwip-v2's igmp_joingroup only supports IPv4
#define LLMNR_MULTICAST_ADDR 224, 0, 0, 252
static const int LLMNR_MULTICAST_TTL = 1;
static const int LLMNR_PORT = 5355;

LLMNRResponder::LLMNRResponder() :
    _conn(0) {
}

LLMNRResponder::~LLMNRResponder() {
    if (_conn)
        _conn->unref();
}

bool LLMNRResponder::begin(const char* hostname) {
    // Max length for a single label in DNS
    if (strlen(hostname) > 63)
        return false;

    _hostname = hostname;
    _hostname.toLowerCase();

    _sta_got_ip_handler = WiFi.onStationModeGotIP([this](const WiFiEventStationModeGotIP& event){
        (void) event;
        _restart();
    });

    _sta_disconnected_handler = WiFi.onStationModeDisconnected([this](const WiFiEventStationModeDisconnected& event) {
        (void) event;
        _restart();
    });

    return _restart();
}

void LLMNRResponder::notify_ap_change() {
    _restart();
}

bool LLMNRResponder::_restart() {
    if (_conn) {
        _conn->unref();
        _conn = 0;
    }

    IPAddress llmnr(LLMNR_MULTICAST_ADDR);

    if (igmp_joingroup((ip_addr_t*)IP4_ADDR_ANY4, llmnr) != ERR_OK)
        return false;

    _conn = new UdpContext;
    _conn->ref();

    if (!_conn->listen(IP_ADDR_ANY, LLMNR_PORT))
        return false;

    _conn->setMulticastTTL(LLMNR_MULTICAST_TTL);
    _conn->onRx(std::bind(&LLMNRResponder::_process_packet, this));
    _conn->connect(llmnr, LLMNR_PORT);
    return true;
}

void LLMNRResponder::_process_packet() {
    if (!_conn || !_conn->next())
        return;

#ifdef LLMNR_DEBUG
    Serial.println("LLMNR: RX'd packet");
#endif

    uint16_t id = _conn_read16();
    uint16_t flags = _conn_read16();
    uint16_t qdcount = _conn_read16();
    uint16_t ancount = _conn_read16();
    uint16_t nscount = _conn_read16();
    uint16_t arcount = _conn_read16();

#ifdef LLMNR_DEBUG
    Serial.print("LLMNR: ID=");
    Serial.println(id, HEX);
    Serial.print("LLMNR: FLAGS=");
    Serial.println(flags, HEX);
    Serial.print("LLMNR: QDCOUNT=");
    Serial.println(qdcount);
    Serial.print("LLMNR: ANCOUNT=");
    Serial.println(ancount);
    Serial.print("LLMNR: NSCOUNT=");
    Serial.println(nscount);
    Serial.print("LLMNR: ARCOUNT=");
    Serial.println(arcount);
#endif

#define BAD_FLAGS (FLAGS_QR | (FLAGS_OP_MASK << FLAGS_OP_SHIFT) | FLAGS_C)
    if (flags & BAD_FLAGS) {
#ifdef LLMNR_DEBUG
        Serial.println("Bad flags");
#endif
        return;
    }

    if (qdcount != 1) {
#ifdef LLMNR_DEBUG
        Serial.println("QDCOUNT != 1");
#endif
        return;
    }

    if (ancount || nscount || arcount) {
#ifdef LLMNR_DEBUG
        Serial.println("AN/NS/AR-COUNT != 0");
#endif
        return;
    }

    uint8_t namelen = _conn_read8();
#ifdef LLMNR_DEBUG
    Serial.print("QNAME len ");
    Serial.println(namelen);
#endif
    if (namelen != _hostname.length()) {
#ifdef LLMNR_DEBUG
        Serial.println("QNAME len mismatch");
#endif
        return;
    }

    char qname[64];
    _conn_readS(qname, namelen);
    _conn_read8();
    qname[namelen] = '\0';
#ifdef LLMNR_DEBUG
    Serial.print("QNAME ");
    Serial.println(qname);
#endif

    if (strcmp(_hostname.c_str(), qname)) {
#ifdef LLMNR_DEBUG
        Serial.println("QNAME mismatch");
#endif
        return;
    }

    uint16_t qtype = _conn_read16();
    uint16_t qclass = _conn_read16();

#ifdef LLMNR_DEBUG
    Serial.print("QTYPE ");
    Serial.print(qtype);
    Serial.print(" QCLASS ");
    Serial.println(qclass);
#endif

    bool have_rr =
        (qtype == 1) && /* A */
        (qclass == 1); /* IN */

    _conn->flush();

#ifdef LLMNR_DEBUG
    Serial.println("Match; responding");
    if (!have_rr)
        Serial.println("(no matching RRs)");
#endif

    IPAddress remote_ip = _conn->getRemoteAddress();

    struct ip_info ip_info;
    bool match_ap = false;
    if (wifi_get_opmode() & SOFTAP_MODE) {
        wifi_get_ip_info(SOFTAP_IF, &ip_info);
        IPAddress infoIp(ip_info.ip);
        IPAddress infoMask(ip_info.netmask);
        if (ip_info.ip.addr && ip_addr_netcmp((const ip_addr_t*)remote_ip, (const ip_addr_t*)infoIp, ip_2_ip4((const ip_addr_t*)infoMask)))
            match_ap = true;
    }
    if (!match_ap)
        wifi_get_ip_info(STATION_IF, &ip_info);
    uint32_t ip = ip_info.ip.addr;

    // Header
    uint8_t header[] = {
        (uint8_t)(id >> 8), (uint8_t)(id & 0xff), // ID
        (uint8_t)(FLAGS_QR >> 8), 0, // FLAGS
        0, 1, // QDCOUNT
        0, !!have_rr, // ANCOUNT
        0, 0, // NSCOUNT
        0, 0, // ARCOUNT
    };
    _conn->append(reinterpret_cast<const char*>(header), sizeof(header));
    // Question
    _conn->append(reinterpret_cast<const char*>(&namelen), 1);
    _conn->append(qname, namelen);
    uint8_t q[] = {
        0, // Name terminator
        0, 1, // TYPE (A)
        0, 1, // CLASS (IN)
    };
    _conn->append(reinterpret_cast<const char*>(q), sizeof(q));
    // Answer, if we have one
    if (have_rr) {
        _conn->append(reinterpret_cast<const char*>(&namelen), 1);
        _conn->append(qname, namelen);
        uint8_t rr[] = {
            0, // Name terminator
            0, 1, // TYPE (A)
            0, 1, // CLASS (IN)
            0, 0, 0, 30, // TTL (30 seconds)
            0, 4, // RDLENGTH
            (uint8_t)(ip & 0xff), (uint8_t)((ip >> 8) & 0xff), (uint8_t)((ip >> 16) & 0xff), (uint8_t)((ip >> 24) & 0xff) // RDATA
        };
        _conn->append(reinterpret_cast<const char*>(rr), sizeof(rr));
    }
    _conn->setMulticastInterface(remote_ip);
    _conn->send(remote_ip, _conn->getRemotePort());
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LLMNR)
LLMNRResponder LLMNR;
#endif
