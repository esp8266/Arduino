/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

#ifndef __NETIF_NRWLANIF_H__
#define __NETIF_NRWLANIF_H__

#include "lwip/netif.h"
#include "lwip/err.h"

err_t wlif_init(struct netif *netif);
void wlif_poll(struct netif *netif);

#endif
