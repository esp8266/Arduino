/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: espconn.c
 *
 * Description: espconn interface for user
 *
 * Modification history:
 *     2014/3/31, v1.0 create this file.
*******************************************************************************/

#include "lwip/netif.h"
#include "lwip/inet.h"
#include "netif/etharp.h"
#include "lwip/tcp.h"
#include "lwip/ip.h"
#include "lwip/init.h"
#include "ets_sys.h"
#include "os_type.h"
//#include "os.h"
#include "lwip/mem.h"

#include "lwip/app/espconn_tcp.h"
#include "lwip/app/espconn_udp.h"
#include "lwip/app/espconn.h"
#include "user_interface.h"

#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR = __FILE__;
#endif

espconn_msg *plink_active = NULL;
espconn_msg *pserver_list = NULL;
remot_info premot[linkMax];

struct espconn_packet pktinfo[2];

static uint8 espconn_tcp_get_buf_count(espconn_buf *pesp_buf);
/******************************************************************************
 * FunctionName : espconn_copy_partial
 * Description  : reconnect with host
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
espconn_copy_partial(struct espconn *pesp_dest, struct espconn *pesp_source)
{
	pesp_dest->type = pesp_source->type;
	pesp_dest->state = pesp_source->state;
	if (pesp_source->type == ESPCONN_TCP){
		pesp_dest->proto.tcp->remote_port = pesp_source->proto.tcp->remote_port;
		pesp_dest->proto.tcp->local_port = pesp_source->proto.tcp->local_port;
		os_memcpy(pesp_dest->proto.tcp->remote_ip, pesp_source->proto.tcp->remote_ip, 4);
		os_memcpy(pesp_dest->proto.tcp->local_ip, pesp_source->proto.tcp->local_ip, 4);
		pesp_dest->proto.tcp->connect_callback = pesp_source->proto.tcp->connect_callback;
		pesp_dest->proto.tcp->reconnect_callback = pesp_source->proto.tcp->reconnect_callback;
		pesp_dest->proto.tcp->disconnect_callback = pesp_source->proto.tcp->disconnect_callback;
	} else {
		pesp_dest->proto.udp->remote_port = pesp_source->proto.udp->remote_port;
		pesp_dest->proto.udp->local_port = pesp_source->proto.udp->local_port;
		os_memcpy(pesp_dest->proto.udp->remote_ip, pesp_source->proto.udp->remote_ip, 4);
		os_memcpy(pesp_dest->proto.udp->local_ip, pesp_source->proto.udp->local_ip, 4);
	}
	pesp_dest->recv_callback = pesp_source->recv_callback;
	pesp_dest->sent_callback = pesp_source->sent_callback;
	pesp_dest->link_cnt = pesp_source->link_cnt;
	pesp_dest->reverse = pesp_source->reverse;
}

/******************************************************************************
 * FunctionName : espconn_copy_partial
 * Description  : insert the node to the active connection list
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR espconn_list_creat(espconn_msg **phead, espconn_msg* pinsert)
{
	espconn_msg *plist = NULL;
//	espconn_msg *ptest = NULL;
	if (*phead == NULL)
		*phead = pinsert;
	else {
		plist = *phead;
		while (plist->pnext != NULL) {
			plist = plist->pnext;
		}
		plist->pnext = pinsert;
	}
	pinsert->pnext = NULL;

/*	ptest = *phead;
	while(ptest != NULL){
		os_printf("espconn_list_creat %p\n", ptest);
		ptest = ptest->pnext;
	}*/
}

/******************************************************************************
 * FunctionName : espconn_list_delete
 * Description  : remove the node from the active connection list
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR espconn_list_delete(espconn_msg **phead, espconn_msg* pdelete)
{
	espconn_msg *plist = NULL;
//	espconn_msg *ptest = NULL;
	plist = *phead;
	if (plist == NULL){
		*phead = NULL;
	} else {
		if (plist == pdelete){
			*phead = plist->pnext;
		} else {
			while (plist != NULL) {
				if (plist->pnext == pdelete){
					plist->pnext = pdelete->pnext;
				}
				plist = plist->pnext;
			}
		}
	}
/*	ptest = *phead;
	while(ptest != NULL){
		os_printf("espconn_list_delete %p\n", ptest);
		ptest = ptest->pnext;
	}*/
}

/******************************************************************************
 * FunctionName : espconn_pbuf_create
 * Description  : insert the node to the active connection list
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR espconn_pbuf_create(espconn_buf **phead, espconn_buf* pinsert)
{
	espconn_buf *plist = NULL;

	if (*phead == NULL)
		*phead = pinsert;
	else {
		plist = *phead;
		while (plist->pnext != NULL) {
			plist = plist->pnext;
		}
		plist->pnext = pinsert;
	}
	pinsert->pnext = NULL;
}

/******************************************************************************
 * FunctionName : espconn_pbuf_delete
 * Description  : remove the node from the active connection list
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR espconn_pbuf_delete(espconn_buf **phead, espconn_buf* pdelete)
{
	espconn_buf *plist = NULL;

	plist = *phead;
	if (plist == NULL){
		*phead = NULL;
	} else {
		if (plist == pdelete){
			*phead = plist->pnext;
		} else {
			while (plist != NULL) {
				if (plist->pnext == pdelete){
					plist->pnext = pdelete->pnext;
				}
				plist = plist->pnext;
			}
		}
	}
}

/******************************************************************************
 * FunctionName : espconn_find_connection
 * Description  : Initialize the server: set up a listening PCB and bind it to
 *                the defined port
 * Parameters   : espconn -- the espconn used to build server
 * Returns      : true or false
 *******************************************************************************/
