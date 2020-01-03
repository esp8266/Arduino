/*
 core_esp8266_vm - Implements logic to enable external SRAM/PSRAM to be used
                   as if it were on-chip memory by code.

 Copyright (c) 2020 Earle F. Philhower, III   All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


 The original exception handler idea was taken from @pvvx's public domain
 misaligned-flash-read exception handler, available here:
 https://github.com/pvvx/esp8266web/blob/master/app/sdklib/system/app_main.c


 Theory of Operation:

 The Xtensa core generates a hardware exception (unrelated to C++ exceptions)
 when an address that's defined as invalid for load or store.  The XTOS ROM
 routines capture the machine state and call a standard C exception handler
 routine (or the default one which resets the system).

 We hook into this exception callback and decode the EXCVADDR (the address
 being accessed) and use the exception PC to read out the faulting
 instruction.  We decode that instruction and simulate it's behavior
 (i.e. either loading or storing some data to a register/external memory)
 and then return to the calling application.

 We use the hardware SPI interface to talk to an external SRAM/PSRAM, and
 implement a simple cache to minimize the amount of times we actually need
 to go out over the (slow) SPI bus.  The SPI is set up in a DIO mode which
 uses no more pins than normal SPI, but provides for ~2X faster transfers.

 NOTE: This works fine for processor accesses, but cannot be used by any
 of the peripherals' DMA.  For that, we'd need a real MMU.

 Hardware Configuration (make sure you have 3.3V compatible SRAMs):
 * SPI interfaced byte-addressible SRAM/PSRAM:  24LC1024 or smaller
      CS   -> GPIO15
      SCK  -> GPIO14
      MOSI -> GPIO13
      MISO -> GPIO12
   (note these are GPIO numbers, not the Arduion Dxx ones.  Refer to your
    ESP8266 board schematic for the mapping of GPIO to pin.)
 * Higher density PSRAM (ESP-PSRAM64H/etc.) should work as well, but I'm
   still waiting on my chips so haven't done any testing.  Biggest concern
   is their command set and functionality in DIO mode.  If DIO mode isn't
   supported, then a fallback to SIO or moving to QIO is a possibility.
 
*/



#include <Arduino.h>
#include <esp8266_undocumented.h>
#include "esp8266_peri.h"
#include "core_esp8266_vm.h"
#include "umm_malloc/umm_malloc.h"


