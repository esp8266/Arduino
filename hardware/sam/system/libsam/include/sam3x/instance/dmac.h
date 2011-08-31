/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_DMAC_INSTANCE_
#define _SAM3XA_DMAC_INSTANCE_

/* ========== Register definition for DMAC peripheral ========== */
#define REG_DMAC_GCFG   REG_ACCESS(RwReg, 0x400C4000U) /**< \brief (DMAC) DMAC Global Configuration Register */
#define REG_DMAC_EN     REG_ACCESS(RwReg, 0x400C4004U) /**< \brief (DMAC) DMAC Enable Register */
#define REG_DMAC_SREQ   REG_ACCESS(RwReg, 0x400C4008U) /**< \brief (DMAC) DMAC Software Single Request Register */
#define REG_DMAC_CREQ   REG_ACCESS(RwReg, 0x400C400CU) /**< \brief (DMAC) DMAC Software Chunk Transfer Request Register */
#define REG_DMAC_LAST   REG_ACCESS(RwReg, 0x400C4010U) /**< \brief (DMAC) DMAC Software Last Transfer Flag Register */
#define REG_DMAC_EBCIER REG_ACCESS(WoReg, 0x400C4018U) /**< \brief (DMAC) DMAC Error, Chained Buffer Transfer Completed Interrupt and Buffer Transfer Completed Interrupt Enable register. */
#define REG_DMAC_EBCIDR REG_ACCESS(WoReg, 0x400C401CU) /**< \brief (DMAC) DMAC Error, Chained Buffer Transfer Completed Interrupt and Buffer Transfer Completed Interrupt Disable register. */
#define REG_DMAC_EBCIMR REG_ACCESS(RoReg, 0x400C4020U) /**< \brief (DMAC) DMAC Error, Chained Buffer Transfer Completed Interrupt and Buffer transfer completed Mask Register. */
#define REG_DMAC_EBCISR REG_ACCESS(RoReg, 0x400C4024U) /**< \brief (DMAC) DMAC Error, Chained Buffer Transfer Completed Interrupt and Buffer transfer completed Status Register. */
#define REG_DMAC_CHER   REG_ACCESS(WoReg, 0x400C4028U) /**< \brief (DMAC) DMAC Channel Handler Enable Register */
#define REG_DMAC_CHDR   REG_ACCESS(WoReg, 0x400C402CU) /**< \brief (DMAC) DMAC Channel Handler Disable Register */
#define REG_DMAC_CHSR   REG_ACCESS(RoReg, 0x400C4030U) /**< \brief (DMAC) DMAC Channel Handler Status Register */
#define REG_DMAC_SADDR0 REG_ACCESS(RwReg, 0x400C403CU) /**< \brief (DMAC) DMAC Channel Source Address Register (ch_num = 0) */
#define REG_DMAC_DADDR0 REG_ACCESS(RwReg, 0x400C4040U) /**< \brief (DMAC) DMAC Channel Destination Address Register (ch_num = 0) */
#define REG_DMAC_DSCR0  REG_ACCESS(RwReg, 0x400C4044U) /**< \brief (DMAC) DMAC Channel Descriptor Address Register (ch_num = 0) */
#define REG_DMAC_CTRLA0 REG_ACCESS(RwReg, 0x400C4048U) /**< \brief (DMAC) DMAC Channel Control A Register (ch_num = 0) */
#define REG_DMAC_CTRLB0 REG_ACCESS(RwReg, 0x400C404CU) /**< \brief (DMAC) DMAC Channel Control B Register (ch_num = 0) */
#define REG_DMAC_CFG0   REG_ACCESS(RwReg, 0x400C4050U) /**< \brief (DMAC) DMAC Channel Configuration Register (ch_num = 0) */
#define REG_DMAC_SADDR1 REG_ACCESS(RwReg, 0x400C4064U) /**< \brief (DMAC) DMAC Channel Source Address Register (ch_num = 1) */
#define REG_DMAC_DADDR1 REG_ACCESS(RwReg, 0x400C4068U) /**< \brief (DMAC) DMAC Channel Destination Address Register (ch_num = 1) */
#define REG_DMAC_DSCR1  REG_ACCESS(RwReg, 0x400C406CU) /**< \brief (DMAC) DMAC Channel Descriptor Address Register (ch_num = 1) */
#define REG_DMAC_CTRLA1 REG_ACCESS(RwReg, 0x400C4070U) /**< \brief (DMAC) DMAC Channel Control A Register (ch_num = 1) */
#define REG_DMAC_CTRLB1 REG_ACCESS(RwReg, 0x400C4074U) /**< \brief (DMAC) DMAC Channel Control B Register (ch_num = 1) */
#define REG_DMAC_CFG1   REG_ACCESS(RwReg, 0x400C4078U) /**< \brief (DMAC) DMAC Channel Configuration Register (ch_num = 1) */
#define REG_DMAC_SADDR2 REG_ACCESS(RwReg, 0x400C408CU) /**< \brief (DMAC) DMAC Channel Source Address Register (ch_num = 2) */
#define REG_DMAC_DADDR2 REG_ACCESS(RwReg, 0x400C4090U) /**< \brief (DMAC) DMAC Channel Destination Address Register (ch_num = 2) */
#define REG_DMAC_DSCR2  REG_ACCESS(RwReg, 0x400C4094U) /**< \brief (DMAC) DMAC Channel Descriptor Address Register (ch_num = 2) */
#define REG_DMAC_CTRLA2 REG_ACCESS(RwReg, 0x400C4098U) /**< \brief (DMAC) DMAC Channel Control A Register (ch_num = 2) */
#define REG_DMAC_CTRLB2 REG_ACCESS(RwReg, 0x400C409CU) /**< \brief (DMAC) DMAC Channel Control B Register (ch_num = 2) */
#define REG_DMAC_CFG2   REG_ACCESS(RwReg, 0x400C40A0U) /**< \brief (DMAC) DMAC Channel Configuration Register (ch_num = 2) */
#define REG_DMAC_SADDR3 REG_ACCESS(RwReg, 0x400C40B4U) /**< \brief (DMAC) DMAC Channel Source Address Register (ch_num = 3) */
#define REG_DMAC_DADDR3 REG_ACCESS(RwReg, 0x400C40B8U) /**< \brief (DMAC) DMAC Channel Destination Address Register (ch_num = 3) */
#define REG_DMAC_DSCR3  REG_ACCESS(RwReg, 0x400C40BCU) /**< \brief (DMAC) DMAC Channel Descriptor Address Register (ch_num = 3) */
#define REG_DMAC_CTRLA3 REG_ACCESS(RwReg, 0x400C40C0U) /**< \brief (DMAC) DMAC Channel Control A Register (ch_num = 3) */
#define REG_DMAC_CTRLB3 REG_ACCESS(RwReg, 0x400C40C4U) /**< \brief (DMAC) DMAC Channel Control B Register (ch_num = 3) */
#define REG_DMAC_CFG3   REG_ACCESS(RwReg, 0x400C40C8U) /**< \brief (DMAC) DMAC Channel Configuration Register (ch_num = 3) */
#define REG_DMAC_SADDR4 REG_ACCESS(RwReg, 0x400C40DCU) /**< \brief (DMAC) DMAC Channel Source Address Register (ch_num = 4) */
#define REG_DMAC_DADDR4 REG_ACCESS(RwReg, 0x400C40E0U) /**< \brief (DMAC) DMAC Channel Destination Address Register (ch_num = 4) */
#define REG_DMAC_DSCR4  REG_ACCESS(RwReg, 0x400C40E4U) /**< \brief (DMAC) DMAC Channel Descriptor Address Register (ch_num = 4) */
#define REG_DMAC_CTRLA4 REG_ACCESS(RwReg, 0x400C40E8U) /**< \brief (DMAC) DMAC Channel Control A Register (ch_num = 4) */
#define REG_DMAC_CTRLB4 REG_ACCESS(RwReg, 0x400C40ECU) /**< \brief (DMAC) DMAC Channel Control B Register (ch_num = 4) */
#define REG_DMAC_CFG4   REG_ACCESS(RwReg, 0x400C40F0U) /**< \brief (DMAC) DMAC Channel Configuration Register (ch_num = 4) */
#define REG_DMAC_SADDR5 REG_ACCESS(RwReg, 0x400C4104U) /**< \brief (DMAC) DMAC Channel Source Address Register (ch_num = 5) */
#define REG_DMAC_DADDR5 REG_ACCESS(RwReg, 0x400C4108U) /**< \brief (DMAC) DMAC Channel Destination Address Register (ch_num = 5) */
#define REG_DMAC_DSCR5  REG_ACCESS(RwReg, 0x400C410CU) /**< \brief (DMAC) DMAC Channel Descriptor Address Register (ch_num = 5) */
#define REG_DMAC_CTRLA5 REG_ACCESS(RwReg, 0x400C4110U) /**< \brief (DMAC) DMAC Channel Control A Register (ch_num = 5) */
#define REG_DMAC_CTRLB5 REG_ACCESS(RwReg, 0x400C4114U) /**< \brief (DMAC) DMAC Channel Control B Register (ch_num = 5) */
#define REG_DMAC_CFG5   REG_ACCESS(RwReg, 0x400C4118U) /**< \brief (DMAC) DMAC Channel Configuration Register (ch_num = 5) */
#define REG_DMAC_WPMR   REG_ACCESS(RwReg, 0x400C41E4U) /**< \brief (DMAC) DMAC Write Protect Mode Register */
#define REG_DMAC_WPSR   REG_ACCESS(RoReg, 0x400C41E8U) /**< \brief (DMAC) DMAC Write Protect Status Register */

#endif /* _SAM3XA_DMAC_INSTANCE_ */
