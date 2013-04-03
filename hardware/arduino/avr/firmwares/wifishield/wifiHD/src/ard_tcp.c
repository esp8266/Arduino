/*
 * ard_tcp.c
 *
 *  Created on: May 27, 2010
 *      Author: mlf by Metodo2 srl
 */

//#define _APP_DEBUG_
#include "lwip/opt.h"

#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/inet_chksum.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"

#include "ard_tcp.h"
#include "ard_spi.h"
#include "timer.h"
#include "util.h"

#include "getopt.h"
#include "ard_utils.h"
#include "debug.h"
#include "trace.h"

unsigned int startTime = 0;
extern bool ifStatus;

static err_t tcp_data_sent(void *arg, struct tcp_pcb *pcb, u16_t len);

static void atcp_init_pend_flags(struct ttcp* _ttcp)
{
	int i = 0;
	for (; i<MAX_CLIENT_ACCEPTED; ++i)
	{
		if (_ttcp) _ttcp->pending_close[i] = false;
	}		
}

/**
 * Clean up and free the ttcp structure
 */
static void ard_tcp_destroy(struct ttcp* ttcp) {
	err_t err = ERR_OK;
	DUMP_TCP_STATE(ttcp);

	uint8_t sock = getSock(ttcp);
	if (sock == -1)
		WARN("ttcp already deallocated!\n");

	freeAllTcpData(sock);
	int i = 0;
	for (; i<MAX_CLIENT_ACCEPTED; ++i)
	{
		if (ttcp->tpcb[i]) {
			tcp_arg(ttcp->tpcb[i], NULL);
			tcp_sent(ttcp->tpcb[i], NULL);
			tcp_recv(ttcp->tpcb[i], NULL);
			tcp_err(ttcp->tpcb[i], NULL);
			//TEMPORAQARY
			//err = tcp_close(ttcp->tpcb);
			INFO_TCP("Closing tpcb: state:0x%x err:%d\n", ttcp->tpcb[i]->state, err);
		}		
	}

	if (ttcp->lpcb) {
		tcp_arg(ttcp->lpcb, NULL);
		tcp_accept(ttcp->lpcb, NULL);
		err = tcp_close(ttcp->lpcb);
		INFO_TCP("Closing lpcb: state:0x%x err:%d\n", ttcp->lpcb->state, err);
	}

	if (ttcp->upcb) {
		udp_disconnect(ttcp->upcb);
		udp_remove(ttcp->upcb);
	}

	FREE_PAYLOAD(ttcp);
	free(ttcp);
}

/**
 * Invoked when transfer is done or aborted (non-zero result).
 */
static void ard_tcp_done(struct ttcp* ttcp, int result) {
//	if (result == 0)
//		ard_tcp_print_stats(ttcp);

	if (ttcp->done_cb)
		ttcp->done_cb(ttcp->opaque, result);

	ard_tcp_destroy(ttcp);
	clearMapSockTcp(getSock(ttcp), GET_TCP_MODE(ttcp));
}

/**
 * Only used in TCP mode. 
 * Will transmit a maximum of pbuf->tot_len bytes. 
 * Called upon connect and when there's space available in the TCP send window
 * 
 */
static err_t tcp_send_data_pcb(struct ttcp *ttcp, struct tcp_pcb *pcb) {
	err_t err = ERR_OK;
	uint32_t len;

	GET_CLIENT_ID(ttcp, pcb);
	
	len = ttcp->left[id];
	ttcp->buff_sent[id] = 0;

	if (len == 0) return ERR_MEM;

	INFO_TCP_VER("left=%d len:%d\n", ttcp->left[id], len);

	/* don't send more than we have in the payload */
	if (len > ttcp->buflen)
		len = ttcp->buflen;

	/* We cannot send more data than space available in the send
	 buffer. */
	if (len > tcp_sndbuf(pcb))
		len = tcp_sndbuf(pcb);

	IF_TCP(startTime = timer_get_ms());
	err = tcp_write(pcb, ttcp->payload[id], len, TCP_WRITE_FLAG_COPY);
	if (err != ERR_OK)
	{
		INFO_TCP("tcp_write failed %p state:%d len:%d err:%d\n", 
				pcb, pcb->state, len, err);
		ttcp->buff_sent[id] = 0;
	}else{
		ttcp->buff_sent[id] = 1;
		ttcp->left[id] -= len;
	}

	return err;
}


