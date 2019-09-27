#ifndef ISR_SAFE_PRINTF_H
#define ISR_SAFE_PRINTF_H
#include <stdio.h>
#include <string.h>
#include <pgmspace.h>

extern "C" {
/*
  ISR Safe to call for debugging. Not really safe to use all the time. The
  problem is going over 10us with interrupts disabled. It takes 86.8us to send
  one 8 bit character with start and stop bit at 115200 bps.
*/
int _isr_safe_printf_P(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
// Note, _isr_safe_printf_P will not handle additional string arguments in
// PROGMEM. Only the 1st parameter, fmt, is supported in PROGMEM.
#define ISR_PRINTF(fmt, ...) _isr_safe_printf_P(PSTR(fmt), ##__VA_ARGS__)
#define ISR_PRINTF_P(fmt, ...) _isr_safe_printf_P(fmt, ##__VA_ARGS__)
}

#endif