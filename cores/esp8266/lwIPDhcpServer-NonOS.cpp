
// STARTS/STOPS DHCP SERVER ON WIFI AP INTERFACE
// these functions must exists as-is with "C" interface,
// nonos-sdk calls them at boot and later

#include <lwip/netif.h>

#include "lwIPDhcpServer.h"

extern netif netif_git[2];

DhcpServer dhcpSoftAP(&netif_git[SOFTAP_IF]);

extern "C"
{

void dhcps_start(struct ip_info *info);
void dhcps_stop(void);

void dhcps_start (struct ip_info *info)
{
    dhcpSoftAP.start(info);
}

void dhcps_stop ()
{
    dhcpSoftAP.stop();
}

} // extern "C"
