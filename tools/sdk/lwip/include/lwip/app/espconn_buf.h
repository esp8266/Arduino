/*
 * ringbuf.h
 *
 *  Created on: Apr 22, 2016
 *      Author: liuhan
 */

#ifndef _ESPCONN_BUF_H_
#define _ESPCONN_BUF_H_

/*
 * ringbuffer.c
 *
 *  Created on: Apr 22, 2016
 *      Author: liuhan
 */
#include "c_types.h"

#include "ets_sys.h"
#include "os_type.h"

typedef struct ringbuf_t {
	uint8_t *buf;
	uint8_t *head, *tail;
	size_t size;
} ringbuf, *ringbuf_t;

ringbuf_t ringbuf_new(size_t capacity);

size_t ringbuf_buffer_size(const struct ringbuf_t *rb);

void ringbuf_reset(ringbuf_t rb);

void ringbuf_free(ringbuf_t *rb);

size_t ringbuf_capacity(const struct ringbuf_t *rb);

size_t ringbuf_bytes_free(const struct ringbuf_t *rb);

size_t ringbuf_bytes_used(const struct ringbuf_t *rb);

int ringbuf_is_full(const struct ringbuf_t *rb);

int ringbuf_is_empty(const struct ringbuf_t *rb);

const void* ringbuf_tail(const struct ringbuf_t *rb);

const void* ringbuf_head(const struct ringbuf_t *rb);

static uint8_t *ringbuf_nextp(ringbuf_t rb, const uint8_t *p);

size_t ringbuf_findchr(const struct ringbuf_t *rb, int c, size_t offset);

size_t ringbuf_memset(ringbuf_t dst, int c, size_t len);

void *ringbuf_memcpy_into(ringbuf_t dst, const void *src, size_t count);

void *ringbuf_memcpy_from(void *dst, ringbuf_t src, size_t count);

#endif /* RINGBUF_H_ */
