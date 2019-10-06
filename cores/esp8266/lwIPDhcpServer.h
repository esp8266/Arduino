
#ifndef __DHCPS_H__
#define __DHCPS_H__

#include <lwip/init.h> // LWIP_VERSION

#if LWIP_VERSION_MAJOR != 1

class DhcpServer
{
public:

    DhcpServer (netif* netif);

    void setDns (int num, const ipv4_addr_t* dns);

    void start (struct ip_info *info);
    void stop  ();

    void wifi_softap_init_dhcps_lease(uint32 ip);
    bool wifi_softap_set_dhcps_lease(struct dhcps_lease *please);
    bool wifi_softap_get_dhcps_lease(struct dhcps_lease *please);
    bool wifi_softap_set_dhcps_offer_option(uint8 level, void* optarg);
    bool wifi_softap_set_dhcps_lease_time(uint32 minute);
    bool wifi_softap_reset_dhcps_lease_time(void);
    uint32 wifi_softap_get_dhcps_lease_time(void);

protected:

    typedef struct _list_node {
        void *pnode;
        struct _list_node *pnext;
    } list_node;

    void dhcps_set_dns(int num, const ipv4_addr_t* dns);
    void node_insert_to_list(list_node **phead, list_node* pinsert);
    void node_remove_from_list(list_node **phead, list_node* pdelete);
    bool wifi_softap_add_dhcps_lease(uint8 *macaddr);
    uint8_t* add_msg_type(uint8_t *optptr, uint8_t type);
    uint8_t* add_offer_options(uint8_t *optptr);
    uint8_t* add_end(uint8_t *optptr);
    void create_msg(struct dhcps_msg *m);
    void send_offer(struct dhcps_msg *m);
    void send_nak(struct dhcps_msg *m);
    void send_ack(struct dhcps_msg *m);
    uint8_t parse_options(uint8_t *optptr, sint16_t len);
    sint16_t parse_msg(struct dhcps_msg *m, u16_t len);
    static void S_handle_dhcp(void *arg,
        struct udp_pcb *pcb,
        struct pbuf *p,
        const ip_addr_t *addr,
        uint16_t port);
    void handle_dhcp(
        struct udp_pcb *pcb,
        struct pbuf *p,
        const ip_addr_t *addr,
        uint16_t port);
    void kill_oldest_dhcps_pool(void);
    void dhcps_coarse_tmr(void);
    void wifi_softap_dhcps_client_leave(u8 *bssid, struct ipv4_addr *ip, bool force);
    uint32 wifi_softap_dhcps_client_update(u8 *bssid, struct ipv4_addr *ip);

    netif* _netif;

    struct udp_pcb *pcb_dhcps;
    ip_addr_t broadcast_dhcps;
    struct ipv4_addr server_address;
    struct ipv4_addr client_address;
    struct ipv4_addr dns_address;
    uint32 dhcps_lease_time;

    struct dhcps_lease dhcps_lease;
    list_node *plist;
    uint8 offer;
    bool renew;

    static const uint32 magic_cookie;
};

extern DhcpServer dhcpSoftAP;

#endif // LWIP_VERSION_MAJOR != 1

#endif // __DHCPS_H__
