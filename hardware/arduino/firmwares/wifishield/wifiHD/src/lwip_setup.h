#ifndef _LWIP_SETUP_H
#define _LWIP_SETUP_H

struct net_cfg {
        struct netif *netif; /* lwip network interface */
        uint8_t dhcp_enabled;
        uint8_t dhcp_running;
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
