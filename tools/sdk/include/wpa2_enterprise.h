#ifndef __WPA2_ENTERPRISE_H__
#define __WPA2_ENTERPRISE_H__

typedef long os_time_t;

struct os_time {
	os_time_t sec;
	os_time_t usec;
};

typedef int (* get_time_func_t)(struct os_time *t);

int  wifi_station_set_wpa2_enterprise_auth(int enable);

int  wifi_station_set_enterprise_cert_key(u8 *client_cert, int client_cert_len,
			u8 *private_key, int private_key_len,
			u8 *private_key_passwd, int private_key_passwd_len);
void  wifi_station_clear_enterprise_cert_key(void);

int  wifi_station_set_enterprise_ca_cert(u8 *ca_cert, int ca_cert_len);
void  wifi_station_clear_enterprise_ca_cert(void);

int  wifi_station_set_enterprise_username(u8 *username, int len);
void  wifi_station_clear_enterprise_username(void);

int  wifi_station_set_enterprise_password(u8 *password, int len);
void  wifi_station_clear_enterprise_password(void);

int  wifi_station_set_enterprise_new_password(u8 *new_password, int len);
void  wifi_station_clear_enterprise_new_password(void);

void  wifi_station_set_enterprise_disable_time_check(bool disable);
bool  wifi_station_get_enterprise_disable_time_check(void);

void  wpa2_enterprise_set_user_get_time(get_time_func_t cb);


#endif /* __WPA2_ENTERPRISE_H__ */
