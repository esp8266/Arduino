
#include <lwip/netif.h>

extern "C"
{

netif* netif_list = nullptr;

err_t dhcp_renew(struct netif *netif)
{
	(void)netif;
	return ERR_OK;
}

} // extern "C"
