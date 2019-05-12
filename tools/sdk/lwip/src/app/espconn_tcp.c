/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: espconn_tcp.c
 *
 * Description: tcp proto interface
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
#include "lwip/tcp_impl.h"
#include "lwip/memp.h"

#include "ets_sys.h"
#include "os_type.h"
//#include "os.h"
#include "lwip/mem.h"
#include "lwip/app/espconn_tcp.h"

#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR = __FILE__;
#endif

extern espconn_msg *plink_active;
extern espconn_msg *pserver_list;
extern struct espconn_packet pktinfo[2];
extern struct tcp_pcb ** const tcp_pcb_lists[];

os_event_t espconn_TaskQueue[espconn_TaskQueueLen];

static err_t
espconn_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static void
espconn_client_close(void *arg, struct tcp_pcb *pcb,u8 type);

static err_t
espconn_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static void
espconn_server_close(void *arg, struct tcp_pcb *pcb,u8 type);

///////////////////////////////common function/////////////////////////////////
/******************************************************************************
 * FunctionName : espconn_kill_oldest
 * Description  : kill the oldest TCP block
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
espconn_kill_oldest(void)
{
	struct tcp_pcb *pcb, *inactive;
	u32_t inactivity;

	inactivity = 0;
	inactive = NULL;
	/* Go through the list of TIME_WAIT pcbs and get the oldest pcb. */
	for (pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next) {
		if ((u32_t) (tcp_ticks - pcb->tmr) >= inactivity) {
			inactivity = tcp_ticks - pcb->tmr;
			inactive = pcb;
		}
	}
	if (inactive != NULL) {
		tcp_abort(inactive);
	}

	/* Go through the list of FIN_WAIT_2 pcbs and get the oldest pcb. */
	inactivity = 0;
	inactive = NULL;
	for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
		if (pcb->state == FIN_WAIT_1 || pcb->state == FIN_WAIT_2){
			if ((u32_t) (tcp_ticks - pcb->tmr) >= inactivity) {
				inactivity = tcp_ticks - pcb->tmr;
				inactive = pcb;
			}
		}
	}
	/*Purges the PCB, removes it from a PCB list and frees the memory*/
	if (inactive != NULL) {
		tcp_pcb_remove(&tcp_active_pcbs, inactive);
		memp_free(MEMP_TCP_PCB, inactive);
	}

	/* Go through the list of LAST_ACK pcbs and get the oldest pcb. */
	inactivity = 0;
	inactive = NULL;
	for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
		if (pcb->state == LAST_ACK) {
			if ((u32_t) (tcp_ticks - pcb->tmr) >= inactivity) {
				inactivity = tcp_ticks - pcb->tmr;
				inactive = pcb;
			}
		}
	}
	/*Purges the PCB, removes it from a PCB list and frees the memory*/
	if (inactive != NULL) {
		tcp_pcb_remove(&tcp_active_pcbs, inactive);
		memp_free(MEMP_TCP_PCB, inactive);
	}
}

/******************************************************************************
 * FunctionName : espconn_kill_oldest_pcb
 * Description  : find the oldest TCP block by state
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR espconn_kill_oldest_pcb(void)
{
	struct tcp_pcb *cpcb = NULL;
	uint8 i = 0;
	uint8 num_tcp_fin = 0;
	for(i = 2; i < 4; i ++){
		for (cpcb = *tcp_pcb_lists[i]; cpcb != NULL; cpcb = cpcb->next) {
			if (cpcb->state == TIME_WAIT){
				num_tcp_fin ++;
				if (num_tcp_fin == MEMP_NUM_TCP_PCB)
					break;
			}

			if (cpcb->state == FIN_WAIT_1 || cpcb->state == FIN_WAIT_2 || cpcb->state == LAST_ACK){
				num_tcp_fin++;
				if (num_tcp_fin == MEMP_NUM_TCP_PCB)
					break;
			}
		}

		if (num_tcp_fin == MEMP_NUM_TCP_PCB){
			num_tcp_fin = 0;
			espconn_kill_oldest();
		} else if (cpcb == NULL){
			num_tcp_fin = 0;
		}
	}
}

/******************************************************************************
 * FunctionName : espconn_kill_pcb
 * Description  : kill all the TCP block by port
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR espconn_kill_pcb(u16_t port)
{
	struct tcp_pcb *cpcb = NULL;
	uint8 i = 0;
	struct tcp_pcb *inactive = NULL;
	struct tcp_pcb *prev = NULL;
	u8_t pcb_remove;
	/* Check if the address already is in use (on all lists) */
	for (i = 1; i < 4; i++) {
		cpcb = *tcp_pcb_lists[i];
		while(cpcb != NULL){
			pcb_remove = 0;
			if (cpcb->local_port == port) {
				++pcb_remove;
			}
			/* If the PCB should be removed, do it. */
			if (pcb_remove) {
				/* Remove PCB from tcp_pcb_lists list. */
				inactive = cpcb;
				cpcb = inactive->next;
				tcp_pcb_remove(tcp_pcb_lists[i], inactive);
				memp_free(MEMP_TCP_PCB, inactive);
			} else {
				cpcb = cpcb->next;
			}
		}
	}
}

/******************************************************************************
 * FunctionName : espconn_find_current_pcb
 * Description  : find the TCP block which option
 * Parameters   : pcurrent_msg -- the node in the list which active
 * Returns      : TCP block point
*******************************************************************************/
struct tcp_pcb *ICACHE_FLASH_ATTR espconn_find_current_pcb(espconn_msg *pcurrent_msg)
{
	uint16 local_port = pcurrent_msg->pcommon.local_port;
	uint32 local_ip = pcurrent_msg->pcommon.local_ip;
	uint16 remote_port = pcurrent_msg->pcommon.remote_port;
	uint32 remote_ip = *((uint32*)&pcurrent_msg->pcommon.remote_ip);
	struct tcp_pcb *find_pcb = NULL;
	if (pcurrent_msg ->preverse == NULL){/*Find the server's TCP block*/
		if (local_ip == 0|| local_port == 0) return pcurrent_msg->pcommon.pcb;

		for (find_pcb = tcp_active_pcbs; find_pcb != NULL; find_pcb = find_pcb->next){
			if ((find_pcb->remote_port == remote_port) && (find_pcb->remote_ip.addr == remote_ip) &&
				(find_pcb->local_port == local_port) && (find_pcb->local_ip.addr == local_ip))
				return find_pcb;
		}

		for (find_pcb = tcp_tw_pcbs; find_pcb != NULL; find_pcb = find_pcb->next){
			if ((find_pcb->remote_port == remote_port) && (find_pcb->remote_ip.addr == remote_ip) &&
				(find_pcb->local_port == local_port) && (find_pcb->local_ip.addr == local_ip))
				return find_pcb;
		}
	} else {/*Find the client's TCP block*/
		if (remote_ip == 0|| remote_port == 0) return pcurrent_msg->pcommon.pcb;

		for (find_pcb = tcp_active_pcbs; find_pcb != NULL; find_pcb = find_pcb->next){
			if ((find_pcb->remote_port == remote_port) && (find_pcb->remote_ip.addr == remote_ip))
				return find_pcb;
		}

		for (find_pcb = tcp_tw_pcbs; find_pcb != NULL; find_pcb = find_pcb->next){
			if ((find_pcb->remote_port == remote_port) && (find_pcb->remote_ip.addr == remote_ip))
				return find_pcb;
		}
	}
	return NULL;
}