/**
 * Only used in TCP mode.
 */
static err_t tcp_connect_cb(void *arg, struct tcp_pcb *tpcb, err_t err) {
	struct ttcp* _ttcp = arg;

	if (_ttcp == NULL) return ERR_ARG;

	GET_CLIENT_ID(_ttcp, tpcb);
	INFO_TCP("TTCP [%p-%p]: connect %d %d\n", _ttcp, tpcb, err, tpcb->state);

	_connected =  ( tpcb->state == ESTABLISHED) ? 1 : 0;
	_ttcp->tcp_poll_retries[id] = 0;

	_ttcp->start_time = timer_get_ms();

	return ERR_OK;
}

static void cleanSockState_cb(void *ctx) {
	struct ttcp* _ttcp = ctx;

	if (_ttcp == NULL) return;

	int sock = getSock(_ttcp);
	if (sock != -1)
		clearMapSockTcp(sock, GET_TCP_MODE(_ttcp));
	INFO_TCP("TTCP [%p]: cleanSockState_cb %d\n", _ttcp, sock);
	_connected = false;
}

/** 
 * Only used in TCP mode.
 */

static err_t close_conn_pcb(struct tcp_pcb* tpcb) {

	err_t err = tcp_close(tpcb);
	if (err== ERR_OK)
	{
		tcp_arg(tpcb, NULL);
		tcp_sent(tpcb, NULL);
		tcp_recv(tpcb, NULL);
	}

	INFO_TCP("Closing tpcb[%p]: state:0x%x err:%d\n", tpcb, tpcb->state, err);
	return err;
}

static void atcp_conn_err_cb(void *arg, err_t err) {
	struct ttcp* _ttcp = arg;

	WARN("TTCP [%p]: connection error: %d currId:%d\n",
			_ttcp, err, getCurrClientConnId());

	if (ifStatus == false)
		printk("Abort connection\n");

	if (err == ERR_ABRT)
	{
		removeNewClientConn(_ttcp, GET_CURR_PCB(_ttcp));
		FREE_PAYLOAD_ID(_ttcp, getCurrClientConnId());
	}
}

static void atcp_conn_cli_err_cb(void *arg, err_t err) {
	struct ttcp* _ttcp = arg;

	if (_ttcp == NULL) return;

	WARN("TTCP [%p]: connection error: %d arg:%p\n",
			_ttcp, err, arg);

	if (ifStatus == false)
		printk("Abort connection\n");

	if ((_ttcp)&&(err == ERR_ABRT))
	{
		WARN("TTCP [%p]: free memory\n", _ttcp);
		cleanSockState_cb(_ttcp);
		// TODO
		FREE_PAYLOAD(_ttcp);
	}

	//atcp_init_pend_flags(_ttcp);
}

static err_t close_conn(struct ttcp *_ttcp, struct tcp_pcb* tpcb) {

	if (_ttcp == NULL) return ERR_MEM;
	
	GET_CLIENT_ID(_ttcp, tpcb);

	err_t err = close_conn_pcb(_ttcp->tpcb[id]);

	if (err == ERR_MEM)
	{
		WARN("Cannot close id:%d-%p put pending\n", id, _ttcp->tpcb[id]);
		_ttcp->pending_close[id] = true;
	}		
	else{
		_ttcp->pending_close[id] = false;
		removeNewClientConn(_ttcp, _ttcp->tpcb[id]);
		FREE_PAYLOAD_ID(_ttcp, id);
		INFO_TCP("----------------------\n");
	}
	return err;
}

void closeConnections()
{
	int ii=0;
	for (; ii<MAX_MODE_NUM; ii++)
	{
		int i = 0;
		for (; i<MAX_SOCK_NUM; i++)
		{
			void* p = getTTCP(i, ii);
			if (p)
			{
				ttcp_t* _ttcp = (ttcp_t* )p;
				if (_ttcp->udp == TCP_MODE)
				{
					ard_tcp_destroy(_ttcp);
					clearMapSockTcp(getSock(_ttcp), GET_TCP_MODE(_ttcp));
				}
			}
		}
	}
}

/**
 * Only used in TCP mode.
 */
