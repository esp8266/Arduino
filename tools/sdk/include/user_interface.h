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
	REASON_DEFAULT_RST		= 0,
	REASON_WDT_RST			= 1,
	REASON_EXCEPTION_RST	= 2,
	REASON_SOFT_WDT_RST   	= 3,
	REASON_SOFT_RESTART 	= 4,
	REASON_DEEP_SLEEP_AWAKE	= 5,
	REASON_EXT_SYS_RST      = 6
};

struct rst_info{
	uint32 reason;
	uint32 exccause;
	uint32 epc1;
	uint32 epc2;
	uint32 epc3;
	uint32 excvaddr;
	uint32 depc;
};

struct rst_info* system_get_rst_info(void);

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
void system_uart_de_swap(void);

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

enum flash_size_map {
    FLASH_SIZE_4M_MAP_256_256 = 0,
    FLASH_SIZE_2M,
    FLASH_SIZE_8M_MAP_512_512,
    FLASH_SIZE_16M_MAP_512_512,
    FLASH_SIZE_32M_MAP_512_512,
    FLASH_SIZE_16M_MAP_1024_1024,
    FLASH_SIZE_32M_MAP_1024_1024
};

enum flash_size_map system_get_flash_size_map(void);

void system_phy_set_max_tpw(uint8 max_tpw);
void system_phy_set_tpw_via_vdd33(uint16 vdd33);
void system_phy_set_rfoption(uint8 option);
void system_phy_set_powerup_option(uint8 option);

bool system_param_save_with_protect(uint16 start_sec, void *param, uint16 len);
bool system_param_load(uint16 start_sec, uint16 offset, void *param, uint16 len);

void system_soft_wdt_stop(void);
void system_soft_wdt_restart(void);
void system_soft_wdt_feed(void);

void system_show_malloc(void);

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
    uint8 ssid_len;
    uint8 channel;
    sint8 rssi;
    AUTH_MODE authmode;
    uint8 is_hidden;
    sint16 freq_offset;
    sint16 freqcal_val;
	uint8 *esp_mesh_ie;
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

sint8 wifi_station_get_rssi(void);

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
uint8 wifi_station_get_ap_info(struct station_config config[]);

bool wifi_station_dhcpc_start(void);
bool wifi_station_dhcpc_stop(void);
enum dhcp_status wifi_station_dhcpc_status(void);
bool wifi_station_dhcpc_set_maxtry(uint8 num);

char* wifi_station_get_hostname(void);
bool wifi_station_set_hostname(char *name);

int wifi_station_set_cert_key(uint8 *client_cert, int client_cert_len,
    uint8 *private_key, int private_key_len,
    uint8 *private_key_passwd, int private_key_passwd_len);
void wifi_station_clear_cert_key(void);

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
	bool enable;
	struct ip_addr start_ip;
	struct ip_addr end_ip;
};

enum dhcps_offer_option{
	OFFER_START = 0x00,
	OFFER_ROUTER = 0x01,
	OFFER_END
};

uint8 wifi_softap_get_station_num(void);
struct station_info * wifi_softap_get_station_info(void);
void wifi_softap_free_station_info(void);

bool wifi_softap_dhcps_start(void);
bool wifi_softap_dhcps_stop(void);

bool wifi_softap_set_dhcps_lease(struct dhcps_lease *please);
bool wifi_softap_get_dhcps_lease(struct dhcps_lease *please);
uint32 wifi_softap_get_dhcps_lease_time(void);
bool wifi_softap_set_dhcps_lease_time(uint32 minute);
bool wifi_softap_reset_dhcps_lease_time(void);

enum dhcp_status wifi_softap_dhcps_status(void);
bool wifi_softap_set_dhcps_offer_option(uint8 level, void* optarg);

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

void wifi_fpm_open(void);
void wifi_fpm_close(void);
void wifi_fpm_do_wakeup(void);
sint8 wifi_fpm_do_sleep(uint32 sleep_time_in_us);
void wifi_fpm_set_sleep_type(enum sleep_type type);
enum sleep_type wifi_fpm_get_sleep_type(void);

enum {
    EVENT_STAMODE_CONNECTED = 0,
    EVENT_STAMODE_DISCONNECTED,
    EVENT_STAMODE_AUTHMODE_CHANGE,
    EVENT_STAMODE_GOT_IP,
    EVENT_STAMODE_DHCP_TIMEOUT,
    EVENT_SOFTAPMODE_STACONNECTED,
	EVENT_SOFTAPMODE_STADISCONNECTED,
	EVENT_SOFTAPMODE_PROBEREQRECVED,
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
	REASON_AUTH_FAIL				= 202,
	REASON_ASSOC_FAIL				= 203,
	REASON_HANDSHAKE_TIMEOUT		= 204,
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

typedef struct {
	int rssi;
	uint8 mac[6];
} Event_SoftAPMode_ProbeReqRecved_t;

typedef union {
	Event_StaMode_Connected_t			connected;
	Event_StaMode_Disconnected_t		disconnected;
	Event_StaMode_AuthMode_Change_t		auth_change;
	Event_StaMode_Got_IP_t				got_ip;
	Event_SoftAPMode_StaConnected_t		sta_connected;
	Event_SoftAPMode_StaDisconnected_t	sta_disconnected;
	Event_SoftAPMode_ProbeReqRecved_t   ap_probereqrecved;
} Event_Info_u;

typedef struct _esp_event {
    uint32 event;
    Event_Info_u event_info;
} System_Event_t;

typedef void (* wifi_event_handler_cb_t)(System_Event_t *event);

void wifi_set_event_handler_cb(wifi_event_handler_cb_t cb);

typedef enum wps_type {
	WPS_TYPE_DISABLE = 0,
	WPS_TYPE_PBC,
	WPS_TYPE_PIN,
	WPS_TYPE_DISPLAY,
	WPS_TYPE_MAX,
} WPS_TYPE_t;

enum wps_cb_status {
	WPS_CB_ST_SUCCESS = 0,
	WPS_CB_ST_FAILED,
	WPS_CB_ST_TIMEOUT,
	WPS_CB_ST_WEP,
};

bool wifi_wps_enable(WPS_TYPE_t wps_type);
bool wifi_wps_disable(void);
bool wifi_wps_start(void);

typedef void (*wps_st_cb_t)(int status);
bool wifi_set_wps_cb(wps_st_cb_t cb);

typedef void (*freedom_outside_cb_t)(uint8 status);
int wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
void wifi_unregister_send_pkt_freedom_cb(void);
int wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);