/******************************************************************************
 * FunctionName : espconn_tcp_memp_free
 * Description  : frees the connection memory in the server mode
 * Parameters   : arg -- Additional argument to pass to the function
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR espconn_tcp_memp_free(espconn_msg *pmemp)
{
	struct espconn *espconn = NULL;
	if (pmemp == NULL)
		return;

	if (pmemp->espconn_mode == ESPCONN_TCPSERVER_MODE){
		if (pmemp->pespconn != NULL && pmemp->pespconn->proto.tcp != NULL)
			os_free(pmemp->pespconn->proto.tcp);
		pmemp->pespconn->proto.tcp = NULL;

		os_free(pmemp->pespconn);
		pmemp->pespconn = NULL;
	}

	if (pmemp->readbuf != NULL){
		ringbuf_free(&pmemp->readbuf);
	}
	os_free(pmemp);
	pmemp = NULL;
}

/******************************************************************************
 * FunctionName : espconn_tcp_reconnect
 * Description  : reconnect with host
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
espconn_tcp_reconnect(void *arg)
{
	espconn_msg *precon_cb = arg;
	sint8 re_err = 0;
	espconn_buf *perr_buf = NULL;
	espconn_buf *perr_back = NULL;
	espconn_kill_oldest_pcb();
	if (precon_cb != NULL) {
		struct espconn *espconn = precon_cb->preverse;

		re_err = precon_cb->pcommon.err;
		if (precon_cb->pespconn != NULL){
			if (espconn != NULL){/*Process the server's message block*/
				if (precon_cb->pespconn->proto.tcp != NULL){
					espconn_copy_partial(espconn, precon_cb->pespconn);
				}
			} else {/*Process the client's message block*/
				espconn = precon_cb->pespconn;
			}
		}

		/*to prevent memory leaks, ensure that each allocated is deleted*/
		perr_buf = precon_cb->pcommon.pbuf;
		while (perr_buf != NULL){
			perr_back = perr_buf;
			perr_buf = perr_back->pnext;
			espconn_pbuf_delete(&precon_cb->pcommon.pbuf,perr_back);
			os_free(perr_back);
			perr_back = NULL;
		}
		os_bzero(&pktinfo[1], sizeof(struct espconn_packet));
		os_memcpy(&pktinfo[1], (void*)&precon_cb->pcommon.packet_info, sizeof(struct espconn_packet));

		if (espconn && espconn->proto.tcp && espconn->proto.tcp->reconnect_callback != NULL) {
			espconn->proto.tcp->reconnect_callback(espconn, re_err);
		}

		/*frees the connection memory*/
		espconn_tcp_memp_free(precon_cb);
	} else {
		espconn_printf("espconn_tcp_reconnect err\n");
	}
}

/******************************************************************************
 * FunctionName : espconn_tcp_disconnect
 * Description  : disconnect with host
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
espconn_tcp_disconnect_successful(void *arg)
{
	espconn_msg *pdiscon_cb = arg;
	sint8 dis_err = 0;
	espconn_buf *pdis_buf = NULL;
	espconn_buf *pdis_back = NULL;
	espconn_kill_oldest_pcb();
	if (pdiscon_cb != NULL) {
		struct espconn *espconn = pdiscon_cb->preverse;

		dis_err = pdiscon_cb->pcommon.err;
		if (pdiscon_cb->pespconn != NULL){
			struct tcp_pcb *pcb = NULL;
			if (espconn != NULL){/*Process the server's message block*/
				if (pdiscon_cb->pespconn->proto.tcp != NULL && espconn->proto.tcp){
					espconn_copy_partial(espconn, pdiscon_cb->pespconn);
				}
			} else {/*Process the client's message block*/
				espconn = pdiscon_cb->pespconn;
			}

			/*process the current TCP block*/
			pcb = espconn_find_current_pcb(pdiscon_cb);
			if (pcb != NULL){
				if (espconn_reuse_disabled(pdiscon_cb)) {
					struct tcp_pcb *cpcb = NULL;
					struct tcp_pcb *prev = NULL;
					u8_t pcb_remove;
					espconn_printf("espconn_tcp_disconnect_successful %d, %d\n",	pcb->state, pcb->local_port);
					cpcb = tcp_tw_pcbs;
					while (cpcb != NULL) {
						pcb_remove = 0;
						if (cpcb->local_port == pcb->local_port) {
							++pcb_remove;
						}
						/* If the PCB should be removed, do it. */
						if (pcb_remove) {
							struct tcp_pcb *backup_pcb = NULL;
							tcp_pcb_purge(cpcb);
							/* Remove PCB from tcp_tw_pcbs list. */
							if (prev != NULL) {
								LWIP_ASSERT("espconn_tcp_delete: middle cpcb != tcp_tw_pcbs",cpcb != tcp_tw_pcbs);
								prev->next = cpcb->next;
							} else {
								/* This PCB was the first. */
								LWIP_ASSERT("espconn_tcp_delete: first cpcb == tcp_tw_pcbs",tcp_tw_pcbs == cpcb);
								tcp_tw_pcbs = cpcb->next;
							}
							backup_pcb = cpcb;
							cpcb = cpcb->next;
							memp_free(MEMP_TCP_PCB, backup_pcb);
						} else {
							prev = cpcb;
							cpcb = cpcb->next;
						}
					}

				} else {
					tcp_arg(pcb, NULL);
					tcp_err(pcb, NULL);
				}
			}
		}

		/*to prevent memory leaks, ensure that each allocated is deleted*/
		pdis_buf = pdiscon_cb->pcommon.pbuf;
		while (pdis_buf != NULL) {
			pdis_back = pdis_buf;
			pdis_buf = pdis_back->pnext;
			espconn_pbuf_delete(&pdiscon_cb->pcommon.pbuf, pdis_back);
			os_free(pdis_back);
			pdis_back = NULL;
		}
		os_bzero(&pktinfo[0], sizeof(struct espconn_packet));
		os_memcpy(&pktinfo[0], (void*)&pdiscon_cb->pcommon.packet_info, sizeof(struct espconn_packet));

		if (espconn->proto.tcp && espconn->proto.tcp->disconnect_callback != NULL) {
			espconn->proto.tcp->disconnect_callback(espconn);
		}

		/*frees the connection memory*/
		espconn_tcp_memp_free(pdiscon_cb);
	} else {
		espconn_printf("espconn_tcp_disconnect err\n");
	}
}