static err_t atcp_recv_cb(void *arg, struct tcp_pcb *pcb, struct pbuf *p,
		err_t err) {
	struct ttcp* ttcp = arg;

	if (err == ERR_OK && p != NULL) {
		DATA_LED_ON();
		/* for print_stats() */
		ttcp->recved += p->tot_len;

		if ((ttcp->verbose)||(verboseDebug & INFO_TCP_FLAG)) {
			INFO_TCP("len:%d\n",p->tot_len);
			DUMP_TCP(p->payload, p->tot_len);
			ttcp->print_cnt++;
		}

		uint8_t* pBufferStore = insert_pBuf(p, ttcp->sock, (void*) pcb);
		INFO_TCP("sock:%d pcb:%p pbuf:%p err:%d bufStore:%p len:%d\n",
				ttcp->sock, pcb, p, err, pBufferStore, p->tot_len);
		pbuf_free(p);
		DATA_LED_OFF();
	}

	/* p will be NULL when remote end is done */
	if (err == ERR_OK && p == NULL) {
		INFO_TCP("atcp_recv_cb p=NULL on sock:%d pcb:%p\n",	ttcp->sock, pcb);
		close_conn(ttcp, pcb);
	}

	if (err!=ERR_OK)
		WARN("err=%d p=%p\n", err, p);
	return ERR_OK;
}

void ack_recved(void* pcb, int len) {
	// Comment the call because it is activated on atcp_recv_cb
	INFO_TCP("Received %p len:%d\n", pcb, len);
	tcp_recved(pcb, len);
}

static err_t atcp_poll(void *arg, struct tcp_pcb *pcb) {
	struct ttcp* _ttcp = arg;

	if (_ttcp == NULL) return ERR_ARG;
	
	GET_CLIENT_ID(_ttcp, pcb);

	if (_ttcp->left[id]>0)
		++_ttcp->tcp_poll_retries[id];

	if (_ttcp->tcp_poll_retries[id] > 4) {
		WARN("ARD TCP [%p] arg=%p retries=%d abort\n",
				pcb, arg, _ttcp->tcp_poll_retries[id]);
		_ttcp->tcp_poll_retries[id] = 0;
		tcp_abort(pcb);
		_ttcp->pending_close[id] = false;
	    return ERR_ABRT;
	}
	
	if (pcb)
		INFO_TCP_POLL("keepAliveCnt:%d keep_idle:%d persist_cnt:%d\n", 
						pcb->keep_cnt_sent, pcb->keep_idle, pcb->persist_cnt);
	
	if (_ttcp->left[id] > 0)
		INFO_TCP("ARD TCP [%p-%p] arg=%p retries=%d pend.close:%d len:%d\n",
			(_ttcp)?GET_FIRST_CLIENT_TCP(_ttcp):0, pcb, arg,
			_ttcp->tcp_poll_retries[id], _ttcp->pending_close[id], (_ttcp)?_ttcp->left[id]:0);
	tcp_send_data_pcb(_ttcp, pcb);

	if (_ttcp->pending_close[id])
	{		
		err_t err = ERR_OK;
		if (id >=0){
			err = tcp_close(pcb);
			if (err == ERR_MEM)
			{
				_ttcp->pending_close[id] = true;
			}
			else
			{
				_ttcp->pending_close[id] = false;
				removeNewClientConn(_ttcp, _ttcp->tpcb[id]);
				FREE_PAYLOAD_ID(_ttcp, id);
				INFO_TCP("----------------------\n");
			}
		}	
		INFO_TCP("ARD TCP [%p-%p] try to close pending:%d err:%d id:%d\n", pcb, 
			(_ttcp)?GET_FIRST_CLIENT_TCP(_ttcp):0, _ttcp->pending_close[id], err, id);
	}
	return ERR_OK;
}

