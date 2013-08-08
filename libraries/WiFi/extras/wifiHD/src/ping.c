/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is derived from a part of the lwIP TCP/IP stack.
 * 
 */
#ifdef PING_CMD
#include "lwip/opt.h"

#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/inet_chksum.h"

#include "ping.h"
#include "timer.h"
#include "util.h"

#include "getopt.h"

#define PING_ID 0xAFAF

struct ping_info_t {
        struct ip_addr destination;
        uint32_t deadline;      /* -w (in seconds) */
        uint32_t interval;      /* -i (in ms) */
        uint32_t timeout;       /* ms */
        uint32_t data_size;     /* -s */
        uint32_t count;         /* -c, 0 means continous ping */
        uint32_t size;
        uint32_t first_tx_tm;
        uint32_t last_tx_tm;
        uint32_t last_rx_tm;
        uint32_t num_tx;
        uint32_t num_rx;
        uint32_t flags;
        uint16_t seq_num;
        Bool quiet;             /* -q */
        ping_complete_cb_t complete_cb;
        void *ctx;
#define PING_REPLY (1 << 0)
};

static struct ping_info_t INFO;

/** Prepare a echo ICMP request */
static void ping_prepare_echo(struct icmp_echo_hdr *iecho,
                              struct ping_info_t* ping_info)
{
        int i;

        ICMPH_TYPE_SET(iecho,ICMP_ECHO);
        ICMPH_CODE_SET(iecho, 0);
        iecho->chksum = 0;
        iecho->id     = PING_ID;
        iecho->seqno  = htons(++ping_info->seq_num);
        iecho->chksum = 0;

        /* fill the additional data buffer with some data */
        for(i = 0; i < ping_info->data_size; i++) {
                ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = i;
        }

        iecho->chksum = inet_chksum(iecho, ping_info->size);
}

/* Ping using the raw ip */
static u8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p,
                      struct ip_addr *addr)
{
        struct icmp_echo_hdr *iecho;
        struct ip_hdr *ip = p->payload;
        struct ping_info_t* ping_info = (struct ping_info_t*) arg;
        uint32_t us;

        if (pbuf_header( p, -PBUF_IP_HLEN)==0) {
                iecho = p->payload;

                if ((iecho->id == PING_ID) &&
                    (iecho->seqno == htons(ping_info->seq_num))) {
                        ping_info->last_rx_tm = timer_get_ms();
                        ping_info->num_rx++;
                        us = 1000 *
                                (ping_info->last_rx_tm - ping_info->last_tx_tm);

                        if (!ping_info->quiet)
                                printk("%d bytes from %s: icmp_seq=%d ttl=%d " \
                                       "time=%d.%03d ms\n",
                                       p->tot_len, ip2str(ip->src),
                                       iecho->seqno,
                                       IPH_TTL(ip),
                                       us / 1000, us % 1000);
                        
                        /* do some ping result processing */
                        ping_info->flags |= PING_REPLY;
                }
        }

        pbuf_free(p);
        return 1; /* eat the event */
}

static void ping_send(struct raw_pcb *raw, struct ping_info_t* ping_info)
{
        struct pbuf *p;
        struct icmp_echo_hdr *iecho;

        if (!(p = pbuf_alloc(PBUF_IP, ping_info->size, PBUF_RAM))) {
                return;
        }
        if ((p->len == p->tot_len) && (p->next == NULL)) {
                iecho = p->payload;

                ping_prepare_echo(iecho, ping_info);
                raw_sendto(raw, p, &ping_info->destination);

                if (!ping_info->first_tx_tm)
                        ping_info->first_tx_tm = timer_get_ms();
                ping_info->last_tx_tm = timer_get_ms();
                ping_info->num_tx++;
        }
        pbuf_free(p);
}

void ping_set_callback(ping_complete_cb_t cb, void *ctx) {
        INFO.complete_cb = cb;
        INFO.ctx = ctx;
}

void ping_stop(uint32_t *tx_cnt, uint32_t *rx_cnt) {
        struct ping_info_t *ping_info = &INFO;

        *tx_cnt = ping_info->num_tx;
        *rx_cnt = ping_info->num_rx;
        ping_info->count = ping_info->num_tx;
        if ( 0 == ping_info->count ) {
                ping_info->count = 1;
        }
}

