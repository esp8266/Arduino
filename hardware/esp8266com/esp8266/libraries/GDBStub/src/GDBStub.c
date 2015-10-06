/* GDB Server stub by Marko Mikulicic (Cesanta)

Copyright (c) 2013-2014 Cesanta Software Limited
All rights reserved

This software is dual-licensed: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation. For the terms of this
license, see <http://www.gnu.org/licenses>.

You are free to use this software under the terms of the GNU General
Public License, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

Alternatively, you can license this software under a commercial
license, as set out in <https://www.cesanta.com/license>.
*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "ets_sys.h"
#include "user_interface.h"
#include "esp8266_peri.h"

#include "xtensa/corebits.h"
#include "xtensa/specreg.h"

#define __stringify_1(x...) #x
#define __stringify(x...) __stringify_1(x)
#define RSR(sr)                                       \
  ({                                                  \
    uint32_t r;                                       \
    asm volatile("rsr %0,"__stringify(sr) : "=a"(r)); \
    r;                                                \
  })


/*
 * the saved registers begin at a fixed position in the xtos
 * low-level exception handler. I don't know if 0x100 it's just an
 * artifact of the actual xtos build ESP8266EX is using (although this nice
 * round number looks deliberate). The exception handler is burned on rom
 * so it should work on future SDK updates, but not necessarily on future
 * revisions of the chip.
 */
#define V7_GDB_SP_OFFSET 0x100

/*
 * Addresses in this range are guaranteed to be readable without faulting.
 * Contains ranges that are unmapped but innocuous.
 *
 * Putative ESP8266 memory map at:
 * https://github.com/esp8266/esp8266-wiki/wiki/Memory-Map
 */
#define ESP_LOWER_VALID_ADDRESS 0x20000000
#define ESP_UPPER_VALID_ADDRESS 0x60000000

/*
 * Constructed by xtos.
 *
 * There is a UserFrame structure in
 * ./esp_iot_rtos_sdk/extra_include/xtensa/xtruntime-frames.h
 */
struct xtos_saved_regs {
  uint32_t pc; /* instruction causing the trap */
  uint32_t ps;
  uint32_t sar;
  uint32_t vpri;  /* current xtos virtual priority */
  uint32_t a0;    /* when __XTENSA_CALL0_ABI__ is true */
  uint32_t a[16]; /* a2 - a15 */
};

/*
 * Register file in the format lx106 gdb port expects it.
 *
 * Inspired by gdb/regformats/reg-xtensa.dat from
 * https://github.com/jcmvbkbc/crosstool-NG/blob/lx106-g%2B%2B/overlays/xtensa_lx106.tar
 */
struct regfile {
  uint32_t a[16];
  uint32_t pc;
  uint32_t sar;
  uint32_t litbase;
  uint32_t sr176;
  uint32_t sr208;
  uint32_t ps;
};

#define printf ets_printf
extern void uart_write_char_d(char c);

/* TODO(mkm): not sure if gdb guarantees lowercase hex digits */
#define fromhex(c) \
  (((c) &0x40) ? ((c) &0x20 ? (c) - 'a' + 10 : (c) - 'A' + 10) : (c) - '0')
#define hexdigit(n) (((n) < 10) ? '0' + (n) : 'a' + ((n) -10))

static struct regfile regs = {0};
static uint8_t gdb_send_checksum;

int gdb_read_uart() {
    static char buf[512];
    static char pos = 0;

    if (pos == 0) {
        size_t rx_count;
        while ((rx_count = (USS(0) >> USRXC) & 0xff)>0 && pos < sizeof(buf)) {
            buf[pos++] = U0F;
        }
    }
    if (pos == 0) {
        return -1;
    }
    return buf[--pos];
}


uint8_t read_unaligned_byte(uint8_t *addr) {
  uint32_t *base = (uint32_t *) ((uintptr_t) addr & ~0x3);
  uint32_t word;

  word = *base;
  return (uint8_t)(word >> 8 * ((uintptr_t) addr & 0x3));
}

void gdb_nack() {
  printf("-");
}

void gdb_ack() {
  printf("+");
}

void gdb_begin_packet() {
  printf("$");
  gdb_send_checksum = 0;
}

void gdb_end_packet() {
  printf("#%c%c", hexdigit(gdb_send_checksum >> 4),
         hexdigit(gdb_send_checksum & 0xF));
}

void gdb_putchar(char ch) {
  gdb_send_checksum += (uint8_t) ch;
  printf("%c", ch);
}

/* output a string while computing the checksum */
void gdb_putstr(char *str) {
  while (*str) gdb_putchar(*str++);
}

void gdb_putbyte(uint8_t val) {
  gdb_putchar(hexdigit(val >> 4));
  gdb_putchar(hexdigit(val & 0xF));
}

/* 32-bit integer in native byte order */
void gdb_putint(uint32_t val) {
  int i;
  uint8_t *v = (uint8_t *) &val;
  for (i = 0; i < 4; i++) {
    gdb_putbyte(v[i]);
  }
}

/* send a gdb packet with checksum */
void gdb_send_packet(char *str) {
  gdb_begin_packet();
  gdb_putstr(str);
  gdb_end_packet();
}

uint8_t gdb_read_unaligned(uint8_t *addr) {
  if (addr < (uint8_t *) ESP_LOWER_VALID_ADDRESS ||
      addr >= (uint8_t *) ESP_UPPER_VALID_ADDRESS) {
    return 0;
  }

  return read_unaligned_byte(addr);
}

