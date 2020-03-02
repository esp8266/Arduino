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

/* The Xtensa OS code in ROM for handling hardware exceptions */
struct __exception_frame
{
  uint32_t epc;
  uint32_t ps;
  uint32_t sar;
  uint32_t unused;
  union {
    struct {
      uint32_t a0;
      // note: no a1 here!
      uint32_t a2;
      uint32_t a3;
      uint32_t a4;
      uint32_t a5;
      uint32_t a6;
      uint32_t a7;
      uint32_t a8;
      uint32_t a9;
      uint32_t a10;
      uint32_t a11;
      uint32_t a12;
      uint32_t a13;
      uint32_t a14;
      uint32_t a15;
    };
    uint32_t a_reg[15];
  };
  uint32_t cause;
};

/*
 ROM Function, _xtos_set_exception_handler(), installs a "C" callable exception handler.
 If reason is out of range, >=64, it returns NULL.
 If the new exception handler is installed, it returns the previous handler.
 If the previous handler was _xtos_unhandled_exception/_xtos_p_none, it returns
 NULL.

 Note, the installed "C" exception handler is called from the
 _xtos_c_wrapper_handler with IRQs enabled.
 */
typedef void (*fn_exception_handler_t)(struct __exception_frame *ef, uint32_t cause);
extern fn_exception_handler_t _xtos_set_exception_handler(uint32_t reason, fn_exception_handler_t fn);

extern void _xtos_unhandled_exception(struct __exception_frame *ef, uint32_t cause);

#ifdef __cplusplus
};
#endif

#endif
