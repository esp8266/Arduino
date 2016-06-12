/**
 * @file
 * MetIO Server
 *
 */

/*
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
 * This file is part of the lwIP TCP/IP stack.
 *
 */
#include "lwip/opt.h"

#if LWIP_TCP
#include "lwip/tcp.h"

#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR = __FILE__;
#endif

/*
 * This implements a netio server.
 *  The client sends a command word (4 bytes) then a data length word (4 bytes).
 *  If the command is "receive", the server is to consume "data length" bytes into
 *   a circular buffer until the first byte is non-zero, then it is to consume
 *   another command/data pair.
 *  If the command is "send", the server is to send "data length" bytes from a circular
 *   buffer with the first byte being zero, until "some time" (6 seconds in the
 *   current netio126.zip download) has passed and then send one final buffer with
 *   the first byte being non-zero. Then it is to consume another command/data pair.
 */

/* See http://www.nwlab.net/art/netio/netio.html to get the netio tool */

/* implementation options */
#define NETIO_BUF_SIZE              (4 * 1024)
#define NETIO_USE_STATIC_BUF        0

/* NetIO server state definition */
#define NETIO_STATE_WAIT_FOR_CMD    0
#define NETIO_STATE_RECV_DATA       1
#define NETIO_STATE_SEND_DATA       2
#define NETIO_STATE_SEND_DATA_LAST  3
#define NETIO_STATE_DONE            4

struct netio_state {
  u32_t  state;
  u32_t  cmd;
  u32_t  data_len;
  u32_t  cntr;
  u8_t * buf_ptr;
  u32_t  buf_pos;
  u32_t  first_byte;
  u32_t  time_stamp;
};

/* NetIO command protocol definition */
#define NETIO_CMD_QUIT              0
#define NETIO_CMD_C2S               1
#define NETIO_CMD_S2C               2
#define NETIO_CMD_RES               3

static err_t netio_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

static void ICACHE_FLASH_ATTR
netio_close(void *arg, struct tcp_pcb *pcb)
{
  err_t err;

  struct netio_state *ns = arg;
  ns->state = NETIO_STATE_DONE;
  tcp_recv(pcb, NULL);
  err = tcp_close(pcb);

  if (err != ERR_OK) {
    /* closing failed, try again later */
    tcp_recv(pcb, netio_recv);
  } else {
    /* closing succeeded */
#if NETIO_USE_STATIC_BUF != 1
    if(ns->buf_ptr != NULL){
      mem_free(ns->buf_ptr);
    }
#endif
    tcp_arg(pcb, NULL);
    tcp_poll(pcb, NULL, 0);
    tcp_sent(pcb, NULL);
    if (arg != NULL) {
      mem_free(arg);
    }
  }
}

static err_t ICACHE_FLASH_ATTR
netio_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  struct netio_state *ns = arg;
  u8_t * data_ptr;
  u32_t data_cntr;
  struct pbuf *q = p;
  u16_t len;

  if (p != NULL) {
    tcp_recved(pcb, p->tot_len);
  }

  if (err == ERR_OK && q != NULL) {

    while (q != NULL) {
      data_cntr = q->len;
      data_ptr = q->payload;
      while (data_cntr--) {
        if (ns->state == NETIO_STATE_DONE){
          netio_close(ns, pcb);
          break;
        } else if (ns->state == NETIO_STATE_WAIT_FOR_CMD) {
          if (ns->cntr < 4) {
            /* build up the CMD field */
            ns->cmd <<= 8;
            ns->cmd |= *data_ptr++;
            ns->cntr++;
          } else if (ns->cntr < 8) {
            /* build up the DATA field */
            ns->data_len <<= 8;
            ns->data_len |= *data_ptr++;
            ns->cntr++;

            if (ns->cntr == 8) {
              /* now we have full command and data words */
              ns->cntr = 0;
              ns->buf_pos = 0;
              ns->buf_ptr[0] = 0;
              if (ns->cmd == NETIO_CMD_C2S) {
                ns->state = NETIO_STATE_RECV_DATA;
              } else if (ns->cmd == NETIO_CMD_S2C) {
                ns->state = NETIO_STATE_SEND_DATA;
                /* start timer */
                ns->time_stamp = sys_now();
                /* send first round of data */

                len = tcp_sndbuf(pcb);
                len = LWIP_MIN(len, ns->data_len - ns->cntr);
                len = LWIP_MIN(len, NETIO_BUF_SIZE - ns->buf_pos);

                do {
                  err = tcp_write(pcb, ns->buf_ptr + ns->buf_pos, len, TCP_WRITE_FLAG_COPY);
                  if (err == ERR_MEM) {
                    len /= 2;
                  }
                } while ((err == ERR_MEM) && (len > 1));

                ns->buf_pos += len;
                ns->cntr    += len;

              } else {
                /* unrecognized command, punt */
                ns->cntr = 0;
                ns->buf_pos = 0;
                ns->buf_ptr[0] = 0;
                netio_close(ns, pcb);
                break;
              }
            }
          } else {
            /* in trouble... shouldn't be in this state! */
          }

        } else if (ns->state == NETIO_STATE_RECV_DATA) {

          if(ns->cntr == 0){
            /* save the first byte of this new round of data
             * this will not match ns->buf_ptr[0] in the case that
             *   NETIO_BUF_SIZE is less than ns->data_len.
             */
            ns->first_byte = *data_ptr;
          }

          ns->buf_ptr[ns->buf_pos++] = *data_ptr++;
          ns->cntr++;

          if (ns->buf_pos == NETIO_BUF_SIZE) {
            /* circularize the buffer */
            ns->buf_pos = 0;
          }

          if(ns->cntr == ns->data_len){
            ns->cntr = 0;
            if (ns->first_byte != 0) {
              /* if this last round did not start with 0,
               *  go look for another command */
              ns->state = NETIO_STATE_WAIT_FOR_CMD;
              ns->data_len = 0;
              ns->cmd = 0;
              /* TODO LWIP_DEBUGF( print out some throughput calculation results... ); */
            } else {
              /* stay here and wait on more data */
            }
          }

        } else if (ns->state == NETIO_STATE_SEND_DATA
            || ns->state == NETIO_STATE_SEND_DATA_LAST) {
          /* I don't think this should happen... */
        } else {
          /* done / quit */
          netio_close(ns, pcb);
          break;
        } /* end of ns->state condition */
      } /* end of while data still in this pbuf */

      q = q->next;
    }

    pbuf_free(p);

  } else {

    /* error or closed by other side */
    if (p != NULL) {
      pbuf_free(p);
    }

    /* close the connection */
    netio_close(ns, pcb);

  }
  return ERR_OK;

}