static err_t atcp_poll_conn(void *arg, struct tcp_pcb *pcb) {
	struct ttcp* _ttcp = arg;

	if (_ttcp == NULL) return ERR_ARG;

	GET_CLIENT_ID(_ttcp, pcb)
	
	INFO_TCP_POLL("ARD TCP [%p-%p] arg=%p retries=%d pend.close:%d conn:%d\n",
		(_ttcp)?GET_FIRST_CLIENT_TCP(_ttcp):0, pcb, arg,
		_ttcp->tcp_poll_retries[id], _ttcp->pending_close[id], _connected);

	if (id != NO_VALID_ID)
	{
		if (_ttcp->pending_close[id])
			++(_ttcp->tcp_poll_retries[id]);
	}

	if (_ttcp->tcp_poll_retries[id] > 8) {
		WARN("ARD TCP [%p-%p] arg=%p retries=%d\n",
				pcb, GET_FIRST_CLIENT_TCP(_ttcp), arg, _ttcp->tcp_poll_retries[id]);
		_ttcp->tcp_poll_retries[id] = 0;
		tcp_abort(pcb);
		return ERR_ABRT;
	}

	if ((_ttcp)&&(_connected)) tcp_send_data_pcb(_ttcp, pcb);

	if ((id != NO_VALID_ID) && (_ttcp->pending_close[id]))
	{
		err_t err = tcp_close(pcb);
		if (err == ERR_MEM)
		{
			_ttcp->pending_close[id] = true;
		}
		else
		{
			cleanSockState_cb(_ttcp);
			FREE_PAYLOAD_ID(_ttcp, id);
			_ttcp->pending_close[id] = false;
		}

		INFO_TCP("ARD TCP [%p-%p] try to close pending:%d\n", pcb, (_ttcp)?GET_FIRST_CLIENT_TCP(_ttcp):0, _ttcp->pending_close[id]);
	}
	return ERR_OK;
}

int8_t currConnId = 0;

int8_t getCurrClientConnId() { return currConnId;}

int8_t getNewClientConnId(struct ttcp* _ttcp, struct tcp_pcb *newpcb)
{
	if (_ttcp != NULL){
		int i = 0;
		for (; i<MAX_CLIENT_ACCEPTED; ++i)
		{
			int idx = GET_IDX_CONN(i);

			if (_ttcp->tpcb[idx] == newpcb)
			{
				INFO_TCP_VER("ttcp:%p id=%d, tpcb=%p\n", _ttcp, idx, newpcb);
				return idx;
			}				
		}
	}
	WARN("No Valid Id for ttcp:%p pcb:%p\n",  _ttcp, newpcb);
	return NO_VALID_ID;
}

struct tcp_pcb * getFirstClient(struct ttcp* _ttcp, bool verbose)
{
	if (_ttcp != NULL){
		int i = 0;
		for (; i<MAX_CLIENT_ACCEPTED; ++i)
		{
			int idx = GET_IDX_CONN(i);
			if (_ttcp->tpcb[idx] != NULL)
			{
				if (verbose) INFO_TCP("ttcp:%p id=%d, tpcb=%p\n", _ttcp, idx, _ttcp->tpcb[idx]);
				currConnId = idx;
				return _ttcp->tpcb[idx];
			}				
		}
	}
	if (verbose) WARN("No Valid client for ttcp:%p\n",  _ttcp);
	return NULL;
}


int8_t setNewClientConn(struct ttcp* _ttcp, struct tcp_pcb *newpcb, uint8_t id)
{
	if ((_ttcp != NULL)&&(id>=0)&&(id<MAX_CLIENT_ACCEPTED)){
		INFO_TCP("ttcp:%p id=%d, tpcb=%p\n", _ttcp, id, newpcb);
		_ttcp->tpcb[id] = newpcb;
		return id;
	}
	return NO_VALID_ID;
}

int8_t insertNewClientConn(struct ttcp* _ttcp, struct tcp_pcb *newpcb)
{
	if (_ttcp != NULL){
		int i = 0;
		for (; i<MAX_CLIENT_ACCEPTED; ++i)
		{
			int idx = GET_IDX_CONN(i);
			if ((_ttcp->tpcb[idx] == NULL)||(_ttcp->tpcb[idx] == newpcb))
			{
				INFO_TCP("ttcp:%p id=%d, tpcb=%p\n", _ttcp, idx, newpcb);
				_ttcp->tpcb[idx] = newpcb;
				return idx;
			}
		}
	}
	return NO_VALID_ID;
}

int8_t removeNewClientConn(struct ttcp* _ttcp, struct tcp_pcb *newpcb)
{
	if (_ttcp != NULL){
		int i = 0;
		for (; i<MAX_CLIENT_ACCEPTED; ++i)
		{
			int idx = GET_IDX_CONN(i);
			if (_ttcp->tpcb[idx] == newpcb)
			{
				INFO_TCP("ttcp:%p id=%d, tpcb=%p\n", _ttcp, idx, newpcb);
				_ttcp->tpcb[idx] = NULL;
				return idx;
			}
		}
	}
	return NO_VALID_ID;
}

