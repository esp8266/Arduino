/* 020819
   Based on PR https://github.com/esp8266/Arduino/pull/6978
   Enhanced to also handle store operations to iRAM and optional range
   validation. Also improved failed path to generate crash report.
   And, partially refactored.

   Apologies if this is being pedantic, I was getting confused over these so
   I tried to understand what makes them different.

   EXCCAUSE_LOAD_STORE_ERROR 3 is a non-32-bit load or store to an address that
   only supports a full 32-bit aligned transfer like IRAM or ICACHE. i.e., No
   8-bit char or 16-bit short transfers allowed.

   EXCCAUSE_UNALIGNED 9 is an exception cause when load or store is not on an
   aligned boundary that matches the element's width.
   eg. *(short *)0x3FFF8001 = 1; or *(long *)0x3FFF8002 = 1;

 */

/*
 * This exception handler handles EXCCAUSE_LOAD_STORE_ERROR. It allows for a
 * byte or short access to iRAM or PROGMEM to succeed without causing a crash.
 * When reading, it is still preferred to use the xxx_P macros when possible
 * since they are probably 30x faster than this exception handler method.
 *
 * Code taken directly from @pvvx's public domain code in
 * https://github.com/pvvx/esp8266web/blob/master/app/sdklib/system/app_main.c
 *
 *
 */

#include <Arduino.h>
#define VERIFY_C_ASM_EXCEPTION_FRAME_STRUCTURE
#include <esp8266_undocumented.h>
#include <core_esp8266_non32xfer.h>
#include <mmu_iram.h>
#include <Schedule.h>
#include <debug.h>

// All of these optimization were tried and now work
// These results were from irammem.ino using GCC 10.2
// DRAM reference                    uint16    9 AVG cycles/transfer
// #pragma GCC optimize("O0")     // uint16, 289 AVG cycles/transfer, IRAM: +180
// #pragma GCC optimize("O1")     // uint16, 241 AVG cycles/transfer, IRAM: +16
#pragma GCC optimize("O2")     // uint16, 230 AVG cycles/transfer, IRAM: +4
// #pragma GCC optimize("O3")     // uint16, 230 AVG cycles/transfer, IRAM: +4
// #pragma GCC optimize("Ofast")  // uint16, 230 AVG cycles/transfer, IRAM: +4
// #pragma GCC optimize("Os")     // uint16, 233 AVG cycles/transfer, IRAM: 27556  +0

extern "C" {

#define LOAD_MASK   0x00f00fu
#define L8UI_MATCH  0x000002u
#define L16UI_MATCH 0x001002u
#define L16SI_MATCH 0x009002u
#define S8I_MATCH   0x004002u
#define S16I_MATCH  0x005002u

#define EXCCAUSE_LOAD_STORE_ERROR 3 /* Non 32-bit read/write error */

static fn_c_exception_handler_t old_c_handler = NULL;

static
IRAM_ATTR void non32xfer_exception_handler(struct __exception_frame *ef, int cause)
{
  do {
    uint32_t insn, excvaddr;

    /* Extract instruction and faulting data address */
    __EXCEPTION_HANDLER_PREAMBLE(ef, excvaddr, insn);

    uint32_t what = insn & LOAD_MASK;
    uint32_t valmask = 0;

    uint32_t is_read = 1;
    if (L8UI_MATCH == what || S8I_MATCH == what) {
      valmask = 0xffu;
      if (S8I_MATCH == what) {
        is_read = 0;
      }
    } else if (L16UI_MATCH == what || L16SI_MATCH == what || S16I_MATCH == what) {
      valmask = 0xffffu;
      if (S16I_MATCH == what) {
        is_read = 0;
      }
    } else {
      continue; /* fail */
    }

    int regno = (insn & 0x0000f0u) >> 4;
    if (regno == 1) {
      continue;              /* we can't support storing into a1, just die */
    } else if (regno != 0) {
      --regno;               /* account for skipped a1 in exception_frame */
    }

#ifdef DEBUG_ESP_MMU
    /* debug option to validate address so we don't hide memory access bugs in APP */
    if (mmu_is_iram((void *)excvaddr) || (is_read && mmu_is_icache((void *)excvaddr))) {
      /* all is good  */
    } else {
      continue;  /* fail */
    }
#endif
    {
      uint32_t *pWord = (uint32_t *)(excvaddr & ~0x3);
      uint32_t pos = (excvaddr & 0x3) * 8;
      uint32_t mem_val = *pWord;

      if (is_read) {
        /* shift and mask down to correct size */
        mem_val >>= pos;
        mem_val &= valmask;

        /* Sign-extend for L16SI, if applicable */
        if (what == L16SI_MATCH && (mem_val & 0x8000)) {
          mem_val |= 0xffff0000;
        }

        ef->a_reg[regno] = mem_val;  /* carry out the load */

      } else { /* is write */
        uint32_t val = ef->a_reg[regno];  /* get value to store from register */
        val <<= pos;
        valmask <<= pos;
        val &= valmask;

        /* mask out field, and merge */
        mem_val &= (~valmask);
        mem_val |= val;
        *pWord = mem_val; /* carry out the store */
      }
    }

    ef->epc += 3;            /* resume at following instruction */
    return;

  } while(false);

/* Fail request, die */
  /*
    The old handler points to the SDK. Be alert for HWDT when Calling with
    INTLEVEL != 0. I cannot create it any more. I thought I saw this as a
    problem; however, my test case shows no problem ?? Maybe I was confused.
   */
  if (old_c_handler) { // if (0 == (ef->ps & 0x0F)) {
    DBG_MMU_PRINTF("\ncalling previous load/store handler(%p)\n", old_c_handler);
    old_c_handler(ef, cause);
    return;
  }

  /*
    Calling _xtos_unhandled_exception(ef, cause) in the Boot ROM, gets us a
    hardware wdt.

    Use panic instead as a fall back. It will produce a stack trace.
   */
  panic();
}

/*
  To operate reliably, this module requires the new
  `_xtos_set_exception_handler` from `exc-sethandler.cpp` and
  `_xtos_c_wrapper_handler` from `exc-c-wrapper-handler.S`. See comment block in
  `exc-sethandler.cpp` for details on issues with interrupts being enabled by
  "C" wrapper.
 */
void install_non32xfer_exception_handler(void) __attribute__((weak));
void install_non32xfer_exception_handler(void) {
  if (NULL == old_c_handler) {
    // Set the "C" exception handler the wrapper will call
    old_c_handler =
    _xtos_set_exception_handler(EXCCAUSE_LOAD_STORE_ERROR,
      non32xfer_exception_handler);
  }
}

};
