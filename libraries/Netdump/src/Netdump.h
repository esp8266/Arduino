/*
    NetDump library - tcpdump-like packet logger facility

    Copyright (c) 2019 Herman Reintke. All rights reserved.
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

#ifndef __NETDUMP_H
#define __NETDUMP_H

#include <Print.h>
#include <functional>
#include <lwipopts.h>
#include <FS.h>
#include "NetdumpPacket.h"
#include <ESP8266WiFi.h>

namespace NetCapture
{

class Netdump
{
public:

    using Filter = std::function<bool(Packet&)>;
    using Callback = std::function<void(Packet&)>;

    Netdump();
    ~Netdump();

    void setCallback(const Callback nc);
    void setCallback(const Callback nc, const Filter nf);
    void setFilter(const Filter nf);
    void reset();

    void printDump(Print& out, Packet::PacketDetail ndd, const Filter nf = nullptr);
    void fileDump(File& outfile, const Filter nf = nullptr);
    void tcpDump(WiFiServer &tcpDumpServer, const Filter nf = nullptr);


private:
    Callback netDumpCallback = nullptr;
    Filter   netDumpFilter   = nullptr;

    static Netdump* self;

    static void capture(int netif_idx, const char* data, size_t len, int out, int success);
    void printDumpProcess(Print& out, Packet::PacketDetail ndd, const Packet& np) const;
    void fileDumpProcess(File& outfile, const Packet& np) const;
    void tcpDumpProcess(const Packet& np);
    void tcpDumpLoop(WiFiServer &tcpDumpServer, const Filter nf);

    void writePcapHeader(Stream& s) const;

    WiFiClient tcpDumpClient;
    char* packetBuffer = nullptr;
    size_t bufferIndex = 0;

    static constexpr int tcpBuffersize = 2048;
    static constexpr int maxPcapLength = 1024;
    static constexpr uint32_t pcapMagic = 0xa1b2c3d4;

};

} // namespace NetCapture

#endif /* __NETDUMP_H */
