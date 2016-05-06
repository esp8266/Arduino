/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: espconn_udp.c
 *
 * Description: udp proto interface
 *
 * Modification history:
 *     2014/3/31, v1.0 create this file.
*******************************************************************************/

#include "ets_sys.h"
#include "os_type.h"
//#include "os.h"

#include "lwip/inet.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/mem.h"
#include "lwip/tcp_impl.h"
#include "lwip/udp.h"

#include "lwip/app/espconn_udp.h"

#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR = __FILE__;
#endif

extern espconn_msg *plink_active;
extern uint8 default_interface;

enum send_opt{
	ESPCONN_SENDTO,
	ESPCONN_SEND
};
static void ICACHE_FLASH_ATTR espconn_data_sentcb(struct espconn *pespconn)
{
    if (pespconn == NULL) {
        return;
    }

    if (pespconn->sent_callback != NULL) {
        pespconn->sent_callback(pespconn);
    }
}

static void ICACHE_FLASH_ATTR espconn_data_sent(void *arg, enum send_opt opt)
{
    espconn_msg *psent = arg;

    if (psent == NULL) {
        return;
    }

    if (psent->pcommon.cntr == 0) {
        psent->pespconn->state = ESPCONN_CONNECT;
        if (psent->pcommon.err == 0)
        	espconn_data_sentcb(psent->pespconn);
    } else {
    	if (opt == ESPCONN_SEND){
    		espconn_udp_sent(arg, psent->pcommon.ptrbuf, psent->pcommon.cntr);
    	} else {
    		espconn_udp_sendto(arg, psent->pcommon.ptrbuf, psent->pcommon.cntr);
    	}
    }
}

/******************************************************************************
 * FunctionName : espconn_udp_sent
 * Description  : sent data for client or server
 * Parameters   : void *arg -- client or server to send
 * 				  uint8* psent -- Data to send
 *                uint16 length -- Length of data to send
 * Returns      : return espconn error code.
 * - ESPCONN_OK. Successful. No error occured.
 * - ESPCONN_MEM. Out of memory.
 * - ESPCONN_RTE. Could not find route to destination address.
 * - More errors could be returned by lower protocol layers.
*******************************************************************************/
err_t ICACHE_FLASH_ATTR
espconn_udp_sent(void *arg, uint8 *psent, uint16 length)
{
    espconn_msg *pudp_sent = arg;
    struct udp_pcb *upcb = pudp_sent->pcommon.pcb;
    struct pbuf *p, *q ,*p_temp;
    u8_t *data = NULL;
    u16_t cnt = 0;
    u16_t datalen = 0;
    u16_t i = 0;
    err_t err;
    LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sent %d %d %p\n", __LINE__, length, upcb));

    if (pudp_sent == NULL || upcb == NULL || psent == NULL || length == 0) {
        return ESPCONN_ARG;
    }

    if (1470 < length) {
        datalen = 1470;
    } else {
        datalen = length;
    }

    p = pbuf_alloc(PBUF_TRANSPORT, datalen, PBUF_RAM);
    LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sent %d %p\n", __LINE__, p));

    if (p != NULL) {
        q = p;

        while (q != NULL) {
            data = (u8_t *)q->payload;
            LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sent %d %p\n", __LINE__, data));

            for (i = 0; i < q->len; i++) {
                data[i] = ((u8_t *) psent)[cnt++];
            }

            q = q->next;
        }
    } else {
        return ESPCONN_MEM;
    }

    upcb->remote_port = pudp_sent->pespconn->proto.udp->remote_port;
    IP4_ADDR(&upcb->remote_ip, pudp_sent->pespconn->proto.udp->remote_ip[0],
    		pudp_sent->pespconn->proto.udp->remote_ip[1],
    		pudp_sent->pespconn->proto.udp->remote_ip[2],
    		pudp_sent->pespconn->proto.udp->remote_ip[3]);

    LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sent %d %x %d\n", __LINE__, upcb->remote_ip, upcb->remote_port));

    struct netif *sta_netif = (struct netif *)eagle_lwip_getif(0x00);
    struct netif *ap_netif =  (struct netif *)eagle_lwip_getif(0x01);
		
    if(wifi_get_opmode() == ESPCONN_AP_STA && default_interface == ESPCONN_AP_STA && sta_netif != NULL && ap_netif != NULL)
    {
    	if(netif_is_up(sta_netif) && netif_is_up(ap_netif) && \
			ip_addr_isbroadcast(&upcb->remote_ip, sta_netif) && \
			ip_addr_isbroadcast(&upcb->remote_ip, ap_netif)) {

    	  p_temp = pbuf_alloc(PBUF_TRANSPORT, datalen, PBUF_RAM);
    	  if (pbuf_copy (p_temp,p) != ERR_OK) {
    		  LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sent: copying to new pbuf failed\n"));
    		  return ESPCONN_ARG;
    	  }
		  netif_set_default(sta_netif);
		  err = udp_send(upcb, p_temp);
		  pbuf_free(p_temp);
		  netif_set_default(ap_netif);
    	}
    }
	      err = udp_send(upcb, p);

    LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sent %d %d\n", __LINE__, err));

    if (p->ref != 0) {
        LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sent %d %p\n", __LINE__, p));
        pbuf_free(p);
        pudp_sent->pcommon.ptrbuf = psent + datalen;
        pudp_sent->pcommon.cntr = length - datalen;
        pudp_sent->pcommon.err = err;
        espconn_data_sent(pudp_sent, ESPCONN_SEND);
        if (err > 0)
        	return ESPCONN_IF;
        return err;
    } else {
    	pbuf_free(p);
    	return ESPCONN_RTE;
    }
}

