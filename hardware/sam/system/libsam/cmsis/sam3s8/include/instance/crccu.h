/* %ATMEL_LICENCE% */

#ifndef _SAM3S8_CRCCU_INSTANCE_
#define _SAM3S8_CRCCU_INSTANCE_

/* ========== Register definition for CRCCU peripheral ========== */
#define REG_CRCCU_DSCR    REG_ACCESS(RwReg, 0x40044000U) /**< \brief (CRCCU) CRCCU Descriptor Base Register */
#define REG_CRCCU_DMA_EN  REG_ACCESS(WoReg, 0x40044008U) /**< \brief (CRCCU) CRCCU DMA Enable Register */
#define REG_CRCCU_DMA_DIS REG_ACCESS(WoReg, 0x4004400CU) /**< \brief (CRCCU) CRCCU DMA Disable Register */
#define REG_CRCCU_DMA_SR  REG_ACCESS(RoReg, 0x40044010U) /**< \brief (CRCCU) CRCCU DMA Status Register */
#define REG_CRCCU_DMA_IER REG_ACCESS(WoReg, 0x40044014U) /**< \brief (CRCCU) CRCCU DMA Interrupt Enable Register */
#define REG_CRCCU_DMA_IDR REG_ACCESS(WoReg, 0x40044018U) /**< \brief (CRCCU) CRCCU DMA Interrupt Disable Register */
#define REG_CRCCU_DMA_IMR REG_ACCESS(RoReg, 0x4004401CU) /**< \brief (CRCCU) CRCCU DMA Interrupt Mask Register */
#define REG_CRCCU_DMA_ISR REG_ACCESS(RoReg, 0x40044020U) /**< \brief (CRCCU) CRCCU DMA Interrupt Status Register */
#define REG_CRCCU_CR      REG_ACCESS(WoReg, 0x40044034U) /**< \brief (CRCCU) CRCCU Control Register */
#define REG_CRCCU_MR      REG_ACCESS(RwReg, 0x40044038U) /**< \brief (CRCCU) CRCCU Mode Register */
#define REG_CRCCU_SR      REG_ACCESS(RoReg, 0x4004403CU) /**< \brief (CRCCU) CRCCU Status Register */
#define REG_CRCCU_IER     REG_ACCESS(WoReg, 0x40044040U) /**< \brief (CRCCU) CRCCU Interrupt Enable Register */
#define REG_CRCCU_IDR     REG_ACCESS(WoReg, 0x40044044U) /**< \brief (CRCCU) CRCCU Interrupt Disable Register */
#define REG_CRCCU_IMR     REG_ACCESS(RoReg, 0x40044048U) /**< \brief (CRCCU) CRCCU Interrupt Mask Register */
#define REG_CRCCU_ISR     REG_ACCESS(RoReg, 0x4004404CU) /**< \brief (CRCCU) CRCCU Interrupt Status Register */

#endif /* _SAM3S8_CRCCU_INSTANCE_ */
