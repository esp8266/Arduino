/* $asf_license$ */

#ifndef _SAM3S8_CRCCU_INSTANCE_
#define _SAM3S8_CRCCU_INSTANCE_

/* ========== Register definition for CRCCU peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_CRCCU_DSCR             (0x40044000U) /**< \brief (CRCCU) CRCCU Descriptor Base Register */
#define REG_CRCCU_DMA_EN           (0x40044008U) /**< \brief (CRCCU) CRCCU DMA Enable Register */
#define REG_CRCCU_DMA_DIS          (0x4004400CU) /**< \brief (CRCCU) CRCCU DMA Disable Register */
#define REG_CRCCU_DMA_SR           (0x40044010U) /**< \brief (CRCCU) CRCCU DMA Status Register */
#define REG_CRCCU_DMA_IER          (0x40044014U) /**< \brief (CRCCU) CRCCU DMA Interrupt Enable Register */
#define REG_CRCCU_DMA_IDR          (0x40044018U) /**< \brief (CRCCU) CRCCU DMA Interrupt Disable Register */
#define REG_CRCCU_DMA_IMR          (0x4004401CU) /**< \brief (CRCCU) CRCCU DMA Interrupt Mask Register */
#define REG_CRCCU_DMA_ISR          (0x40044020U) /**< \brief (CRCCU) CRCCU DMA Interrupt Status Register */
#define REG_CRCCU_CR               (0x40044034U) /**< \brief (CRCCU) CRCCU Control Register */
#define REG_CRCCU_MR               (0x40044038U) /**< \brief (CRCCU) CRCCU Mode Register */
#define REG_CRCCU_SR               (0x4004403CU) /**< \brief (CRCCU) CRCCU Status Register */
#define REG_CRCCU_IER              (0x40044040U) /**< \brief (CRCCU) CRCCU Interrupt Enable Register */
#define REG_CRCCU_IDR              (0x40044044U) /**< \brief (CRCCU) CRCCU Interrupt Disable Register */
#define REG_CRCCU_IMR              (0x40044048U) /**< \brief (CRCCU) CRCCU Interrupt Mask Register */
#define REG_CRCCU_ISR              (0x4004404CU) /**< \brief (CRCCU) CRCCU Interrupt Status Register */
#else
#define REG_CRCCU_DSCR    (*(RwReg*)0x40044000U) /**< \brief (CRCCU) CRCCU Descriptor Base Register */
#define REG_CRCCU_DMA_EN  (*(WoReg*)0x40044008U) /**< \brief (CRCCU) CRCCU DMA Enable Register */
#define REG_CRCCU_DMA_DIS (*(WoReg*)0x4004400CU) /**< \brief (CRCCU) CRCCU DMA Disable Register */
#define REG_CRCCU_DMA_SR  (*(RoReg*)0x40044010U) /**< \brief (CRCCU) CRCCU DMA Status Register */
#define REG_CRCCU_DMA_IER (*(WoReg*)0x40044014U) /**< \brief (CRCCU) CRCCU DMA Interrupt Enable Register */
#define REG_CRCCU_DMA_IDR (*(WoReg*)0x40044018U) /**< \brief (CRCCU) CRCCU DMA Interrupt Disable Register */
#define REG_CRCCU_DMA_IMR (*(RoReg*)0x4004401CU) /**< \brief (CRCCU) CRCCU DMA Interrupt Mask Register */
#define REG_CRCCU_DMA_ISR (*(RoReg*)0x40044020U) /**< \brief (CRCCU) CRCCU DMA Interrupt Status Register */
#define REG_CRCCU_CR      (*(WoReg*)0x40044034U) /**< \brief (CRCCU) CRCCU Control Register */
#define REG_CRCCU_MR      (*(RwReg*)0x40044038U) /**< \brief (CRCCU) CRCCU Mode Register */
#define REG_CRCCU_SR      (*(RoReg*)0x4004403CU) /**< \brief (CRCCU) CRCCU Status Register */
#define REG_CRCCU_IER     (*(WoReg*)0x40044040U) /**< \brief (CRCCU) CRCCU Interrupt Enable Register */
#define REG_CRCCU_IDR     (*(WoReg*)0x40044044U) /**< \brief (CRCCU) CRCCU Interrupt Disable Register */
#define REG_CRCCU_IMR     (*(RoReg*)0x40044048U) /**< \brief (CRCCU) CRCCU Interrupt Mask Register */
#define REG_CRCCU_ISR     (*(RoReg*)0x4004404CU) /**< \brief (CRCCU) CRCCU Interrupt Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3S8_CRCCU_INSTANCE_ */
