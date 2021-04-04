#ifndef __CORE_ESP8266_NON32XFER_H
#define __CORE_ESP8266_NON32XFER_H

#ifdef __cplusplus
extern "C" {
#endif

extern void install_non32xfer_exception_handler();


    /*
       In adapting the public domain version, a crash would come or go away with
       the slightest unrelated changes elsewhere in the function. Observed that
       register a15 was used for epc1, then clobbered by `rsr.` I now believe a
       "&" on the output register would have resolved the problem.

       However, I have refactored the Extended ASM to reduce and consolidate
       register usage and corrected the issue.

       The positioning of the Extended ASM block (as early as possible in the
       compiled function) is in part controlled by the immediate need for
       output variable `insn`. This placement aids in getting excvaddr read as
       early as possible.
     */

#if 0
    {
      __asm__ __volatile__ ("rsr.excvaddr %0;" : "=r"(excvaddr):: "memory");
      /*
        "C" reference code for the ASM to document intent.
        May also prove useful when issolating possible issues with Extended ASM,
        optimizations, new compilers, etc.
      */
      uint32_t epc = ef->epc;
      uint32_t *pWord = (uint32_t *)(epc & ~3);
      uint64_t big_word = ((uint64_t)pWord[1] << 32) | pWord[0];
      uint32_t pos = (epc & 3) * 8;
      insn = (uint32_t)(big_word >>= pos);
    }
#endif

#define __EXCEPTION_HANDLER_PREAMBLE(ef, excvaddr, insn) \
    { \
      uint32_t tmp; \
      __asm__ ( \
        "rsr.excvaddr %[vaddr]\n\t"             /* Read faulting address as early as possible */ \
        "movi.n %[tmp],  ~3\n\t"                /* prepare a mask for the EPC */ \
        "and    %[tmp],  %[tmp],  %[epc]\n\t"   /* apply mask for 32-bit aligned base */ \
        "ssa8l  %[epc]\n\t"                     /* set up shift register for src op */ \
        "l32i   %[insn], %[tmp],  0\n\t"        /* load part 1 */ \
        "l32i   %[tmp],  %[tmp],  4\n\t"        /* load part 2 */ \
        "src    %[insn], %[tmp],  %[insn]\n\t"  /* right shift to get faulting instruction */ \
        : [vaddr]"=&r"(excvaddr), [insn]"=&r"(insn), [tmp]"=&r"(tmp) \
        : [epc]"r"(ef->epc) :); \
    }


#ifdef __cplusplus
}
#endif

#endif
