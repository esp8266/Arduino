/*
 *  Copyright (C) 2013 -2014  Espressif System
 *
 */

#ifndef __USER_INTERFACE_H__
#define __USER_INTERFACE_H__

#include "os_type.h"
#ifdef LWIP_OPEN_SRC
#include "lwip/ip_addr.h"
#else
#include "ip_addr.h"
#endif

#include "queue.h"
#include "user_config.h"
#include "spi_flash.h"

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#define UPGRADE_FW_BIN1         0x00
#define UPGRADE_FW_BIN2         0x01

typedef void (*upgrade_states_check_callback)(void * arg);

struct upgrade_server_info {
    uint8 ip[4];
    uint16 port;

    uint8 upgrade_flag;

    uint8 pre_version[8];
    uint8 upgrade_version[8];

    uint32 check_times;
    uint8 *url;

    upgrade_states_check_callback check_cb;
    struct espconn *pespconn;
};

bool system_upgrade_start(struct upgrade_server_info *server);
bool system_upgrade_start_ssl(struct upgrade_server_info *server);
uint8 system_upgrade_userbin_check(void);
void system_upgrade_reboot(void);

void system_restore(void);
void system_restart(void);
void system_deep_sleep(uint32 time_in_us);

void system_timer_reinit(void);
uint32 system_get_time(void);

/* user task's prio must be 0/1/2 !!!*/
enum {
    USER_TASK_PRIO_0 = 0,
    USER_TASK_PRIO_1,
    USER_TASK_PRIO_2,
    USER_TASK_PRIO_MAX
};

void system_os_task(os_task_t task, uint8 prio, os_event_t *queue, uint8 qlen);
void system_os_post(uint8 prio, os_signal_t sig, os_param_t par);

void system_print_meminfo(void);
uint32 system_get_free_heap_size(void);

void system_set_os_print(uint8 onoff);

uint64 system_mktime(uint32 year, uint32 mon, uint32 day, uint32 hour, uint32 min, uint32 sec);

uint32 system_get_chip_id(void);

typedef void (* init_done_cb_t)(void);

void system_init_done_cb(init_done_cb_t cb);

#define NULL_MODE       0x00
#define STATION_MODE    0x01
#define SOFTAP_MODE     0x02
#define STATIONAP_MODE  0x03

uint8 wifi_get_opmode(void);
bool wifi_set_opmode(uint8 opmode);

struct bss_info {
    STAILQ_ENTRY(bss_info)     next;

    u8 bssid[6];
    u8 ssid[32];
    u8 channel;
    s8 rssi;
    u8 authmode;
};

typedef struct _scaninfo {
    STAILQ_HEAD(, bss_info) *pbss;
    struct espconn *pespconn;
    u8 totalpage;
    u8 pagenum;
    u8 page_sn;
    u8 data_cnt;
} scaninfo;

typedef void (* scan_done_cb_t)(void *arg, STATUS status);

struct station_config {
    uint8 ssid[32];
    uint8 password[64];
};

bool wifi_station_get_config(struct station_config *config);
bool wifi_station_set_config(struct station_config *config);

bool wifi_station_connect(void);
bool wifi_station_disconnect(void);

struct scan_config {
    uint8 *ssid;
    uint8 *bssid;
    uint8 channel;
};

bool wifi_station_scan(struct scan_config *config, scan_done_cb_t cb);

uint8 wifi_station_get_auto_connect(void);
bool wifi_station_set_auto_connect(uint8 set);

enum {
    STATION_IDLE = 0,
    STATION_CONNECTING,
    STATION_WRONG_PASSWORD,
    STATION_NO_AP_FOUND,
    STATION_CONNECT_FAIL,
    STATION_GOT_IP
};

uint8 wifi_station_get_connect_status(void);

typedef enum _auth_mode {
    AUTH_OPEN           = 0,
    AUTH_WEP,
    AUTH_WPA_PSK,
    AUTH_WPA2_PSK,
    AUTH_WPA_WPA2_PSK
} AUTH_MODE;

struct softap_config {
    uint8 ssid[32];
    uint8 password[64];
    uint8 channel;
    uint8 authmode;
    uint8 ssid_hidden;
    uint8 max_connection;
};

bool wifi_softap_get_config(struct softap_config *config);
bool wifi_softap_set_config(struct softap_config *config);

struct station_info {
	STAILQ_ENTRY(station_info)	next;

	uint8 bssid[6];
	struct ip_addr ip;
};

struct station_info * wifi_softap_get_station_info(void);
void wifi_softap_free_station_info(void);

#define STATION_IF      0x00
#define SOFTAP_IF       0x01

bool wifi_get_ip_info(uint8 if_index, struct ip_info *info);
bool wifi_set_ip_info(uint8 if_index, struct ip_info *info);
bool wifi_get_macaddr(uint8 if_index, uint8 *macaddr);
bool wifi_set_macaddr(uint8 if_index, uint8 *macaddr);

uint8 wifi_get_channel(void);
bool wifi_set_channel(uint8 channel);

void wifi_status_led_install(uint8 gpio_id, uint32 gpio_name, uint8 gpio_func);

/** Get the absolute difference between 2 u32_t values (correcting overflows)
 * 'a' is expected to be 'higher' (without overflow) than 'b'. */
#define ESP_U32_DIFF(a, b) (((a) >= (b)) ? ((a) - (b)) : (((a) + ((b) ^ 0xFFFFFFFF) + 1)))

void wifi_promiscuous_enable(uint8 promiscuous);

typedef void (* wifi_promiscuous_cb_t)(uint8 *buf, uint16 len);

void wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);

uint8 wifi_station_get_current_ap_id(void);
bool wifi_station_ap_change(uint8 current_ap_id);
bool wifi_station_ap_number_set(uint8 ap_number);

#endif
