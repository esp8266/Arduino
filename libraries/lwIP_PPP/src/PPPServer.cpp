
// testing on linux:
// sudo /usr/sbin/pppd /dev/ttyUSB1 4800 noipdefault nocrtscts local defaultroute noauth nodetach debug dump
// sudo /usr/sbin/pppd /dev/ttyUSB1 4800 noipdefault nocrtscts local defaultroute noauth

// proxy arp needed
// http://lwip.100.n7.nabble.com/PPP-proxy-arp-support-tp33286p33345.html

#include <Arduino.h>
#include <Schedule.h>
#include <IPAddress.h>
#include <lwip/dns.h>

#include "../PPPServer.h"

PPP::PPP (Stream* sio): _sio(sio)
{
}

err_t PPP::handlePackets ()
{
    int avail;
    if ((avail = _sio->available()) > 0)
    {
        //XXX USE BSTREAM
        if (avail > 128)
            avail = 128;
        uint8_t buffer[avail];
        avail = _sio->readBytes(buffer, avail);
        pppos_input(_ppp, buffer, avail);
    }
    return ERR_OK;
}

void PPP::link_status_cb_s (ppp_pcb* pcb, int err_code, void* ctx)
{
    //PPP* This = static_cast<PPP*>(ctx);
    netif* nif = ppp_netif(pcb);

    switch(err_code) {
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
}

u32_t PPP::output_cb_s (ppp_pcb* pcb, u8_t* data, u32_t len, void* ctx)
{
    (void)pcb;
    return static_cast<PPP*>(ctx)->_sio->write(data, len);
}

void PPP::netif_status_cb_s (netif* nif)
{
//ip4_addr_get_u32(ip_2_ip4(&nif->ip_addr)) = IPAddress(192,168,0,1).v4();
//ip4_addr_get_u32(ip_2_ip4(&nif->netmask)) = IPAddress(255,255,255,0).v4();
//ip4_addr_get_u32(ip_2_ip4(&nif->gw)) = IPAddress(192,168,0,2).v4();

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

bool PPP::begin ()
{
    // lwip2-src/doc/ppp.txt
    
    _ppp = pppos_create(&_netif, PPP::output_cb_s, PPP::link_status_cb_s, this);
    if (!_ppp)
        return false;

    ip4_addr_t addr;
    
    // PPP server
    IP4_ADDR(&addr, 10,0,1,225);
    ppp_set_ipcp_ouraddr(_ppp, &addr);
    IP4_ADDR(&addr, 10,0,1,129);
    ppp_set_ipcp_hisaddr(_ppp, &addr);
    IP4_ADDR(&addr, 10,0,1,254);
    ppp_set_ipcp_dnsaddr(_ppp, 0, &addr);
    //ppp_set_auth(_ppp, PPPAUTHTYPE_ANY, "login", "password");
    //ppp_set_auth_required(_ppp, 1);
    //ppp_set_silent(_ppp, 1);
    
    ppp_listen(_ppp);

    //netif_set_status_callback(&_netif, PPP::netif_status_cb_s);
    
    if (!schedule_recurrent_function_us([&]() { this->handlePackets(); return true; }, 1000))
    {
        netif_remove(&_netif);
        return false;
    }

    return true;
}