/*
 * Handles the GDB server protocol.
 * We currently support only the simple command set.
 *
 * Data is exchanged in packets like `$Cxxxxx#cc`
 * where `C` is a single letter command name, `xxxx` is some data payload,
 * and `cc` is a two digit hex checksum of the packet body.
 * Replies follow the same structure except that they lack the command symbol.
 *
 * For a more complete description of the protocol, see
 * https://sourceware.org/gdb/current/onlinedocs/gdb/Remote-Protocol.html
 */
void gdb_handle_char(int ch) {
  static enum {
    GDB_JUNK,
    GDB_DATA,
    GDB_CHECKSUM,
    GDB_CHECKSUM2
  } state = GDB_JUNK;
  static char data[128];
  static int pos = 0;
  static uint8_t checksum;

  switch (state) {
    case GDB_JUNK:
      if (ch == '$') {
        checksum = 0;
        state = GDB_DATA;
      }
      break;
    case GDB_DATA:
      if (ch == '#') {
        state = GDB_CHECKSUM;
        break;
      }
      /* ignore too long commands, by acking and sending empty response */
      if (pos > sizeof(data)) {
        state = GDB_JUNK;
        gdb_ack();
        gdb_send_packet("");
        break;
      }
      checksum += (uint8_t) ch;
      data[pos++] = ch;
      break;
    case GDB_CHECKSUM:
      if (fromhex(ch) != (checksum >> 4)) {
        gdb_nack();
        state = GDB_JUNK;
      } else {
        state = GDB_CHECKSUM2;
      }
      break;
    case GDB_CHECKSUM2:
      state = GDB_JUNK;
      if (fromhex(ch) != (checksum & 0xF)) {
        gdb_nack();
        pos = 0;
        break;
      }
      gdb_ack();

      /* process commands */
      switch (data[0]) {
        case '?':
          /* stop status */
          gdb_send_packet("S09"); /* TRAP */
          break;
        case 'm': {
          /* read memory */
          int i;
          uint32_t addr = 0;
          uint32_t num = 0;
          for (i = 1; i < pos && data[i] != ','; i++) {
            addr <<= 4;
            addr |= fromhex(data[i]);
          }
          for (i++; i < pos; i++) {
            num <<= 4;
            num |= fromhex(data[i]); /* should be decimal */
          }
          gdb_begin_packet();
          for (i = 0; i < num; i++) {
            gdb_putbyte(gdb_read_unaligned(((uint8_t *) addr) + i));
          }
          gdb_end_packet();
          break;
        }
        case 'g': {
          /* dump registers */
          int i;
          gdb_begin_packet();
          for (i = 0; i < sizeof(regs); i++) {
            gdb_putbyte(((uint8_t *) &regs)[i]);
          }
          gdb_end_packet();
          break;
        }
        default:
          gdb_send_packet("");
          break;
      }
      pos = 0;
      break;
  }
}

/* The user should detach and let gdb do the talkin' */
void gdb_server() {
  printf("waiting for gdb\n");
  /*
   * polling since we cannot wait for interrupts inside
   * an interrupt handler of unknown level.
   *
   * Interrupts disabled so that the user (or v7 prompt)
   * uart interrupt handler doesn't interfere.
   */
  xthal_set_intenable(0);
  for (;;) {
    int ch = gdb_read_uart();
    if (ch != -1) gdb_handle_char(ch);
  }
}

/*
 * xtos low level exception handler (in rom)
 * populates an xtos_regs structure with (most) registers
 * present at the time of the exception and passes it to the
 * high-level handler.
 *
 * Note that the a1 (sp) register is clobbered (bug? necessity?),
 * however the original stack pointer can be inferred from the address
 * of the saved registers area, since the exception handler uses the same
 * user stack. This might be different in other execution modes on the
 * quite variegated xtensa platform family, but that's how it works on ESP8266.
 */
void ICACHE_RAM_ATTR gdb_exception_handler(struct xtos_saved_regs *frame) {
  ESP8266_REG(0x900) &= 0x7e; // Disable WDT
  int i;
  uint32_t cause = RSR(EXCCAUSE);
  uint32_t vaddr = RSR(EXCVADDR);
  memcpy(&regs.a[2], frame->a, sizeof(frame->a));

  regs.a[0] = frame->a0;
  regs.a[1] = (uint32_t) frame + V7_GDB_SP_OFFSET;
  regs.pc = frame->pc;
  regs.sar = frame->sar;
  regs.ps = frame->ps;
  regs.litbase = RSR(LITBASE);

  U0IE = 0;
  ets_install_putc1(&uart_write_char_d);

  printf("\nTrap %d: pc=%p va=%p\n", cause, frame->pc, vaddr);
  gdb_server();

  _ResetVector();
}

void gdb_init() {
  char causes[] = {EXCCAUSE_ILLEGAL,          EXCCAUSE_INSTR_ERROR,
                   EXCCAUSE_LOAD_STORE_ERROR, EXCCAUSE_DIVIDE_BY_ZERO,
                   EXCCAUSE_UNALIGNED,        EXCCAUSE_INSTR_PROHIBITED,
                   EXCCAUSE_LOAD_PROHIBITED,  EXCCAUSE_STORE_PROHIBITED};
  int i;
  for (i = 0; i < (int) sizeof(causes); i++) {
    _xtos_set_exception_handler(causes[i], gdb_exception_handler);
  }
}