bool ICACHE_FLASH_ATTR espconn_find_connection(struct espconn *pespconn, espconn_msg **pnode)
{
	espconn_msg *plist = NULL;
	struct ip_addr ip_remot;
	struct ip_addr ip_list;

    if (pespconn == NULL)
		return false;

    /*find the active connection node*/
    for (plist = plink_active; plist != NULL; plist = plist->pnext){
		if (pespconn == plist->pespconn) {
			*pnode = plist;
			return true;
		}
	}

    /*find the active server node*/
    for (plist = pserver_list; plist != NULL; plist = plist->pnext){
    	if (pespconn == plist->pespconn) {
			if (pespconn->proto.tcp == NULL)
				return false;

			IP4_ADDR(&ip_remot, pespconn->proto.tcp->remote_ip[0],
					pespconn->proto.tcp->remote_ip[1],
					pespconn->proto.tcp->remote_ip[2],
					pespconn->proto.tcp->remote_ip[3]);
			if ((ip_remot.addr == IPADDR_ANY) || (pespconn->proto.tcp->remote_port == 0))
				return false;

			/*find the active connection node*/
			for (plist = plink_active; plist != NULL; plist = plist->pnext){
				IP4_ADDR(&ip_list, plist->pcommon.remote_ip[0],
						plist->pcommon.remote_ip[1], plist->pcommon.remote_ip[2],
						plist->pcommon.remote_ip[3]);
				if ((ip_list.addr == ip_remot.addr)	&& (pespconn->proto.tcp->remote_port == plist->pcommon.remote_port)) {
					*pnode = plist;
					return true;
				}
			}
			return false;
		}
	}
    return false;
}

/******************************************************************************
 * FunctionName : espconn_get_acticve_num
 * Description  : get the count of simulatenously active connections
 * Parameters   : type -- the type
 * Returns      : the count of simulatenously active connections
 *******************************************************************************/
static uint8 ICACHE_FLASH_ATTR
espconn_get_acticve_num(uint8 type)
{
	espconn_msg *plist = NULL;
	uint8 num_tcp_active = 0;

	for (plist = plink_active; plist != NULL; plist = plist->pnext) {
		if (plist->pespconn != NULL && plist->pespconn->type == type) {
			num_tcp_active++;
		}
	}

	return num_tcp_active;
}

/******************************************************************************
 * FunctionName : espconn_connect
 * Description  : The function given as the connect
 * Parameters   : espconn -- the espconn used to listen the connection
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_connect(struct espconn *espconn)
{
	struct ip_addr ipaddr;
	struct ip_info ipinfo;
	uint8 connect_status = 0;
	sint8 value = ESPCONN_OK;
	espconn_msg *plist = NULL;
	remot_info *pinfo = NULL;

    if (espconn == NULL) {
        return ESPCONN_ARG;
    } else if (espconn ->type != ESPCONN_TCP)
    	return ESPCONN_ARG;

    /*Check the active node count whether is the limit or not*/
    if (espconn_get_acticve_num(ESPCONN_TCP) >= espconn_tcp_get_max_con())
    	return ESPCONN_ISCONN;

    /*Check the IP address whether is zero or not in different mode*/
    if (wifi_get_opmode() == ESPCONN_STA){
    	wifi_get_ip_info(STA_NETIF,&ipinfo);
    	if (ipinfo.ip.addr == 0){
   	 		return ESPCONN_RTE;
   	 	}
    } else if(wifi_get_opmode() == ESPCONN_AP){
    	wifi_get_ip_info(AP_NETIF,&ipinfo);
    	if (ipinfo.ip.addr == 0){
    		return ESPCONN_RTE;
    	}
    } else if(wifi_get_opmode() == ESPCONN_AP_STA){
    	IP4_ADDR(&ipaddr, espconn->proto.tcp->remote_ip[0],
    	    	    		espconn->proto.tcp->remote_ip[1],
    	    	    		espconn->proto.tcp->remote_ip[2],
    	    	    		espconn->proto.tcp->remote_ip[3]);
    	ipaddr.addr <<= 8;
    	wifi_get_ip_info(AP_NETIF,&ipinfo);
    	ipinfo.ip.addr <<= 8;
    	espconn_printf("softap_addr = %x, remote_addr = %x\n", ipinfo.ip.addr, ipaddr.addr);

    	if (ipaddr.addr != ipinfo.ip.addr){
    		connect_status = wifi_station_get_connect_status();
			if (connect_status == STATION_GOT_IP){
				wifi_get_ip_info(STA_NETIF,&ipinfo);
				if (ipinfo.ip.addr == 0)
					return ESPCONN_RTE;
			} else if (connect_status == STATION_IDLE){
				return ESPCONN_RTE;
			} else {
				return connect_status;
			}
    	}
    }

    /*check the active node information whether is the same as the entity or not*/
    for (plist = plink_active; plist != NULL; plist = plist->pnext){
    	if (plist->pespconn && plist->pespconn->type == ESPCONN_TCP){
    		if (espconn->proto.tcp->local_port == plist->pespconn->proto.tcp->local_port){
    			return ESPCONN_ISCONN;
    		}
    	}
    }

    value = espconn_tcp_client(espconn);

    return value;
}