bool cleanNewClientConn(struct ttcp* _ttcp)
{
	if (_ttcp != NULL){
		int i = 0;
		for (; i<MAX_CLIENT_ACCEPTED; ++i)
			_ttcp->tpcb[i] = NULL;
		return true;
	}
	return false;
}


/**
 * Only used in TCP mode.
 */
static err_t atcp_accept_cb(void *arg, struct tcp_pcb *newpcb, err_t err) {
	struct ttcp* _ttcp = arg;

	if (_ttcp == NULL) return ERR_ARG;

	INFO_TCP("ARD TCP [%p]: accept new [%p]\n", _ttcp, newpcb);
	INFO_TCP("local:%d remote:%d state:%d\n", newpcb->local_port, newpcb->remote_port, newpcb->state);

	int8_t id = insertNewClientConn(_ttcp, newpcb);

	ASSERT((_ttcp->payload[id]==NULL), "payload not freed!");
	_ttcp->payload[id] = malloc(_ttcp->buflen);
	INFO_TCP("Alloc payload %d-%p\n", id, _ttcp->payload[id]);
	if (_ttcp->payload[id] == NULL) {
		WARN("TTCP [%p]: could not allocate payload\n", _ttcp);
		return -1;
	}
	tcp_arg(_ttcp->tpcb[id], _ttcp);
	tcp_recv(_ttcp->tpcb[id], atcp_recv_cb);
	tcp_err(_ttcp->tpcb[id], atcp_conn_err_cb);
	tcp_poll(_ttcp->tpcb[id], atcp_poll, 4);
	// Copy the pointer to ttcp also to TRANSMIT mode for the clients connected to the server
	int _sock = getSock(_ttcp);
	if ((_sock != -1)&&(IS_VALID_SOCK(_sock)))
		setMapSockMode(_sock, _ttcp, TTCP_MODE_TRANSMIT);
	_ttcp->start_time = timer_get_ms();
	return ERR_OK;
}

/**
 * Start TCP transfer.
 */
static int atcp_start(struct ttcp* ttcp) {
	err_t err = ERR_OK;

	struct tcp_pcb * p = tcp_new();
	
	if (p == NULL) {
		WARN("TTCP [%p]: could not allocate pcb\n", ttcp);
		return -1;
	}

	currConnId = 0;
	tcp_arg(p, ttcp);
	atcp_init_pend_flags(ttcp);

	if (ttcp->mode == TTCP_MODE_TRANSMIT) {
		int8_t id = insertNewClientConn(ttcp, p);
		ttcp->payload[id] = malloc(ttcp->buflen);
		INFO_TCP("Alloc payload %d-%p\n", id, ttcp->payload[id]);
		if (ttcp->payload[id] == NULL) {
			WARN("TTCP [%p]: could not allocate payload\n", ttcp);
			return -1;
		}
		
		struct tcp_pcb * pcb = p;
		tcp_err(pcb, atcp_conn_cli_err_cb);
		tcp_recv(pcb, atcp_recv_cb);
		tcp_sent(pcb, tcp_data_sent);
		tcp_poll(pcb, atcp_poll_conn, 4);
		_connected = false;
		INFO_TCP("[tpcb]-%p payload:%p\n", pcb, ttcp->payload[id]);
		DUMP_TCP_STATE(ttcp);
		if (tcp_connect(pcb, &ttcp->addr, ttcp->port, tcp_connect_cb)
				!= ERR_OK) {
			WARN("TTCP [%p]: tcp connect failed\n", ttcp);
			return -1;
		}

	} else {
		INFO_TCP("BEFORE BIND ttcp:%p lpcb:%p pcb:%p\n", ttcp, ttcp->lpcb, GET_FIRST_CLIENT_TCP(ttcp));

		err = tcp_bind(p, IP_ADDR_ANY, ttcp->port);
		if (err != ERR_OK){
			WARN("TTCP [%p]: bind failed err=%d Port already used\n", ttcp, err);
			return -1;
		}

		ttcp->lpcb = tcp_listen(p);
		if (ttcp->lpcb == NULL) {
			WARN("TTCP [%p]: listen failed\n", ttcp);
			return -1;
		}

		DUMP_TCP_STATE(ttcp);
		tcp_accept(ttcp->lpcb, atcp_accept_cb);
	}

	return 0;
}

