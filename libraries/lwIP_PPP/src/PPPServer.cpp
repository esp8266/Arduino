
// This is still beta / a work in progress

// testing on linux:
// sudo /usr/sbin/pppd /dev/ttyUSB1 38400 noipdefault nocrtscts local defaultroute noauth nodetach debug dump
// sudo /usr/sbin/pppd /dev/ttyUSB1 38400 noipdefault nocrtscts local defaultroute noauth

// proxy arp is needed but we don't have it
// http://lwip.100.n7.nabble.com/PPP-proxy-arp-support-tp33286p33345.html
// using NAT instead (see in example)

#include <Arduino.h>
#include <Schedule.h>
#include <IPAddress.h>
#include <lwip/dns.h>

#include "PPPServer.h"

PPPServer::PPPServer(Stream* sio): _sio(sio), _cb(netif_status_cb_s), _enabled(false)
{
}

bool PPPServer::handlePackets()
{
    size_t avail;
    if ((avail = _sio->available()) > 0)
    {
        // XXX block peeking would be useful here
        if (avail > _bufsize)
        {
            avail = _bufsize;
        }
        avail = _sio->readBytes(_buf, avail);
        pppos_input(_ppp, _buf, avail);
    }
    return _enabled;
}

void PPPServer::link_status_cb_s(ppp_pcb* pcb, int err_code, void* ctx)
{
    bool stop = true;
    netif* nif = ppp_netif(pcb);

    switch (err_code)
    {
    case PPPERR_NONE:               /* No error. */
    {
#if LWIP_DNS
        const ip_addr_t *ns;
#endif /* LWIP_DNS */
        ets_printf("ppp_link_status_cb: PPPERR_NONE\n\r");
#if LWIP_IPV4
        ets_printf("   our_ip4addr = %s\n\r", ip4addr_ntoa(netif_ip4_addr(nif)));
        ets_printf("   his_ipaddr  = %s\n\r", ip4addr_ntoa(netif_ip4_gw(nif)));
        ets_printf("   netmask     = %s\n\r", ip4addr_ntoa(netif_ip4_netmask(nif)));
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
        ets_printf("   our_ip6addr = %s\n\r", ip6addr_ntoa(netif_ip6_addr(nif, 0)));
#endif /* LWIP_IPV6 */

#if LWIP_DNS
        ns = dns_getserver(0);
        ets_printf("   dns1        = %s\n\r", ipaddr_ntoa(ns));
        ns = dns_getserver(1);
        ets_printf("   dns2        = %s\n\r", ipaddr_ntoa(ns));
#endif /* LWIP_DNS */
#if PPP_IPV6_SUPPORT
        ets_printf("   our6_ipaddr = %s\n\r", ip6addr_ntoa(netif_ip6_addr(nif, 0)));
#endif /* PPP_IPV6_SUPPORT */
    }
    stop = false;
    break;

    case PPPERR_PARAM:             /* Invalid parameter. */
        ets_printf("ppp_link_status_cb: PPPERR_PARAM\n");
        break;

    case PPPERR_OPEN:              /* Unable to open PPP session. */
        ets_printf("ppp_link_status_cb: PPPERR_OPEN\n");
        break;

    case PPPERR_DEVICE:            /* Invalid I/O device for PPP. */
        ets_printf("ppp_link_status_cb: PPPERR_DEVICE\n");
        break;

    case PPPERR_ALLOC:             /* Unable to allocate resources. */
        ets_printf("ppp_link_status_cb: PPPERR_ALLOC\n");
        break;

    case PPPERR_USER:              /* User interrupt. */
        ets_printf("ppp_link_status_cb: PPPERR_USER\n");
        break;

    case PPPERR_CONNECT:           /* Connection lost. */
        ets_printf("ppp_link_status_cb: PPPERR_CONNECT\n");
        break;

    case PPPERR_AUTHFAIL:          /* Failed authentication challenge. */
        ets_printf("ppp_link_status_cb: PPPERR_AUTHFAIL\n");
        break;

    case PPPERR_PROTOCOL:          /* Failed to meet protocol. */
        ets_printf("ppp_link_status_cb: PPPERR_PROTOCOL\n");
        break;

    case PPPERR_PEERDEAD:          /* Connection timeout. */
        ets_printf("ppp_link_status_cb: PPPERR_PEERDEAD\n");
        break;

    case PPPERR_IDLETIMEOUT:       /* Idle Timeout. */
        ets_printf("ppp_link_status_cb: PPPERR_IDLETIMEOUT\n");
        break;

    case PPPERR_CONNECTTIME:       /* PPPERR_CONNECTTIME. */
        ets_printf("ppp_link_status_cb: PPPERR_CONNECTTIME\n");
        break;

    case PPPERR_LOOPBACK:          /* Connection timeout. */
        ets_printf("ppp_link_status_cb: PPPERR_LOOPBACK\n");
        break;

    default:
        ets_printf("ppp_link_status_cb: unknown errCode %d\n", err_code);
        break;
    }

    if (stop)
    {
        netif_remove(&static_cast<PPPServer*>(ctx)->_netif);
    }
}

u32_t PPPServer::output_cb_s(ppp_pcb* pcb, u8_t* data, u32_t len, void* ctx)
{
    (void)pcb;
    (void)ctx;
    return static_cast<PPPServer*>(ctx)->_sio->write(data, len);
}

void PPPServer::netif_status_cb_s(netif* nif)
{
    ets_printf("PPPNETIF: %c%c%d is %s\n", nif->name[0], nif->name[1], nif->num,
               netif_is_up(nif) ? "UP" : "DOWN");
#if LWIP_IPV4
    ets_printf("IPV4: Host at %s ", ip4addr_ntoa(netif_ip4_addr(nif)));
    ets_printf("mask %s ", ip4addr_ntoa(netif_ip4_netmask(nif)));
    ets_printf("gateway %s\n", ip4addr_ntoa(netif_ip4_gw(nif)));
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
    ets_printf("IPV6: Host at %s\n", ip6addr_ntoa(netif_ip6_addr(nif, 0)));
#endif /* LWIP_IPV6 */
    ets_printf("FQDN: %s\n", netif_get_hostname(nif));
}

bool PPPServer::begin(const IPAddress& ourAddress, const IPAddress& peer)
{
    // lwip2-src/doc/ppp.txt

    _ppp = pppos_create(&_netif, PPPServer::output_cb_s, PPPServer::link_status_cb_s, this);
    if (!_ppp)
    {
        return false;
    }

    ppp_set_ipcp_ouraddr(_ppp, ip_2_ip4((const ip_addr_t*)ourAddress));
    ppp_set_ipcp_hisaddr(_ppp, ip_2_ip4((const ip_addr_t*)peer));

    //ip4_addr_t addr;
    //IP4_ADDR(&addr, 10,0,1,254);
    //ppp_set_ipcp_dnsaddr(_ppp, 0, &addr);

    //ppp_set_auth(_ppp, PPPAUTHTYPE_ANY, "login", "password");
    //ppp_set_auth_required(_ppp, 1);

    ppp_set_silent(_ppp, 1);
    ppp_listen(_ppp);
    netif_set_status_callback(&_netif, _cb);

    _enabled = true;
    if (!schedule_recurrent_function_us([&]()
{
    return this->handlePackets();
    }, 1000))
    {
        netif_remove(&_netif);
        return false;
    }

    return true;
}

void PPPServer::stop()
{
    _enabled = false;
    ppp_close(_ppp, 0);
}
