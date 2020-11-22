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
  The Boot ROM sets up a table of dispatch handlers at 0x3FFFC000. This table
  has an entry for each of the EXCCAUSE values, 0 through 63. The exception
  handler at the `User Exception Vector` uses EXCCAUSE with the base address
  0x3FFFC000 to build a jump address to the respective cause handler. Of the
  cause handle functions `_xtos_c_wrapper_handler` and `_xtos_unhandled_exception`
  are of interest.

  Entries that do not have a specific handler are set to
  `_xtos_unhandled_exception`. This handler will execute a `break 1, 1`
  (0x4000DC4Bu) before doing a `rfe` (return from exception).  Since the PC has
  not changed, the event that caused the 1st exception will likely keep
  repeating until the HWDT kicks in.

  These exception handling functions are in assembly, and do not conform to the
  typical "C" function conventions. However, some form of prototype is needed to
  reference these function addresses in "C" code. In
  `RTOS_SDK/components/esp8266/include/xtensa/xtruntime.h`, it uses a compounded
  definition that equates to `void (*)(...)` for .cpp modules to use. I have
  noticed this creates sufficient confusion at compilation to get your attention
  when used in the wrong place. I have copied that definition here.

  Unfortunately RTOS_SDK and other ESP projects on the Internet also reuses
  this loose definition, _xtos_handler, for a function table of "C" callable
  functions, `_xtos_c_handler_table`. This issue falls into a different PR so I
  will leave it for that PR where there is more context.
//C TODO: Update this last paragraph when merged with related PR.
*/

// This is copy/paste from RTOS_SDK/components/esp8266/include/xtensa/xtruntime.h
#ifdef __cplusplus
typedef void (_xtos_handler_func)(...);
#else
typedef void (_xtos_handler_func)();
#endif
typedef _xtos_handler_func *_xtos_handler;

/*
  Added to eagle.rom.addr.v6.ld
  PROVIDE ( _xtos_exc_handler_table = 0x3fffc000 );
*/
extern _xtos_handler _xtos_exc_handler_table[];

#ifdef __cplusplus
};
#endif

#endif
