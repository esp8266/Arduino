
#ifndef _LWIPETH_H
#define _LWIPETH_H

// TODO:
// remove all Serial.print
// unchain pbufs

#include <netif/ethernet.h>
#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/etharp.h>
#include <lwip/dhcp.h>

#ifdef ESP8266
#include <user_interface.h>	// wifi_get_macaddr()
#include <Schedule.h>
#endif
#include <SPI.h>
#include <IPAddress.h>
#include <FunctionalInterrupt.h>

#ifndef DEFAULT_MTU
#define DEFAULT_MTU 1500
#endif

template <class RawEthernet>
class LwipEthernet: public RawEthernet {

public:

    LwipEthernet (SPIClass& spi = SPI, int8_t cs = SS, int8_t intr = -1):
        RawEthernet(spi, cs, intr),
        _mtu(DEFAULT_MTU),
        _default(false),
        _intrPin(intr)
    {
        memset(&_netif, 0, sizeof(_netif));
    }

    // start with dhcp client
    // default mac-address is inferred from esp8266's STA interface
    boolean begin (const uint8_t *macAddress = nullptr, uint16_t mtu = DEFAULT_MTU);

    const netif* getNetIf   () const { return &_netif; }

#if LWIP_VERSION_MAJOR == 1
    IPAddress    localIP    () const { return IPAddress(_netif.ip_addr.u_addr.ip4.addr); }
    IPAddress    subnetMask () const { return IPAddress(_netif.netmask.u_addr.ip4.addr); }
    IPAddress    gatewayIP  () const { return IPAddress(_netif.gw.u_addr.ip4.addr); }
#else
    IPAddress    localIP    () const { return IPAddress(ip4_addr_get_u32(ip_2_ip4(&_netif.ip_addr))); }
    IPAddress    subnetMask () const { return IPAddress(ip4_addr_get_u32(ip_2_ip4(&_netif.netmask))); }
    IPAddress    gatewayIP  () const { return IPAddress(ip4_addr_get_u32(ip_2_ip4(&_netif.gw))); }
#endif

    void setDefault ();

    bool connected () { return !!ip4_addr_get_u32(ip_2_ip4(&_netif.ip_addr)); }

protected:

    netif _netif;
    uint16_t _mtu;
    bool _default;
    int8_t _intrPin;
    uint8_t _macAddress[6];

    err_t start_with_dhclient ();
    err_t netif_init ();
    void  netif_status_callback ();

    static err_t netif_init_s (netif* netif);
    static err_t linkoutput_s (netif *netif, struct pbuf *p);
    static void  netif_status_callback_s (netif* netif);

    // called on a regular basis or on interrupt
    err_t handlePackets ();
};

template <class RawEthernet>
boolean LwipEthernet<RawEthernet>::begin (const uint8_t* macAddress, uint16_t mtu)
{
    if (macAddress)
        memcpy(_macAddress, macAddress, 6);
    else
    {
        _netif.num = 2;
        for (auto n = netif_list; n; n = n->next)
            if (n->num >= _netif.num)
                _netif.num = n->num + 1;

#ifdef ESP8266
        // make a new mac-address from the esp's wifi sta one
        // I understand this is cheating with an official mac-address
        wifi_get_macaddr(STATION_IF, (uint8*)_macAddress);
#else
        // https://serverfault.com/questions/40712/what-range-of-mac-addresses-can-i-safely-use-for-my-virtual-machines
        // TODO ESP32: get wifi mac address like with esp8266 above
        memset(_macAddress, 0, 6);
        _macAddress[0] = 0xEE;
#endif
        _macAddress[3] += _netif.num;
        memcpy(_netif.hwaddr, _macAddress, 6);
    }

    if (!RawEthernet::begin(_macAddress))
        return false;
    _mtu = mtu;

    switch (start_with_dhclient())
    {
    case ERR_OK:
        break;

    case ERR_IF:
        return false;

    default:
        netif_remove(&_netif);
        return false;
    }

    if (_intrPin >= 0)
    {
        ::printf((PGM_P)F("w500-lwIP: Interrupt not implemented yet, enabling transparent polling\r\n"));
        // still need to enable interrupt in wiznet driver
        _intrPin = -1;
    }

    if (_intrPin >= 0)
        attachInterrupt(_intrPin, [&]() { this->handlePackets(); }, FALLING);
    else if (!schedule_recurrent_function_us([&]() { this->handlePackets(); return true; }, 100))
    {
        netif_remove(&_netif);
        return false;
    }

    return true;
}

template <class RawEthernet>
err_t LwipEthernet<RawEthernet>::start_with_dhclient ()
{
    ip4_addr_t ip, mask, gw;

    ip4_addr_set_zero(&ip);
    ip4_addr_set_zero(&mask);
    ip4_addr_set_zero(&gw);

    _netif.hwaddr_len = sizeof _macAddress;
    memcpy(_netif.hwaddr, _macAddress, sizeof _macAddress);

    if (!netif_add(&_netif, &ip, &mask, &gw, this, netif_init_s, ethernet_input))
        return ERR_IF;

    _netif.flags |= NETIF_FLAG_UP;

    return dhcp_start(&_netif);
}

