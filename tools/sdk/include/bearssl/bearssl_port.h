#ifndef _bearssl_port_h
#define _bearssl_port_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern void br_esp8266_stack_proxy_init(uint8_t *space, uint16_t size);
extern size_t br_esp8266_stack_proxy_max();
extern size_t br_esp8266_stack_proxy_usage();
extern void br_esp8266_stack_proxy_deinit();

#ifdef __cplusplus
};
#endif

#endif