/******************************************************************************
 * FunctionName : espconn_Task
 * Description  : espconn processing task
 * Parameters   : events -- contain the espconn processing data
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
espconn_Task(os_event_t *events)
{
	espconn_msg *plist = NULL;
	bool active_flag = false;
	espconn_msg *task_msg = NULL;
	struct espconn *pespconn = NULL;

	task_msg = (espconn_msg *) events->par;
	/*find the active connection node*/
	for (plist = plink_active; plist != NULL; plist = plist->pnext){
		if (task_msg == plist) {
			active_flag = true;
			break;
		}
	}

	if (active_flag){
		switch (events->sig) {
			case SIG_ESPCONN_WRITE: {
				pespconn = task_msg->pespconn;
				if (pespconn == NULL) {
					return;
				}

				if (pespconn->proto.tcp->write_finish_fn != NULL) {
					pespconn->proto.tcp->write_finish_fn(pespconn);
				}
			}
				break;
			case SIG_ESPCONN_ERRER:
				/*remove the node from the client's active connection list*/
				espconn_list_delete(&plink_active, task_msg);
				espconn_tcp_reconnect(task_msg);
				break;
			case SIG_ESPCONN_CLOSE:
				/*remove the node from the client's active connection list*/
				espconn_list_delete(&plink_active, task_msg);
				espconn_tcp_disconnect_successful(task_msg);
				break;
			default:
				break;
		}
	}
}

/******************************************************************************
 * FunctionName : espconn_tcp_sent
 * Description  : sent data for client or server
 * Parameters   : void *arg -- client or server to send
 *                uint8* psent -- Data to send
 *                uint16 length -- Length of data to send
 * Returns      : return espconn error code.
 * - ESPCONN_OK. Successful. No error occured.
 * - ESPCONN_MEM. Out of memory.
 * - ESPCONN_RTE. Could not find route to destination address.
 * - More errors could be returned by lower protocol layers.
*******************************************************************************/
err_t ICACHE_FLASH_ATTR
espconn_tcp_sent(void *arg, uint8 *psent, uint16 length)
{
	espconn_msg *ptcp_sent = arg;
    struct tcp_pcb *pcb = NULL;
    err_t err = 0;
    u16_t len = 0;
    u8_t data_to_send = false;

    espconn_printf("espconn_tcp_sent ptcp_sent %p psent %p length %d\n", ptcp_sent, psent, length);

    /*Check the parameters*/
    if (ptcp_sent == NULL || psent == NULL || length == 0) {
        return ESPCONN_ARG;
    }

    /*Set the packet length depend on the sender buffer space*/
    pcb = ptcp_sent->pcommon.pcb;
    if (tcp_sndbuf(pcb) < length) {
        len = tcp_sndbuf(pcb);
    } else {
        len = length;
        LWIP_ASSERT("length did not fit into uint16!", (len == length));
    }

    if (len > (2*pcb->mss)) {
        len = 2*pcb->mss;
    }

    /*Write data for sending, but does not send it immediately*/
	do {
		espconn_printf("espconn_tcp_sent writing %d bytes %p\n", len, pcb);
		if (espconn_copy_disabled(ptcp_sent))
			err = tcp_write(pcb, psent, len, 1);
		else
			err = tcp_write(pcb, psent, len, 0);

        if (err == ERR_MEM) {
			if(len < 3)
				len--;
			else
            	len /= 2;
        }

    } while (err == ERR_MEM && len > 0);

	/*Find out what we can send and send it, offset the buffer point for next send*/
    if (err == ERR_OK) {
    	ptcp_sent->pcommon.ptail->punsent = psent + len;
		ptcp_sent->pcommon.ptail->unsent = length - len;
		err = tcp_output(pcb);
		/*If enable the copy option, change the flag for next write*/
		if (espconn_copy_disabled(ptcp_sent)){
			if (ptcp_sent->pcommon.ptail->unsent == 0) {
				ptcp_sent->pcommon.write_flag = true;
				ets_post(espconn_TaskPrio, SIG_ESPCONN_WRITE, (uint32_t)ptcp_sent);
			}
		}
        espconn_printf("espconn_tcp_sent %d\n", err);
    }
    return err;
}

/******************************************************************************
 * FunctionName : espconn_close
 * Description  : The connection has been successfully closed.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR espconn_tcp_disconnect(espconn_msg *pdiscon,u8 type)
{
	if (pdiscon != NULL){
		/*disconnect with the host by send the FIN frame*/
		if (pdiscon->preverse != NULL)
			espconn_server_close(pdiscon, pdiscon->pcommon.pcb,type);
		else
			espconn_client_close(pdiscon, pdiscon->pcommon.pcb,type);
	} else{
		espconn_printf("espconn_tcp_disconnect err.\n");
	}
}

///////////////////////////////client function/////////////////////////////////
/******************************************************************************
 * FunctionName : espconn_client_close
 * Description  : The connection shall be actively closed.
 * Parameters   : pcb -- Additional argument to pass to the callback function
 *                pcb -- the pcb to close
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
espconn_client_close(void *arg, struct tcp_pcb *pcb, u8 type)
{
    err_t err;
    espconn_msg *pclose = arg;

	pclose->pcommon.pcb = pcb;
	/*avoid recalling the disconnect function*/
	tcp_recv(pcb, NULL);

	if(type == 0)
		err = tcp_close(pcb);
	else {
		tcp_sent(pcb, NULL);
		tcp_err(pcb, NULL);
		tcp_abort(pcb);
		err = ERR_OK;
	}

	if (err != ERR_OK) {
		/* closing failed, try again later */
		tcp_recv(pcb, espconn_client_recv);
	} else {
		/* closing succeeded */
		if (type == 0) {
			tcp_sent(pcb, NULL);
			tcp_err(pcb, NULL);
		}
		/*switch the state of espconn for application process*/
		pclose->pespconn->state = ESPCONN_CLOSE;
		ets_post(espconn_TaskPrio, SIG_ESPCONN_CLOSE, (uint32_t)pclose);
	}
}