/**
 * Only used in UDP mode. Will finalize the ttcp process when an end marker
 * is seen.
 */
static void audp_recv_cb(void *arg, struct udp_pcb *upcb, struct pbuf *p,
		struct ip_addr *addr, u16_t port) {
	struct ttcp* ttcp = arg;

	/* for print_stats() */
	ttcp->recved += p->tot_len;
	DUMP(p->payload,p->tot_len);
	if (ttcp->verbose) {
		printk(".");
		if (ttcp->print_cnt % 80 == 0)
			printk("\n");
		ttcp->print_cnt++;
	}
	INFO_TCP("UDP Insert %p sock:%d addr:%s port:%d\n", p, ttcp->sock,
			ip2str(*addr), port);
	insert_pBuf(p, ttcp->sock, (void*) upcb);
	setRemoteClient(ttcp->sock, addr->addr, port);

	pbuf_free(p);
}

/**
 * Start UDP transfer.
 */
static int udp_start(struct ttcp* ttcp) {
	err_t err = ERR_OK;
	ttcp->udp_end_marker_left = 5;
	ttcp->upcb = udp_new();
	if (ttcp->upcb == NULL) {
		WARN("TTCP [%p]: could not allocate pcb\n", ttcp);
		return -1;
	}

	INFO_TCP("%s, upcb:%p %s:%d\n", __FUNCTION__, ttcp->upcb, ip2str(ttcp->addr), ttcp->port);
	if (ttcp->mode == TTCP_MODE_TRANSMIT) {
		if (udp_connect(ttcp->upcb, &(ttcp->addr), ttcp->port) != ERR_OK) {
			WARN("TTCP [%p]: udp connect failed\n", ttcp);
			return -1;
		}
		udp_recv(ttcp->upcb, audp_recv_cb, ttcp);
	} else {
		/* bind to any IP address on port specified */
		err = udp_bind(ttcp->upcb, IP_ADDR_ANY, ttcp->port);
		if  (err!= ERR_OK) {
			WARN("TTCP [%p]: bind failed err=%d Port already used\n", ttcp, err);
		    return -1;
		}
		// clear remote client data
		setRemoteClient(ttcp->sock, 0, 0);
		udp_recv(ttcp->upcb, audp_recv_cb, ttcp);
	}
	INFO_TCP("%s, loc:0x%x-%d rem:0x%x-%d\n", __FUNCTION__, 
		 ttcp->upcb->local_ip.addr, ttcp->upcb->local_port, 
		 ttcp->upcb->remote_ip.addr, ttcp->upcb->remote_port);
	return 0;
}

/**
 * Start a new ttcp transfer. It should be possible to call this function
 * multiple times in order to get multiple ttcp streams. done_cb() will be
 * invoked upon completion.
 * 
 */
int ard_tcp_start(struct ip_addr addr, uint16_t port, void *opaque,
		ard_tcp_done_cb_t *done_cb, int mode, uint16_t nbuf, uint16_t buflen,
		int udp, int verbose, uint8_t sock, void** _ttcp) {
	struct ttcp* ttcp;
	int status;

	if (mode != TTCP_MODE_TRANSMIT && mode != TTCP_MODE_RECEIVE) {
		WARN("TTCP [-]: invalid mode\n");
		return -1;
	}

	if (nbuf == 0) {
		WARN("TTCP [-]: invalid nbuf\n");
		return -1;
	}

	if (buflen == 0) {
		WARN("TTCP [-]: invalid buflen\n");
		return -1;
	}

	ttcp = calloc(1, sizeof(struct ttcp));
	if (ttcp == NULL) {
		WARN("TTCP [-]: could not allocate memory for ttcp\n");
		return -1;
	}

	ttcp->addr = addr;
	ttcp->port = port;
	ttcp->nbuf = nbuf;
	ttcp->mode = mode;
	ttcp->done_cb = done_cb;
	ttcp->opaque = opaque;
	ttcp->udp = udp;
	ttcp->verbose = verbose;
	ttcp->buflen = buflen;
	cleanNewClientConn(ttcp);

	if (ttcp->udp)
		status = udp_start(ttcp);
	else
		status = atcp_start(ttcp);

	if (status) {
		WARN("Start server FAILED!\n");
		goto fail;
	}
	INFO_TCP("TTCP [%p-%p]: nbuf=%d, buflen=%d, port=%d (%s/%s)\n", ttcp,
			((ttcp->udp==1)?(void*)ttcp->upcb:GET_FIRST_CLIENT_TCP(ttcp)), ttcp->nbuf, ttcp->buflen, 
			ttcp->port, ProtMode2Str(ttcp->udp), Mode2Str(ttcp->mode));

	*_ttcp = (void*) ttcp;
	ttcp->sock = sock;

	return 0;

	fail: ard_tcp_destroy(ttcp);
	return -1;
}

