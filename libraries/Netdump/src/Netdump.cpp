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
CallBackList<Netdump::LwipCallback> Netdump::lwipCallback;

Netdump::Netdump()
{
    using namespace std::placeholders;
    phy_capture = capture;
    lwipHandler = lwipCallback.add(std::bind(&Netdump::netdumpCapture, this, _1, _2, _3, _4, _5));
};

Netdump::~Netdump()
{
    reset();
    if (packetBuffer)
    {
        delete[] packetBuffer;
    }
};

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

void Netdump::printDump(Print& out, Packet::PacketDetail ndd, const Filter nf)
{
    out.printf_P(PSTR("netDump starting\r\n"));
    setCallback([&out, ndd, this](const Packet& ndp)
                { printDumpProcess(out, ndd, ndp); },
                nf);
}

void Netdump::fileDump(File& outfile, const Filter nf)
{
    writePcapHeader(outfile);
    setCallback([&outfile, this](const Packet& ndp)
                { fileDumpProcess(outfile, ndp); },
                nf);
}
bool Netdump::tcpDump(WiFiServer& tcpDumpServer, const Filter nf)
{
    if (!packetBuffer)
    {
        packetBuffer = new (std::nothrow) char[tcpBufferSize];

        if (!packetBuffer)
        {
            return false;
        }
    }
    bufferIndex = 0;

    schedule_function([&tcpDumpServer, this, nf]()
                      { tcpDumpLoop(tcpDumpServer, nf); });
    return true;
}

void Netdump::capture(int netif_idx, const char* data, size_t len, int out, int success)
{
    if (lwipCallback.execute(netif_idx, data, len, out, success) == 0)
    {
        phy_capture = nullptr;  // No active callback/netdump instances, will be set again by new object.
    }
}

void Netdump::netdumpCapture(int netif_idx, const char* data, size_t len, int out, int success)
{
    if (netDumpCallback)
    {
        Packet np(millis(), netif_idx, data, len, out, success);
        if (netDumpFilter && !netDumpFilter(np))
        {
            return;
        }
        netDumpCallback(np);
    }
}

void Netdump::writePcapHeader(Stream& s) const
{
    uint32_t pcapHeader[6];
    pcapHeader[0] = 0xa1b2c3d4;     // pcap magic number
    pcapHeader[1] = 0x00040002;     // pcap major/minor version
    pcapHeader[2] = 0;              // pcap UTC correction in seconds
    pcapHeader[3] = 0;              // pcap time stamp accuracy
    pcapHeader[4] = maxPcapLength;  // pcap max packet length per record
    pcapHeader[5] = 1;              // pacp data linkt type = ethernet
    s.write(reinterpret_cast<char*>(pcapHeader), 24);
}

void Netdump::printDumpProcess(Print& out, Packet::PacketDetail ndd, const Packet& np) const
{
    out.printf_P(PSTR("%8lld %s"), np.getTime(), np.toString(ndd).c_str());
}

void Netdump::fileDumpProcess(File& outfile, const Packet& np) const
{
    size_t         incl_len = np.getPacketSize() > maxPcapLength ? maxPcapLength : np.getPacketSize();
    uint32_t       pcapHeader[4];

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    pcapHeader[0] = tv.tv_sec;
    pcapHeader[1] = tv.tv_usec;
    pcapHeader[2] = incl_len;
    pcapHeader[3] = np.getPacketSize();
    outfile.write(reinterpret_cast<char*>(pcapHeader), 16);  // pcap record header

    outfile.write(np.rawData(), incl_len);
}

void Netdump::tcpDumpProcess(const Packet& np)
{
    if (np.isTCP() && np.hasPort(tcpDumpClient.localPort()))
    {
        // skip myself
        return;
    }
    size_t incl_len = np.getPacketSize() > maxPcapLength ? maxPcapLength : np.getPacketSize();

    if (bufferIndex + 16 + incl_len < tcpBufferSize)  // only add if enough space available
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        uint32_t* pcapHeader = reinterpret_cast<uint32_t*>(&packetBuffer[bufferIndex]);
        pcapHeader[0]        = tv.tv_sec;  // add pcap record header
        pcapHeader[1]        = tv.tv_usec;
        pcapHeader[2]        = incl_len;
        pcapHeader[3]        = np.getPacketSize();
        bufferIndex += 16;  // pcap header size
        memcpy(&packetBuffer[bufferIndex], np.rawData(), incl_len);
        bufferIndex += incl_len;
    }

    if (bufferIndex && tcpDumpClient && tcpDumpClient.availableForWrite() >= (int)bufferIndex)
    {
        tcpDumpClient.write(packetBuffer, bufferIndex);
        bufferIndex = 0;
    }
}

void Netdump::tcpDumpLoop(WiFiServer& tcpDumpServer, const Filter nf)
{
    if (tcpDumpServer.hasClient())
    {
        tcpDumpClient = tcpDumpServer.accept();
        tcpDumpClient.setNoDelay(true);

        bufferIndex = 0;
        writePcapHeader(tcpDumpClient);

        setCallback([this](const Packet& ndp)
                    { tcpDumpProcess(ndp); },
                    nf);
    }
    if (!tcpDumpClient || !tcpDumpClient.connected())
    {
        setCallback(nullptr);
    }
    if (bufferIndex && tcpDumpClient && tcpDumpClient.availableForWrite() >= (int)bufferIndex)
    {
        tcpDumpClient.write(packetBuffer, bufferIndex);
        bufferIndex = 0;
    }

    if (tcpDumpServer.status() != CLOSED)
    {
        schedule_function([&tcpDumpServer, this, nf]()
                          { tcpDumpLoop(tcpDumpServer, nf); });
    }
}

}  // namespace NetCapture
