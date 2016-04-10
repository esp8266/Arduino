/*
 *  Copyright (c) 2010-2011 	Espressif System
 *   
*/

#ifndef _WLAN_LWIP_IF_H_
#define _WLAN_LWIP_IF_H_

#define LWIP_IF0_PRIO   28
#define LWIP_IF1_PRIO   29

enum {
	SIG_LWIP_RX = 0, 
};

struct netif * eagle_lwip_if_alloc(struct ieee80211_conn *conn, const uint8 *macaddr, struct ip_info *info);
struct netif * eagle_lwip_getif(uint8 index);

#ifndef IOT_SIP_MODE
sint8 ieee80211_output_pbuf(struct netif *ifp, struct pbuf* pb);
#else
sint8 ieee80211_output_pbuf(struct ieee80211_conn *conn, esf_buf *eb);
#endif

#endif /*  _WLAN_LWIP_IF_H_ */
