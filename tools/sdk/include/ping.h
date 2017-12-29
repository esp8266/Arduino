/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __PING_H__
#define __PING_H__


typedef void (* ping_recv_function)(void* arg, void *pdata);
typedef void (* ping_sent_function)(void* arg, void *pdata);

struct ping_option{
	uint32 count;
	uint32 ip;
	uint32 coarse_time;
	ping_recv_function recv_function;
	ping_sent_function sent_function;
	void* reverse;
};

struct ping_resp{
	uint32 total_count;
	uint32 resp_time;
	uint32 seqno;
	uint32 timeout_count;
	uint32 bytes;
	uint32 total_bytes;
	uint32 total_time;
	sint8  ping_err;
};

bool ping_start(struct ping_option *ping_opt);
bool ping_regist_recv(struct ping_option *ping_opt, ping_recv_function ping_recv);
bool ping_regist_sent(struct ping_option *ping_opt, ping_sent_function ping_sent);

#endif /* __PING_H__ */
