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
  * ets_vprintf - by it self is safe.
  * newlibc printf - not safe - lives in flash.
  * newlibc snprintf - not safe - lives in flash.
  * builtin putc1 print function - Is installed when you use
    ets_install_uart_printf. Which calls ets_install_putc1. The selection of UART
    is performed by calling uart_buff_switch with 0 for UART0 and 1 for UART1.
    This should work for our purpose here, if handled as follows:
    * call uart_buff_switch at each printf call to reselect UART
    * use a stack buffer to hold a copy the PROGMEM string to print from.
    * use ets_vprintf for printing with putc1 function.

  Research TODO, Unknowns:
  * ets_printf_plus - is it safe?
    * check if it uses alloc? the old versions used malloc for PROGMEM, if
      that fails, they try to use the stack for PROGMEM.
    * confirmed it is in IRAM in SDK!
  * Is there a problem with the ROM "serial print functions"?
    Rtos SDK does not use them. igrr also didn't use them in ...postmortem.
    See "ets_printf" above.

 */

#include <stdio.h>
#include <string.h>
#include <pgmspace.h>
#include <core_esp8266_features.h>

extern "C" {

#if 1


int _isr_safe_printf_P(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
// Note, _isr_safe_printf_P will not handle additional string arguments in
// PROGMEM. Only the 1st parameter, fmt, is supported in PROGMEM.
#define ISR_PRINTF(fmt, ...) _isr_safe_printf_P(PSTR(fmt), ##__VA_ARGS__)
#define ISR_PRINTF_P(fmt, ...) _isr_safe_printf_P(fmt, ##__VA_ARGS__)

//D int (*f)(int) ets_install_uart_printf(void);
//D int (*_rom_putc1)(int) = (int (*)(int))0x40001dcc;

// ROM _putc1, ignores CRs and sends CR/LF for LF, newline.
// Always returns character sent.
typedef int (*fptr_t)(int);
fptr_t ets_install_uart_printf(void);
void uart_buff_switch(uint8_t);

int _isr_safe_printf_P(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
int ICACHE_RAM_ATTR _isr_safe_printf_P(const char *fmt, ...) {
    // This will replace, if any, the UART handler that was installed by uart.cpp
    // calls ets_install_putc1.
    fptr_t _rom_putc1 = ets_install_uart_printf();
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

#if 0
// Alternate print driver

#ifdef DEBUG_ESP_PORT
#define VALUE(x) __STRINGIFY(x)

void ICACHE_RAM_ATTR uart_write_char_d(char c) {
    // Preprocessor and compiler together will optimize away the if.
    if (strcmp("Serial", VALUE(DEBUG_ESP_PORT)) == 0) {
        // USS - get uart{0,1} status word
        // USTXC - bit offset to TX FIFO count, 8 bit field
        // USF - Uart FIFO
        // Wait for space for two or more characters.
        while (((USS(0) >> USTXC) & 0xff) >= 0x7e) { }

        if (c == '\n') {
            USF(0) = '\r';
        }
        USF(0) = c;
    } else {
        while (((USS(1) >> USTXC) & 0xff) >= 0x7e) { }

        if (c == '\n') {
            USF(1) = '\r';
        }
        USF(1) = c;
    }
}
#else // ! DEBUG_ESP_PORT
void ICACHE_RAM_ATTR uart_write_char_d(char c) {
    uart0_write_char_d(c);
    uart1_write_char_d(c);
}

void ICACHE_RAM_ATTR uart0_write_char_d(char c) {
    while (((USS(0) >> USTXC) & 0xff)) { }

    if (c == '\n') {
        USF(0) = '\r';
    }
    USF(0) = c;
}

void ICACHE_RAM_ATTR uart1_write_char_d(char c) {
    while (((USS(1) >> USTXC) & 0xff) >= 0x7e) { }

    if (c == '\n') {
        USF(1) = '\r';
    }
    USF(1) = c;
}
#endif

#endif

};
