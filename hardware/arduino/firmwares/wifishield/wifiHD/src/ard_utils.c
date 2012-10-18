/*
 * ard_utils.c
 *
 *  Created on: Jul 4, 2010
 *      Author: mlf by Metodo2 srl
 */
#undef _APP_DEBUG_

#include "lwip/pbuf.h"
#include "wifi_spi.h"
#include "ard_utils.h"
#include "debug.h"

#define MAX_PBUF_STORED	30

tData pBufStore[MAX_PBUF_STORED][MAX_SOCK_NUM];

unsigned char headBuf = 0;
unsigned char tailBuf = 0;

#define IS_BUF_AVAIL() (tailBuf!=headBuf)
#define IS_BUF_EMPTY() ((tailBuf == 0) && (headBuf == 0))

void init_pBuf()
{
	memset(pBufStore, 0, sizeof(pBufStore));
}

void insert_pBuf(struct pbuf* q, uint8_t sock, void* _pcb)
{
	if (q == NULL)
		return;

	if (pBufStore[headBuf][sock].data != NULL)
	{
		WARN("Overwriting buffer %p idx:%d!\n", pBufStore[headBuf][sock].data, headBuf);
		// to avoid memory leak free the oldest buffer
		freetDataIdx(headBuf, sock);
	}

	u8_t* p = (u8_t*)calloc(q->tot_len,sizeof(u8_t));
    if(p != NULL) {
      if (pbuf_copy_partial(q, p, q->tot_len,0) != q->tot_len) {
    	  WARN("pbuf_copy_partial failed: src:%p, dst:%p, len:%d\n", q, p, q->tot_len);
    	  free(p);
    	  p = NULL;
    	  return;
      }

      pBufStore[headBuf][sock].data = p;
      pBufStore[headBuf][sock].len = q->tot_len;
      pBufStore[headBuf][sock].idx = 0;
      pBufStore[headBuf][sock].pcb = _pcb;
      headBuf++;

  	  if (headBuf == MAX_PBUF_STORED)
  		headBuf = 0;
  	  if (headBuf == tailBuf)
  		  WARN("Overwriting data [%d-%d]!\n", headBuf, tailBuf);
  	  INFO_UTIL("Insert: %p:%d-%d [%d,%d]\n", p, q->tot_len, p[0], headBuf, tailBuf);
    }
}

tData* get_pBuf(uint8_t sock)
{
	if (IS_BUF_EMPTY())
		return NULL;

	if (IS_BUF_AVAIL())
	{
		tData* p = &(pBufStore[tailBuf][sock]);
		INFO_UTIL_VER("%p [%d,%d]\n", p, headBuf, tailBuf);
		return p;
	}
	return NULL;
}

void freetData(void * buf, uint8_t sock)
{
	if (buf==NULL)
	{
		WARN("Buf == NULL!");
		return;
	}

    pBufStore[tailBuf][sock].data = NULL;
    pBufStore[tailBuf][sock].len = 0;
    pBufStore[tailBuf][sock].idx = 0;
    pBufStore[tailBuf][sock].pcb = 0;

	if (++tailBuf == MAX_PBUF_STORED)
		tailBuf = 0;
	INFO_UTIL("%p [%d,%d]\n", buf, headBuf, tailBuf);
	free(buf);
}

void freetDataIdx(uint8_t idxBuf, uint8_t sock)
{
	if (idxBuf >=MAX_PBUF_STORED)
	{
		WARN("idxBuf out of range: %d\n", idxBuf);
		return;
	}

	void * buf = pBufStore[idxBuf][sock].data;

	INFO_UTIL("%p idx:%d\n", buf, idxBuf);

	free(buf);

    pBufStore[idxBuf][sock].data = 0;
    pBufStore[idxBuf][sock].len = 0;
    pBufStore[idxBuf][sock].idx = 0;
    pBufStore[idxBuf][sock].pcb = 0;
}


void ack_recved(void* pcb, int len);

bool isAvailTcpDataByte(uint8_t sock)
{
	tData* p = get_pBuf(sock);

	if (p != NULL)
	{
		INFO_UTIL_VER("check:%d %d %p\n",p->idx, p->len, p->data);
		if (p->idx == p->len)
		{
			freetData(p->data, sock);
			ack_recved(p->pcb, p->len);
			INFO_UTIL("Free %p other buf %d tail:%d head:%d\n",
					p->data, IS_BUF_AVAIL(), tailBuf, headBuf);
			return (IS_BUF_AVAIL());
		}else{
			return true;
		}
	}
	return false;
}



bool getTcpDataByte(uint8_t sock, uint8_t* payload, uint8_t peek)
{
	// ref field in struct pbuf has been used as index pointer for byte data
	tData* p = get_pBuf(sock);

	if (p != NULL)
	{
		if (p->idx < p->len)
		{
		uint8_t* buf = (uint8_t*)p->data;
		if (peek)
			*payload = buf[p->idx];
		else
			*payload = buf[p->idx++];
		INFO_UTIL_VER("get:%d %p %d\n",p->idx, p->data, *payload);
		return true;
		}else{
			//dealloc current buffer
			INFO_UTIL("Free %p\n", p->data);
			freetData(p->data, sock);
			ack_recved(p->pcb, p->len);
		}
	}
	return false;
}

bool getTcpData(uint8_t sock, void** payload, uint16_t* len)
{
	tData* p = NULL;
	p = get_pBuf(sock);
	if (p != NULL)
	{
		*payload = p->data;
		*len = p->len;
		return true;
	}
	return false;
}

bool freeTcpData(uint8_t sock)
{
	tData* p = NULL;
	p = get_pBuf(sock);
	if (p != NULL)
	{
		freetData(p->data, sock);
		ack_recved(p->pcb, p->len);
		return true;
	}
	return false;
}



