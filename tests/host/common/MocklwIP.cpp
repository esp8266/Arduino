
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

} // extern "C"