//***********Code for WIFI_BLOCK from upper**************
sint8 ICACHE_FLASH_ATTR
espconn_recv_hold(struct espconn *pespconn)
{
	//1st, according to espconn code, have to find out the escpconn_msg by pespconn;
	espconn_msg *pnode = NULL;
	bool value = false;
    if (pespconn == NULL) {
        return ESPCONN_ARG;
    }
    value = espconn_find_connection(pespconn, &pnode);
	if(value != true)
	{
		os_printf("RecvHold, By pespconn,find conn_msg fail\n");
		return ESPCONN_ARG;
	}

	//2nd, the actual operation
	if(pnode->recv_hold_flag == 0)
	{
		pnode->recv_hold_flag = 1;
		pnode->recv_holded_buf_Len = 0;
	}
	return ESPCONN_OK;
}

sint8 ICACHE_FLASH_ATTR
espconn_recv_unhold(struct espconn *pespconn)
{
	//1st, according to espconn code, have to find out the escpconn_msg by pespconn;
	espconn_msg *pnode = NULL;
	bool value = false;
    if (pespconn == NULL) {
        return ESPCONN_ARG;
    }
    value = espconn_find_connection(pespconn, &pnode);
	if(value != true)
	{
		os_printf("RecvHold, By pespconn,find conn_msg fail\n");
		return ESPCONN_ARG;
	}

	//2nd, the actual operation
	if(pnode->recv_hold_flag == 1)
	{
		if(pespconn->type == ESPCONN_TCP) {
			tcp_recved(pnode->pcommon.pcb, pnode->recv_holded_buf_Len);
		}
		pnode->recv_holded_buf_Len = 0;
		pnode->recv_hold_flag = 0;
	}
	return ESPCONN_OK;
}

//***********Code for WIFI_BLOCK from upper**************
sint8 ICACHE_FLASH_ATTR
espconn_lock_recv(espconn_msg *plockmsg)
{
	if (plockmsg == NULL || plockmsg->pespconn == NULL) {
		return ESPCONN_ARG;
	}

	if (plockmsg->pespconn->recv_callback == NULL){
		if (plockmsg->readbuf == NULL){
			plockmsg->readbuf = ringbuf_new(TCP_WND);
			if (plockmsg->readbuf == NULL)
				return ESPCONN_MEM;
		}
		return espconn_recv_hold(plockmsg->pespconn);
	}

	return ESPCONN_OK;
}

sint8 ICACHE_FLASH_ATTR
espconn_unlock_recv(espconn_msg *punlockmsg)
{
	if (punlockmsg == NULL || punlockmsg->pespconn == NULL) {
		return ESPCONN_ARG;
	}

	if (punlockmsg->pespconn->recv_callback != NULL)
		return espconn_recv_unhold(punlockmsg->pespconn);

	return ESPCONN_OK;
}
/******************************************************************************
 * FunctionName : espconn_client_recv
 * Description  : Data has been received on this pcb.
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pcb -- The connection pcb which received data
 *                p -- The received data (or NULL when the connection has been closed!)
 *                err -- An error code if there has been an error receiving
 * Returns      : ERR_ABRT: if you have called tcp_abort from within the function!
*******************************************************************************/
static err_t ICACHE_FLASH_ATTR
espconn_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	espconn_msg *precv_cb = arg;

	tcp_arg(pcb, arg);
	/*lock the window because of application layer don't need the data*/
	espconn_lock_recv(precv_cb);

    if (p != NULL) {
    	/*To update and advertise a larger window*/
		if(precv_cb->recv_hold_flag == 0)
        	tcp_recved(pcb, p->tot_len);
		else
			precv_cb->recv_holded_buf_Len += p->tot_len;
    }

    if (precv_cb->pespconn->recv_callback != NULL){
		if (err == ERR_OK && p != NULL) {
			char *pdata = NULL;
			u16_t length = 0;
			/*Copy the contents of a packet buffer to an application buffer.
			 *to prevent memory leaks, ensure that each allocated is deleted*/
			pdata = (char *)os_zalloc(p ->tot_len + 1);
			length = pbuf_copy_partial(p, pdata, p ->tot_len, 0);
			pbuf_free(p);

			if (length != 0) {
				/*switch the state of espconn for application process*/
				precv_cb->pespconn ->state = ESPCONN_READ;
				precv_cb->pcommon.pcb = pcb;
				precv_cb->pespconn->recv_callback(precv_cb->pespconn, pdata, length);

				/*switch the state of espconn for next packet copy*/
				if (pcb->state == ESTABLISHED)
					precv_cb->pespconn ->state = ESPCONN_CONNECT;
			}

			/*to prevent memory leaks, ensure that each allocated is deleted*/
			os_free(pdata);
			pdata = NULL;
		}
    } else{
    	/*unregister receive function*/
    	struct pbuf *pthis = NULL;
    	for (pthis = p; pthis != NULL; pthis = pthis->next) {
    		ringbuf_memcpy_into(precv_cb->readbuf, pthis->payload, pthis->len);
    		pbuf_free(pthis);
    	}
    }

    if (err == ERR_OK && p == NULL) {
        espconn_client_close(precv_cb, pcb,0);
    }

    return ERR_OK;
}

/******************************************************************************
 * FunctionName : espconn_tcp_write
 * Description  : write the packet which in the active connection's list.
 * Parameters   : arg -- the node pointer which reverse the packet
 * Returns      : ESPCONN_MEM: memory error
 * 				  ESPCONN_OK:have enough space for write packet
*******************************************************************************/
err_t ICACHE_FLASH_ATTR espconn_tcp_write(void *arg)
{
	espconn_msg *pwrite = arg;
	err_t err = ERR_OK;
	struct tcp_pcb *pcb = pwrite->pcommon.pcb;
	/*for one active connection,limit the sender buffer space*/
	if (tcp_nagle_disabled(pcb) && (pcb->snd_queuelen >= TCP_SND_QUEUELEN))
		return ESPCONN_MEM;

	while (tcp_sndbuf(pcb) != 0){
		if (pwrite->pcommon.ptail != NULL) {
			/*Find the node whether in the list's tail or not*/
			if (pwrite->pcommon.ptail->unsent == 0) {
				pwrite->pcommon.ptail = pwrite->pcommon.ptail->pnext;
				continue;
			}

			/*Send the packet for the active connection*/
			err = espconn_tcp_sent(pwrite, pwrite->pcommon.ptail->punsent,pwrite->pcommon.ptail->unsent);
			if (err != ERR_OK)
				break;
		} else
			break;
	}
	return err;
}

