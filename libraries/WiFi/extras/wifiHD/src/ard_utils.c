/*
 * ard_utils.c
 *
 *  Created on: Jul 4, 2010
 *      Author: mlf by Metodo2 srl
 */
//#define _APP_DEBUG_

#include "lwip/pbuf.h"
#include "wifi_spi.h"
#include "ard_utils.h"
#include "debug.h"
#include "ard_spi.h"
#include "ard_tcp.h"

#define MAX_PBUF_STORED	30

tData pBufStore[MAX_PBUF_STORED][MAX_SOCK_NUM];

unsigned char headBuf[MAX_SOCK_NUM] = {0};
unsigned char tailBuf[MAX_SOCK_NUM] = {0};

#define IS_BUF_AVAIL(x) (tailBuf[x] != headBuf[x])
#define IS_BUF_EMPTY(x) ((tailBuf[x] == 0) && (headBuf[x] == 0))

void init_pBuf()
{
	memset(pBufStore, 0, sizeof(pBufStore));
}

uint8_t* insertBuf(uint8_t sock, uint8_t* buf, uint16_t len)
{
	DUMP(buf,len);
	if (sock>= MAX_SOCK_NUM)
	{
		WARN("Sock out of range: sock=%d", sock);
		return NULL;
	}		
	if (pBufStore[headBuf[sock]][sock].data != NULL)
	{
		WARN("Overwriting buffer %p idx:%d!\n", pBufStore[headBuf[sock]][sock].data, headBuf[sock]);
		// to avoid memory leak free the oldest buffer
		freetDataIdx(headBuf[sock], sock);
	}

	u8_t* p = (u8_t*)calloc(len,sizeof(u8_t));
    if(p != NULL) {
    	memcpy(p, buf, len);

    	pBufStore[headBuf[sock]][sock].data = p;
    	pBufStore[headBuf[sock]][sock].len = len;
    	pBufStore[headBuf[sock]][sock].idx = 0;
    	pBufStore[headBuf[sock]][sock].pcb = getTTCP(sock, TTCP_MODE_TRANSMIT);
    	headBuf[sock]++;

    	if (headBuf[sock] == MAX_PBUF_STORED)
    		headBuf[sock] = 0;
    	if (headBuf[sock] == tailBuf[sock])
    	{
    		WARN("Avoid to Overwrite data [%d-%d]!\n", headBuf[sock], tailBuf[sock]);
    		if (headBuf[sock] != 0)
    			--headBuf[sock];
    		else
    			headBuf[sock] = MAX_PBUF_STORED-1;
    	}
    	INFO_UTIL("Insert[%d]: %p:%d-%d [%d,%d]\n", sock, p, len, p[0], headBuf[sock], tailBuf[sock]);
    }
    return p;
}


uint16_t calcMergeLen(uint8_t sock)
{
	uint16_t len = 0;

	unsigned char index = tailBuf[sock];
	do {
	if (pBufStore[index][sock].data != NULL)
	{
		len += pBufStore[index][sock].len;
		len -= pBufStore[index][sock].idx;
		INFO_UTIL_VER(" [%d]: len:%d idx:%d tot:%d\n", sock, pBufStore[index][sock].len, pBufStore[index][sock].idx, len);
	}
	++index;
	if (index == MAX_PBUF_STORED)
		index = 0;
	}while (index!=headBuf[sock]);
	return len;
}

uint16_t clearBuf(uint8_t sock)
{
	uint16_t len = 0;

	unsigned char index = tailBuf[sock];
	do {
	if (pBufStore[index][sock].data != NULL)
	{
		freetDataIdx(index,sock);
	}
	++index;
	if (index == MAX_PBUF_STORED)
		index = 0;
	}while (index!=headBuf[sock]);
	tailBuf[sock]=index;
	return len;
}

uint8_t* mergeBuf(uint8_t sock, uint8_t** buf, uint16_t* _len)
{
	uint16_t len = calcMergeLen(sock);
	uint8_t* p = (u8_t*)calloc(len,sizeof(u8_t));
	uint8_t* _p = p;
	if(p != NULL) {
		unsigned char index = tailBuf[sock];
		do {
		if (pBufStore[index][sock].data != NULL)
		{
			memcpy(p, pBufStore[index][sock].data, pBufStore[index][sock].len);
			p += pBufStore[index][sock].len;
		}
		++index;
		if (index == MAX_PBUF_STORED)
			index = 0;
		}while (index!=headBuf[sock]);
	}
	DUMP(_p,len);
	if (buf != NULL)
		*buf = _p;
	if (_len != NULL)
		*_len = len;
	return _p;
}