/******************************************************************************
 * FunctionName : espconn_create
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to the data transmission
 * Returns      : result
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_create(struct espconn *espconn)
{
	sint8 value = ESPCONN_OK;
	espconn_msg *plist = NULL;

	if (espconn == NULL) {
		return ESPCONN_ARG;
	} else if (espconn ->type != ESPCONN_UDP){
		return ESPCONN_ARG;
	}

	/*check the active node information whether is the same as the entity or not*/
	for (plist = plink_active; plist != NULL; plist = plist->pnext){
		if (plist->pespconn && plist->pespconn->type == ESPCONN_UDP){
			if (espconn->proto.udp->local_port == plist->pespconn->proto.udp->local_port){
				return ESPCONN_ISCONN;
			}
		}
	}

	value = espconn_udp_server(espconn);

	return value;
}

/******************************************************************************
 * FunctionName : espconn_sent
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to set for client or server
 *                psent -- data to send
 *                length -- length of data to send
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_sent(struct espconn *espconn, uint8 *psent, uint16 length)
{
	espconn_msg *pnode = NULL;
	bool value = false;
	err_t error = ESPCONN_OK;
	
    if (espconn == NULL || psent == NULL || length == 0) {
        return ESPCONN_ARG;
    }

	/*Find the node depend on the espconn message*/
	value = espconn_find_connection(espconn, &pnode);

    if (value){
    	espconn ->state = ESPCONN_WRITE;
		switch (espconn ->type) {
			case ESPCONN_TCP:
				/* calling sent function frequently,make sure last packet has been backup or sent fully*/
				if (pnode->pcommon.write_flag){
					espconn_buf *pbuf = NULL;
					/*If total number of espconn_buf on the unsent lists exceeds the set maximum, return an error */
					if (espconn_copy_enabled(pnode)){
						if (espconn_tcp_get_buf_count(pnode->pcommon.pbuf) >= pnode ->pcommon.pbuf_num)
							return ESPCONN_MAXNUM;
					} else {
						struct tcp_pcb *pcb = pnode->pcommon.pcb;
						if (pcb->snd_queuelen >= TCP_SND_QUEUELEN)
							return ESPCONN_MAXNUM;
					}

					pbuf = (espconn_buf*) os_zalloc(sizeof(espconn_buf));
					if (pbuf == NULL)
						return ESPCONN_MEM;
					else {
						/*Backup the application packet information for send more data*/
						pbuf->payload = psent;
						pbuf->punsent = pbuf->payload;
						pbuf->unsent = length;
						pbuf->len = length;
						/*insert the espconn_pbuf to the list*/
						espconn_pbuf_create(&pnode->pcommon.pbuf, pbuf);
						if (pnode->pcommon.ptail == NULL)
							pnode->pcommon.ptail = pbuf;
					}
					/*when set the data copy option. change the flag for next packet*/
					if (espconn_copy_disabled(pnode))
						pnode->pcommon.write_flag = false;
					error = espconn_tcp_write(pnode);
//					if (error != ESPCONN_OK){
//						/*send the application packet fail,
//						 * ensure that each allocated is deleted*/
//						espconn_pbuf_delete(&pnode->pcommon.pbuf, pbuf);
//						os_free(pbuf);
//						pbuf = NULL;
//					}
					return error;
				} else
					return ESPCONN_ARG;
				break;

			case ESPCONN_UDP:
				return espconn_udp_sent(pnode, psent, length);
				break;

			default :
				break;
		}
    }
    return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_sendto
 * Description  : send data for UDP
 * Parameters   : espconn -- espconn to set for UDP
 *                psent -- data to send
 *                length -- length of data to send
 * Returns      : error
