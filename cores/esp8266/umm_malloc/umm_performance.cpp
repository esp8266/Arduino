/*
 * umm_malloc performance measurments and ESP specifics
 */

#include <stdio.h>
#include <string.h>
#include <pgmspace.h>
#include <core_esp8266_features.h>
#include "umm_performance.h"
#include "umm_stats.h"

extern "C" {

UMM_STATS ummStats = {0, 0, 0, 0};

#ifdef UMM_CRITICAL_PERIOD_ANALYZE
struct _UMM_TIME_STATS time_stats = {
  {0xFFFFFFFF, 0U, 0U, 0U},
  {0xFFFFFFFF, 0U, 0U, 0U},
  {0xFFFFFFFF, 0U, 0U, 0U},
  {0xFFFFFFFF, 0U, 0U, 0U} };

bool ICACHE_FLASH_ATTR get_umm_get_perf_data(struct _UMM_TIME_STATS *p, size_t size) {
    if (p && sizeof(time_stats) == size) {
        uint32_t save_ps = xt_rsil(DEFAULT_CRITICAL_SECTION_INTLEVEL);
        memcpy(p, &time_stats, size);
        xt_wsr_ps(save_ps);
        return true;
    }
    return false;
}
#endif

#if defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_ISR)
/*
  Printing from the malloc routines is tricky. Since a lot of library calls
  will want to do malloc.

  Objective:  To be able to print "last gasp" diagnostic messages
  when interrupts are disabled and w/o availability of heap resources.
*/

/*
 * ROM builtin "print character function" _putc1, ignores CRs and sends CR/LF
 * for LF, newline. This function is used internally by ets_uart_printf. It is
 * also the function that gets installed by ets_uart_install_printf through a
 * call to ets_install_putc1.
 */
#define _rom_putc1 ((fp_putc_t)0x40001dcc)

int _isr_safe_printf_P(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
int ICACHE_RAM_ATTR _isr_safe_printf_P(const char *fmt, ...) {
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
  int result = ets_vprintf(_rom_putc1, ram_buf, argPtr);
  va_end(argPtr);
  return result;
}

#endif

};
