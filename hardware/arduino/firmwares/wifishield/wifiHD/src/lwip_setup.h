#ifndef _LWIP_SETUP_H
#define _LWIP_SETUP_H

#define INIT_IP_CONFIG		0xff
#define STATIC_IP_CONFIG	0
#define DYNAMIC_IP_CONFIG	1

struct net_cfg {
        struct netif *netif; /* lwip network interface */
        uint8_t dhcp_enabled;
        uint8_t dhcp_running;
};

struct ctx_server {
	struct net_cfg net_cfg;
	uint8_t wl_init_complete;
};

/*! Start the IP stack.
 * If cfg->netif must have been allocated and lwip_init()
 * must have been called before this function is called
 * (since the IP stack may have to be polled before this
 * function can be called).
 */
int start_ip_stack(struct net_cfg *cfg,
                   struct ip_addr ipaddr, 
                   struct ip_addr netmask,
                   struct ip_addr gw);

#endif /* _LWIP_SETUP_H */
