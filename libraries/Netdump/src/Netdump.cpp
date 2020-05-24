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
#include "sdk_structs.h"
#include "ieee80211_structs.h"
#include "string_utils.h"

namespace NetCapture
{

CallBackList<Netdump::LwipCallback> Netdump::lwipCallback;
CallBackList<Netdump::WifiCallback> Netdump::wifiCallback;

Netdump::Netdump(interface ifc)
{
	using namespace std::placeholders;
    if (ifc == interface::LWIP)
    {
        phy_capture = lwipCapture;
        lwipHandler = lwipCallback.add(std::bind(&Netdump::netdumpCapture, this, _1, _2, _3, _4, _5));
    }
    else
    {
    	  wifi_set_opmode(STATION_MODE);
    	  wifi_promiscuous_enable(0);
    	  WiFi.disconnect();
    	  wifi_set_promiscuous_rx_cb(wifiCapture);
    	  wifi_set_channel(6);
    	  wifi_promiscuous_enable(1);
    	  Serial.write("Prom mode\r\n");
    	  wifiHandler = wifiCallback.add(std::bind(&Netdump::wifidumpCapture,this, _1, _2));
    }
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
    out.printf("netDump starting\r\n");
    setCallback([&out, ndd, this](const Packet & ndp)
    {
        printDumpProcess(out, ndd, ndp);
    }, nf);
}

void Netdump::fileDump(File& outfile, const Filter nf)
{

    writePcapHeader(outfile);
    setCallback([&outfile, this](const Packet & ndp)
    {
        fileDumpProcess(outfile, ndp);
    }, nf);
}
void Netdump::tcpDump(WiFiServer &tcpDumpServer, const Filter nf)
{

    if (!packetBuffer)
    {
        packetBuffer = new char[tcpBuffersize];
    }
    bufferIndex = 0;

    schedule_function([&tcpDumpServer, this, nf]()
    {
        tcpDumpLoop(tcpDumpServer, nf);
    });
}

void Netdump::lwipCapture(int netif_idx, const char* data, size_t len, int out, int success)
{
    if (lwipCallback.execute(netif_idx, data, len, out, success) == 0)
    {
        phy_capture = nullptr; // No active callback/netdump instances, will be set again by new object.
    }
}

void Netdump::netdumpCapture(int netif_idx, const char* data, size_t len, int out, int success)
{
    if (netDumpCallback)
    {
        Packet np(millis(), netif_idx, data, len, out, success);
        if (netDumpFilter  && !netDumpFilter(np))
        {
            return;
        }
        netDumpCallback(np);
    }
}

void Netdump::wifiCapture(unsigned char* data, uint16_t len)
{
	wifiCallback.execute(reinterpret_cast<const char*>(data),len);
}

void Netdump::wifidumpCapture(const char* buff, uint16_t len)
{
//	netdumpCapture(0, data, len, 0, 1);

	  // First layer: type cast the received buffer into our generic SDK structure
	  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
	  // Second layer: define pointer to where the actual 802.11 packet is within the structure
	  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
	  // Third layer: define pointers to the 802.11 packet header and payload
	  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;
	  const uint8_t *data = ipkt->payload;

	  // Pointer to the frame control section within the packet header
	  const wifi_header_frame_control_t *frame_ctrl = (wifi_header_frame_control_t *)&hdr->frame_ctrl;
	  // Parse MAC addresses contained in packet header into human-readable strings
	  char addr1[] = "00:00:00:00:00:00\0";
	  char addr2[] = "00:00:00:00:00:00\0";
	  char addr3[] = "00:00:00:00:00:00\0";

	  mac2str(hdr->addr1, addr1);
	  mac2str(hdr->addr2, addr2);
	  mac2str(hdr->addr3, addr3);

	  // Output info to serial
//	  if (!(frame_ctrl->type == WIFI_PKT_MGMT && frame_ctrl->subtype == BEACON))
		  if ((frame_ctrl->type == WIFI_PKT_DATA))
	  Serial.printf("\r\n%s | %s | %s | %u | %02d | %u | %u(%-2u) | %-28s | %u | %u | %u | %u | %u | %u | %u | %u | ",
	    addr1,
	    addr2,
	    addr3,
	    wifi_get_channel(),
	    ppkt->rx_ctrl.rssi,
	    frame_ctrl->protocol,
	    frame_ctrl->type,
	    frame_ctrl->subtype,
	    wifi_pkt_type2str((wifi_promiscuous_pkt_type_t)frame_ctrl->type, (wifi_mgmt_subtypes_t)frame_ctrl->subtype),
	    frame_ctrl->to_ds,
	    frame_ctrl->from_ds,
	    frame_ctrl->more_frag,
	    frame_ctrl->retry,
	    frame_ctrl->pwr_mgmt,
	    frame_ctrl->more_data,
	    frame_ctrl->wep,
	    frame_ctrl->strict);
	  if (frame_ctrl->type == WIFI_PKT_DATA)
	  {
		  Serial.printf("\r\nData %04x",frame_ctrl->subtype);
	  }
	  // Print ESSID if beacon
	  /*
	  if (frame_ctrl->type == WIFI_PKT_MGMT && frame_ctrl->subtype == BEACON)
	  {
	    const wifi_mgmt_beacon_t *beacon_frame = (wifi_mgmt_beacon_t*) ipkt->payload;
	    char ssid[32] = {0};

	    if (beacon_frame->tag_length >= 32)
	    {
	      strncpy(ssid, beacon_frame->ssid, 31);
	    }
	    else
	    {
	      strncpy(ssid, beacon_frame->ssid, beacon_frame->tag_length);
	    }

	    Serial.printf("%s", ssid);
	  }
	  */
}

/*
  uint32_t timestamp = now(); //current timestamp
  uint32_t microseconds = (unsigned int)(micros() - millis() * 1000); //micro seconds offset (0 - 999)
  pcap.newPacketSerial(timestamp, microseconds, len, buf); //write packet to file
*/

void Netdump::writePcapHeader(Stream& s) const
{
    uint32_t pcapHeader[6];
    pcapHeader[0] = 0xa1b2c3d4;     // pcap magic number
    pcapHeader[1] = 0x00040002;     // pcap major/minor version
    pcapHeader[2] = 0;			     // pcap UTC correction in seconds
    pcapHeader[3] = 0;			     // pcap time stamp accuracy
    pcapHeader[4] = maxPcapLength;  // pcap max packet length per record
    pcapHeader[5] = 1;              // pacp data linkt type = ethernet
    s.write(reinterpret_cast<char*>(pcapHeader), 24);
}

void Netdump::printDumpProcess(Print& out, Packet::PacketDetail ndd, const Packet& np) const
{
    out.printf_P(PSTR("%8d %s"), np.getTime(), np.toString(ndd).c_str());
}

void Netdump::fileDumpProcess(File& outfile, const Packet& np) const
{
    size_t incl_len = np.getPacketSize() > maxPcapLength ? maxPcapLength : np.getPacketSize();
    uint32_t pcapHeader[4];

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    pcapHeader[0] = tv.tv_sec;
    pcapHeader[1] = tv.tv_usec;
    pcapHeader[2] = incl_len;
    pcapHeader[3] = np.getPacketSize();
    outfile.write(reinterpret_cast<char*>(pcapHeader), 16); // pcap record header

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

    if (bufferIndex + 16 + incl_len < tcpBuffersize) // only add if enough space available
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        uint32_t* pcapHeader = reinterpret_cast<uint32_t*>(&packetBuffer[bufferIndex]);
        pcapHeader[0] = tv.tv_sec;      // add pcap record header
        pcapHeader[1] = tv.tv_usec;
        pcapHeader[2] = incl_len;
        pcapHeader[3] = np.getPacketSize();
        bufferIndex += 16; // pcap header size
        memcpy(&packetBuffer[bufferIndex], np.rawData(), incl_len);
        bufferIndex += incl_len;
    }

    if (bufferIndex && tcpDumpClient && tcpDumpClient.availableForWrite() >= bufferIndex)
    {
        tcpDumpClient.write(packetBuffer, bufferIndex);
        bufferIndex = 0;
    }
}

void Netdump::tcpDumpLoop(WiFiServer &tcpDumpServer, const Filter nf)
{
    if (tcpDumpServer.hasClient())
    {
        tcpDumpClient = tcpDumpServer.available();
        tcpDumpClient.setNoDelay(true);

        bufferIndex = 0;
        writePcapHeader(tcpDumpClient);

        setCallback([this](const Packet & ndp)
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
