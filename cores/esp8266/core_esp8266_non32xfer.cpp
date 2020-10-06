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
 * This exception handler, allows for byte or short accesses to iRAM or PROGMEM
 * to succeed without causing a crash. It is still preferred to use the xxx_P
 * macros whenever possible, since they are probably 30x faster than this
 * exception handler method.
 *
 * Code taken directly from @pvvx's public domain code in
 * https://github.com/pvvx/esp8266web/blob/master/app/sdklib/system/app_main.c
 *
 *
 */

#include <Arduino.h>
#include <core_esp8266_non32xfer.h>
#include <esp8266_undocumented.h>
#include <mmu_iram.h>
#include <Schedule.h>
#include <debug.h>

extern "C" {

#define LOAD_MASK   0x00f00fu
#define L8UI_MATCH  0x000002u
#define L16UI_MATCH 0x001002u
#define L16SI_MATCH 0x009002u
#define S8I_MATCH   0x004002u
#define S16I_MATCH  0x005002u

#define EXCCAUSE_LOAD_STORE_ERROR 3 /* Non 32-bit read/write error */

static fn_exception_handler_t old_handler = NULL;

static IRAM_ATTR void non32xfer_exception_handler(struct __exception_frame *ef, uint32_t cause)
{
  do {
    /*
       Had to split out some of the asm, compiler was reusing a register that it
       needed later. A crash would come or go away with the slightest unrelated
       changes elsewhere in the function.

       Register a15 was used for epc1, then clobbered for rsr. Maybe an
       __asm("":::"memory") before starting the asm would help for these cases.
       For this instance moved setting epc1 closer to where it was used.
       Edit. "&" on output register would have resolved the problem.
       Refactored to reduce and consolidate register usage.
     */
    uint32_t insn;
    __asm(
      "movi  %0, ~3\n\t"         /* prepare a mask for the EPC */
      "and   %0, %0, %1\n\t"     /* apply mask for 32bit aligned base */
      "ssa8l %1\n\t"             /* set up shift register for src op */
      "l32i  %1, %0, 0\n\t"      /* load part 1 */
      "l32i  %0, %0, 4\n\t"      /* load part 2 */
      "src   %0, %0, %1\n\t"     /* right shift to get faulting instruction */
      :"=&r"(insn)
      :"r"(ef->epc)
      :
    );

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

    uint32_t excvaddr;
    /* read out the faulting address */
    __asm("rsr %0, EXCVADDR;" :"=r"(excvaddr)::);

#ifdef DEBUG_ESP_MMU
    /* debug option to validate address so we don't hide memory access bugs in APP */
    if (mmu_is_iram((void *)excvaddr) || (is_read && mmu_is_icache((void *)excvaddr))) {
      /* all is good  */
    } else {
      continue;  /* fail */
    }
#endif

    if (is_read) {
      /* Load, shift and mask down to correct size */
      uint32_t val = (*(uint32_t *)(excvaddr & ~0x3));
      val >>= (excvaddr & 0x3) * 8;
      val &= valmask;

      /* Sign-extend for L16SI, if applicable */
      if (what == L16SI_MATCH && (val & 0x8000)) {
        val |= 0xffff0000;
      }

      ef->a_reg[regno] = val;  /* carry out the load */

    } else { /* is write */
      uint32_t val = ef->a_reg[regno];  /* get value to store from register */
      val <<= (excvaddr & 0x3) * 8;
      valmask <<= (excvaddr & 0x3) * 8;
      val &= valmask;

      /* Load, mask out field, and merge */
      uint32_t dst_val = (*(uint32_t *)(excvaddr & ~0x3));
      dst_val &= (~valmask);
      dst_val |= val;
      (*(uint32_t *)(excvaddr & ~0x3)) = dst_val; /* carry out the store */
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
  if (old_handler) { // if (0 == (ef->ps & 0x0F)) {
    DBG_MMU_PRINTF("\ncalling previous load/store handler(%p)\n", old_handler);
    old_handler(ef, cause);
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
  An issue, the Boot ROM "C" wrapper for exception handlers,
  _xtos_c_wrapper_handler, turns interrupts back on. To address this issue we
  use our replacement in file `exc-c-wrapper-handler.S`.

  An overview, of an exception at entry: New interrupts are blocked by EXCM
  being set. Once cleared, interrupts above the current INTLEVEL and exceptions
  (w/o creating a DoubleException) can occur.

  Using our replacement for _xtos_c_wrapper_handler, INTLEVEL is raised to 15
  with EXCM cleared.

  The original Boot ROM `_xtos_c_wrapper_handler` would set INTLEVEL to 1 with
  EXCM cleared, saved registers, then do a `rsil 0`, and called the registered
  "C" Exception handler with interrupts fully enabled! Our replacement keeps
  INTLEVEL at 15. This is needed to support the Arduino model of interrupts
  disabled while an ISR runs.

  And we also need it for umm_malloc to work safely with an IRAM heap from an
  ISR call. While malloc() will supply DRAM for all allocation from an ISR,
  we want free() to safely operate from an ISR to avoid a leak potential.

  This alternative "C" Wrapper is only applied to this exception handler.
 */

#define ROM_xtos_c_wrapper_handler (reinterpret_cast<_xtos_handler>(0x40000598))

static void IRAM_ATTR _set_exception_handler_wrapper(uint32_t cause) {
  _xtos_handler old_wrapper = _xtos_exc_handler_table[cause];
  if (old_wrapper == ROM_xtos_c_wrapper_handler) {
    _xtos_exc_handler_table[cause] = _xtos_c_wrapper_handler;
  }
}

void IRAM_ATTR install_non32xfer_exception_handler(void) {
  if (NULL == old_handler) {
    old_handler =
    _xtos_set_exception_handler(EXCCAUSE_LOAD_STORE_ERROR,
      non32xfer_exception_handler);

    _set_exception_handler_wrapper(EXCCAUSE_LOAD_STORE_ERROR);
  }
}

};