void ard_tcp_stop(void* ttcp) {
	struct ttcp* _ttcp = (struct ttcp*) ttcp;
	if (_ttcp == NULL)
	{
		WARN("ttcp = NULL!\n");
		return;
	}
	if (_ttcp->mode == TTCP_MODE_TRANSMIT) {
		int i = getCurrClientConnId();
		ard_tcp_destroy(_ttcp);
    	clearMapSockTcp(getSock(_ttcp), GET_TCP_MODE(_ttcp));
    	_ttcp->tcp_poll_retries[i] = 0;
	}else{
		DUMP_TCP_STATE(_ttcp);

		int i = getCurrClientConnId();
		if ((_ttcp)&&(_ttcp->tpcb[i])&&(_ttcp->tpcb[i]->state!=LAST_ACK)&&(_ttcp->tpcb[i]->state!=CLOSED))
		{
			// Flush all the data
			err_t err=tcp_output(_ttcp->tpcb[i]);
			INFO_TCP("flush data: tpcb:%p err:%d\n", _ttcp->tpcb[i], err);
			// if any socket  cannot be close stop the close connection
			close_conn(_ttcp, _ttcp->tpcb[i]);
		}
	}
}

uint8_t getStateTcp(void* p, bool client) {
	struct ttcp* _ttcp = (struct ttcp*) p;

	if (ifStatus == false)
		return CLOSED;
	struct tcp_pcb * pcb = GET_FIRST_CLIENT_TCP_NV(_ttcp);
	if ((_ttcp != NULL) && ((pcb != NULL) || (client==0))) {
		IF_SPI_POLL(DUMP_TCP_STATE(_ttcp));
		if (client)
		{
			if ((pcb->state != ESTABLISHED)&&(pcb->state != CLOSED))
				DUMP_TCP_STATE(_ttcp);
			return pcb->state;
		}			
		else
		{
			return _ttcp->lpcb->state;
		}			
	} else {
		WARN_POLL("TCP not initialized ttcp:%p tpcb:%p lpcb:%p\n",
				_ttcp, ((_ttcp)?pcb:0), ((_ttcp)?_ttcp->lpcb:0));
	}
	return CLOSED;
}

uint8_t getModeTcp(void* p) {
	struct ttcp* _ttcp = (struct ttcp*) p;

	if (_ttcp != NULL)
		return _ttcp->mode;
	return 0;
}

uint8_t isDataSent(void* p) {
	struct ttcp *_ttcp = (struct ttcp *)p;

	int8_t id = getCurrClientConnId();
	if ((_ttcp)&&(!_ttcp->buff_sent[id]))
	{
		return 0;
	}

	return 1;
}

static err_t tcp_data_sent(void *arg, struct tcp_pcb *pcb, u16_t len) {
	struct ttcp *_ttcp;

	LWIP_UNUSED_ARG(len);

	_ttcp = arg;

	if (_ttcp == NULL) return ERR_ARG;

	GET_CLIENT_ID(_ttcp, pcb);
	_ttcp->tcp_poll_retries[id] = 0;
	_ttcp->buff_sent[id] = 1;

	INFO_TCP("Packet sent pcb:%p len:%d dur:%d left:%d\n", pcb, len, timer_get_ms() - startTime,
			(_ttcp)?(_ttcp->left[id]):0);

	if ((_ttcp)&&(_ttcp->left[id] > 0)) {
		tcp_send_data_pcb(_ttcp, pcb);
	}

	return ERR_OK;
}