template <class RawEthernet>
err_t LwipEthernet<RawEthernet>::linkoutput_s (netif *netif, struct pbuf *pbuf)
{
    LwipEthernet* ths = (LwipEthernet*)netif->state;

#ifdef ESP8266
    if (pbuf->len != pbuf->tot_len || pbuf->next)
        Serial.println("ERRTOT\r\n");
#endif

    uint16_t len = ths->sendFrame((const uint8_t*)pbuf->payload, pbuf->len);

#if defined(ESP8266) && PHY_HAS_CAPTURE
    if (phy_capture)
        phy_capture(ths->_netif.num, (const char*)pbuf->payload, pbuf->len, /*out*/1, /*success*/len == pbuf->len);
#endif

    return len == pbuf->len? ERR_OK: ERR_MEM;
}

template <class RawEthernet>
err_t LwipEthernet<RawEthernet>::netif_init_s (struct netif* netif)
{
    return ((LwipEthernet*)netif->state)->netif_init();
}

template <class RawEthernet>
void LwipEthernet<RawEthernet>::netif_status_callback_s (struct netif* netif)
{
    ((LwipEthernet*)netif->state)->netif_status_callback();
}

template <class RawEthernet>
err_t LwipEthernet<RawEthernet>::netif_init ()
{
    _netif.name[0] = 'e';
    _netif.name[1] = '0' + _netif.num;
    _netif.mtu = _mtu;
    _netif.chksum_flags = NETIF_CHECKSUM_ENABLE_ALL;
    _netif.flags =
          NETIF_FLAG_ETHARP
        | NETIF_FLAG_IGMP
        | NETIF_FLAG_BROADCAST
        | NETIF_FLAG_LINK_UP;

    // lwIP's doc: This function typically first resolves the hardware
    // address, then sends the packet.  For ethernet physical layer, this is
    // usually lwIP's etharp_output()
    _netif.output = etharp_output;

    // lwIP's doc: This function outputs the pbuf as-is on the link medium
    // (this must points to the raw ethernet driver, meaning: us)
    _netif.linkoutput = linkoutput_s;

    _netif.status_callback = netif_status_callback_s;

    return ERR_OK;
}

template <class RawEthernet>
void LwipEthernet<RawEthernet>::netif_status_callback ()
{
    //XXX is it wise ?
    if (_default && connected())
        netif_set_default(&_netif);
    else if (netif_default == &_netif && !connected())
        netif_set_default(nullptr);
}

template <class RawEthernet>
err_t LwipEthernet<RawEthernet>::handlePackets ()
{
    int pkt = 0;
    while(1)
    {
        if (++pkt == 10)
            // prevent starvation
            return ERR_OK;

        uint16_t tot_len = RawEthernet::readFrameSize();
        if (!tot_len)
            return ERR_OK;

        // from doc: use PBUF_RAM for TX, PBUF_POOL from RX
        // however:
        // PBUF_POOL can return chained pbuf (not in one piece)
        // and WiznetDriver does not have the proper API to deal with that
        // so in the meantime, we use PBUF_RAM instead which is currently
        // guarantying to deliver a continuous chunk of memory.
        // TODO: tweak the wiznet driver to allow copying partial chunk
        //       of received data and use PBUF_POOL.
        pbuf* pbuf = pbuf_alloc(PBUF_RAW, tot_len, PBUF_RAM);
        if (!pbuf || pbuf->len < tot_len)
        {
            if (pbuf)
                pbuf_free(pbuf);
            RawEthernet::discardFrame(tot_len);
            return ERR_BUF;
        }

        uint16_t len = RawEthernet::readFrameData((uint8_t*)pbuf->payload, tot_len);
        if (len != tot_len)
        {
            // tot_len is given by readFrameSize()
            // and is supposed to be honoured by readFrameData()
            // todo: ensure this test is unneeded, remove the print
            Serial.println("read error?\r\n");
            pbuf_free(pbuf);
            return ERR_BUF;
        }

        err_t err = _netif.input(pbuf, &_netif);

#if defined(ESP8266) && PHY_HAS_CAPTURE
        if (phy_capture)
            phy_capture(_netif.num, (const char*)pbuf->payload, tot_len, /*out*/0, /*success*/err == ERR_OK);
#endif

        if (err != ERR_OK)
        {
            pbuf_free(pbuf);
            return err;
        }
        // (else) allocated pbuf is now on lwIP's responsibility

    }
}

template <class RawEthernet>
void LwipEthernet<RawEthernet>::setDefault ()
{
    _default = true;
    if (connected())
        netif_set_default(&_netif);
}

#endif // _LWIPETH_H