/******************************************************************************
 * FunctionName : espconn_udp_sendto
 * Description  : sent data for UDP
 * Parameters   : void *arg -- UDP to send
 * 				  uint8* psent -- Data to send
 *                uint16 length -- Length of data to send
 * Returns      : return espconn error code.
 * - ESPCONN_OK. Successful. No error occured.
 * - ESPCONN_MEM. Out of memory.
 * - ESPCONN_RTE. Could not find route to destination address.
 * - More errors could be returned by lower protocol layers.
*******************************************************************************/
err_t ICACHE_FLASH_ATTR
espconn_udp_sendto(void *arg, uint8 *psent, uint16 length)
{
    espconn_msg *pudp_sent = arg;
    struct udp_pcb *upcb = pudp_sent->pcommon.pcb;
    struct espconn *pespconn = pudp_sent->pespconn;
    struct pbuf *p, *q ,*p_temp;
    struct ip_addr dst_ip;
    u16_t dst_port;
    u8_t *data = NULL;
    u16_t cnt = 0;
    u16_t datalen = 0;
    u16_t i = 0;
    err_t err;
    LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sent %d %d %p\n", __LINE__, length, upcb));

    if (pudp_sent == NULL || upcb == NULL || psent == NULL || length == 0) {
        return ESPCONN_ARG;
    }

    if (1470 < length) {
        datalen = 1470;
    } else {
        datalen = length;
    }

    p = pbuf_alloc(PBUF_TRANSPORT, datalen, PBUF_RAM);
    LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sent %d %p\n", __LINE__, p));

    if (p != NULL) {
        q = p;

        while (q != NULL) {
            data = (u8_t *)q->payload;
            LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sent %d %p\n", __LINE__, data));

            for (i = 0; i < q->len; i++) {
                data[i] = ((u8_t *) psent)[cnt++];
            }

            q = q->next;
        }
    } else {
        return ESPCONN_MEM;
    }

    dst_port = pespconn->proto.udp->remote_port;
    IP4_ADDR(&dst_ip, pespconn->proto.udp->remote_ip[0],
			pespconn->proto.udp->remote_ip[1], pespconn->proto.udp->remote_ip[2],
			pespconn->proto.udp->remote_ip[3]);
    LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sent %d %x %d\n", __LINE__, upcb->remote_ip, upcb->remote_port));

    struct netif *sta_netif = (struct netif *)eagle_lwip_getif(0x00);
	struct netif *ap_netif =  (struct netif *)eagle_lwip_getif(0x01);

    if(wifi_get_opmode() == ESPCONN_AP_STA && default_interface == ESPCONN_AP_STA && sta_netif != NULL && ap_netif != NULL)
	{
		if(netif_is_up(sta_netif) && netif_is_up(ap_netif) && \
			ip_addr_isbroadcast(&upcb->remote_ip, sta_netif) && \
			ip_addr_isbroadcast(&upcb->remote_ip, ap_netif)) {

		  p_temp = pbuf_alloc(PBUF_TRANSPORT, datalen, PBUF_RAM);
		  if (pbuf_copy (p_temp,p) != ERR_OK) {
			  LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_sendto: copying to new pbuf failed\n"));
			  return ESPCONN_ARG;
		  }
		  netif_set_default(sta_netif);
		  err = udp_sendto(upcb, p_temp, &dst_ip, dst_port);
		  pbuf_free(p_temp);
		  netif_set_default(ap_netif);
		}
	}
    err = udp_sendto(upcb, p, &dst_ip, dst_port);

    if (p->ref != 0) {
    	pbuf_free(p);
    	pudp_sent->pcommon.ptrbuf = psent + datalen;
		pudp_sent->pcommon.cntr = length - datalen;
		pudp_sent->pcommon.err = err;
		espconn_data_sent(pudp_sent, ESPCONN_SENDTO);

		if (err > 0)
			return ESPCONN_IF;
		return err;
    } else {
    	pbuf_free(p);
    	return ESPCONN_RTE;
    }
}

