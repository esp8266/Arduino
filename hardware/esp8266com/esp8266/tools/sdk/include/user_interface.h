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

enum rst_reason {
	DEFAULT_RST_FLAG	= 0,
	WDT_RST_FLAG	= 1,
	EXP_RST_FLAG    = 2
};

struct rst_info{
	uint32 flag;
	uint32 exccause;
	uint32 epc1;
	uint32 epc2;
	uint32 epc3;
	uint32 excvaddr;
	uint32 depc;
};

#define UPGRADE_FW_BIN1         0x00
#define UPGRADE_FW_BIN2         0x01

void system_restore(void);
void system_restart(void);

bool system_deep_sleep_set_option(uint8 option);
void system_deep_sleep(uint32 time_in_us);

uint8 system_upgrade_userbin_check(void);
void system_upgrade_reboot(void);
uint8 system_upgrade_flag_check();
void system_upgrade_flag_set(uint8 flag);

void system_timer_reinit(void);
uint32 system_get_time(void);

/* user task's prio must be 0/1/2 !!!*/
enum {
    USER_TASK_PRIO_0 = 0,
    USER_TASK_PRIO_1,
    USER_TASK_PRIO_2,
    USER_TASK_PRIO_MAX
};

bool system_os_task(os_task_t task, uint8 prio, os_event_t *queue, uint8 qlen);
bool system_os_post(uint8 prio, os_signal_t sig, os_param_t par);

void system_print_meminfo(void);
uint32 system_get_free_heap_size(void);

void system_set_os_print(uint8 onoff);
uint8 system_get_os_print();

uint64 system_mktime(uint32 year, uint32 mon, uint32 day, uint32 hour, uint32 min, uint32 sec);

uint32 system_get_chip_id(void);

typedef void (* init_done_cb_t)(void);

void system_init_done_cb(init_done_cb_t cb);

uint32 system_rtc_clock_cali_proc(void);
uint32 system_get_rtc_time(void);

bool system_rtc_mem_read(uint8 src_addr, void *des_addr, uint16 load_size);
bool system_rtc_mem_write(uint8 des_addr, const void *src_addr, uint16 save_size);

void system_uart_swap(void);

uint16 system_adc_read(void);
uint16 system_get_vdd33(void);

const char *system_get_sdk_version(void);

#define SYS_BOOT_ENHANCE_MODE	0
#define SYS_BOOT_NORMAL_MODE	1

#define SYS_BOOT_NORMAL_BIN		0
#define SYS_BOOT_TEST_BIN		1

uint8 system_get_boot_version(void);
uint32 system_get_userbin_addr(void);
uint8 system_get_boot_mode(void);
bool system_restart_enhance(uint8 bin_type, uint32 bin_addr);

#define SYS_CPU_80MHZ	80
#define SYS_CPU_160MHZ	160

bool system_update_cpu_freq(uint8 freq);
uint8 system_get_cpu_freq(void);

#define NULL_MODE       0x00
#define STATION_MODE    0x01
#define SOFTAP_MODE     0x02
#define STATIONAP_MODE  0x03

typedef enum _auth_mode {
    AUTH_OPEN           = 0,
    AUTH_WEP,
    AUTH_WPA_PSK,
    AUTH_WPA2_PSK,
    AUTH_WPA_WPA2_PSK,
    AUTH_MAX
} AUTH_MODE;

uint8 wifi_get_opmode(void);
uint8 wifi_get_opmode_default(void);
bool wifi_set_opmode(uint8 opmode);
bool wifi_set_opmode_current(uint8 opmode);
uint8 wifi_get_broadcast_if(void);
bool wifi_set_broadcast_if(uint8 interface);

struct bss_info {
    STAILQ_ENTRY(bss_info)     next;

    uint8 bssid[6];
    uint8 ssid[32];
    uint8 channel;
    sint8 rssi;
    AUTH_MODE authmode;
    uint8 is_hidden;
};

typedef struct _scaninfo {
    STAILQ_HEAD(, bss_info) *pbss;
    struct espconn *pespconn;
    uint8 totalpage;
    uint8 pagenum;
    uint8 page_sn;
    uint8 data_cnt;
} scaninfo;

typedef void (* scan_done_cb_t)(void *arg, STATUS status);