/******************************************************************************
 * FunctionName : espconn_tcp_reconnect
 * Description  : reconnect with host
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR espconn_tcp_finish(void *arg)
{
	espconn_msg *pfinish = arg;
	espconn_buf *premove = NULL;
	uint16 len = 0;
	espconn_tcp_write(pfinish);
	while (pfinish->pcommon.pbuf != NULL){
		premove = pfinish->pcommon.pbuf;
		pfinish->pcommon.pbuf->tot_len += len;
		/*application packet has been sent and acknowledged by the remote host,
		 * to prevent memory leaks, ensure that each allocated is deleted*/
		if (premove->tot_len >= premove->len){
			espconn_pbuf_delete(&pfinish->pcommon.pbuf,premove);
			len = premove->tot_len - premove->len;
			pfinish->pcommon.packet_info.sent_length = premove->len;
			os_free(premove);
			premove = NULL;
			pfinish->pespconn->state = ESPCONN_CONNECT;
			if (pfinish->pespconn->sent_callback != NULL) {
				pfinish->pespconn->sent_callback(pfinish->pespconn);
			}
			pfinish->pcommon.packet_info.sent_length = len;
		} else
			break;
	}
}

/******************************************************************************
 * FunctionName : espconn_client_sent
 * Description  : Data has been sent and acknowledged by the remote host.
 *                This means that more data can be sent.
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pcb -- The connection pcb for which data has been acknowledged
 *                len -- The amount of bytes acknowledged
 * Returns      : ERR_OK: try to send some data by calling tcp_output
 *                ERR_ABRT: if you have called tcp_abort from within the function!
*******************************************************************************/
static err_t ICACHE_FLASH_ATTR
espconn_client_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
	espconn_msg *psent_cb = arg;

	psent_cb->pcommon.pcb = pcb;
	psent_cb->pcommon.pbuf->tot_len += len;
	psent_cb->pcommon.packet_info.sent_length = len;

	/*Send more data for one active connection*/
	espconn_tcp_finish(psent_cb);

    return ERR_OK;
}

/******************************************************************************
 * FunctionName : espconn_client_err
 * Description  : The pcb had an error and is already deallocated.
 *                The argument might still be valid (if != NULL).
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                err -- Error code to indicate why the pcb has been closed
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
espconn_client_err(void *arg, err_t err)
{
	espconn_msg *perr_cb = arg;
	struct tcp_pcb *pcb = NULL;
    LWIP_UNUSED_ARG(err);

    if (perr_cb != NULL) {
        pcb = perr_cb->pcommon.pcb;
        perr_cb->pespconn->state = ESPCONN_CLOSE;
        espconn_printf("espconn_client_err %d %d %d\n", pcb->state, pcb->nrtx, err);

//        /*remove the node from the client's active connection list*/
//        espconn_list_delete(&plink_active, perr_cb);

        /*Set the error code depend on the error type and control block state*/
        if (err == ERR_ABRT) {
        	switch (pcb->state) {
					case SYN_SENT:
						if (pcb->nrtx == TCP_SYNMAXRTX) {
							perr_cb->pcommon.err = ESPCONN_CONN;
						} else {
							perr_cb->pcommon.err = err;
						}

						break;

					case ESTABLISHED:
						if (pcb->nrtx == TCP_MAXRTX) {
							perr_cb->pcommon.err = ESPCONN_TIMEOUT;
						} else {
							perr_cb->pcommon.err = err;
						}
						break;

					case FIN_WAIT_1:
						if (pcb->nrtx == TCP_MAXRTX) {
							perr_cb->pcommon.err = ESPCONN_CLSD;
						} else {
							perr_cb->pcommon.err = err;
						}
						break;
					case FIN_WAIT_2:
						perr_cb->pcommon.err = ESPCONN_CLSD;
						break;
					case CLOSED:
						perr_cb->pcommon.err = ESPCONN_CONN;
						break;
				}
			} else {
				perr_cb->pcommon.err = err;
			}
        	/*post the singer to the task for processing the connection*/
        	ets_post(espconn_TaskPrio, SIG_ESPCONN_ERRER, (uint32_t)perr_cb);
		}
}

/******************************************************************************
 * FunctionName : espconn_client_connect
 * Description  : A new incoming connection has been connected.
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                tpcb -- The connection pcb which is connected
 *                err -- An unused error code, always ERR_OK currently
 * Returns      : connection result
*******************************************************************************/
static err_t ICACHE_FLASH_ATTR
espconn_client_connect(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    espconn_msg *pcon = arg;

    espconn_printf("espconn_client_connect pcon %p tpcb %p\n", pcon, tpcb);
    if (err == ERR_OK){
    	/*Reserve the remote information for current active connection*/
		pcon->pespconn->state = ESPCONN_CONNECT;
		pcon->pcommon.err = err;
		pcon->pcommon.pcb = tpcb;
		pcon->pcommon.local_port = tpcb->local_port;
		pcon->pcommon.local_ip = tpcb->local_ip.addr;
		pcon->pcommon.remote_port = tpcb->remote_port;
		pcon->pcommon.remote_ip[0] = ip4_addr1_16(&tpcb->remote_ip);
		pcon->pcommon.remote_ip[1] = ip4_addr2_16(&tpcb->remote_ip);
		pcon->pcommon.remote_ip[2] = ip4_addr3_16(&tpcb->remote_ip);
		pcon->pcommon.remote_ip[3] = ip4_addr4_16(&tpcb->remote_ip);
		pcon->pcommon.write_flag = true;
		tcp_arg(tpcb, (void *) pcon);

		/*Set the specify function that should be called
		 * when TCP data has been successfully delivered,
		 * when active connection receives data*/
		tcp_sent(tpcb, espconn_client_sent);
		tcp_recv(tpcb, espconn_client_recv);
		/*Disable Nagle algorithm default*/
		tcp_nagle_disable(tpcb);
		/*Default set the total number of espconn_buf on the unsent lists for one*/
		espconn_tcp_set_buf_count(pcon->pespconn, 1);

		if (pcon->pespconn->proto.tcp->connect_callback != NULL) {
			pcon->pespconn->proto.tcp->connect_callback(pcon->pespconn);
		}

		/*Enable keep alive option*/
		if (espconn_keepalive_disabled(pcon))
			espconn_keepalive_enable(tpcb);

//		/*lock the window because of application layer don't need the data*/
//		espconn_lock_recv(pcon);
    } else{
    	os_printf("err in host connected (%s)\n",lwip_strerr(err));
    }
    return err;
}

