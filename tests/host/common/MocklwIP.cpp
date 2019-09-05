
#include <lwip/netif.h>

extern "C"
{

netif* netif_list = nullptr;

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
