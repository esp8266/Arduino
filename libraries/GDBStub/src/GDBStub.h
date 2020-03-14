#ifndef GDBSTUB_H
#define GDBSTUB_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <gdb_hooks.h>
#include "internal/gdbstub-cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

void gdbstub_init();

//Indicates whether gdbstub will attach to these or not
//Useful for other uart libs to avoid conflicts
bool gdbstub_has_putc1_control();
bool gdbstub_has_uart_isr_control();

#if GDBSTUB_REDIRECT_CONSOLE_OUTPUT
void gdbstub_set_putc1_callback(void (*callback)(char));
#endif

void gdbstub_write_char(char c);
void gdbstub_write(const char* buf, size_t size);

#if GDBSTUB_CTRLC_BREAK && !GDBSTUB_FREERTOS
void gdbstub_set_uart_isr_callback(void (*callback)(void*, uint8_t), void* arg);

//Override points for enabling tx and rx pins for uart0
void gdbstub_hook_enable_tx_pin_uart0(uint8_t pin);
void gdbstub_hook_enable_rx_pin_uart0(uint8_t pin);
#endif

#ifdef __cplusplus
}
#endif

#endif