/******************************************************************************
 * FunctionName : espconn_tcp_client
 * Description  : Initialize the client: set up a connect PCB and bind it to
 *                the defined port
 * Parameters   : espconn -- the espconn used to build client
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_tcp_client(struct espconn *espconn)
{
    struct tcp_pcb *pcb = NULL;
    struct ip_addr ipaddr;
    espconn_msg *pclient = NULL;

    /*Creates a new client control message*/
	pclient = (espconn_msg *)os_zalloc(sizeof(espconn_msg));
	if (pclient == NULL){
		return ESPCONN_MEM;
 	}

	/*Set an IP address given for Little-endian.*/
    IP4_ADDR(&ipaddr, espconn->proto.tcp->remote_ip[0],
    		espconn->proto.tcp->remote_ip[1],
    		espconn->proto.tcp->remote_ip[2],
    		espconn->proto.tcp->remote_ip[3]);

    /*Creates a new TCP protocol control block*/
    pcb = tcp_new();

    if (pcb == NULL) {
    	/*to prevent memory leaks, ensure that each allocated is deleted*/
    	os_free(pclient);
    	pclient = NULL;
        return ESPCONN_MEM;
    } else {

    	/*insert the node to the active connection list*/
    	espconn_list_creat(&plink_active, pclient);
    	tcp_arg(pcb, (void *)pclient);
    	tcp_err(pcb, espconn_client_err);
    	pclient->preverse = NULL;
    	pclient->pespconn = espconn;
    	pclient->pespconn->state = ESPCONN_WAIT;
    	pclient->pcommon.pcb = pcb;
    	tcp_bind(pcb, IP_ADDR_ANY, pclient->pespconn->proto.tcp->local_port);
#if 0
    	pclient->pcommon.err = tcp_bind(pcb, IP_ADDR_ANY, pclient->pespconn->proto.tcp->local_port);
    	if (pclient->pcommon.err != ERR_OK){
    		/*remove the node from the client's active connection list*/
    		espconn_list_delete(&plink_active, pclient);
    		memp_free(MEMP_TCP_PCB, pcb);
    		os_free(pclient);
    		pclient = NULL;
    		return ERR_USE;
    	}
#endif
        /*Establish the connection*/
    	pclient->espconn_mode = ESPCONN_TCPCLIENT_MODE;
        pclient->pcommon.err = tcp_connect(pcb, &ipaddr,
        		pclient->pespconn->proto.tcp->remote_port, espconn_client_connect);
        if (pclient->pcommon.err == ERR_RTE){
			/*remove the node from the client's active connection list*/
			espconn_list_delete(&plink_active, pclient);
			espconn_kill_pcb(pcb->local_port);
			os_free(pclient);
			pclient = NULL;
			return ESPCONN_RTE;
		}
        return pclient->pcommon.err;
    }
}

///////////////////////////////server function/////////////////////////////////
/******************************************************************************
 * FunctionName : espconn_server_close
 * Description  : The connection shall be actively closed.
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pcb -- the pcb to close
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
espconn_server_close(void *arg, struct tcp_pcb *pcb,u8 type)
{
    err_t err;
    espconn_msg *psclose = arg;

	psclose->pcommon.pcb = pcb;
	/*avoid recalling the disconnect function*/
	tcp_recv(pcb, NULL);

	if(type ==0)
		err = tcp_close(pcb);
	else {
		tcp_poll(pcb, NULL, 0);
	    tcp_sent(pcb, NULL);
		tcp_err(pcb, NULL);
		tcp_abort(pcb);
		err = ERR_OK;
	}

    if (err != ERR_OK) {
        /* closing failed, try again later */
        tcp_recv(pcb, espconn_server_recv);
    } else {
        /* closing succeeded */
    	if (type == 0) {
			tcp_poll(pcb, NULL, 0);
			tcp_sent(pcb, NULL);
			tcp_err(pcb, NULL);
    	}
        /*switch the state of espconn for application process*/
        psclose->pespconn->state = ESPCONN_CLOSE;
        ets_post(espconn_TaskPrio, SIG_ESPCONN_CLOSE, (uint32_t)psclose);
	}
}

/******************************************************************************
 * FunctionName : espconn_server_recv
 * Description  : Data has been received on this pcb.
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pcb -- The connection pcb which received data
 *                p -- The received data (or NULL when the connection has been closed!)
 *                err -- An error code if there has been an error receiving
 * Returns      : ERR_ABRT: if you have called tcp_abort from within the function!
*******************************************************************************/
static err_t ICACHE_FLASH_ATTR
espconn_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	espconn_msg *precv_cb = arg;

    tcp_arg(pcb, arg);
    espconn_printf("server has application data received: %d\n", system_get_free_heap_size());
    /*lock the window because of application layer don't need the data*/
    espconn_lock_recv(precv_cb);

    if (p != NULL) {
    	/*To update and advertise a larger window*/
		if(precv_cb->recv_hold_flag == 0)
        	tcp_recved(pcb, p->tot_len);
		else
			precv_cb->recv_holded_buf_Len += p->tot_len;
    }

    /*register receive function*/
	if (precv_cb->pespconn->recv_callback != NULL) {
		if (err == ERR_OK && p != NULL) {
			u8_t *data_ptr = NULL;
			u32_t data_cntr = 0;
			/*clear the count for connection timeout*/
			precv_cb->pcommon.recv_check = 0;
			/*Copy the contents of a packet buffer to an application buffer.
			 *to prevent memory leaks, ensure that each allocated is deleted*/
			data_ptr = (u8_t *) os_zalloc(p ->tot_len + 1);
			data_cntr = pbuf_copy_partial(p, data_ptr, p->tot_len, 0);
			pbuf_free(p);

			if (data_cntr != 0) {
				/*switch the state of espconn for application process*/
				precv_cb->pespconn->state = ESPCONN_READ;
				precv_cb->pcommon.pcb = pcb;
				precv_cb->pespconn->recv_callback(precv_cb->pespconn, data_ptr, data_cntr);

				/*switch the state of espconn for next packet copy*/
				if (pcb->state == ESTABLISHED)
					precv_cb->pespconn->state = ESPCONN_CONNECT;
			}

			/*to prevent memory leaks, ensure that each allocated is deleted*/
			os_free(data_ptr);
			data_ptr = NULL;
			espconn_printf("server's application data has been processed: %d\n", system_get_free_heap_size());
		}
	} else {
		/*unregister receive function*/
		struct pbuf *pthis = NULL;
		for (pthis = p; pthis != NULL; pthis = pthis->next) {
			ringbuf_memcpy_into(precv_cb->readbuf, pthis->payload, pthis->len);
			pbuf_free(pthis);
		}
	}

	if (err == ERR_OK && p == NULL) {
		espconn_server_close(precv_cb, pcb, 0);
	}
    return ERR_OK;
}

