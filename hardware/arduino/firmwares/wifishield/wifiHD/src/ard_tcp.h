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

typedef struct ttcp {

	/* options */
	struct ip_addr addr; /* host */
	uint16_t port; /* -p */
	uint16_t nbuf; /* -n */
	int mode; /* -t */
	int verbose; /* -v */
	int udp; /* -u */
	uint8_t sock;
	uint8_t buff_sent;

	/* common */
	uint16_t print_cnt;
	uint32_t start_time;
	uint32_t left;
	uint32_t recved;
	ard_tcp_done_cb_t* done_cb;
	void* opaque;
	uint32_t buflen; /* -l */
	uint32_t tid;

	/* TCP specific */
	struct tcp_pcb* tpcb;
	struct tcp_pcb* lpcb;
	char* payload;

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

uint8_t isDataSent(void* p );

cmd_state_t cmd_ttcp(int argc, char* argv[], void* ctx);

void closeConnections();

#endif
