/*
 * ard_tcp.h
 *
 *  Created on: May 27, 2010
 *      Author: mlf by Metodo2 srl
 */

#ifndef ARD_TCP_H
#define ARD_TCP_H

#include "console.h"

typedef void (ard_tcp_done_cb_t)(void *opaque, int result);

int ard_tcp_start(struct ip_addr addr, uint16_t port, void *opaque,
           ard_tcp_done_cb_t *done_cb, int mode, uint16_t nbuf, uint16_t buflen, int udp, int verbose, uint8_t sock, void** _ttcp);

void ard_tcp_stop(void* ttcp);

uint8_t getStateTcp(void* p, bool client );

int sendTcpData(void* p, uint8_t* buf, uint16_t len);

uint8_t isDataSent(void* p );

cmd_state_t cmd_ttcp(int argc, char* argv[], void* ctx);

#endif