/******************************************************************************
 * FunctionName : espconn_server_sent
 * Description  : Data has been sent and acknowledged by the remote host.
 * This means that more data can be sent.
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pcb -- The connection pcb for which data has been acknowledged
 *                len -- The amount of bytes acknowledged
 * Returns      : ERR_OK: try to send some data by calling tcp_output
 *                ERR_ABRT: if you have called tcp_abort from within the function!
*******************************************************************************/
static err_t ICACHE_FLASH_ATTR
espconn_server_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
	espconn_msg *psent_cb = arg;

	psent_cb->pcommon.pcb = pcb;
	psent_cb->pcommon.recv_check = 0;
	psent_cb->pcommon.pbuf->tot_len += len;
	psent_cb->pcommon.packet_info.sent_length = len;

	/*Send more data for one active connection*/
	espconn_tcp_finish(psent_cb);
    return ERR_OK;
}

/******************************************************************************
 * FunctionName : espconn_server_poll
 * Description  : The poll function is called every 3nd second.
 * If there has been no data sent (which resets the retries) in 3 seconds, close.
 * If the last portion of a file has not been sent in 3 seconds, close.
 *
 * This could be increased, but we don't want to waste resources for bad connections.
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pcb -- The connection pcb for which data has been acknowledged
 * Returns      : ERR_OK: try to send some data by calling tcp_output
 *                ERR_ABRT: if you have called tcp_abort from within the function!
*******************************************************************************/
static err_t ICACHE_FLASH_ATTR
espconn_server_poll(void *arg, struct tcp_pcb *pcb)
{
	espconn_msg *pspoll_cb = arg;

	/*exception calling abandon the connection for send a RST frame*/
    if (arg == NULL) {
        tcp_abandon(pcb, 0);
        tcp_poll(pcb, NULL, 0);
        return ERR_OK;
    }

    espconn_printf("espconn_server_poll %d %d\n", pspoll_cb->pcommon.recv_check, pcb->state);
    pspoll_cb->pcommon.pcb = pcb;
    if (pcb->state == ESTABLISHED) {
		pspoll_cb->pcommon.recv_check++;
		if (pspoll_cb->pcommon.timeout != 0){/*no data sent in one active connection's set timeout, close.*/
			if (pspoll_cb->pcommon.recv_check >= pspoll_cb->pcommon.timeout) {
				pspoll_cb->pcommon.recv_check = 0;
				espconn_server_close(pspoll_cb, pcb,0);
			}
		} else {
			espconn_msg *ptime_msg = pserver_list;
			while (ptime_msg != NULL) {
				if (ptime_msg->pespconn == pspoll_cb->preverse){
					if (ptime_msg->pcommon.timeout != 0){/*no data sent in server's set timeout, close.*/
						if (pspoll_cb->pcommon.recv_check >= ptime_msg->pcommon.timeout){
							pspoll_cb->pcommon.recv_check = 0;
							espconn_server_close(pspoll_cb, pcb,0);
						}
					} else {/*don't close for ever*/
						pspoll_cb->pcommon.recv_check = 0;
					}
					break;
				}
				ptime_msg = ptime_msg->pnext;
			}
		}
    } else {
        espconn_server_close(pspoll_cb, pcb,0);
    }

    return ERR_OK;
}

/******************************************************************************
 * FunctionName : esponn_server_err
 * Description  : The pcb had an error and is already deallocated.
 *                The argument might still be valid (if != NULL).
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                err -- Error code to indicate why the pcb has been closed
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
esponn_server_err(void *arg, err_t err)
{
	espconn_msg *pserr_cb = arg;
	struct tcp_pcb *pcb = NULL;
    if (pserr_cb != NULL) {

    	pcb = pserr_cb->pcommon.pcb;
    	pserr_cb->pespconn->state = ESPCONN_CLOSE;

//		/*remove the node from the server's active connection list*/
//		espconn_list_delete(&plink_active, pserr_cb);

    	/*Set the error code depend on the error type and control block state*/
		if (err == ERR_ABRT) {
			switch (pcb->state) {
				case SYN_RCVD:
					if (pcb->nrtx == TCP_SYNMAXRTX) {
						pserr_cb->pcommon.err = ESPCONN_CONN;
					} else {
						pserr_cb->pcommon.err = err;
					}

					break;

				case ESTABLISHED:
					if (pcb->nrtx == TCP_MAXRTX) {
						pserr_cb->pcommon.err = ESPCONN_TIMEOUT;
					} else {
						pserr_cb->pcommon.err = err;
					}

					break;

				case CLOSE_WAIT:
					if (pcb->nrtx == TCP_MAXRTX) {
						pserr_cb->pcommon.err = ESPCONN_CLSD;
					} else {
						pserr_cb->pcommon.err = err;
					}
					break;
				case LAST_ACK:
					pserr_cb->pcommon.err = ESPCONN_CLSD;
					break;

				case CLOSED:
					pserr_cb->pcommon.err = ESPCONN_CONN;
					break;
				default :
					break;
			}
		} else {
			pserr_cb->pcommon.err = err;
		}
		/*post the singer to the task for processing the connection*/
		ets_post(espconn_TaskPrio, SIG_ESPCONN_ERRER, (uint32_t)pserr_cb);
    }
}

