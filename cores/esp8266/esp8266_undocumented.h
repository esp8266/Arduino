// ROM and blob calls without official headers available

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

#ifdef __cplusplus
};
#endif
