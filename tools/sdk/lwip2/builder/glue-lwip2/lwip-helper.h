
#ifndef LWIP_HELPER_H
#define LWIP_HELPER_H

#include "lwip/err.h"
#include "lwip/netif.h"

#define netif_sta (&netif_git[STATION_IF])
#define netif_ap  (&netif_git[SOFTAP_IF])
extern struct netif netif_git[2];
extern int netif_git_initialized[2];
extern const char netif_name[2][8];

// return 1 if OK, 0 if bad and display message
int lwiperr_check (const char* what, err_t err);

#endif // LWIP_HELPER_H