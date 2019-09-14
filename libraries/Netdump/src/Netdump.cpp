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

#include "Netdump.h"
#include <lwip/init.h>
#include "Schedule.h"

Netdump* Netdump::self;

void Netdump::setCallback(NetdumpCallback nc)
{
    netDumpCallback = nc;
}
void Netdump::setCallback(NetdumpCallback nc, NetdumpFilter nf)
{
    netDumpFilter   = nf;
    netDumpCallback = nc;
}
void Netdump::setFilter(NetdumpFilter nf)
{
    netDumpFilter = nf;
}
void Netdump::reset()
{
    setCallback(nullptr, nullptr);
}
void Netdump::printDump(Print& out, NetdumpPacket::PacketDetail ndd, NetdumpFilter nf)
{
    out.printf("netDump starting\r\n");
//    setCallback(std::bind(&Netdump::printDumpProcess, this, std::ref(out), ndd, std::placeholders::_1), nf);
    setCallback([&out, ndd, this](NetdumpPacket & ndp)
    {
        printDumpProcess(out, ndd, ndp);
    }, nf);


}
void Netdump::fileDump(File outfile, NetdumpFilter nf)
{

    char buf[24];

    *(uint32_t*)&buf[0] = 0xa1b2c3d4;
    *(uint32_t*)&buf[4] = 0x00040002;
    *(uint32_t*)&buf[8] = 0;
    *(uint32_t*)&buf[12] = 0;
    *(uint32_t*)&buf[16] = 1024;
    *(uint32_t*)&buf[20] = 1;

    outfile.write(buf, 24);
    //	setCallback( std::bind(&Netdump::fileDumpProcess, this, outfile, std::placeholders::_1));
    setCallback([outfile, this](NetdumpPacket & ndp)
    {
        fileDumpProcess(outfile, ndp);
    }, nf);
}
void Netdump::tcpDump(WiFiServer &tcpDumpServer, NetdumpFilter nf)
{
    // Get initialize code from netdumpout.cpp
    if (packetBuffer)
    {
        delete packetBuffer;
    }
    packetBuffer = new char[2048];
    bufferIndex = 0;

    //	schedule_function(std::bind(&Netdump::tcpDumpLoop,this,std::ref(tcpDumpServer)));
    schedule_function([&tcpDumpServer, this]()
    {
        tcpDumpLoop(tcpDumpServer);
    });
    Serial.printf("scheduled\r\n");
}

void Netdump::capture(int netif_idx, const char* data, size_t len, int out, int success)
{
    NetdumpPacket np(netif_idx, data, len, out, success);
    if (self->netDumpCallback)
    {
        if (self->netDumpFilter  && !self->netDumpFilter(np))
        {
            return;
        }
        self->netDumpCallback(np);
    }
}

void Netdump::printDumpProcess(Print& out, NetdumpPacket::PacketDetail ndd, NetdumpPacket np)
{
    out.printf("%8d %s", millis(), np.toString(ndd).c_str());
}
void Netdump::fileDumpProcess(File outfile, NetdumpPacket np)
{
    size_t incl_len = np.len > 1024 ? 1024 : np.len;
    char buf[16];

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    *(uint32_t*)&buf[0] = tv.tv_sec;
    *(uint32_t*)&buf[4] = tv.tv_usec;
    *(uint32_t*)&buf[8] = incl_len;
    *(uint32_t*)&buf[12] = np.len;
    outfile.write(buf, 16);

    outfile.write(np.data, incl_len);
}
void Netdump::tcpDumpProcess(NetdumpPacket np)
{
    // Get capture code from netdumpout.cpp
    if (np.isIPv4() && np.isTCP()
            && ((np.out && np.getSrcPort() == tcpDumpClient.localPort())
                || (!np.out && np.getDstPort() == tcpDumpClient.localPort())
               )
       )
    {
        // skip myself
        return;
    }
    size_t incl_len = np.len > 1024 ? 1024 : np.len;

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    *(uint32_t*)&packetBuffer[bufferIndex] = tv.tv_sec;
    *(uint32_t*)&packetBuffer[bufferIndex + 4] = tv.tv_usec;
    *(uint32_t*)&packetBuffer[bufferIndex + 8] = incl_len;
    *(uint32_t*)&packetBuffer[bufferIndex + 12] = np.len;
    bufferIndex += 16;
    memcpy(&packetBuffer[bufferIndex], np.data, incl_len);
    bufferIndex += incl_len;
    if (bufferIndex && tcpDumpClient && tcpDumpClient.availableForWrite() >= bufferIndex)
    {
        tcpDumpClient.write(packetBuffer, bufferIndex);
        bufferIndex = 0;
    }


}
void Netdump::tcpDumpLoop(WiFiServer &tcpDumpServer)
{

    if (tcpDumpServer.hasClient())
    {
        tcpDumpClient = tcpDumpServer.available();
        //if (fastsend)
        tcpDumpClient.setNoDelay(true);


        // pcap-savefile(5) capture preamble
        *(uint32_t*)&packetBuffer[0] = 0xa1b2c3d4;
        *(uint32_t*)&packetBuffer[4] = 0x00040002;
        *(uint32_t*)&packetBuffer[8] = 0;
        *(uint32_t*)&packetBuffer[12] = 0;
        *(uint32_t*)&packetBuffer[16] = 1024;
        *(uint32_t*)&packetBuffer[20] = 1;
        tcpDumpClient.write(packetBuffer, 24);
        bufferIndex = 0;
        //      setCallback(std::bind(&Netdump::tcpDumpProcess,this,std::placeholders::_1));
        setCallback([this](NetdumpPacket & ndp)
        {
            tcpDumpProcess(ndp);
        });

        Serial.printf("client started\r\n");
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
    //  schedule_function(std::bind(&Netdump::tcpDumpLoop,this,std::ref(tcpDumpServer)));
    schedule_function([&tcpDumpServer, this]()
    {
        tcpDumpLoop(tcpDumpServer);
    });
}
