/*
 * isr_safe_printf.cpp - Intended for diagnostic printing from a restricted setting.
 *
 * Meant to be a print from anywhere and work. There will be more limitations
 * than your typical printf function.
 *
 * Still researching options for printing.
 */
#include <stdio.h>
#include <string.h>
#include <pgmspace.h>
#include <core_esp8266_features.h>
#include "umm_malloc_cfg.h"
extern "C" {

#if 0 //defined(DEBUG_ESP_ISR)

int _isr_safe_printf_P(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
int ICACHE_RAM_ATTR _isr_safe_printf_P(const char *fmt, ...) {
    if (ETS_INTR_WITHINISR() && (uint32_t)fmt >= 0x40200000) {
        ets_uart_printf("\nCannot print flash string, %p, from ISR\n", fmt);
        return 0;
    }
    /*
      To use ets_strlen() and ets_memcpy() safely with PROGMEM, flash storage,
      the PROGMEM address must be word (4 bytes) aligned. The destination
      address for ets_memcpy must also be word-aligned. We also round the
      buf_len up to the nearest word boundary. So that all transfers will be
      whole words.
    */
    size_t str_len = ets_strlen(fmt);
    size_t buf_len = (str_len + 1 + 3) & ~0x03U;
    char ram_buf[buf_len] __attribute__ ((aligned(4)));
    ets_memcpy(ram_buf, fmt, buf_len);
    va_list argPtr;
    va_start(argPtr, fmt);
    int result = ets_vprintf(ets_uart_putc1, ram_buf, argPtr);
    va_end(argPtr);
    return result;
}

#endif

};