static int init_ping_info(int argc, char* argv[], struct ping_info_t* ping_info)
{
        int c;
        ping_complete_cb_t cb;
        void *ctx;

        cb = ping_info->complete_cb;
        ctx = ping_info->ctx;
        memset(ping_info, 0, sizeof(struct ping_info_t));
        ping_info->complete_cb = cb;
        ping_info->ctx = ctx;

        ping_info->deadline = 0;
        ping_info->interval = 1000;
        ping_info->timeout = 3000;
        ping_info->data_size = 32;
        ping_info->count = 3;
        ping_info->destination =
                netif_default ? netif_default->gw : ip_addr_any;

        optind = 1;
        while ((c = getopt(argc, argv, "c:i:s:w:q")) != -1) {
                switch (c) {
                case 'c':
                        ping_info->count = atoi(optarg);
                        break;
                        
                case 'i':
                        ping_info->interval = atoi(optarg);
                        break;
                        
                case 's':
                        ping_info->data_size = atoi(optarg);
                        break;

                case 'q':
                        ping_info->quiet = TRUE;
                        break;

                case 'w':
                        ping_info->deadline = atoi(optarg);
                        break;
                }
        }

        ping_info->size = sizeof(struct icmp_echo_hdr) + ping_info->data_size;

        if (optind >= argc)
                return -1;

        ping_info->destination = str2ip(argv[optind]);
        if (!ping_info->destination.addr)
                return -1;
                

        ping_info->last_rx_tm = timer_get_ms();

        return 0;
}

static void print_stats(struct ping_info_t* ping_info)
{
        printk("\n--- %s ping statistics ---\n",
               ip2str(ping_info->destination));
        printk("%d packets transmitted, %d received, %d%% packet loss, "\
               "time %dms\n\n",
               ping_info->num_tx, ping_info->num_rx,
               100 * (ping_info->num_tx - ping_info->num_rx) /
               ping_info->num_tx,
               timer_get_ms() - ping_info->first_tx_tm);
}

static void ping_finalize(struct ping_info_t* ping_info) {
        print_stats(ping_info);
        if (ping_info->complete_cb) {
                ping_info->complete_cb(ping_info->num_tx, ping_info->num_rx, ping_info->ctx);
        }
}

cmd_state_t cmd_ping(int argc, char* argv[], void* ctx)
{
        static enum {
                INIT,
                PING,
                WAIT_REPLY
        } state = INIT;

        struct ping_info_t *ping_info = &INFO;
        static struct raw_pcb *pcb;
        
        switch (state) {
        case INIT:
                if (init_ping_info(argc, argv, ping_info) != 0) {
                        printk("Usage: ping [-c count] [-i interval] "  \
                               "[-s packetsize]\n            " \
                               "[-w deadline] [-q] destination\n");
                        return CMD_DONE;
                }

                if (!(pcb = raw_new(IP_PROTO_ICMP))) {
                        printk("could not allocate pcb\n");
                        state = INIT;
                        return CMD_DONE;
                }
                raw_recv(pcb, ping_recv, ping_info);
                raw_bind(pcb, IP_ADDR_ANY);

                printk("PING %s %d(%d) bytes of data\n",
                       ip2str(ping_info->destination),
                       ping_info->data_size,
                       ping_info->size);
                state = PING;
                /* fall through */
                
        case PING:
                if (!netif_is_up(netif_default)) {
                        printk("netif is down\n");
                        raw_remove(pcb);
                        state = INIT;
                        return CMD_DONE;
                }
                
                if (ping_info->count && ping_info->num_tx == ping_info->count) {
                        ping_finalize(ping_info);
                        raw_remove(pcb);
                        state = INIT;
                        return CMD_DONE;
                }

                
                if (timer_get_ms() < ping_info->last_rx_tm + ping_info->interval) {
                        return CMD_INPROGRESS;
                }
                ping_send(pcb, ping_info);
                        
                state = WAIT_REPLY;
                return CMD_INPROGRESS;
                
        case WAIT_REPLY:
                if (ping_info->flags & PING_REPLY) {
                        ping_info->flags &= (~PING_REPLY);
                        state = PING;
                        return CMD_INPROGRESS;
                }

                if (timer_get_ms() >
                    ping_info->last_tx_tm + ping_info->timeout) {
                        if (!ping_info->quiet)
                                printk("timeout from %s\n",
                                       ip2str(ping_info->destination));
                        state = PING;
                        return CMD_INPROGRESS;
                }

                if (ping_info->deadline &&
                    timer_get_ms() >
                    ping_info->first_tx_tm + ping_info->deadline * 1000) {
                        ping_finalize(ping_info);
                        raw_remove(pcb);
                        state = INIT;
                        return CMD_DONE;
                }

                return CMD_INPROGRESS;
        }

        /* unreachable */
        Assert(0);
	return CMD_DONE;
}
#endif
