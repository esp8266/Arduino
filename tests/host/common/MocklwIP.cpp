
#include <AddrList.h>

#include "MocklwIP.h"

esp8266::AddressListImplementation::AddressList addrList;

extern "C"
{
    extern netif netif0;

    netif* netif_list = &netif0;

    err_t dhcp_renew(struct netif* netif)
    {
        (void)netif;
        return ERR_OK;
    }

    void sntp_setserver(u8_t, const ip_addr_t) { }

    const ip_addr_t* sntp_getserver(u8_t) { return IP_ADDR_ANY; }

    err_t etharp_request(struct netif* netif, const ip4_addr_t* ipaddr)
    {
        (void)netif;
        (void)ipaddr;
        return ERR_OK;
    }

    err_t igmp_start(struct netif* netif)
    {
        (void)netif;
        return ERR_OK;
    }

    err_t igmp_joingroup_netif(struct netif* netif, const ip4_addr_t* groupaddr)
    {
        (void)netif;
        (void)groupaddr;
        return ERR_OK;
    }

    err_t igmp_leavegroup_netif(struct netif* netif, const ip4_addr_t* groupaddr)
    {
        (void)netif;
        (void)groupaddr;
        return ERR_OK;
    }

    struct netif* netif_get_by_index(u8_t idx)
    {
        (void)idx;
        return &netif0;
    }

}  // extern "C"