static err_t ICACHE_FLASH_ATTR
netio_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
  struct netio_state *ns = arg;
  err_t err = ERR_OK;

  if (ns->cntr >= ns->data_len && ns->state == NETIO_STATE_SEND_DATA) {
    /* done with this round of sending */
    ns->buf_pos = 0;
    ns->cntr = 0;

    /* check if timer expired */
    if (sys_now() - ns->time_stamp > 600) {
      ns->buf_ptr[0] = 1;
      ns->state = NETIO_STATE_SEND_DATA_LAST;
    } else {
      ns->buf_ptr[0] = 0;
    }
  }

  if(ns->state == NETIO_STATE_SEND_DATA_LAST || ns->state == NETIO_STATE_SEND_DATA){
    len = tcp_sndbuf(pcb);
    len = LWIP_MIN(len, ns->data_len - ns->cntr);
    len = LWIP_MIN(len, NETIO_BUF_SIZE - ns->buf_pos);

    if(ns->cntr < ns->data_len){
      do {
        err = tcp_write(pcb, ns->buf_ptr + ns->buf_pos, len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_MEM) {
          len /= 2;
        }
      } while ((err == ERR_MEM) && (len > 1));

      ns->buf_pos += len;
      if(ns->buf_pos >= NETIO_BUF_SIZE){
        ns->buf_pos = 0;
      }

      ns->cntr += len;
    }
  }

  if(ns->cntr >= ns->data_len && ns->state == NETIO_STATE_SEND_DATA_LAST){
    /* we have buffered up all our data to send this last round, go look for a command */
   ns->state = NETIO_STATE_WAIT_FOR_CMD;
   ns->cntr  = 0;
   /* TODO LWIP_DEBUGF( print out some throughput calculation results... ); */
  }

  return ERR_OK;
}

static err_t ICACHE_FLASH_ATTR
netio_poll(void *arg, struct tcp_pcb *pcb)
{
  struct netio_state * ns = arg;
  if(ns->state == NETIO_STATE_SEND_DATA){

  } else if(ns->state == NETIO_STATE_DONE){
    netio_close(ns, pcb);
  }

  return ERR_OK;

}

#if NETIO_USE_STATIC_BUF == 1
static u8_t netio_buf[NETIO_BUF_SIZE];
#endif

static err_t ICACHE_FLASH_ATTR
netio_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  struct netio_state * ns;

  LWIP_UNUSED_ARG(err);

  ns = (struct netio_state *)mem_malloc(sizeof(struct netio_state));

  if(ns == NULL){
    return ERR_MEM;
  }

  ns->state = NETIO_STATE_WAIT_FOR_CMD;
  ns->data_len = 0;
  ns->cmd = 0;
  ns->cntr = 0;
  ns->buf_pos = 0;
#if NETIO_USE_STATIC_BUF == 1
  ns->buf_ptr = netio_buf;
#else
  ns->buf_ptr = (u8_t *)mem_malloc(NETIO_BUF_SIZE);

  if(ns->buf_ptr == NULL){
    mem_free(ns);
    return ERR_MEM;
  }
#endif

  ns->buf_ptr[0] = 0;

  tcp_arg(pcb, ns);
  tcp_sent(pcb, netio_sent);
  tcp_recv(pcb, netio_recv);
  tcp_poll(pcb, netio_poll, 4); /* every 2 seconds */
  return ERR_OK;
}

void ICACHE_FLASH_ATTR netio_init(void)
{
  struct tcp_pcb *pcb;

  pcb = tcp_new();
  tcp_bind(pcb, IP_ADDR_ANY, 18767);
  pcb = tcp_listen(pcb);
  tcp_accept(pcb, netio_accept);
}

#endif /* LWIP_TCP */
