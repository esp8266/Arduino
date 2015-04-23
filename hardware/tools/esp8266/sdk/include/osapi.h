/*
 * Copyright (c) 2010 Espressif System
 */

#ifndef _OSAPI_H_
#define _OSAPI_H_

#include <string.h>
#include "user_config.h"

#define os_bzero ets_bzero
#define os_delay_us ets_delay_us
#define os_install_putc1 ets_install_putc1
#define os_install_putc2 ets_install_putc2
#define os_intr_lock ets_intr_lock
#define os_intr_unlock ets_intr_unlock
#define os_isr_attach ets_isr_attach
#define os_isr_mask ets_isr_mask
#define os_isr_unmask ets_isr_unmask
#define os_memcmp ets_memcmp
#define os_memcpy ets_memcpy
#define os_memmove ets_memmove
#define os_memset ets_memset
#define os_putc ets_putc
#define os_str2macaddr ets_str2macaddr
#define os_strcat strcat
#define os_strchr strchr
#define os_strcmp ets_strcmp
#define os_strcpy ets_strcpy
#define os_strlen ets_strlen
#define os_strncmp ets_strncmp
#define os_strncpy ets_strncpy
#define os_strstr ets_strstr
#ifdef USE_US_TIMER
#define os_timer_arm_us(a, b, c) ets_timer_arm_new(a, b, c, 0)
#endif
#define os_timer_arm(a, b, c) ets_timer_arm_new(a, b, c, 1)
#define os_timer_disarm ets_timer_disarm
#define os_timer_done ets_timer_done
#define os_timer_handler_isr ets_timer_handler_isr
#define os_timer_init ets_timer_init
#define os_timer_setfn ets_timer_setfn

#define os_sprintf  ets_sprintf
#define os_update_cpu_frequency ets_update_cpu_frequency

#ifdef USE_OPTIMIZE_PRINTF
#define os_printf(fmt, ...) do {	\
	static const char flash_str[] ICACHE_RODATA_ATTR = fmt;	\
	os_printf_plus(flash_str, ##__VA_ARGS__);	\
	} while(0)
#else
extern int os_printf_plus(const char * format, ...);
#define os_printf	os_printf_plus
#endif

#endif

