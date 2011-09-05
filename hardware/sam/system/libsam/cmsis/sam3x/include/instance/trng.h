/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_TRNG_INSTANCE_
#define _SAM3XA_TRNG_INSTANCE_

/* ========== Register definition for TRNG peripheral ========== */
#define REG_TRNG_CR    REG_ACCESS(WoReg, 0x400BC000U) /**< \brief (TRNG) Control Register */
#define REG_TRNG_IER   REG_ACCESS(WoReg, 0x400BC010U) /**< \brief (TRNG) Interrupt Enable Register */
#define REG_TRNG_IDR   REG_ACCESS(WoReg, 0x400BC014U) /**< \brief (TRNG) Interrupt Disable Register */
#define REG_TRNG_IMR   REG_ACCESS(RoReg, 0x400BC018U) /**< \brief (TRNG) Interrupt Mask Register */
#define REG_TRNG_ISR   REG_ACCESS(RoReg, 0x400BC01CU) /**< \brief (TRNG) Interrupt Status Register */
#define REG_TRNG_ODATA REG_ACCESS(RoReg, 0x400BC050U) /**< \brief (TRNG) Output Data Register */

#endif /* _SAM3XA_TRNG_INSTANCE_ */