*******************************************************************************/
sint16 ICACHE_FLASH_ATTR
espconn_sendto(struct espconn *espconn, uint8 *psent, uint16 length)
{
	espconn_msg *pnode = NULL;
	bool value = false;
	err_t error = ESPCONN_OK;

	if (espconn == NULL || psent == NULL || length == 0) {
		return ESPCONN_ARG;
	}

	/*Find the node depend on the espconn message*/
	value = espconn_find_connection(espconn, &pnode);
	if (value && espconn->type == ESPCONN_UDP)
		return espconn_udp_sendto(pnode, psent, length);
	else
		return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_send
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to set for client or server
 *                psent -- data to send
 *                length -- length of data to send
 * Returns      : none
*******************************************************************************/

sint8 espconn_send(struct espconn *espconn, uint8 *psent, uint16 length) __attribute__((alias("espconn_sent")));

/******************************************************************************
 * FunctionName : espconn_tcp_get_wnd
 * Description  : get the window size of simulatenously active TCP connections
 * Parameters   : none
 * Returns      : the number of TCP_MSS active TCP connections
*******************************************************************************/
uint8 ICACHE_FLASH_ATTR espconn_tcp_get_wnd(void)
{
	uint8 tcp_num = 0;

	tcp_num = (TCP_WND / TCP_MSS);

	return tcp_num;
}
/******************************************************************************
 * FunctionName : espconn_tcp_set_max_con
 * Description  : set the window size simulatenously active TCP connections
 * Parameters   : num -- the number of TCP_MSS
 * Returns      : ESPCONN_ARG -- Illegal argument
 * 				  ESPCONN_OK  -- No error
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_tcp_set_wnd(uint8 num)
{
	if (num == 0 || num > linkMax)
		return ESPCONN_ARG;

	TCP_WND = (num * TCP_MSS);
	return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_tcp_get_mss
 * Description  : get the mss size of simulatenously active TCP connections
 * Parameters   : none
 * Returns      : the size of TCP_MSS active TCP connections
*******************************************************************************/
uint16 ICACHE_FLASH_ATTR espconn_tcp_get_mss(void)
{
	uint16 tcp_num = 0;

	tcp_num = TCP_MSS;

	return tcp_num;
}

/******************************************************************************
 * FunctionName : espconn_tcp_get_max_con
 * Description  : get the number of simulatenously active TCP connections
 * Parameters   : espconn -- espconn to set the connect callback
 * Returns      : none
*******************************************************************************/
uint8 ICACHE_FLASH_ATTR espconn_tcp_get_max_con(void)
{
	uint8 tcp_num = 0;

	tcp_num = MEMP_NUM_TCP_PCB;

	return tcp_num;
}

/******************************************************************************
 * FunctionName : espconn_tcp_set_max_con
 * Description  : set the number of simulatenously active TCP connections
 * Parameters   : espconn -- espconn to set the connect callback
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_tcp_set_max_con(uint8 num)
{
	if (num == 0 || num > linkMax)
		return ESPCONN_ARG;

	MEMP_NUM_TCP_PCB = num;
	return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_tcp_get_max_retran
 * Description  : get the Maximum number of retransmissions of data active TCP connections
 * Parameters   : none
 * Returns      : the Maximum number of retransmissions
*******************************************************************************/
uint8 ICACHE_FLASH_ATTR espconn_tcp_get_max_retran(void)
{
	uint8 tcp_num = 0;

	tcp_num = TCP_MAXRTX;

	return tcp_num;
}

/******************************************************************************
 * FunctionName : espconn_tcp_set_max_retran
 * Description  : set the Maximum number of retransmissions of data active TCP connections
 * Parameters   : num -- the Maximum number of retransmissions
 * Returns      : result
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_tcp_set_max_retran(uint8 num)
{
	if (num == 0 || num > 12)
		return ESPCONN_ARG;

	TCP_MAXRTX = num;
	return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_tcp_get_max_syn
 * Description  : get the Maximum number of retransmissions of SYN segments
 * Parameters   : none
 * Returns      : the Maximum number of retransmissions
*******************************************************************************/
uint8 ICACHE_FLASH_ATTR espconn_tcp_get_max_syn(void)
{
	uint8 tcp_num = 0;

	tcp_num = TCP_SYNMAXRTX;

	return tcp_num;
}

/******************************************************************************
 * FunctionName : espconn_tcp_set_max_syn
 * Description  : set the Maximum number of retransmissions of SYN segments
 * Parameters   : num -- the Maximum number of retransmissions
 * Returns      : result
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_tcp_set_max_syn(uint8 num)
{
	if (num == 0 || num > 12)
		return ESPCONN_ARG;

	TCP_SYNMAXRTX = num;
	return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_tcp_get_max_con_allow
 * Description  : get the count of simulatenously active connections on the server
 * Parameters   : espconn -- espconn to get the count
 * Returns      : result
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_tcp_get_max_con_allow(struct espconn *espconn)
{
	espconn_msg *pget_msg = NULL;
	if ((espconn == NULL) || (espconn->type == ESPCONN_UDP))
		return ESPCONN_ARG;

	pget_msg = pserver_list;
	while (pget_msg != NULL){
		if (pget_msg->pespconn == espconn){
			return pget_msg->count_opt;
		}
		pget_msg = pget_msg->pnext;
	}
	return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_tcp_set_max_con_allow
 * Description  : set the count of simulatenously active connections on the server
 * Parameters   : espconn -- espconn to set the count
 * Returns      : result
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_tcp_set_max_con_allow(struct espconn *espconn, uint8 num)
{
	espconn_msg *pset_msg = NULL;
	if ((espconn == NULL) || (num > MEMP_NUM_TCP_PCB) || (espconn->type == ESPCONN_UDP))
		return ESPCONN_ARG;

	pset_msg = pserver_list;
	while (pset_msg != NULL){
		if (pset_msg->pespconn == espconn){
			pset_msg->count_opt = num;
			return ESPCONN_OK;
		}
		pset_msg = pset_msg->pnext;
	}
	return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_tcp_set_buf_count
 * Description  : set the total number of espconn_buf on the unsent lists for one
 * 				  activate connection
 * Parameters   : espconn -- espconn to set the count
 * 				  num -- the total number of espconn_buf
 * Returns      : result
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_tcp_set_buf_count(struct espconn *espconn, uint8 num)
{
	espconn_msg *plist = NULL;
	if (espconn == NULL || (num > TCP_SND_QUEUELEN))
		return ESPCONN_ARG;

	/*find the node from the active connection list*/
	for (plist = plink_active; plist != NULL; plist = plist->pnext){
		if (plist->pespconn && plist->pespconn == espconn && espconn->type == ESPCONN_TCP){
			plist->pcommon.pbuf_num = num;
			return ESPCONN_OK;
		}
	}

	if (plist == NULL)
		return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_tcp_get_buf_count
 * Description  : get the count of the current node which has espconn_buf
 * Parameters   : pesp_buf -- the list head of espconn_buf type
 * Returns      : the count of the current node which has espconn_buf
*******************************************************************************/
static uint8 ICACHE_FLASH_ATTR espconn_tcp_get_buf_count(espconn_buf *pesp_buf)
{
	espconn_buf *pbuf_list = pesp_buf;
	uint8 pbuf_num = 0;

	/*polling the list get the count of the current node*/
	while (pbuf_list != NULL){
		pbuf_list = pbuf_list->pnext;
		pbuf_num ++;
	}
	return pbuf_num;
}

/******************************************************************************
 * FunctionName : espconn_regist_sentcb
 * Description  : Used to specify the function that should be called when data
 *                has been successfully delivered to the remote host.
 * Parameters   : espconn -- espconn to set the sent callback
 *                sent_cb -- sent callback function to call for this espconn
 *                when data is successfully sent
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_regist_sentcb(struct espconn *espconn, espconn_sent_callback sent_cb)
{
    if (espconn == NULL) {
    	return ESPCONN_ARG;
    }

    espconn ->sent_callback = sent_cb;
    return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_regist_sentcb
 * Description  : Used to specify the function that should be called when data
 *                has been successfully delivered to the remote host.
 * Parameters   : espconn -- espconn to set the sent callback
 *                sent_cb -- sent callback function to call for this espconn
 *                when data is successfully sent
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_regist_write_finish(struct espconn *espconn, espconn_connect_callback write_finish_fn)
{
    if (espconn == NULL || espconn ->proto.tcp == NULL || espconn->type == ESPCONN_UDP) {
    	return ESPCONN_ARG;
    }

    espconn ->proto.tcp->write_finish_fn = write_finish_fn;
    return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_regist_connectcb
 * Description  : used to specify the function that should be called when
 *                connects to host.
 * Parameters   : espconn -- espconn to set the connect callback
 *                connect_cb -- connected callback function to call when connected
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_regist_connectcb(struct espconn *espconn, espconn_connect_callback connect_cb)
{
    if (espconn == NULL) {
    	return ESPCONN_ARG;
    }

    espconn->proto.tcp->connect_callback = connect_cb;
    return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_regist_recvcb
 * Description  : used to specify the function that should be called when recv
 *                data from host.
 * Parameters   : espconn -- espconn to set the recv callback
 *                recv_cb -- recv callback function to call when recv data
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_regist_recvcb(struct espconn *espconn, espconn_recv_callback recv_cb)
{
    if (espconn == NULL) {
    	return ESPCONN_ARG;
    }

    espconn ->recv_callback = recv_cb;
    return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_regist_reconcb
 * Description  : used to specify the function that should be called when connection
 *                because of err disconnect.
 * Parameters   : espconn -- espconn to set the err callback
 *                recon_cb -- err callback function to call when err
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_regist_reconcb(struct espconn *espconn, espconn_reconnect_callback recon_cb)
{
    if (espconn == NULL) {
    	return ESPCONN_ARG;
    }

    espconn ->proto.tcp->reconnect_callback = recon_cb;
    return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_regist_disconcb
 * Description  : used to specify the function that should be called when disconnect
 * Parameters   : espconn -- espconn to set the err callback
 *                discon_cb -- err callback function to call when err
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_regist_disconcb(struct espconn *espconn, espconn_connect_callback discon_cb)
{
    if (espconn == NULL) {
    	return ESPCONN_ARG;
    }

    espconn ->proto.tcp->disconnect_callback = discon_cb;
    return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_get_connection_info
 * Description  : used to specify the function that should be called when disconnect
 * Parameters   : espconn -- espconn to set the err callback
 *                discon_cb -- err callback function to call when err
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_get_connection_info(struct espconn *pespconn, remot_info **pcon_info, uint8 typeflags)
{
	espconn_msg *plist = NULL;

	if (pespconn == NULL)
		return ESPCONN_ARG;

	os_memset(premot, 0, sizeof(premot));
	pespconn->link_cnt = 0;
	plist = plink_active;
	switch (pespconn->type){
		case ESPCONN_TCP:
			while(plist != NULL){
				if (plist->preverse == pespconn){
					premot[pespconn->link_cnt].state = plist->pespconn->state;
					premot[pespconn->link_cnt].remote_port = plist->pcommon.remote_port;
					os_memcpy(premot[pespconn->link_cnt].remote_ip,	plist->pcommon.remote_ip, 4);
					pespconn->link_cnt ++;
				}
				plist = plist->pnext;
			}

			break;
		case ESPCONN_UDP:
			while(plist != NULL){
				if (plist->pespconn == pespconn){
					premot[pespconn->link_cnt].state = plist->pespconn->state;
					premot[pespconn->link_cnt].remote_port = plist->pcommon.remote_port;
					os_memcpy(premot[pespconn->link_cnt].remote_ip, plist->pcommon.remote_ip, 4);
					pespconn->link_cnt ++;
				}
				plist = plist->pnext;
			}
			break;
		default:
			break;
	}
	*pcon_info = premot;
	if (pespconn->link_cnt == 0)
		return ESPCONN_ARG;
	return ESPCONN_OK;
}

/******************************************************************************
 * FunctionName : espconn_accept
 * Description  : The function given as the listen
 * Parameters   : espconn -- the espconn used to listen the connection
 * Returns      :
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_accept(struct espconn *espconn)
{
	sint8 value = ESPCONN_OK;
	espconn_msg *plist = NULL;

    if (espconn == NULL) {
        return ESPCONN_ARG;
    } else if (espconn ->type != ESPCONN_TCP)
    	return ESPCONN_ARG;

    /*check the active node information whether is the same as the entity or not*/
    for (plist = plink_active; plist != NULL; plist = plist->pnext){
    	if (plist->pespconn && plist->pespconn->type == ESPCONN_TCP){
    		if (espconn->proto.tcp->local_port == plist->pespconn->proto.tcp->local_port){
    			return ESPCONN_ISCONN;
    		}
    	}
    }
    value = espconn_tcp_server(espconn);

    return value;
}