uint8_t* insert_pBuf(struct pbuf* q, uint8_t sock, void* _pcb)
{
	if (q == NULL)
		return NULL;

	if (pBufStore[headBuf[sock]][sock].data != NULL)
	{
		WARN("Overwriting buffer %p idx:%d!\n", pBufStore[headBuf[sock]][sock].data, headBuf[sock]);
		// to avoid memory leak free the oldest buffer
		freetDataIdx(headBuf[sock], sock);
	}

	u8_t* p = (u8_t*)calloc(q->tot_len,sizeof(u8_t));
    if(p != NULL) {
      if (pbuf_copy_partial(q, p, q->tot_len,0) != q->tot_len) {
    	  WARN("pbuf_copy_partial failed: src:%p, dst:%p, len:%d\n", q, p, q->tot_len);
    	  free(p);
    	  p = NULL;
    	  return p;
      }

      pBufStore[headBuf[sock]][sock].data = p;
      pBufStore[headBuf[sock]][sock].len = q->tot_len;
      pBufStore[headBuf[sock]][sock].idx = 0;
      pBufStore[headBuf[sock]][sock].pcb = _pcb;
      headBuf[sock]++;

  	  if (headBuf[sock] == MAX_PBUF_STORED)
  		headBuf[sock] = 0;
  	  if (headBuf[sock] == tailBuf[sock])
  	  {
  		  WARN("Avoid to Overwrite data [%d-%d]!\n", headBuf[sock], tailBuf[sock]);
  		  if (headBuf[sock] != 0)
  			  --headBuf[sock];
  		  else
  			  headBuf[sock] = MAX_PBUF_STORED-1;
  	  }
  	  INFO_UTIL("Insert[%d]: %p:%d-%d [%d,%d]\n", sock, p, q->tot_len, p[0], headBuf[sock], tailBuf[sock]);
    }
    return p;
}

void dumpPbuf(uint8_t sock)
{
	unsigned char index = tailBuf[sock];
	printk("headBuf=%d tailBuf=%d\n", headBuf[sock], tailBuf[sock]);
	do {
	if (pBufStore[index][sock].data != NULL)
	{
		printk("%d] pcb:%p Buf: %p Len:%d\n", pBufStore[index][sock].idx, pBufStore[index][sock].pcb, 
			pBufStore[index][sock].data, pBufStore[index][sock].len);
	}
	++index;
	if (index == MAX_PBUF_STORED)
		index = 0;
	}while (index!=headBuf[sock]);
}

tData* get_pBuf(uint8_t sock)
{
	if (IS_BUF_EMPTY(sock))
		return NULL;

	if (IS_BUF_AVAIL(sock))
	{
		tData* p = &(pBufStore[tailBuf[sock]][sock]);
		INFO_UTIL_VER("%p [%d,%d]\n", p, headBuf[sock], tailBuf[sock]);
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

    pBufStore[tailBuf[sock]][sock].data = NULL;
    pBufStore[tailBuf[sock]][sock].len = 0;
    pBufStore[tailBuf[sock]][sock].idx = 0;
    pBufStore[tailBuf[sock]][sock].pcb = 0;

	if (++tailBuf[sock] == MAX_PBUF_STORED)
		tailBuf[sock] = 0;
	INFO_UTIL("%p [%d,%d]\n", buf, headBuf[sock], tailBuf[sock]);
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

void ackAndFreeData(void* pcb, int len, uint8_t sock, uint8_t* data)
{
	INFO_TCP("Ack pcb:%p len:%d sock:%d data:%p\n", pcb, len, sock, data);
	if (!IS_UDP_SOCK(sock))
		ack_recved(pcb, len);
	if (data != NULL)
		freetData(data, sock);
}


bool isAvailTcpDataByte(uint8_t sock)
{
	tData* p = get_pBuf(sock);

	if (p != NULL)
	{
		INFO_UTIL_VER("check:%d %d %p\n",p->idx, p->len, p->data);
		if (p->idx == p->len)
		{
			INFO_UTIL("Free %p other buf %d tail:%d head:%d\n",
					p->data, IS_BUF_AVAIL(sock), tailBuf[sock], headBuf[sock]);
			ackAndFreeData(p->pcb, p->len, sock, p->data);						
			return (IS_BUF_AVAIL(sock));
		}else{
			return true;
		}
	}
	return false;
}

uint16_t getAvailTcpDataByte(uint8_t sock)
{
	uint16_t len = calcMergeLen(sock);
	INFO_UTIL_VER("Availabled data: %d\n", len);
	return len;
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
		if (p->idx == p->len)
			ackAndFreeData(p->pcb, p->len, sock, p->data);
		return true;
		}else{
			ackAndFreeData(p->pcb, p->len, sock, p->data);
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
		ackAndFreeData(p->pcb, p->len, sock, p->data);
		return true;
	}
	return false;
}

void freeAllTcpData(uint8_t sock)
{
	tData* p = NULL;
	do{
		p = get_pBuf(sock);
		if (p != NULL)
			freetData(p->data, sock);
	}while(p!=NULL);
}