int wifi_rfid_locp_recv_open(void);
void wifi_rfid_locp_recv_close(void);

typedef void (*rfid_locp_cb_t)(uint8 *frm, int len, int rssi);
int wifi_register_rfid_locp_recv_cb(rfid_locp_cb_t cb);
void wifi_unregister_rfid_locp_recv_cb(void);

enum FIXED_RATE {
        PHY_RATE_48       = 0x8,
        PHY_RATE_24       = 0x9,
        PHY_RATE_12       = 0xA,
        PHY_RATE_6        = 0xB,
        PHY_RATE_54       = 0xC,
        PHY_RATE_36       = 0xD,
        PHY_RATE_18       = 0xE,
        PHY_RATE_9        = 0xF,
};

#define FIXED_RATE_MASK_NONE	0x00
#define FIXED_RATE_MASK_STA		0x01
#define FIXED_RATE_MASK_AP		0x02
#define FIXED_RATE_MASK_ALL		0x03

int wifi_set_user_fixed_rate(uint8 enable_mask, uint8 rate);
int wifi_get_user_fixed_rate(uint8 *enable_mask, uint8 *rate);

enum support_rate {
	RATE_11B5M	= 0,
	RATE_11B11M	= 1,
	RATE_11B1M	= 2,
	RATE_11B2M	= 3,
	RATE_11G6M	= 4,
	RATE_11G12M	= 5,
	RATE_11G24M	= 6,
	RATE_11G48M	= 7,
	RATE_11G54M	= 8,
	RATE_11G9M	= 9,
	RATE_11G18M	= 10,
	RATE_11G36M	= 11,
};

int wifi_set_user_sup_rate(uint8 min, uint8 max);

enum RATE_11B_ID {
	RATE_11B_B11M	= 0,
	RATE_11B_B5M	= 1,
	RATE_11B_B2M	= 2,
	RATE_11B_B1M	= 3,
};

enum RATE_11G_ID {
	RATE_11G_G54M	= 0,
	RATE_11G_G48M	= 1,
	RATE_11G_G36M	= 2,
	RATE_11G_G24M	= 3,
	RATE_11G_G18M	= 4,
	RATE_11G_G12M	= 5,
	RATE_11G_G9M	= 6,
	RATE_11G_G6M	= 7,
	RATE_11G_B5M	= 8,
	RATE_11G_B2M	= 9,
	RATE_11G_B1M	= 10
};

enum RATE_11N_ID {
	RATE_11N_MCS7S	= 0,
	RATE_11N_MCS7	= 1,
	RATE_11N_MCS6	= 2,
	RATE_11N_MCS5	= 3,
	RATE_11N_MCS4	= 4,
	RATE_11N_MCS3	= 5,
	RATE_11N_MCS2	= 6,
	RATE_11N_MCS1	= 7,
	RATE_11N_MCS0	= 8,
	RATE_11N_B5M	= 9,
	RATE_11N_B2M	= 10,
	RATE_11N_B1M	= 11
};

#define RC_LIMIT_11B		0
#define RC_LIMIT_11G		1
#define RC_LIMIT_11N		2
#define RC_LIMIT_P2P_11G	3
#define RC_LIMIT_P2P_11N	4
#define RC_LIMIT_NUM		5

#define LIMIT_RATE_MASK_NONE	0x00
#define LIMIT_RATE_MASK_STA		0x01
#define LIMIT_RATE_MASK_AP		0x02
#define LIMIT_RATE_MASK_ALL		0x03

bool wifi_set_user_rate_limit(uint8 mode, uint8 ifidx, uint8 max, uint8 min);
uint8 wifi_get_user_limit_rate_mask(void);
bool wifi_set_user_limit_rate_mask(uint8 enable_mask);

enum {
	USER_IE_BEACON = 0,
	USER_IE_PROBE_REQ,
	USER_IE_PROBE_RESP,
	USER_IE_ASSOC_REQ,
	USER_IE_ASSOC_RESP,
	USER_IE_MAX
};

typedef void (*user_ie_manufacturer_recv_cb_t)(uint8 type, const uint8 sa[6], const uint8 m_oui[3], uint8 *ie, uint8 ie_len, int rssi);

bool wifi_set_user_ie(bool enable, uint8 *m_oui, uint8 type, uint8 *user_ie, uint8 len);
int wifi_register_user_ie_manufacturer_recv_cb(user_ie_manufacturer_recv_cb_t cb);
void wifi_unregister_user_ie_manufacturer_recv_cb(void);

#endif
