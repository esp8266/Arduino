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

#ifndef GDBSTUB_H
/*
  GDBSTUB duplicates these with some variances that are not compatible with our
  references (offsets), which are synced with those used by the BootROM. In
  particular, the BootROM does not have register "a1" in the structure where
  GDBSTUB does.
*/

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
/*?
 TODO: A set of static_asserts to confim we are matching these.

 Extracted from information at
 From https://github.com/fdivitto/ESPWebFramework/blob/master/SDK/xtensa-lx106-elf/xtensa-lx106-elf/lib/libhandlers-null.txt

#define UEXC_pc             0x0000
#define UEXC_ps             0x0004
#define UEXC_sar            0x0008
#define UEXC_vpri           0x000c
#define UEXC_a0             0x0010
#define UEXC_a2             0x0014
#define UEXC_a3             0x0018
#define UEXC_a4             0x001c
#define UEXC_a5             0x0020
#define UEXC_a6             0x0024
#define UEXC_a7             0x0028
#define UEXC_a8             0x002c
#define UEXC_a9             0x0030
#define UEXC_a10            0x0034
#define UEXC_a11            0x0038
#define UEXC_a12            0x003c
#define UEXC_a13            0x0040
#define UEXC_a14            0x0044
#define UEXC_a15            0x0048
#define UEXC_exccause       0x004c
#define UserFrameSize       0x0050
#define UserFrameTotalSize  0x0100
*/

/*
 Most of the comments here are gleamed from the xtensa files found at the site
 listed below and are mostly unverified:
 https://github.com/qca/open-ath9k-htc-firmware/tree/master/sboot/magpie_1_1/sboot/athos/src/xtos
  * exc-c-wrapper-handler.S
  * exc-sethandler.c
*/

/*
 ROM Function, _xtos_set_exception_handler(), installs a "C" callable exception handler.
  * If reason is out of range, >=64, it returns NULL.
  * If the new exception handler is installed, it returns the previous handler.
  * If the previous handler was _xtos_unhandled_exception/_xtos_p_none, it
    returns NULL.

 Note, the installed "C" exception handler is called from the
 _xtos_c_wrapper_handler with IRQs enabled.
 */
typedef void (*fn_exception_handler_t)(struct __exception_frame *ef, uint32_t cause);
fn_exception_handler_t _xtos_set_exception_handler(uint32_t reason, fn_exception_handler_t fn);

#endif

/*
 Added to eagle.rom.addr.v6.ld
 PROVIDE ( _xtos_exc_handler_table = 0x3fffc000 );
 PROVIDE ( _xtos_c_handler_table = 0x3fffc100 );
*/
#ifdef __cplusplus
typedef void (_xtos_handler_func)(...);

/*
 Assembly-level handler, used in the _xtos_exc_handler_table[], a wrapper for
 calling registered "C" exception handlers.
*/
void _xtos_c_wrapper_handler(...);

/*
 Assembly-level handler, used in the _xtos_exc_handler_table[], for exceptions
 without a registered handler.
*/
void _xtos_unhandled_exception(...);

/*
 The default/empty "C" exception handler, used in the _xtos_c_handler_table[],
 when an exception handler has not been registered.
 Supply this to _xtos_set_exception_handler() to reset an exception handler
 back to the unhandled state, _xtos_unhandled_exception.
 Note, if you have nesting handlers this is much more complicated than this.
*/
void _xtos_p_none(void);

#else
typedef void (_xtos_handler_func)();
void	_xtos_c_wrapper_handler();
void _xtos_unhandled_exception();
void _xtos_p_none();

#endif

typedef _xtos_handler_func *_xtos_handler;

extern _xtos_handler _xtos_exc_handler_table[];
extern _xtos_handler _xtos_c_handler_table[];

#ifdef __cplusplus
};
#endif


#endif
