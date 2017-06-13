/*
 *  Copyright (C) 2015 -2018  Espressif System
 *
 */

#ifndef __ESPNOW_H__
#define __ESPNOW_H__

enum esp_now_role {
	ESP_NOW_ROLE_IDLE = 0,
	ESP_NOW_ROLE_CONTROLLER,
	ESP_NOW_ROLE_SLAVE,
	ESP_NOW_ROLE_COMBO,
	ESP_NOW_ROLE_MAX,
};

typedef void (*esp_now_recv_cb_t)(u8 *mac_addr, u8 *data, u8 len);
typedef void (*esp_now_send_cb_t)(u8 *mac_addr, u8 status);

int esp_now_init(void);
int esp_now_deinit(void);

int esp_now_register_send_cb(esp_now_send_cb_t cb);
int esp_now_unregister_send_cb(void);

int esp_now_register_recv_cb(esp_now_recv_cb_t cb);
int esp_now_unregister_recv_cb(void);

int esp_now_send(u8 *da, u8 *data, int len);

int esp_now_add_peer(u8 *mac_addr, u8 role, u8 channel, u8 *key, u8 key_len);
int esp_now_del_peer(u8 *mac_addr);

int esp_now_set_self_role(u8 role);
int esp_now_get_self_role(void);

int esp_now_set_peer_role(u8 *mac_addr, u8 role);
int esp_now_get_peer_role(u8 *mac_addr);

int esp_now_set_peer_channel(u8 *mac_addr, u8 channel);
int esp_now_get_peer_channel(u8 *mac_addr);

int esp_now_set_peer_key(u8 *mac_addr, u8 *key, u8 key_len);
int esp_now_get_peer_key(u8 *mac_addr, u8 *key, u8 *key_len);

u8 *esp_now_fetch_peer(bool restart);

int esp_now_is_peer_exist(u8 *mac_addr);

int esp_now_get_cnt_info(u8 *all_cnt, u8 *encrypt_cnt);

int esp_now_set_kok(u8 *key, u8 len);

#endif
