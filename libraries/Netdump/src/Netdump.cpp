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

#include "Netdump.h"
#include <lwip/init.h>
#include "Schedule.h"

namespace NetCapture
{

Netdump* Netdump::self;

void Netdump::setCallback(const Callback nc)
{
    netDumpCallback = nc;
}

void Netdump::setCallback(const Callback nc, const Filter nf)
{
    netDumpFilter   = nf;
    netDumpCallback = nc;
}

void Netdump::setFilter(const Filter nf)
{
    netDumpFilter = nf;
}

void Netdump::reset()
{
    setCallback(nullptr, nullptr);
}

void Netdump::printDump(Print& out, Packet::PacketDetail ndd, Filter nf)
{
    out.printf("netDump starting\r\n");
    setCallback([&out, ndd, this](Packet & ndp)
    {
        printDumpProcess(out, ndd, ndp);
    }, nf);
}

void Netdump::fileDump(File& outfile, Filter nf)
{
    uint32_t buf[6];
    buf[0] = 0xa1b2c3d4;
    buf[1] = 0x00040002;
    buf[2] = 0;
    buf[3] = 0;
    buf[4] = maxPcapLength;
    buf[5] = 1;

    outfile.write((uint8_t*)buf, 24);
    setCallback([&outfile, this](Packet & ndp)
    {
        fileDumpProcess(outfile, ndp);
    }, nf);
}
void Netdump::tcpDump(WiFiServer &tcpDumpServer, Filter nf)
{
    if (packetBuffer)
    {
        delete packetBuffer;
    }
    packetBuffer = new char[tcpBuffersize];
    bufferIndex = 0;

    schedule_function([&tcpDumpServer, this, nf]()
    {
        tcpDumpLoop(tcpDumpServer, nf);
    });
}

void Netdump::capture(int netif_idx, const char* data, size_t len, int out, int success)
{
    Packet np(millis(), netif_idx, data, len, out, success);
    if (self->netDumpCallback)
    {
        if (self->netDumpFilter  && !self->netDumpFilter(np))
        {
            return;
        }
        self->netDumpCallback(np);
    }
}

void Netdump::printDumpProcess(Print& out, Packet::PacketDetail ndd, const Packet& np)
{
    out.printf("%8d %s", np.getTime(), np.toString(ndd).c_str());
}

void Netdump::fileDumpProcess(File& outfile, const Packet& np)
{
    size_t incl_len = np.getPacketSize() > maxPcapLength ? maxPcapLength : np.getPacketSize();
    char buf[16];

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    *(uint32_t*)&buf[0] = tv.tv_sec;
    *(uint32_t*)&buf[4] = tv.tv_usec;
    *(uint32_t*)&buf[8] = incl_len;
    *(uint32_t*)&buf[12] = np.getPacketSize();
    outfile.write(buf, 16);

    outfile.write(np.rawData(), incl_len);
}

void Netdump::tcpDumpProcess(const Packet& np)
{
    if (np.isIPv4() && np.isTCP()
            && ((np.getInOut() && np.getSrcPort() == tcpDumpClient.localPort())
                || (!np.getInOut() && np.getDstPort() == tcpDumpClient.localPort())
               )
       )
    {
        // skip myself
        return;
    }
    size_t incl_len = np.getPacketSize() > maxPcapLength ? maxPcapLength : np.getPacketSize();

    if (bufferIndex+16+incl_len < tcpBuffersize) // only add if enough space available
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        *(uint32_t*)&packetBuffer[bufferIndex] = tv.tv_sec;
        *(uint32_t*)&packetBuffer[bufferIndex + 4] = tv.tv_usec;
        *(uint32_t*)&packetBuffer[bufferIndex + 8] = incl_len;
        *(uint32_t*)&packetBuffer[bufferIndex + 12] = np.getPacketSize();
        bufferIndex += 16;
        memcpy(&packetBuffer[bufferIndex], np.rawData(), incl_len);
        bufferIndex += incl_len;
    }

    if (bufferIndex && tcpDumpClient && tcpDumpClient.availableForWrite() >= bufferIndex)
    {
        tcpDumpClient.write(packetBuffer, bufferIndex);
        bufferIndex = 0;
    }
}

void Netdump::tcpDumpLoop(WiFiServer &tcpDumpServer, Filter nf)
{
    if (tcpDumpServer.hasClient())
    {
        tcpDumpClient = tcpDumpServer.available();

        tcpDumpClient.setNoDelay(true);

        // pcap-savefile(5) capture preamble
        *(uint32_t*)&packetBuffer[0] = 0xa1b2c3d4;
        *(uint32_t*)&packetBuffer[4] = 0x00040002;
        *(uint32_t*)&packetBuffer[8] = 0;
        *(uint32_t*)&packetBuffer[12] = 0;
        *(uint32_t*)&packetBuffer[16] = maxPcapLength;
        *(uint32_t*)&packetBuffer[20] = 1;
        tcpDumpClient.write(packetBuffer, 24);
        bufferIndex = 0;

        setCallback([this](Packet & ndp)
        {
            tcpDumpProcess(ndp);
        }, nf);
    }
    if (!tcpDumpClient || !tcpDumpClient.connected())
    {
        setCallback(nullptr);
    }
    if (bufferIndex && tcpDumpClient && tcpDumpClient.availableForWrite() >= bufferIndex)
    {
        Serial.printf("tcp write %d\r\n", bufferIndex);
        tcpDumpClient.write(packetBuffer, bufferIndex);
        bufferIndex = 0;
    }

    if (tcpDumpServer.status() != CLOSED)
    {
        schedule_function([&tcpDumpServer, this, nf]()
        {
            tcpDumpLoop(tcpDumpServer, nf);
        });
    }
}

} // namespace NetCapture