struct station_config {
    uint8 ssid[32];
    uint8 password[64];
    uint8 bssid_set;	// Note: If bssid_set is 1, station will just connect to the router
                        // with both ssid[] and bssid[] matched. Please check about this.
    uint8 bssid[6];
};

bool wifi_station_get_config(struct station_config *config);
bool wifi_station_get_config_default(struct station_config *config);
bool wifi_station_set_config(struct station_config *config);
bool wifi_station_set_config_current(struct station_config *config);

bool wifi_station_connect(void);
bool wifi_station_disconnect(void);

struct scan_config {
    uint8 *ssid;	// Note: ssid == NULL, don't filter ssid.
    uint8 *bssid;	// Note: bssid == NULL, don't filter bssid.
    uint8 channel;	// Note: channel == 0, scan all channels, otherwise scan set channel.
    uint8 show_hidden;	// Note: show_hidden == 1, can get hidden ssid routers' info.
};

bool wifi_station_scan(struct scan_config *config, scan_done_cb_t cb);

uint8 wifi_station_get_auto_connect(void);
bool wifi_station_set_auto_connect(uint8 set);

bool wifi_station_set_reconnect_policy(bool set);

enum {
    STATION_IDLE = 0,
    STATION_CONNECTING,
    STATION_WRONG_PASSWORD,
    STATION_NO_AP_FOUND,
    STATION_CONNECT_FAIL,
    STATION_GOT_IP
};

enum dhcp_status {
	DHCP_STOPPED,
	DHCP_STARTED
};

uint8 wifi_station_get_connect_status(void);

uint8 wifi_station_get_current_ap_id(void);
bool wifi_station_ap_change(uint8 current_ap_id);
bool wifi_station_ap_number_set(uint8 ap_number);

bool wifi_station_dhcpc_start(void);
bool wifi_station_dhcpc_stop(void);
enum dhcp_status wifi_station_dhcpc_status(void);

struct softap_config {
    uint8 ssid[32];
    uint8 password[64];
    uint8 ssid_len;	// Note: Recommend to set it according to your ssid
    uint8 channel;	// Note: support 1 ~ 13
    AUTH_MODE authmode;	// Note: Don't support AUTH_WEP in softAP mode.
    uint8 ssid_hidden;	// Note: default 0
    uint8 max_connection;	// Note: default 4, max 4
    uint16 beacon_interval;	// Note: support 100 ~ 60000 ms, default 100
};

bool wifi_softap_get_config(struct softap_config *config);
bool wifi_softap_get_config_default(struct softap_config *config);
bool wifi_softap_set_config(struct softap_config *config);
bool wifi_softap_set_config_current(struct softap_config *config);

struct station_info {
	STAILQ_ENTRY(station_info)	next;

	uint8 bssid[6];
	struct ip_addr ip;
};

struct dhcps_lease {
	struct ip_addr start_ip;
	struct ip_addr end_ip;
};

enum dhcps_offer_option{
	OFFER_START = 0x00,
	OFFER_ROUTER = 0x01,
	OFFER_END
};

struct station_info * wifi_softap_get_station_info(void);
void wifi_softap_free_station_info(void);
uint8 wifi_station_get_ap_info(struct station_config config[]);

bool wifi_softap_dhcps_start(void);
bool wifi_softap_dhcps_stop(void);
bool wifi_softap_set_dhcps_lease(struct dhcps_lease *please);
enum dhcp_status wifi_softap_dhcps_status(void);
bool wifi_softap_dhcps_set_offer_option(uint8 level, void* optarg);

#define STATION_IF      0x00
#define SOFTAP_IF       0x01

bool wifi_get_ip_info(uint8 if_index, struct ip_info *info);
bool wifi_set_ip_info(uint8 if_index, struct ip_info *info);
bool wifi_get_macaddr(uint8 if_index, uint8 *macaddr);
bool wifi_set_macaddr(uint8 if_index, uint8 *macaddr);

uint8 wifi_get_channel(void);
bool wifi_set_channel(uint8 channel);

void wifi_status_led_install(uint8 gpio_id, uint32 gpio_name, uint8 gpio_func);
void wifi_status_led_uninstall();

/** Get the absolute difference between 2 u32_t values (correcting overflows)
 * 'a' is expected to be 'higher' (without overflow) than 'b'. */
#define ESP_U32_DIFF(a, b) (((a) >= (b)) ? ((a) - (b)) : (((a) + ((b) ^ 0xFFFFFFFF) + 1)))

