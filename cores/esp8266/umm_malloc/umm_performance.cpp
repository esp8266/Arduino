/*
 * umm_malloc performance measurments and ESP specifics
 */

#include <stdio.h>
#include <string.h>
#include <pgmspace.h>
#include <core_esp8266_features.h>
#include "umm_performance.h"

extern "C" {

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

// ROM _putc1, ignores CRs and sends CR/LF for LF, newline.
// Always returns character sent.
int constexpr (*_rom_putc1)(int) = (int (*)(int))0x40001dcc;
void uart_buff_switch(uint8_t);

int _isr_safe_printf_P(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
int ICACHE_RAM_ATTR _isr_safe_printf_P(const char *fmt, ...) {
#ifdef DEBUG_ESP_PORT
#define VALUE(x) __STRINGIFY(x)
  // Preprocessor and compiler together will optimize away the if.
  if (strcmp("Serial1", VALUE(DEBUG_ESP_PORT)) == 0) {
    uart_buff_switch(1U);
  } else {
    uart_buff_switch(0U);
  }
#else
  uart_buff_switch(0U); // Side effect, clears RX FIFO
#endif
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
