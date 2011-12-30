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
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#ifndef WITH_HTTP

#define httpd_start() 0
#define httpd_stop()

#else

#include "lwip/debug.h"

#include "lwip/stats.h"

#include "httpd.h"

#include "lwip/tcp.h"

#include "fs.h"
#include "printf-stdarg.h"

struct http_state {
  char *file;
  u16_t left;
  u8_t retries;
};

/*-----------------------------------------------------------------------------------*/
static void
conn_err(void *arg, err_t err)
{
  struct http_state *hs;

  LWIP_UNUSED_ARG(err);

  hs = arg;
  mem_free(hs);
}
/*-----------------------------------------------------------------------------------*/
static void
close_conn(struct tcp_pcb *pcb, struct http_state *hs)
{
  tcp_arg(pcb, NULL);
  tcp_sent(pcb, NULL);
  tcp_recv(pcb, NULL);
  mem_free(hs);
  tcp_close(pcb);
}
/*-----------------------------------------------------------------------------------*/
static void
send_data(struct tcp_pcb *pcb, struct http_state *hs)
{
  err_t err;
  u16_t len;

  /* We cannot send more data than space available in the send
     buffer. */     
  if (tcp_sndbuf(pcb) < hs->left) {
    len = tcp_sndbuf(pcb);
  } else {
    len = hs->left;
  }

  do {
    err = tcp_write(pcb, hs->file, len, 0);
    if (err == ERR_MEM) {
      len /= 2;
    }
  } while (err == ERR_MEM && len > 1);  
  
  if (err == ERR_OK) {
    hs->file += len;
    hs->left -= len;
  }
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_poll(void *arg, struct tcp_pcb *pcb)
{
  struct http_state *hs;

  hs = arg;
  
  if (hs == NULL) {
    tcp_abort(pcb);
    return ERR_ABRT;
  } else {
    ++hs->retries;
    if (hs->retries == 4) {
      tcp_abort(pcb);
      return ERR_ABRT;
    }
    send_data(pcb, hs);
  }

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
  struct http_state *hs;

  LWIP_UNUSED_ARG(len);

  hs = arg;

  hs->retries = 0;
  
  if (hs->left > 0) {    
    send_data(pcb, hs);
  } else {
    close_conn(pcb, hs);
  }

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  int i;
  char *data;
  struct fs_file file;
  struct http_state *hs;
  hs = arg;

  if (err == ERR_OK && p != NULL) {

    /* Inform TCP that we have taken the data. */
    tcp_recved(pcb, p->tot_len);
    
    if (hs->file == NULL) {
      data = p->payload;

      if (strncmp(data, "GET ", 4) == 0) {
        for(i = 0; i < 40; i++) {
          if (((char *)data + 4)[i] == ' ' ||
             ((char *)data + 4)[i] == '\r' ||
             ((char *)data + 4)[i] == '\n') {
            ((char *)data + 4)[i] = 0;
          }
        }

        if (*(char *)(data + 4) == '/' &&
           *(char *)(data + 5) == 0) {
          fs_open("/index.html", &file);
        } else if (!fs_open((char *)data + 4, &file)) {
          fs_open("/404.html", &file);
        }

        hs->file = file.data;
        hs->left = file.len;

        pbuf_free(p);
        send_data(pcb, hs);

        /* Tell TCP that we wish be to informed of data that has been
           successfully sent by a call to the http_sent() function. */
        tcp_sent(pcb, http_sent);
      } 
      else {
        printk("%s\n", p->payload);
        pbuf_free(p);
        close_conn(pcb, hs);
      }
    } else {
      pbuf_free(p);
    }
  }

  if (err == ERR_OK && p == NULL) {
    close_conn(pcb, hs);
  }
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  struct http_state *hs;
  
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  tcp_setprio(pcb, TCP_PRIO_MIN);
  
  /* Allocate memory for the structure that holds the state of the
     connection. */
  hs = (struct http_state *)mem_malloc(sizeof(struct http_state));

  if (hs == NULL) {
    printk("http_accept: Out of memory\n");
    return ERR_MEM;
  }
  
  /* Initialize the structure. */
  hs->file = NULL;
  hs->left = 0;
  hs->retries = 0;
  
  /* Tell TCP that this is the structure we wish to be passed for our
     callbacks. */
  tcp_arg(pcb, hs);

  /* Tell TCP that we wish to be informed of incoming data by a call
     to the http_recv() function. */
  tcp_recv(pcb, http_recv);

  tcp_err(pcb, conn_err);
  
  tcp_poll(pcb, http_poll, 4);
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
struct tcp_pcb *pcb = NULL;

err_t httpd_start(void)
{
	pcb = tcp_new();
	if (pcb == NULL)
		return ERR_MEM;
	tcp_bind(pcb, IP_ADDR_ANY, 80);
	pcb = tcp_listen(pcb);
	tcp_accept(pcb, http_accept);
	return ERR_OK;
}

void httpd_stop(void)
{
	if (pcb)
		tcp_close(pcb);
}
#endif