/******************************************************************************
 * FunctionName : espconn_tcp_accept
 * Description  : A new incoming connection has been accepted.
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pcb -- The connection pcb which is accepted
 *                err -- An unused error code, always ERR_OK currently
 * Returns      : acception result
*******************************************************************************/
static err_t ICACHE_FLASH_ATTR
espconn_tcp_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    struct espconn *espconn = arg;
    espconn_msg *paccept = NULL;
    remot_info *pinfo = NULL;
    LWIP_UNUSED_ARG(err);

    if (!espconn || !espconn->proto.tcp) {
    	return ERR_ARG;
    }

    tcp_arg(pcb, paccept);
    tcp_err(pcb, esponn_server_err);
    /*Ensure the active connection is less than the count of active connections on the server*/
    espconn_get_connection_info(espconn, &pinfo , 0);
	espconn_printf("espconn_tcp_accept link_cnt: %d\n", espconn->link_cnt);
	if (espconn->link_cnt == espconn_tcp_get_max_con_allow(espconn))
		return ERR_ISCONN;

	/*Creates a new active connect control message*/
    paccept = (espconn_msg *)os_zalloc(sizeof(espconn_msg));
    tcp_arg(pcb, paccept);

	if (paccept == NULL)
		return ERR_MEM;
	/*Insert the node to the active connection list*/
	espconn_list_creat(&plink_active, paccept);

    paccept->preverse = espconn;
    paccept->espconn_mode = ESPCONN_TCPSERVER_MODE;
	paccept->pespconn = (struct espconn *)os_zalloc(sizeof(struct espconn));
	if (paccept->pespconn == NULL)
		return ERR_MEM;
	paccept->pespconn->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	if (paccept->pespconn->proto.tcp == NULL)
		return ERR_MEM;

	/*Reserve the remote information for current active connection*/
	paccept->pcommon.pcb = pcb;

	paccept->pcommon.remote_port = pcb->remote_port;
	paccept->pcommon.remote_ip[0] = ip4_addr1_16(&pcb->remote_ip);
	paccept->pcommon.remote_ip[1] = ip4_addr2_16(&pcb->remote_ip);
	paccept->pcommon.remote_ip[2] = ip4_addr3_16(&pcb->remote_ip);
	paccept->pcommon.remote_ip[3] = ip4_addr4_16(&pcb->remote_ip);
	paccept->pcommon.write_flag = true;

	os_memcpy(espconn->proto.tcp->remote_ip, paccept->pcommon.remote_ip, 4);
	espconn->proto.tcp->remote_port = pcb->remote_port;
	espconn->state = ESPCONN_CONNECT;
	espconn_copy_partial(paccept->pespconn, espconn);

	/*Set the specify function that should be called
	 * when TCP data has been successfully delivered,
	 * when active connection receives data,
	 * or periodically from active connection*/
	tcp_sent(pcb, espconn_server_sent);
	tcp_recv(pcb, espconn_server_recv);
	tcp_poll(pcb, espconn_server_poll, 4); /* every 1 seconds */
	/*Disable Nagle algorithm default*/
	tcp_nagle_disable(pcb);
	/*Default set the total number of espconn_buf on the unsent lists for one*/
	espconn_tcp_set_buf_count(paccept->pespconn, 1);

	if (paccept->pespconn->proto.tcp->connect_callback != NULL) {
		paccept->pespconn->proto.tcp->connect_callback(paccept->pespconn);
	}

	/*Enable keep alive option*/
	if (espconn_keepalive_disabled(paccept))
		espconn_keepalive_enable(pcb);

//	/*lock the window because of application layer don't need the data*/
//	espconn_lock_recv(paccept);
    return ERR_OK;
}

/******************************************************************************
 * FunctionName : espconn_tcp_server
 * Description  : Initialize the server: set up a listening PCB and bind it to
 *                the defined port
 * Parameters   : espconn -- the espconn used to build server
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_tcp_server(struct espconn *espconn)
{
    struct tcp_pcb *pcb = NULL;
    espconn_msg *pserver = NULL;

    /*Creates a new server control message*/
    pserver = (espconn_msg *)os_zalloc(sizeof(espconn_msg));
    if (pserver == NULL){
    	return ESPCONN_MEM;
    }

    /*Creates a new TCP protocol control block*/
    pcb = tcp_new();
    if (pcb == NULL) {
    	/*to prevent memory leaks, ensure that each allocated is deleted*/
    	os_free(pserver);
    	pserver = NULL;
        return ESPCONN_MEM;
    } else {
    	struct tcp_pcb *lpcb = NULL;
    	/*Binds the connection to a local port number and any IP address*/
        tcp_bind(pcb, IP_ADDR_ANY, espconn->proto.tcp->local_port);
        lpcb = pcb;
        /*malloc and set the state of the connection to be LISTEN*/
        pcb = tcp_listen(pcb);
        if (pcb != NULL) {
        	/*insert the node to the active connection list*/
        	espconn_list_creat(&pserver_list, pserver);
        	pserver->preverse = pcb;
        	pserver->pespconn = espconn;
        	pserver->count_opt = MEMP_NUM_TCP_PCB;
			pserver->pcommon.timeout = 0x0a;
            espconn ->state = ESPCONN_LISTEN;
            /*set the specify argument that should be passed callback function*/
            tcp_arg(pcb, (void *)espconn);
            /*accept callback function to call for this control block*/
            tcp_accept(pcb, espconn_tcp_accept);
            return ESPCONN_OK;
        } else {
        	/*to prevent memory leaks, ensure that each allocated is deleted*/
        	memp_free(MEMP_TCP_PCB,lpcb);
        	os_free(pserver);
        	pserver = NULL;
            return ESPCONN_MEM;
        }
    }
}

/******************************************************************************
 * FunctionName : espconn_tcp_delete
 * Description  : delete the server: delete a listening PCB and free it
 * Parameters   : pdeletecon -- the espconn used to delete a server
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_tcp_delete(struct espconn *pdeletecon)
{
	err_t err;
	remot_info *pinfo = NULL;
	espconn_msg *pdelete_msg = NULL;
	struct tcp_pcb *pcb = NULL;

	if (pdeletecon == NULL)
		return ESPCONN_ARG;

	espconn_get_connection_info(pdeletecon, &pinfo , 0);
	/*make sure all the active connection have been disconnect*/
	if (pdeletecon->link_cnt != 0)
		return ESPCONN_INPROGRESS;
	else {
		espconn_printf("espconn_tcp_delete %p\n",pdeletecon);
		pdelete_msg = pserver_list;
		while (pdelete_msg != NULL){
			if (pdelete_msg->pespconn == pdeletecon){
				/*remove the node from the client's active connection list*/
				espconn_list_delete(&pserver_list, pdelete_msg);
				pcb = pdelete_msg->preverse;
				os_printf("espconn_tcp_delete %d, %d\n",pcb->state, pcb->local_port);
				espconn_kill_pcb(pcb->local_port);
				err = tcp_close(pcb);
				os_free(pdelete_msg);
				pdelete_msg = NULL;
				break;
			}
			pdelete_msg = pdelete_msg->pnext;
		}
		if (err == ERR_OK)
			return err;
		else
			return ESPCONN_ARG;
	}
}

/******************************************************************************
 * FunctionName : espconn_init
 * Description  : used to init the function that should be used when
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR espconn_init(void)
{
  ets_task(espconn_Task, espconn_TaskPrio, espconn_TaskQueue, espconn_TaskQueueLen);
}