/******************************************************************************
 * FunctionName : espconn_regist_time
 * Description  : used to specify the time that should be called when don't recv data
 * Parameters   : espconn -- the espconn used to the connection
 * 				  interval -- the timer when don't recv data
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_regist_time(struct espconn *espconn, uint32 interval, uint8 type_flag)
{
	espconn_msg *pnode = NULL;
	espconn_msg *ptime_msg = NULL;
	bool value = false;
	if ((espconn == NULL) || (type_flag > 0x01))
		return ESPCONN_ARG;

	if (type_flag == 0x01){
		/*set the timeout time for one active connection of the server*/
		value = espconn_find_connection(espconn, &pnode);
		if (value){
			pnode->pcommon.timeout = interval;
			return ESPCONN_OK;
		} else
			return ESPCONN_ARG;
	} else {
		/*set the timeout time for all active connection of the server*/
		ptime_msg = pserver_list;
		while (ptime_msg != NULL){
			if (ptime_msg->pespconn == espconn){
				ptime_msg->pcommon.timeout = interval;
				return ESPCONN_OK;
			}
			ptime_msg = ptime_msg->pnext;
		}
	}
	return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_disconnect
 * Description  : disconnect with host
 * Parameters   : espconn -- the espconn used to disconnect the connection
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_disconnect(struct espconn *espconn)
{
	espconn_msg *pnode = NULL;
	bool value = false;

    if (espconn == NULL) {
        return ESPCONN_ARG;;
    } else if (espconn ->type != ESPCONN_TCP)
    	return ESPCONN_ARG;

    /*Find the node depend on the espconn message*/
    value = espconn_find_connection(espconn, &pnode);

    if (value){
    	/*protect for redisconnection*/
    	if (pnode->preverse == NULL && espconn->state == ESPCONN_CLOSE)
    		return ESPCONN_INPROGRESS;
    	espconn_tcp_disconnect(pnode,0);	//1 force, 0 normal
    	return ESPCONN_OK;
    } else
    	return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_abort
 * Description  : Forcely abort with host
 * Parameters   : espconn -- the espconn used to disconnect the connection
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_abort(struct espconn *espconn)
{
	espconn_msg *pnode = NULL;
	bool value = false;

    if (espconn == NULL) {
        return ESPCONN_ARG;;
    } else if (espconn ->type != ESPCONN_TCP)
    	return ESPCONN_ARG;

    /*Find the node depend on the espconn message*/
    value = espconn_find_connection(espconn, &pnode);

    if (value){
    	/*protect for redisconnection*/
    	if (espconn->state == ESPCONN_CLOSE)
    		return ESPCONN_INPROGRESS;
    	espconn_tcp_disconnect(pnode,1);	//1 force, 0 normal
    	return ESPCONN_OK;
    } else
    	return ESPCONN_ARG;
}


/******************************************************************************
 * FunctionName : espconn_get_packet_info
 * Description  : get the packet info with host
 * Parameters   : espconn -- the espconn used to disconnect the connection
 * 				  infoarg -- the packet info
 * Returns      : the errur code
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_get_packet_info(struct espconn *espconn, struct espconn_packet* infoarg)
{
	espconn_msg *pnode = NULL;
	err_t err;
	bool value = false;

	if (espconn == NULL || infoarg == NULL) {
		return ESPCONN_ARG;;
	} else if (espconn->type != ESPCONN_TCP)
		return ESPCONN_ARG;

	/*Find the node depend on the espconn message*/
	value = espconn_find_connection(espconn, &pnode);
	if (value) {
		struct tcp_pcb *pcb = pnode->pcommon.pcb;
		if (pcb == NULL)
			return ESPCONN_ARG;

		pnode->pcommon.packet_info.packseq_nxt = pcb->rcv_nxt;
		pnode->pcommon.packet_info.packseqno = pcb->snd_nxt;
		pnode->pcommon.packet_info.snd_buf_size = pcb->snd_buf;
		pnode->pcommon.packet_info.total_queuelen = TCP_SND_QUEUELEN;
		pnode->pcommon.packet_info.snd_queuelen = pnode->pcommon.packet_info.total_queuelen - pcb->snd_queuelen;
		os_memcpy(infoarg,(void*)&pnode->pcommon.packet_info, sizeof(struct espconn_packet));
		return ESPCONN_OK;
	} else {
		switch (espconn->state){
			case ESPCONN_CLOSE:
				os_memcpy(infoarg,(void*)&pktinfo[0], sizeof(struct espconn_packet));
				err = ESPCONN_OK;
				break;
			case ESPCONN_NONE:
				os_memcpy(infoarg,(void*)&pktinfo[1], sizeof(struct espconn_packet));
				err = ESPCONN_OK;
				break;
			default:
				err = ESPCONN_ARG;
				break;
		}
		return err;
	}
}

