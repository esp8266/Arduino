/*
 *  Copyright (C) 2015 -2018  Espressif System
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