/******************************************************************************
 * FunctionName : espconn_udp_server_recv
 * Description  : This callback will be called when receiving a datagram.
 * Parameters   : arg -- user supplied argument
 *                upcb -- the udp_pcb which received data
 *                p -- the packet buffer that was received
 *                addr -- the remote IP address from which the packet was received
 *                port -- the remote port from which the packet was received
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
espconn_udp_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                 struct ip_addr *addr, u16_t port)
{
    espconn_msg *precv = arg;
    struct pbuf *q = NULL;
    u8_t *pdata = NULL;
    u16_t length = 0;
    struct ip_info ipconfig;

    LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("espconn_udp_server_recv %d %p\n", __LINE__, upcb));

    precv->pcommon.remote_ip[0] = ip4_addr1_16(addr);
    precv->pcommon.remote_ip[1] = ip4_addr2_16(addr);
    precv->pcommon.remote_ip[2] = ip4_addr3_16(addr);
    precv->pcommon.remote_ip[3] = ip4_addr4_16(addr);
    precv->pcommon.remote_port = port;
    precv->pcommon.pcb = upcb;

	if (wifi_get_opmode() != 1) {
		wifi_get_ip_info(1, &ipconfig);

		if (!ip_addr_netcmp(addr, &ipconfig.ip, &ipconfig.netmask)) {
			wifi_get_ip_info(0, &ipconfig);
		}
	} else {
		wifi_get_ip_info(0, &ipconfig);
	}

	precv->pespconn->proto.udp->local_ip[0] = ip4_addr1_16(&ipconfig.ip);
	precv->pespconn->proto.udp->local_ip[1] = ip4_addr2_16(&ipconfig.ip);
	precv->pespconn->proto.udp->local_ip[2] = ip4_addr3_16(&ipconfig.ip);
	precv->pespconn->proto.udp->local_ip[3] = ip4_addr4_16(&ipconfig.ip);

    if (p != NULL) {
    	pdata = (u8_t *)os_zalloc(p ->tot_len + 1);
    	length = pbuf_copy_partial(p, pdata, p ->tot_len, 0);
    	precv->pcommon.pcb = upcb;
        pbuf_free(p);
		if (length != 0) {
			if (precv->pespconn->recv_callback != NULL) {
				precv->pespconn->recv_callback(precv->pespconn, pdata, length);
			}
		}
		os_free(pdata);
    } else {
        return;
    }
}

/******************************************************************************
 * FunctionName : espconn_udp_disconnect
 * Description  : A new incoming connection has been disconnected.
 * Parameters   : espconn -- the espconn used to disconnect with host
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR espconn_udp_disconnect(espconn_msg *pdiscon)
{
    if (pdiscon == NULL) {
        return;
    }

    struct udp_pcb *upcb = pdiscon->pcommon.pcb;

    udp_disconnect(upcb);

    udp_remove(upcb);

    espconn_list_delete(&plink_active, pdiscon);

    os_free(pdiscon);
    pdiscon = NULL;
}

/******************************************************************************
 * FunctionName : espconn_udp_server
 * Description  : Initialize the server: set up a PCB and bind it to the port
 * Parameters   : pespconn -- the espconn used to build server
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_udp_server(struct espconn *pespconn)
{
    struct udp_pcb *upcb = NULL;
    espconn_msg *pserver = NULL;
    upcb = udp_new();

    if (upcb == NULL) {
        return ESPCONN_MEM;
    } else {
        pserver = (espconn_msg *)os_zalloc(sizeof(espconn_msg));

        if (pserver == NULL) {
            udp_remove(upcb);
            return ESPCONN_MEM;
        }

        pserver->pcommon.pcb = upcb;
        pserver->pespconn = pespconn;
        espconn_list_creat(&plink_active, pserver);
        udp_bind(upcb, IP_ADDR_ANY, pserver->pespconn->proto.udp->local_port);
        udp_recv(upcb, espconn_udp_recv, (void *)pserver);
        return ESPCONN_OK;
    }
}

/******************************************************************************
 * FunctionName : espconn_igmp_leave
 * Description  : leave a multicast group
 * Parameters   : host_ip -- the ip address of udp server
 * 				  multicast_ip -- multicast ip given by user
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_igmp_leave(ip_addr_t *host_ip, ip_addr_t *multicast_ip)
{
    if (igmp_leavegroup(host_ip, multicast_ip) != ERR_OK) {
        LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("udp_leave_multigrup failed!\n"));
        return -1;
    };

    return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_igmp_join
 * Description  : join a multicast group
 * Parameters   : host_ip -- the ip address of udp server
 * 				  multicast_ip -- multicast ip given by user
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_igmp_join(ip_addr_t *host_ip, ip_addr_t *multicast_ip)
{
    if (igmp_joingroup(host_ip, multicast_ip) != ERR_OK) {
        LWIP_DEBUGF(ESPCONN_UDP_DEBUG, ("udp_join_multigrup failed!\n"));
        return -1;
    };

    /* join to any IP address at the port  */
    return ESPCONN_OK;
}
