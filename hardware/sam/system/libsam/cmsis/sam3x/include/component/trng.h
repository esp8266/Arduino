/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_TRNG_COMPONENT_
#define _SAM3XA_TRNG_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR True Random Number Generator */
/* ============================================================================= */
/** \addtogroup SAM3XA_TRNG True Random Number Generator */
/*@{*/

#ifndef __ASSEMBLY__
/** \brief Trng hardware registers */
typedef struct {
  WoReg TRNG_CR;       /**< \brief (Trng Offset: 0x00) Control Register */
  RoReg Reserved1[3];
  WoReg TRNG_IER;      /**< \brief (Trng Offset: 0x10) Interrupt Enable Register */
  WoReg TRNG_IDR;      /**< \brief (Trng Offset: 0x14) Interrupt Disable Register */
  RoReg TRNG_IMR;      /**< \brief (Trng Offset: 0x18) Interrupt Mask Register */
  RoReg TRNG_ISR;      /**< \brief (Trng Offset: 0x1C) Interrupt Status Register */
  RoReg Reserved2[12];
  RoReg TRNG_ODATA;    /**< \brief (Trng Offset: 0x50) Output Data Register */
} Trng;
#endif /* __ASSEMBLY__ */
/* -------- TRNG_CR : (TRNG Offset: 0x00) Control Register -------- */
#define TRNG_CR_ENABLE (0x1u << 0) /**< \brief (TRNG_CR) Enables the TRNG to provide random values */
#define TRNG_CR_KEY_Pos 8
#define TRNG_CR_KEY_Msk (0xffffffu << TRNG_CR_KEY_Pos) /**< \brief (TRNG_CR) Security Key */
#define TRNG_CR_KEY(value) ((TRNG_CR_KEY_Msk & ((value) << TRNG_CR_KEY_Pos)))
/* -------- TRNG_IER : (TRNG Offset: 0x10) Interrupt Enable Register -------- */
#define TRNG_IER_DATRDY (0x1u << 0) /**< \brief (TRNG_IER) Data Ready Interrupt Enable */
/* -------- TRNG_IDR : (TRNG Offset: 0x14) Interrupt Disable Register -------- */
#define TRNG_IDR_DATRDY (0x1u << 0) /**< \brief (TRNG_IDR) Data Ready Interrupt Disable */
/* -------- TRNG_IMR : (TRNG Offset: 0x18) Interrupt Mask Register -------- */
#define TRNG_IMR_DATRDY (0x1u << 0) /**< \brief (TRNG_IMR) Data Ready Interrupt Mask */
/* -------- TRNG_ISR : (TRNG Offset: 0x1C) Interrupt Status Register -------- */
#define TRNG_ISR_DATRDY (0x1u << 0) /**< \brief (TRNG_ISR) Data Ready */
/* -------- TRNG_ODATA : (TRNG Offset: 0x50) Output Data Register -------- */
#define TRNG_ODATA_ODATA_Pos 0
#define TRNG_ODATA_ODATA_Msk (0xffffffffu << TRNG_ODATA_ODATA_Pos) /**< \brief (TRNG_ODATA) Output Data */

/*@}*/


#endif /* _SAM3XA_TRNG_COMPONENT_ */