void wifi_promiscuous_enable(uint8 promiscuous);

typedef void (* wifi_promiscuous_cb_t)(uint8 *buf, uint16 len);

void wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);

void wifi_promiscuous_set_mac(const uint8_t *address);

enum phy_mode {
	PHY_MODE_11B	= 1,
	PHY_MODE_11G	= 2,
	PHY_MODE_11N    = 3
};

enum phy_mode wifi_get_phy_mode(void);
bool wifi_set_phy_mode(enum phy_mode mode);

enum sleep_type {
	NONE_SLEEP_T	= 0,
	LIGHT_SLEEP_T,
	MODEM_SLEEP_T
};

bool wifi_set_sleep_type(enum sleep_type type);
enum sleep_type wifi_get_sleep_type(void);

enum {
    EVENT_STAMODE_CONNECTED = 0,
    EVENT_STAMODE_DISCONNECTED,
    EVENT_STAMODE_AUTHMODE_CHANGE,
    EVENT_STAMODE_GOT_IP,
    EVENT_SOFTAPMODE_STACONNECTED,
	EVENT_SOFTAPMODE_STADISCONNECTED,
    EVENT_MAX
};

enum {
	REASON_UNSPECIFIED              = 1,
	REASON_AUTH_EXPIRE              = 2,
	REASON_AUTH_LEAVE               = 3,
	REASON_ASSOC_EXPIRE             = 4,
	REASON_ASSOC_TOOMANY            = 5,
	REASON_NOT_AUTHED               = 6,
	REASON_NOT_ASSOCED              = 7,
	REASON_ASSOC_LEAVE              = 8,
	REASON_ASSOC_NOT_AUTHED         = 9,
	REASON_DISASSOC_PWRCAP_BAD      = 10,  /* 11h */
	REASON_DISASSOC_SUPCHAN_BAD     = 11,  /* 11h */
	REASON_IE_INVALID               = 13,  /* 11i */
	REASON_MIC_FAILURE              = 14,  /* 11i */
	REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,  /* 11i */
	REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,  /* 11i */
	REASON_IE_IN_4WAY_DIFFERS       = 17,  /* 11i */
	REASON_GROUP_CIPHER_INVALID     = 18,  /* 11i */
	REASON_PAIRWISE_CIPHER_INVALID  = 19,  /* 11i */
	REASON_AKMP_INVALID             = 20,  /* 11i */
	REASON_UNSUPP_RSN_IE_VERSION    = 21,  /* 11i */
	REASON_INVALID_RSN_IE_CAP       = 22,  /* 11i */
	REASON_802_1X_AUTH_FAILED       = 23,  /* 11i */
	REASON_CIPHER_SUITE_REJECTED    = 24,  /* 11i */

	REASON_BEACON_TIMEOUT           = 200,
	REASON_NO_AP_FOUND              = 201,
};

typedef struct {
	uint8 ssid[32];
	uint8 ssid_len;
	uint8 bssid[6];
	uint8 channel;
} Event_StaMode_Connected_t;

typedef struct {
	uint8 ssid[32];
	uint8 ssid_len;
	uint8 bssid[6];
	uint8 reason;
} Event_StaMode_Disconnected_t;

typedef struct {
	uint8 old_mode;
	uint8 new_mode;
} Event_StaMode_AuthMode_Change_t;

typedef struct {
	struct ip_addr ip;
	struct ip_addr mask;
	struct ip_addr gw;
} Event_StaMode_Got_IP_t;

typedef struct {
	uint8 mac[6];
	uint8 aid;
} Event_SoftAPMode_StaConnected_t;

typedef struct {
	uint8 mac[6];
	uint8 aid;
} Event_SoftAPMode_StaDisconnected_t;

typedef union {
	Event_StaMode_Connected_t			connected;
	Event_StaMode_Disconnected_t		disconnected;
	Event_StaMode_AuthMode_Change_t		auth_change;
	Event_StaMode_Got_IP_t				got_ip;
	Event_SoftAPMode_StaConnected_t		sta_connected;
	Event_SoftAPMode_StaDisconnected_t	sta_disconnected;
} Event_Info_u;

typedef struct _esp_event {
    uint32 event;
    Event_Info_u event_info;
} System_Event_t;

typedef void (* wifi_event_handler_cb_t)(System_Event_t *event);

void wifi_set_event_handler_cb(wifi_event_handler_cb_t cb);

#endif
