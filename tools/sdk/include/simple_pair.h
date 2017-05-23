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

#ifndef __SIMPLE_PAIR_H__
#define __SIMPLE_PAIR_H__

typedef enum {
	SP_ST_STA_FINISH = 0,
	SP_ST_AP_FINISH = 0,
	SP_ST_AP_RECV_NEG,
	SP_ST_STA_AP_REFUSE_NEG,
	/* all following is err */
	SP_ST_WAIT_TIMEOUT,
	SP_ST_SEND_ERROR,
	SP_ST_KEY_INSTALL_ERR,
	SP_ST_KEY_OVERLAP_ERR,  //means the same macaddr has two different keys 
	SP_ST_OP_ERROR,
	SP_ST_UNKNOWN_ERROR,
	SP_ST_MAX,
} SP_ST_t;


typedef void (*simple_pair_status_cb_t)(u8 *sa, u8 status);

int register_simple_pair_status_cb(simple_pair_status_cb_t cb);
void unregister_simple_pair_status_cb(void);

int simple_pair_init(void);
void simple_pair_deinit(void);

int simple_pair_state_reset(void);
int simple_pair_ap_enter_announce_mode(void);
int simple_pair_sta_enter_scan_mode(void);

int simple_pair_sta_start_negotiate(void);
int simple_pair_ap_start_negotiate(void);
int simple_pair_ap_refuse_negotiate(void);

int simple_pair_set_peer_ref(u8 *peer_mac, u8 *tmp_key, u8 *ex_key);
int simple_pair_get_peer_ref(u8 *peer_mac, u8 *tmp_key, u8 *ex_key);


#endif /* __SIMPLE_PAIR_H__ */
