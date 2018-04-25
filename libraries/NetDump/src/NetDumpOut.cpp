/*
 NetDump library - tcpdump-like packet logger facility

 Copyright (c) 2018 David Gauchard. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <ESP8266WiFi.h>
#include <NetDump.h>
#include <lwipopts.h>

static WiFiServer tcpdump_server(2); // port will be overwritten
static WiFiClient tcpdump_client;    // only one allowed

static bool fastsend;
static size_t snaplen, ptr;
static char* buf = nullptr;
static uint16_t svcport;

size_t tcpdump_err = 0;

#define BUFSIZE TCP_MSS // one tcp segment max, multiple of 4

static void dump (int netif_idx, const char* data, size_t len, int out, int success)
{
    (void)netif_idx;
    (void)success;
    
    if (   netDump_is_IPv4(data)
        && netDump_is_TCP(data)
        && (   ( out && netDump_getSrcPort(data) == svcport)
            || (!out && netDump_getDstPort(data) == svcport)
           )
       )
    {
        // skip myself
        return;
    }
    
    size_t snap = (len + 3) & ~3;
    if (snaplen < snap)
        snap = snaplen;
    
    if ((ptr + (4*4) + snap) > BUFSIZE)
    {
        // no room, lost capture
        tcpdump_err++;
        return;
    }

    if (!buf || snap <= 0)
        return;

    // pcap-savefile(5) packet header
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    *(uint32_t*)&buf[ptr] = tv.tv_sec;
    *(uint32_t*)&buf[ptr+4] = tv.tv_usec;
    *(uint32_t*)&buf[ptr+8] = snap;
    *(uint32_t*)&buf[ptr+12] = len < snap? snap: len;

    memcpy(buf + ptr + 4*4, data, snap > len? len: snap);
    
    ptr += 4*4 + snap;
}

bool tcpdump_setup (uint16_t port, size_t snap, bool fast)
{
    if (!buf)
        buf = new char[BUFSIZE];

    if (buf)
    {
        snaplen = (snap + 3) & ~3;
        fastsend = fast;
        svcport = port;
        tcpdump_server.begin(svcport);
        return true; //!!tcpdump_server;
    }

    if (buf)
      delete [] buf;
    buf = nullptr;
    return false;
}

void tcpdump_loop ()
{
    if (tcpdump_server.hasClient())
    {
        tcpdump_client = tcpdump_server.available();
        if (fastsend)
            tcpdump_client.setNoDelay(true);

        // pcap-savefile(5) capture preamble
        *(uint32_t*)&buf[0] = 0xa1b2c3d4;
        *(uint32_t*)&buf[4] = 0x00040002;
        *(uint32_t*)&buf[8] = 0;
        *(uint32_t*)&buf[12] = 0;
        *(uint32_t*)&buf[16] = snaplen;
        *(uint32_t*)&buf[20] = 1;
        tcpdump_client.write(buf, 24);

        ptr = 0;
        phy_capture = dump;
    }
    
    if (!tcpdump_client || !tcpdump_client.connected())
        phy_capture = nullptr;
   
    if (ptr && tcpdump_client && tcpdump_client.availableForWrite() >= ptr)
    {
        tcpdump_client.write(buf, ptr);
        ptr = 0;
    }
}
