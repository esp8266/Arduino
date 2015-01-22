/*
 * ard_tcp.h
 *
 *  Created on: May 27, 2010
 *      Author: mlf by Metodo2 srl
 */

#ifndef ARD_TCP_H
#define ARD_TCP_H

#include "console.h"
#include "lwip/tcp.h"

typedef void (ard_tcp_done_cb_t)(void *opaque, int result);

#define TTCP_MODE_TRANSMIT          0
#define TTCP_MODE_RECEIVE           1
#define MAX_MODE_NUM 				2

#define GET_TCP_MODE(X)	((X!=NULL)?((struct ttcp*)(X))->mode:0)
#define IS_VALID_SOCK(SOCK) ((SOCK>=0)&&(SOCK<MAX_SOCK_NUM))
#define IS_UDP_SOCK(SOCK)	((getTTCP(SOCK, TTCP_MODE_RECEIVE)!=NULL)?((struct ttcp*)(getTTCP(SOCK, TTCP_MODE_RECEIVE)))->udp:0)

// Maximum number of client connection accepted by server
#define MAX_CLIENT_ACCEPTED			4
#define NO_VALID_ID					0xff

#define GET_FIRST_CLIENT_TCP(TTCP)		getFirstClient(TTCP, 1)
#define GET_FIRST_CLIENT_TCP_NV(TTCP)	getFirstClient(TTCP, 0)
#define GET_CLIENT_TCP(TTCP,ID)	(((TTCP!=NULL)&&(ID>=0)&&(ID<MAX_CLIENT_ACCEPTED))?TTCP->tpcb[ID] : NULL)
#define GET_CLIENT_ID(TTCP, PCB)	uint8_t id = NO_VALID_ID; do {		\
		id = getNewClientConnId(TTCP, PCB);	\
		if (id == NO_VALID_ID) return ERR_MEM;		\
	}while(0);	
#define GET_IDX_CONN(I) ((I+currConnId)<MAX_CLIENT_ACCEPTED ? (I+currConnId) : (I+currConnId-MAX_CLIENT_ACCEPTED))
#define GET_CURR_PCB(TTCP) GET_CLIENT_TCP(TTCP,getCurrClientConnId())

#define FREE_PAYLOAD(TTCP) do { \
	int id = getCurrClientConnId(); \
	INFO_TCP("Freeing payload %d-%p\n", id, TTCP->payload[id]); \
	if (TTCP->payload[id]) { \
		free(TTCP->payload[id]); \
		TTCP->payload[id] = NULL; } \
}while(0);		

#define FREE_PAYLOAD_ID(TTCP,ID) do { \
	INFO_TCP("Freeing payload %d-%p\n", ID, TTCP->payload[ID]); \
	if (TTCP->payload[ID]) { \
		free(TTCP->payload[ID]); \
		TTCP->payload[ID] = NULL; } \
}while(0);	


typedef struct ttcp {

	/* options */
	struct ip_addr addr; /* host */
	uint16_t port; /* -p */
	uint16_t nbuf; /* -n */
	int mode; /* -t */
	int verbose; /* -v */
	int udp; /* -u */
	uint8_t sock;
	uint8_t buff_sent[MAX_CLIENT_ACCEPTED];

	/* common */
	uint16_t print_cnt;
	uint32_t start_time;
	uint32_t left[MAX_CLIENT_ACCEPTED];
	uint32_t recved;
	ard_tcp_done_cb_t* done_cb;
	void* opaque;
	uint32_t buflen; /* -l */
	uint32_t tid;

	/* TCP specific */
	struct tcp_pcb* tpcb[MAX_CLIENT_ACCEPTED];
	struct tcp_pcb* lpcb;
	char* payload[MAX_CLIENT_ACCEPTED];
	uint8_t tcp_poll_retries[MAX_CLIENT_ACCEPTED];
	bool pending_close[MAX_CLIENT_ACCEPTED];

	/* UDP specific */
	int udp_started;
	uint16_t udp_end_marker_left;
	struct udp_pcb* upcb;
}ttcp_t;

bool _connected;

int ard_tcp_start(struct ip_addr addr, uint16_t port, void *opaque,
           ard_tcp_done_cb_t *done_cb, int mode, uint16_t nbuf, uint16_t buflen, int udp, int verbose, uint8_t sock, void** _ttcp);

void ard_tcp_stop(void* ttcp);

uint8_t getStateTcp(void* p, bool client );

uint8_t getModeTcp(void* p);

int sendTcpData(void* p, uint8_t* buf, uint16_t len);

int sendUdpData(void* p, uint8_t* buf, uint16_t len);

uint8_t isDataSent(void* p );

cmd_state_t cmd_ttcp(int argc, char* argv[], void* ctx);

int8_t setNewClientConn(struct ttcp* _ttcp, struct tcp_pcb *newpcb, uint8_t id);

int8_t insertNewClientConn(struct ttcp* _ttcp, struct tcp_pcb *newpcb);

int8_t removeNewClientConn(struct ttcp* _ttcp, struct tcp_pcb *newpcb);

bool cleanNewClientConn(struct ttcp* _ttcp);

int8_t getNewClientConnId(struct ttcp* _ttcp, struct tcp_pcb *newpcb);

int8_t getCurrClientConnId();

struct tcp_pcb * getFirstClient(struct ttcp* _ttcp, bool verbose);

void closeConnections();

#endif
