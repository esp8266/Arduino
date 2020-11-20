// ROM and blob calls without official headers available

#ifndef __ESP8266_UNDOCUMENTED_H
#define __ESP8266_UNDOCUMENTED_H

#ifdef __cplusplus
extern "C" {
#endif

// ROM

extern void rom_i2c_writeReg_Mask(int, int, int, int, int, int);
extern int rom_i2c_readReg_Mask(int, int, int, int, int);

extern int uart_baudrate_detect(int, int);

/*
ROM function, uart_buff_switch(), is used to switch printing between UART0 and
UART1. It updates a structure that only controls a select group of print
functions. ets_putc() and ets_uart_printf() are examples and are not affected by
calls to ets_install_putc1().

 Use:
   0 for UART0, also clears RX FIFO
   1 for UART1
 */
extern void uart_buff_switch(uint8_t);

/*
 ROM function, ets_uart_printf(), prints on the UART selected by
 uart_buff_switch(). Supported format options are the same as vprintf(). Also
 has cooked newline behavior. No flash format/string support; however, ISR safe.
 Also, uses a static function in ROM to print characters which is only
 controlled by uart_buff_switch().
 */
extern int ets_uart_printf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));

extern void ets_delay_us(uint32_t us);

/*
  The Boot ROM sets up a table of dispatch handlers at 0x3FFFC000.
  This table has an entry for each of the EXCCAUSE values, 0 through 63.

  Entries that do not have a specific handler are set to
  `_xtos_unhandled_exception`. This handler will execute a `break 1, 1`
  (0x4000DC4Bu) before doing a `rfe` (return from exception).  Since the PC has
  not been changed, the event that caused the 1st exception will likely keep
  repeating until the HWDT kicks in.

  This table is normally managed through calls to _xtos_set_exception_handler()
*/

/*
 Added to eagle.rom.addr.v6.ld
 PROVIDE ( _xtos_exc_handler_table = 0x3fffc000 );
*/
using _xtos_handler = void (*)(void);
extern _xtos_handler _xtos_exc_handler_table[];

#ifdef __cplusplus
};
#endif

#endif
