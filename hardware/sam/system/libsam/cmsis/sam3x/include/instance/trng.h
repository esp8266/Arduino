/* $asf_license$ */

#ifndef _SAM3XA_TRNG_INSTANCE_
#define _SAM3XA_TRNG_INSTANCE_

/* ========== Register definition for TRNG peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_TRNG_CR             (0x400BC000U) /**< \brief (TRNG) Control Register */
#define REG_TRNG_IER            (0x400BC010U) /**< \brief (TRNG) Interrupt Enable Register */
#define REG_TRNG_IDR            (0x400BC014U) /**< \brief (TRNG) Interrupt Disable Register */
#define REG_TRNG_IMR            (0x400BC018U) /**< \brief (TRNG) Interrupt Mask Register */
#define REG_TRNG_ISR            (0x400BC01CU) /**< \brief (TRNG) Interrupt Status Register */
#define REG_TRNG_ODATA          (0x400BC050U) /**< \brief (TRNG) Output Data Register */
#else
#define REG_TRNG_CR    (*(WoReg*)0x400BC000U) /**< \brief (TRNG) Control Register */
#define REG_TRNG_IER   (*(WoReg*)0x400BC010U) /**< \brief (TRNG) Interrupt Enable Register */
#define REG_TRNG_IDR   (*(WoReg*)0x400BC014U) /**< \brief (TRNG) Interrupt Disable Register */
#define REG_TRNG_IMR   (*(RoReg*)0x400BC018U) /**< \brief (TRNG) Interrupt Mask Register */
#define REG_TRNG_ISR   (*(RoReg*)0x400BC01CU) /**< \brief (TRNG) Interrupt Status Register */
#define REG_TRNG_ODATA (*(RoReg*)0x400BC050U) /**< \brief (TRNG) Output Data Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3XA_TRNG_INSTANCE_ */