int sendTcpData(void* p, uint8_t* buf, uint16_t len) 
{
	struct ttcp* _ttcp = (struct ttcp*) p;

	if (_ttcp==NULL)
	{
		WARN("ttcp == NULL!\n");
		return WL_FAILURE;
	}
	
	struct tcp_pcb * pcb = GET_FIRST_CLIENT_TCP_NV(_ttcp);
	GET_CLIENT_ID(_ttcp, pcb);
	
	INFO_TCP_VER("ttcp:%p pcb:%p buf:%p len:%d\n", _ttcp, pcb, buf, len);
	DUMP_TCP(buf,len);
	IF_TCP_VER(DUMP_TCP_STATE(_ttcp));

	if ((_ttcp != NULL) && (pcb != NULL) &&
			(buf != NULL) && (len != 0) && (_ttcp->payload[id] != NULL)) {
		if (pcb->state == ESTABLISHED || pcb->state == CLOSE_WAIT ||
			pcb->state == SYN_SENT || pcb->state == SYN_RCVD) {

		memcpy(_ttcp->payload[id], buf, len);
		_ttcp->payload[id][len]='\0';
		INFO_TCP_VER("'%s'\n", _ttcp->payload[id]);
		_ttcp->left[id] = len;
		tcp_sent(pcb, tcp_data_sent);
		tcp_send_data_pcb(_ttcp, pcb);

		return WL_SUCCESS;
		}
	}
	//printk("Write failure _ttcp=%p _ttcp->tpcb=%p buf=%p len=%d\n", _ttcp, _ttcp->tpcb, buf, len);
	return WL_FAILURE;
}

int sendUdpData(void* ttcp, uint8_t* buf, uint16_t len) {
	struct ttcp* _ttcp = (struct ttcp*) ttcp;
	if ((_ttcp != NULL) && (buf != NULL) && (len != 0))
	{
		INFO_TCP("buf:%p len:%d\n", buf, len);
		DUMP_TCP(buf,len);
	}else{
		return WL_FAILURE;
	}

	struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
	if (p == NULL) {
		WARN("TTCP [%p]: could not allocate pbuf\n", ttcp);
		return WL_FAILURE;
	}
	memcpy(p->payload, buf, len);
	if (udp_send(_ttcp->upcb, p) != ERR_OK) {
		WARN("TTCP [%p]: udp_send() failed\n", _ttcp);
		pbuf_free(p);
		return WL_FAILURE;
	}

	pbuf_free(p);
	return WL_SUCCESS;
}



char
		usage[] =
				"Usage: ttcp -t/-r [-options] host\n\
        -l      length of bufs written to network (default 1024)\n\
        -n      number of bufs written to network (default 1024)\n\
        -p      port number to send to (default 2000)\n\
        -u      udp\n\
        -v      verbose\n";

/**
 *
 */
cmd_state_t cmd_ttcp(int argc, char* argv[], void* ctx) {

	int c;
	int mode = TTCP_MODE_TRANSMIT;
	int verbose = 0;
	uint16_t buflen = 1024;
	uint16_t nbuf = 1024;
	uint16_t port = 2000;
	int udp = 0;
	struct ip_addr addr = { 0 };

	optind = 1;
	while ((c = getopt(argc, argv, "utrl:n:p:v")) != -1) {
		switch (c) {
		case 't':
			mode = TTCP_MODE_TRANSMIT;
			break;
		case 'r':
			mode = TTCP_MODE_RECEIVE;
			break;
		case 'l':
			buflen = atoi(optarg);
			break;
		case 'v':
			verbose = 1;
			break;
		case 'n':
			nbuf = atoi(optarg);
			break;
		case 'u':
			udp = 1;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		}
	}

	if (mode == TTCP_MODE_TRANSMIT) {
		if (optind >= argc) {
			printk("%s", usage);
			return CMD_DONE;
		}

		addr = str2ip(argv[optind]);
		if (!addr.addr) {
			printk("%s", usage);
			return CMD_DONE;
		}
	}
	void* _ttcp = NULL;
	if (ard_tcp_start(addr, port, NULL, NULL, mode, nbuf, buflen, udp, verbose,
			0, &_ttcp))
		return CMD_DONE;

	return CMD_DONE;
}


#if 0
#include "lwip/sockets.h"

void testlwip()
{
	int Sock;
	fd_set fdsetR;
	FD_ZERO(&fdsetR);
	FD_SET(Sock, &fdsetR);
	fd_set fdsetE = fdsetR;

	int rc;
	const int cMillies = 10000;
	struct timeval timeout;
	timeout.tv_sec = cMillies / 1000;
	timeout.tv_usec = (cMillies % 1000) * 1000;
	//rc = lwip_select(Sock + 1, &fdsetR, NULL, &fdsetE, &timeout);
}
#endif