/******************************************************************************
 * FunctionName : espconn_set_opt
 * Description  : set the option for connections so that we don't end up bouncing
 *                all connections at the same time .
 * Parameters   : espconn -- the espconn used to set the connection
 * 				  opt -- the option for set
 * Returns      : the result
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_set_opt(struct espconn *espconn, uint8 opt)
{
	espconn_msg *pnode = NULL;
	struct tcp_pcb *tpcb;
	bool value = false;

	if (espconn == NULL) {
		return ESPCONN_ARG;;
	} else if (espconn->type != ESPCONN_TCP)
		return ESPCONN_ARG;

	/*Find the node depend on the espconn message*/
	value = espconn_find_connection(espconn, &pnode);
	if (value) {
		pnode->pcommon.espconn_opt |= opt;
		tpcb = pnode->pcommon.pcb;
		if (espconn_delay_disabled(pnode))
			tcp_nagle_disable(tpcb);

		if (espconn_keepalive_disabled(pnode))
			espconn_keepalive_enable(tpcb);

		return ESPCONN_OK;
	} else
		return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_clear_opt
 * Description  : clear the option for connections so that we don't end up bouncing
 *                all connections at the same time .
 * Parameters   : espconn -- the espconn used to set the connection
 * 				  opt -- the option for clear
 * Returns      : the result
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_clear_opt(struct espconn *espconn, uint8 opt)
{
	espconn_msg *pnode = NULL;
	struct tcp_pcb *tpcb;
	bool value = false;

	if (espconn == NULL) {
		return ESPCONN_ARG;;
	} else if (espconn->type != ESPCONN_TCP)
		return ESPCONN_ARG;

	/*Find the node depend on the espconn message*/
	value = espconn_find_connection(espconn, &pnode);
	if (value) {
		pnode->pcommon.espconn_opt &= ~opt;
		tpcb = pnode->pcommon.pcb;
		if (espconn_keepalive_enabled(pnode))
			espconn_keepalive_disable(tpcb);

		if (espconn_delay_enabled(pnode))
			tcp_nagle_enable(tpcb);

		return ESPCONN_OK;
	} else
		return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_set_keepalive
 * Description  : access level value for connection so that we set the value for
 * 				  keep alive
 * Parameters   : espconn -- the espconn used to set the connection
 * 				  level -- the connection's level
 * 				  value -- the value of time(s)
 * Returns      : access port value
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_set_keepalive(struct espconn *espconn, uint8 level, void* optarg)
{
	espconn_msg *pnode = NULL;
	bool value = false;
	sint8 ret = ESPCONN_OK;

	if (espconn == NULL || optarg == NULL) {
		return ESPCONN_ARG;;
	} else if (espconn->type != ESPCONN_TCP)
		return ESPCONN_ARG;

	/*Find the node depend on the espconn message*/
	value = espconn_find_connection(espconn, &pnode);
	if (value && espconn_keepalive_disabled(pnode)) {
		struct tcp_pcb *pcb = pnode->pcommon.pcb;
		switch (level){
			case ESPCONN_KEEPIDLE:
				pcb->keep_idle = 1000 * (u32_t)(*(int*)optarg);
				ret = ESPCONN_OK;
				break;
			case ESPCONN_KEEPINTVL:
				pcb->keep_intvl = 1000 * (u32_t)(*(int*)optarg);
				ret = ESPCONN_OK;
				break;
			case ESPCONN_KEEPCNT:
				pcb->keep_cnt = (u32_t)(*(int*)optarg);
				ret = ESPCONN_OK;
				break;
			default:
				ret = ESPCONN_ARG;
				break;
		}
		return ret;
	} else
		return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_get_keepalive
 * Description  : access level value for connection so that we get the value for
 * 				  keep alive
 * Parameters   : espconn -- the espconn used to get the connection
 * 				  level -- the connection's level
 * Returns      : access keep alive value
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_get_keepalive(struct espconn *espconn, uint8 level, void *optarg)
{
	espconn_msg *pnode = NULL;
	bool value = false;
	sint8 ret = ESPCONN_OK;

	if (espconn == NULL || optarg == NULL) {
		return ESPCONN_ARG;;
	} else if (espconn->type != ESPCONN_TCP)
		return ESPCONN_ARG;

	/*Find the node depend on the espconn message*/
	value = espconn_find_connection(espconn, &pnode);
	if (value && espconn_keepalive_disabled(pnode)) {
		struct tcp_pcb *pcb = pnode->pcommon.pcb;
		switch (level) {
		case ESPCONN_KEEPIDLE:
			*(int*)optarg = (int)(pcb->keep_idle/1000);
			ret = ESPCONN_OK;
			break;
		case ESPCONN_KEEPINTVL:
			*(int*)optarg = (int)(pcb->keep_intvl/1000);
			ret = ESPCONN_OK;
			break;
		case ESPCONN_KEEPCNT:
			*(int*)optarg = (int)(pcb->keep_cnt);
			ret = ESPCONN_OK;
			break;
		default:
			ret = ESPCONN_ARG;
			break;
		}
		return ret;
	} else
		return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_delete
 * Description  : disconnect with host
 * Parameters   : espconn -- the espconn used to disconnect the connection
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_delete(struct espconn *espconn)
{
	espconn_msg *pnode = NULL;
	bool value = false;

    if (espconn == NULL) {
        return ESPCONN_ARG;
    } else if (espconn ->type != ESPCONN_UDP)
    	return espconn_tcp_delete(espconn);

    /*Find the node depend on the espconn message*/
    value = espconn_find_connection(espconn, &pnode);

    if (value){
    	espconn_udp_disconnect(pnode);
    	return ESPCONN_OK;
    } else
    	return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_port
 * Description  : access port value for client so that we don't end up bouncing
 *                all connections at the same time .
 * Parameters   : none
 * Returns      : access port value
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
espconn_port(void)
{
    uint32 port = 0;
    static uint32 randnum = 0;

    do {
        port = os_random();

        if (port < 0) {
            port = os_random() - port;
        }

        port %= 0xc350;

        if (port < 0x400) {
            port += 0x400;
        }

    } while (port == randnum);

    randnum = port;

    return port;
}

/******************************************************************************
 * FunctionName : espconn_gethostbyname
 * Description  : Resolve a hostname (string) into an IP address.
 * Parameters   : pespconn -- espconn to resolve a hostname
 *                hostname -- the hostname that is to be queried
 *                addr -- pointer to a ip_addr_t where to store the address if 
 *                        it is already cached in the dns_table (only valid if
 *                        ESPCONN_OK is returned!)
 *                found -- a callback function to be called on success, failure
 *                         or timeout (only if ERR_INPROGRESS is returned!)
 * Returns      : err_t return code
 *                - ESPCONN_OK if hostname is a valid IP address string or the host
 *                  name is already in the local names table.
 *                - ESPCONN_INPROGRESS enqueue a request to be sent to the DNS server
 *                  for resolution if no errors are present.
 *                - ESPCONN_ARG: dns client not initialized or invalid hostname
*******************************************************************************/
err_t ICACHE_FLASH_ATTR
espconn_gethostbyname(struct espconn *pespconn, const char *hostname, ip_addr_t *addr, dns_found_callback found)
{
    return dns_gethostbyname(hostname, addr, found, pespconn);
}

/******************************************************************************
 * FunctionName : espconn_dns_setserver
 * Description  : Initialize one of the DNS servers.
 * Parameters   : numdns -- the index of the DNS server to set must
 * 				  be < DNS_MAX_SERVERS = 2
 * 			      dnsserver -- IP address of the DNS server to set
 *  Returns     : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
espconn_dns_setserver(u8_t numdns, ip_addr_t *dnsserver)
{
	dns_setserver(numdns,dnsserver);
}

