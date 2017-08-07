
#ifndef ESP_MISSING_H
#define ESP_MISSING_H

// these declarations are guessed and used by lwip1.4 from sdk2.0.0

uint32_t r_rand (void);
void ets_bzero (void*, size_t);
int ets_memcmp (const void*, const void*, size_t n);
void* pvPortZalloc (size_t, const char*, int);
struct netif* eagle_lwip_getif (int netif_index);

struct ip_addr;
void wifi_softap_set_station_info (uint8_t* mac, struct ip_addr*);

#endif
