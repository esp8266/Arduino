
// STARTS/STOPS DHCP SERVER ON WIFI AP INTERFACE
// these functions must exists as-is with "C" interface,
// nonos-sdk calls them at boot and later

#include <lwip/init.h> // LWIP_VERSION

#if LWIP_VERSION_MAJOR != 1

#include <lwip/netif.h>
#include "lwIPDhcpServer.h"

extern netif netif_git[2];

// global DHCP instance for softAP interface
DhcpServer dhcpSoftAP(&netif_git[SOFTAP_IF]);

extern "C"
{

void dhcps_start (struct ip_info *info, netif* apnetif)
{
    // apnetif is esp interface, replaced by lwip2's
    // netif_git[SOFTAP_IF] interface in constructor
    (void)apnetif;

#if 0
    // can't use C++ now, global ctors are not initialized yet
    dhcpSoftAP.begin(info);
#else
    (void)info;
    fw_has_started_softap_dhcps = 1;
#endif
}

void dhcps_stop ()
{
    dhcpSoftAP.end();
}

} // extern "C"

#endif // LWIP_VERSION_MAJOR != 1
