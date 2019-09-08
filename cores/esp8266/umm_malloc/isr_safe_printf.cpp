/*
 * isr_safe_printf.cpp - Intended for diagnostic printing from a restricted setting.
 *
 * Meant to be a print from anywhere and work. There will be more limitations
 * than your typical printf function.
 *
 * Still researching options for printing.
 */

/*
  Printing from the malloc routines is tricky. Since a print library
  might call *alloc. Then recusion may follow as each error call may fail
  into another error and so on.

  Objective:  To be able to print "last gasp" diagnostic messages
  when interrupts are disabled and w/o availability of heap resources.

  Considerations:
  * can be called from ISR
  * can be called from malloc code, cannot use malloc
  * can be called from malloc code that was called from an ISR
  * can be called from with in a critical section, eg. xt_rsil(15);
    * this may be effectively the same as being called from an ISR?

  Knowns:
  * ets_printf - For RTOS SDK they replaced this function with one in the SDK.
    Most of the problems I can see with ets_printf center around not being
    able to maintain a port to thread context. That is you cannot have one
    thread using one port while another thread uses the other. In the no OS
    case we cannot have one area of code using one port and another area of
    code using the other port. Most of the ROM printf functions are not built
    to support this kind of usage. Things get especially dangerous when you
    try to use the ...external_printf stuff.
  * ets_vprintf - by itself is safe.
  * newlibc printf - not safe - lives in flash.
  * newlibc snprintf - not safe - lives in flash.
  * builtin putc1 print function - Is installed when you use
    ets_install_uart_printf. Which calls ets_install_putc1. The selection of UART
    is performed by calling uart_buff_switch with 0 for UART0 and 1 for UART1.
    This should work for our purpose here, if handled as follows:
    * call uart_buff_switch at each printf call to reselect UART
    * use a stack buffer to hold a copy the PROGMEM string to print from.
    * use ets_vprintf for printing with putc1 function.
  * os_printf_plus looks interesting. It is in IRAM. If no heap is available it
    will use up to 64 bytes of stack space to copy a PROGMEM fmt for printing.
    Issues:
    * Printing is turned off by system_set_os_print
    * putc1 needs to be in IRAM - this is a uart.cpp issue
    * Need to force system_get_free_heap_size to return 0 during critical periods.
    * won't work for umm_info it prints over 64 characters.
    * along with umm_info there are other debug messages that exceed 64 characters.

  Research TODO, Unknowns:
  * Is there a problem with the ROM "serial print functions"?
    Rtos SDK does not use them. igrr also didn't use them in ...postmortem.
    See "ets_printf" above.

 */

#include <stdio.h>
#include <string.h>
#include <pgmspace.h>
#include <core_esp8266_features.h>

extern "C" {

#if defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_OOM) || defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)

int _isr_safe_printf_P(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
// Note, _isr_safe_printf_P will not handle additional string arguments in
// PROGMEM. Only the 1st parameter, fmt, is supported in PROGMEM.
#define ISR_PRINTF(fmt, ...) _isr_safe_printf_P(PSTR(fmt), ##__VA_ARGS__)
#define ISR_PRINTF_P(fmt, ...) _isr_safe_printf_P(fmt, ##__VA_ARGS__)

// Boot ROM _putc1, ignores CRs and sends CR/LF for LF, newline.
// Always returns character sent.
typedef int (*fp_putc_t)(int);
#define _rom_putc1 ((fp_putc_t)0x40001dcc)

void uart_buff_switch(uint8_t);

int _isr_safe_printf_P(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
int ICACHE_RAM_ATTR _isr_safe_printf_P(const char *fmt, ...) {

//C This #ifdef block is obsolete if the PR for ets_putc UART selection is approved.
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
//C - end

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
