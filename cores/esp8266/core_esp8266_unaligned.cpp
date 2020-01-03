/* Unaligned exception handler, allows for byte accesses to PROGMEM to 
 * succeed without causing a crash.  It is still preferred to use the
 * xxx_P macros whenever possible, since they are probably 30x faster than
 * this exception handler method.
 *
 * Code taken directly from @pvvx's public domain code in
 * https://github.com/pvvx/esp8266web/blob/master/app/sdklib/system/app_main.c
 */

#include <Arduino.h>
#include <core_esp8266_unaligned.h>

extern "C" {

#define LOAD_MASK   0x00f00fu
#define L8UI_MATCH  0x000002u
#define L16UI_MATCH 0x001002u
#define L16SI_MATCH 0x009002u

#define EXCCAUSE_LOAD_STORE_ERROR 3 // Unaligned or NULL error

struct exception_frame
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

extern void _xtos_set_exception_handler(uint32_t reason, void (*fn)(struct exception_frame *ef, uint32_t cause));
extern void _xtos_unhandled_exception(struct exception_frame *ef, uint32_t cause);

static ICACHE_RAM_ATTR void read_align_exception_handler(struct exception_frame *ef, uint32_t cause)
{
  /* If this is not EXCCAUSE_LOAD_STORE_ERROR you're doing it wrong! */
  (void)cause;

  uint32_t epc1 = ef->epc;
  uint32_t excvaddr;
  uint32_t insn;
  asm (
    "rsr   %0, EXCVADDR;"    /* read out the faulting address */
    "movi  a4, ~3;"          /* prepare a mask for the EPC */
    "and   a4, a4, %2;"      /* apply mask for 32bit aligned base */
    "l32i  a5, a4, 0;"       /* load part 1 */
    "l32i  a6, a4, 4;"       /* load part 2 */
    "ssa8l %2;"              /* set up shift register for src op */
    "src   %1, a6, a5;"      /* right shift to get faulting instruction */
    :"=r"(excvaddr), "=r"(insn)
    :"r"(epc1)
    :"a4", "a5", "a6"
  );

  uint32_t valmask = 0;
  uint32_t what = insn & LOAD_MASK;

  if (what == L8UI_MATCH)
    valmask = 0xffu;
  else if (what == L16UI_MATCH || what == L16SI_MATCH)
    valmask = 0xffffu;
  else
  {
die:
    /* Turns out we couldn't fix this, trigger a system break instead
     * and hang if the break doesn't get handled. This is effectively
     * what would happen if the default handler was installed. */
    _xtos_unhandled_exception(ef, cause);
    asm ("break 1, 1");
    while (1) {}
  }

  /* Load, shift and mask down to correct size */
  uint32_t val = (*(uint32_t *)(excvaddr & ~0x3));
  val >>= (excvaddr & 0x3) * 8;
  val &= valmask;

  /* Sign-extend for L16SI, if applicable */
  if (what == L16SI_MATCH && (val & 0x8000))
    val |= 0xffff0000;

  int regno = (insn & 0x0000f0u) >> 4;
  if (regno == 1)
    goto die;              /* we can't support loading into a1, just die */
  else if (regno != 0)
    --regno;               /* account for skipped a1 in exception_frame */

  ef->a_reg[regno] = val;  /* carry out the load */
  ef->epc += 3;            /* resume at following instruction */
}



void install_unaligned_exception_handler()
{
  _xtos_set_exception_handler(EXCCAUSE_LOAD_STORE_ERROR, read_align_exception_handler);
}


};
