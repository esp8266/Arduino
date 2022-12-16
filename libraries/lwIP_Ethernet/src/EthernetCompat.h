
#pragma once

#include <LwipEthernet.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ArduinoWiFiServer.h>

using EthernetUDP    = WiFiUDP;
using EthernetClient = WiFiClient;
using EthernetServer = ArduinoWiFiServer;

enum EthernetLinkStatus
{
    Unknown,
    LinkON,
    LinkOFF
};

enum
{
    DHCP_CHECK_NONE        = 0,
    DHCP_CHECK_RENEW_FAIL  = 1,
    DHCP_CHECK_RENEW_OK    = 2,
    DHCP_CHECK_REBIND_FAIL = 3,
    DHCP_CHECK_REBIND_OK   = 4,
};

enum HardwareStatus
{
    EthernetNoHardware,
    EthernetHardwareFound,
};

template<class RawDev>
class ArduinoEthernet: public LwipIntfDev<RawDev>
{
public:
    ArduinoEthernet(int8_t cs = SS, SPIClass& spi = SPI, int8_t intr = -1) :
        LwipIntfDev<RawDev>(cs, spi, intr)
    {
        _hardwareStatus = EthernetNoHardware;
        _linkStatus     = Unknown;
    }

    // Arduino-Ethernet API compatibility, order can be either:
    // mac, ip, gateway, netmask, dns (esp8266 or natural order)
    // mac, ip, dns, gateway, netmask (Arduino legacy)
    boolean begin(const uint8_t* macAddress, const IPAddress& local_ip = IPADDR_NONE,
                  const IPAddress& arg1 = IPADDR_NONE, const IPAddress& arg2 = IPADDR_NONE,
                  const IPAddress& arg3 = IPADDR_NONE)
    {
        SPI4EthInit();  // Arduino Ethernet self-initializes SPI
        bool ret = true;
        if (local_ip.isSet())
            ret = LwipIntfDev<RawDev>::config(local_ip, arg1, arg2, arg3);
        if (ret)
        {
            ret = LwipIntfDev<RawDev>::begin(macAddress);
            if (!local_ip.isSet())
            {
                // Arduino API waits for DHCP answer
                while (!LwipIntfDev<RawDev>::connected())
                {
                    delay(100);
                }
            }
        }

        if (ret)
        {
            _hardwareStatus = EthernetHardwareFound;
            _linkStatus     = LinkON;
        }

        return ret;
    }

    HardwareStatus hardwareStatus() const
    {
        return _hardwareStatus;
    }

    EthernetLinkStatus linkStatus() const
    {
        return _linkStatus;
    }

    int maintain() const
    {
        return DHCP_CHECK_NONE;
    }

protected:
    HardwareStatus     _hardwareStatus;
    EthernetLinkStatus _linkStatus;
};

using ArduinoWiznet5500lwIP = ArduinoEthernet<Wiznet5500>;
using ArduinoWiznet5100lwIP = ArduinoEthernet<Wiznet5100>;
using ArduinoENC28J60lwIP   = ArduinoEthernet<ENC28J60>;