extern "C" {

#define SHORT_MASK  0x000008u
#define LOAD_MASK   0x00f00fu
#define L8UI_MATCH  0x000002u
#define L16UI_MATCH 0x001002u
#define L16SI_MATCH 0x009002u
#define L16_MASK    0x001000u
#define SIGNED_MASK 0x008000u
#define L32IN_MATCH 0x000008u
#define L32I_MATCH  0x002002u
#define L32R_MATCH  0x000001u
#define L32_MASK    0x002009u

#define STORE_MASK  0x00f00fu
#define S8I_MATCH   0x004002u
#define S16I_MATCH  0x005002u
#define S16_MASK    0x001000u
#define S32I_MATCH  0x006002u
#define S32IN_MATCH 0x000009u
#define S32_MASK    0x002001u

#define EXCCAUSE_LOAD_PROHIBITED  28 // Cache Attribute does not allow Load
#define EXCCAUSE_STORE_PROHIBITED 29 // Cache Attribute does not allow Store
#define EXCCAUSE_STORE_MASK        1 // Fast way of deciding if it's a ld or s that faulted

// MINI SPI implementation inlined to have max performance and minimum code
// bloat.  Can't include a library (SPI) in the core, anyway.

// Place in a struct so hopefully compiler will generate smaller, base+offset
// based code to access it
typedef struct {
    volatile uint32_t spi_cmd; // The SPI can change this behind our backs, so volatile!
    uint32_t spi_addr;
    uint32_t spi_ctrl;
    uint32_t spi_ctrl1; // undocumented?  Not shown in the reg map
    uint32_t spi_rd_status;
    uint32_t spi_ctrl2;
    uint32_t spi_clock;
    uint32_t spi_user;
    uint32_t spi_user1;
    uint32_t spi_user2;
    uint32_t spi_wr_status;
    uint32_t spi_pin;
    uint32_t spi_slave;
    uint32_t spi_slave1;
    uint32_t spi_slave2;
    uint32_t spi_slave3;
    uint32_t spi_w[16]; // NOTE: You need a memory barrier before reading these after a read xaction
    uint32_t spi_ext3;
} spi_regs;

// The standard HSPI bus pins are used
constexpr uint8_t cs = 15;
constexpr uint8_t miso = 12;
constexpr uint8_t mosi = 13;
constexpr uint8_t sck = 14;

#define DECLARE_SPI1 spi_regs *spi1 = (spi_regs*)&SPI1CMD

constexpr uint32_t spi_clkval = 0x00041001; // 5MHz = 0x001c1001   10 = 0x000c1001   20 = 0x00041001  30 = 0x00002001  40 = 0x00001001

typedef enum { sio = 0, dio = 1 } iotype;
constexpr iotype hspi_mode = dio;

constexpr int read_delay = (hspi_mode == dio) ? 4-1 : 0;

constexpr int cache_ways = 4;   // N-way, fully associative cache
constexpr int cache_words = 16; // Must be 16 words or smaller to fit in SPI buffer

static struct cache_line {
  int32_t addr;            // Address, lower bits masked off
  int dirty;               // Needs writeback
  struct cache_line *next; // We'll keep linked list in MRU order
  union {
    uint32_t w[cache_words];
    uint16_t s[cache_words * 2];
    uint8_t  b[cache_words * 4];
  };
} __vm_cache_line[cache_ways];
static struct cache_line *__vm_cache; // Always points to MRU (hence the line being read/written)

constexpr int addrmask = ~(sizeof(__vm_cache[0].w)-1); // Helper to mask off bits present in cache entry


static void spi_init(spi_regs *spi1)
{
  pinMode(sck, SPECIAL);
  pinMode(miso, SPECIAL);
  pinMode(mosi, SPECIAL);
  pinMode(cs, SPECIAL);
  spi1->spi_cmd = 0;
  GPMUX &= ~(1 << 9);
  spi1->spi_clock = spi_clkval;
  spi1->spi_ctrl = 0 ; // MSB first + plain SPI mode
  spi1->spi_ctrl1 = 0; // undocumented, clear for safety?
  spi1->spi_ctrl2 = 0; // No add'l delays on signals
  spi1->spi_user2 = 0; // No insn or insn_bits to set
}

// Note: GCC optimization -O2 and -O3 tried and returned *slower* code than the default

// The SPI hardware cannot make the "command" portion dual or quad, only the addr and data
// So using the command portion of the cycle will not work.  Comcatenate the address
// and command into a single 32-bit chunk "address" which will be sent across both bits.

inline ICACHE_RAM_ATTR void spi_writetransaction(spi_regs *spi1, int addr, int addr_bits, int dummy_bits, int data_bits, iotype dual)
{
  // Ensure no writes are still ongoing
  while (spi1->spi_cmd & SPIBUSY) { /* busywait */ }

  spi1->spi_addr = addr;
  spi1->spi_user = (addr_bits? SPIUADDR : 0) | (dummy_bits ? SPIUDUMMY : 0) | (data_bits ? SPIUMOSI : 0) | (dual ? SPIUFWDIO : 0);
  spi1->spi_user1 = (addr_bits << 26) | (data_bits << 17) | dummy_bits;
  // No need to set spi_user2, insn field never used
  __asm ( "" ::: "memory" );
  spi1->spi_cmd = SPIBUSY;
  // The write may continue on in the background, letting core do useful work instead of waiting, unless we're in cacheless mode
  if (cache_ways == 0) {
    while (spi1->spi_cmd & SPIBUSY) { /* busywait */ }
  }
}

inline ICACHE_RAM_ATTR uint32_t spi_readtransaction(spi_regs *spi1, int addr, int addr_bits, int dummy_bits, int data_bits, iotype dual)
{
  // Ensure no writes are still ongoing
  while (spi1->spi_cmd & SPIBUSY) { /* busywait */ }

  spi1->spi_addr = addr;
  spi1->spi_user = (addr_bits? SPIUADDR : 0) | (dummy_bits ? SPIUDUMMY : 0) | SPIUMISO | (dual ? SPIUFWDIO : 0);
  spi1->spi_user1 = (addr_bits << 26) | (data_bits << 8) | dummy_bits;
  // No need to set spi_user2, insn field never used
  __asm ( "" ::: "memory" );
  spi1->spi_cmd = SPIBUSY;
  while (spi1->spi_cmd & SPIBUSY) { /* busywait */ }
  __asm ( "" ::: "memory" );
  return spi1->spi_w[0];
}

static inline ICACHE_RAM_ATTR void cache_flushrefill(spi_regs *spi1, int addr)
{
  addr &= addrmask;
  struct cache_line *way = __vm_cache;

  if (__vm_cache->addr == addr) return; // Fast case, it already is the MRU
  struct cache_line *last = way;
  way = way->next;

  for (auto i = 1; i < cache_ways; i++) {
    if (way->addr == addr) {
      last->next = way->next;
      way->next = __vm_cache;
      __vm_cache = way;
      return;
    } else {
      last = way;
      way = way->next;
    }
  }

  // At this point we know the line is not in the cache and way points to the LRU.

  // We allow reads to go before writes since the write can happen in the background.
  // We need to keep the data to be written back since it will be overwritten with read data
  uint32_t wb[cache_words];
  if (last->dirty) {
    memcpy(wb, last->w, sizeof(last->w));
  }

  // Update MRU info, list
  last->next = __vm_cache;
  __vm_cache = last;

  // Do the actual read
  spi_readtransaction(spi1, (0x03 << 24) | addr, 32-1, read_delay, sizeof(last->w) * 8 - 1, hspi_mode);
  memcpy(last->w, spi1->spi_w, sizeof(last->w));

  // We fire a background writeback now, if needed
  if (last->dirty) {
    memcpy(spi1->spi_w, wb, sizeof(wb));
    spi_writetransaction(spi1, (0x02 << 24) | last->addr, 32-1, 0, sizeof(last->w) * 8 - 1, hspi_mode);
    last->dirty = 0;
  }

  // Update the addr at this point since we no longer need the old one
  last->addr = addr;
}

static inline ICACHE_RAM_ATTR void spi_ramwrite(spi_regs *spi1, int addr, int data_bits, uint32_t val)
{
  if (cache_ways == 0) {
    spi1->spi_w[0] = val;
    spi_writetransaction(spi1, (0x02<<24) | addr, 32-1, 0, data_bits, hspi_mode);
  } else {
    cache_flushrefill(spi1, addr);
    __vm_cache->dirty = 1;
    addr -= __vm_cache->addr;
    switch (data_bits) {
      case 31: __vm_cache->w[addr >> 2] = val; break;
      case  7: __vm_cache->b[addr] = val; break;
      default: __vm_cache->s[addr >> 1] = val; break;
    }
  }
}

static inline ICACHE_RAM_ATTR uint32_t spi_ramread(spi_regs *spi1, int addr, int data_bits)
{
  if (cache_ways == 0) {
    spi1->spi_w[0] = 0;
    return spi_readtransaction(spi1, (0x03 << 24) | addr, 32-1, read_delay, data_bits, hspi_mode);
  } else {
    cache_flushrefill(spi1, addr);
    addr -= __vm_cache->addr;
    switch (data_bits) {
      case 31: return __vm_cache->w[addr >> 2];
      case  7: return __vm_cache->b[addr];
      default: return __vm_cache->s[addr >> 1];
    }
  }
}

static void (*__old_handler)(struct __exception_frame *ef, uint32_t cause);

static ICACHE_RAM_ATTR void loadstore_exception_handler(struct __exception_frame *ef, uint32_t cause)
{
  uint32_t epc1 = ef->epc;
  uint32_t excvaddr;
  uint32_t insn;

  __asm (
    "rsr   %0, EXCVADDR;"    // read out the faulting address
    "movi  a4, ~3;"          // prepare a mask for the EPC
    "and   a4, a4, %2;"      // apply mask for 32bit aligned base
    "l32i  a5, a4, 0;"       // load part 1
    "l32i  a6, a4, 4;"       // load part 2
    "ssa8l %2;"              // set up shift register for src op
    "src   %1, a6, a5;"      // right shift to get faulting instruction
    :"=r"(excvaddr), "=r"(insn)
    :"r"(epc1)
    :"a4", "a5", "a6"
  );

  // Check that we're really accessing VM and not some other illegal range
  if ((excvaddr >> 28) != 1) {
    // Reinstall the old handler, and retry the instruction to keep us out of the stack dump
    _xtos_set_exception_handler(EXCCAUSE_LOAD_PROHIBITED, __old_handler);
    _xtos_set_exception_handler(EXCCAUSE_STORE_PROHIBITED, __old_handler);
    return;
  }

  DECLARE_SPI1;
  ef->epc += (insn & SHORT_MASK) ? 2 : 3; // resume at following instruction

  int regno = (insn & 0x0000f0u) >> 4;
  if (regno != 0) --regno;  // account for skipped a1 in exception_frame

  if (cause & EXCCAUSE_STORE_MASK) {
    uint32_t val = ef->a_reg[regno];
    uint32_t what = insn & STORE_MASK;
    if (what == S8I_MATCH) {
       spi_ramwrite(spi1, excvaddr & 0x1ffff, 8-1, val);
    } else if (what == S16I_MATCH) {
      spi_ramwrite(spi1, excvaddr & 0x1ffff, 16-1, val);
    } else {
      spi_ramwrite(spi1, excvaddr & 0x1ffff, 32-1, val);
    }
  } else {
    if (insn & L32_MASK) {
      ef->a_reg[regno] = spi_ramread(spi1, excvaddr & 0x1ffff, 32-1);
    } else if (insn & L16_MASK) {
      ef->a_reg[regno] = spi_ramread(spi1, excvaddr & 0x1ffff, 16-1);
      if ((insn & SIGNED_MASK ) && (ef->a_reg[regno] & 0x8000))
        ef->a_reg[regno] |= 0xffff0000;
    } else {
      ef->a_reg[regno] = spi_ramread(spi1, excvaddr & 0x1ffff, 8-1);
    }
  }
}

void install_vm_exception_handler()
{
  // There is no way to get the current exception handler via an API, so pull it
  // out using the decoded ROM dump which points to 0x3fffc100 as the start of
  // the XTOS exception table (of C-callable functions)
  uint32_t old = *(uint32_t*)(0x3fffc100 + EXCCAUSE_LOAD_PROHIBITED * 4);
  __old_handler = (void (*)(struct __exception_frame *ef, uint32_t cause))old;
  _xtos_set_exception_handler(EXCCAUSE_LOAD_PROHIBITED, loadstore_exception_handler);
  _xtos_set_exception_handler(EXCCAUSE_STORE_PROHIBITED, loadstore_exception_handler);

  DECLARE_SPI1;

  // Manually reset chip from DIO to SIO mode (HW SPI has issues with <8 bits/clocks total output)
  digitalWrite(cs, HIGH);
  digitalWrite(mosi, HIGH);
  digitalWrite(miso, HIGH);
  digitalWrite(sck, LOW);
  pinMode(cs, OUTPUT);
  pinMode(miso, OUTPUT);
  pinMode(mosi, OUTPUT);
  pinMode(sck, OUTPUT);
  digitalWrite(cs, LOW);
  for (int i = 0; i < 4; i++) {
    digitalWrite(sck, HIGH);
    digitalWrite(sck, LOW);
  }
  digitalWrite(cs, HIGH);

  // Set up the SPI regs
  spi_init(spi1);

  // Enable streaming read/write mode
  spi1->spi_w[0] = 0x40;
  spi_writetransaction(spi1, 0x01<<24, 8-1, 0, 8-1, sio);

  if (hspi_mode == dio) {
    // Ramp up to DIO mode
    spi_writetransaction(spi1, 0x3b<<24, 8-1, 0, 0, sio);
    spi1->spi_ctrl |= SPICDIO | SPICFASTRD;
  }

  // Bring cache structures to baseline
  if (cache_ways > 0) {
    for (auto i = 0; i < cache_ways; i++) {
      __vm_cache_line[i].addr = -1; // Invalid, bits set in lower region so will never hatch
      __vm_cache_line[i].next = &__vm_cache_line[i+1];
    }
    __vm_cache = &__vm_cache_line[0];
    __vm_cache_line[cache_ways - 1].next = NULL;
  }

  // Hook into memory manager
  umm_init_vm( (void *)0x10000000, 0x20000);
}


};
