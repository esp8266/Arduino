
#include <AddrList.h>
#include <lwip/netif.h>

esp8266::AddressListImplementation::AddressList addrList;

extern "C"
{

extern netif netif0;

netif* netif_list = &netif0;

err_t dhcp_renew(struct netif *netif)
{
	(void)netif;
	return ERR_OK;
}

void sntp_setserver(u8_t, const ip_addr_t)
{
}

const ip_addr_t* sntp_getserver(u8_t)
{
    return IP_ADDR_ANY;
}

} // extern "C"
