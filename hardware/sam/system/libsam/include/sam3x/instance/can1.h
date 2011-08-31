/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_CAN1_INSTANCE_
#define _SAM3XA_CAN1_INSTANCE_

/* ========== Register definition for CAN1 peripheral ========== */
#define REG_CAN1_MR      REG_ACCESS(RwReg, 0x400B8000U) /**< \brief (CAN1) Mode Register */
#define REG_CAN1_IER     REG_ACCESS(WoReg, 0x400B8004U) /**< \brief (CAN1) Interrupt Enable Register */
#define REG_CAN1_IDR     REG_ACCESS(WoReg, 0x400B8008U) /**< \brief (CAN1) Interrupt Disable Register */
#define REG_CAN1_IMR     REG_ACCESS(RoReg, 0x400B800CU) /**< \brief (CAN1) Interrupt Mask Register */
#define REG_CAN1_SR      REG_ACCESS(RoReg, 0x400B8010U) /**< \brief (CAN1) Status Register */
#define REG_CAN1_BR      REG_ACCESS(RwReg, 0x400B8014U) /**< \brief (CAN1) Baudrate Register */
#define REG_CAN1_TIM     REG_ACCESS(RoReg, 0x400B8018U) /**< \brief (CAN1) Timer Register */
#define REG_CAN1_TIMESTP REG_ACCESS(RoReg, 0x400B801CU) /**< \brief (CAN1) Timestamp Register */
#define REG_CAN1_ECR     REG_ACCESS(RoReg, 0x400B8020U) /**< \brief (CAN1) Error Counter Register */
#define REG_CAN1_TCR     REG_ACCESS(WoReg, 0x400B8024U) /**< \brief (CAN1) Transfer Command Register */
#define REG_CAN1_ACR     REG_ACCESS(WoReg, 0x400B8028U) /**< \brief (CAN1) Abort Command Register */
#define REG_CAN1_MMR0    REG_ACCESS(RwReg, 0x400B8200U) /**< \brief (CAN1) Mailbox Mode Register (MB = 0) */
#define REG_CAN1_MAM0    REG_ACCESS(RwReg, 0x400B8204U) /**< \brief (CAN1) Mailbox Acceptance Mask Register (MB = 0) */
#define REG_CAN1_MID0    REG_ACCESS(RwReg, 0x400B8208U) /**< \brief (CAN1) Mailbox ID Register (MB = 0) */
#define REG_CAN1_MFID0   REG_ACCESS(RoReg, 0x400B820CU) /**< \brief (CAN1) Mailbox Family ID Register (MB = 0) */
#define REG_CAN1_MSR0    REG_ACCESS(RoReg, 0x400B8210U) /**< \brief (CAN1) Mailbox Status Register (MB = 0) */
#define REG_CAN1_MDL0    REG_ACCESS(RwReg, 0x400B8214U) /**< \brief (CAN1) Mailbox Data Low Register (MB = 0) */
#define REG_CAN1_MDH0    REG_ACCESS(RwReg, 0x400B8218U) /**< \brief (CAN1) Mailbox Data High Register (MB = 0) */
#define REG_CAN1_MCR0    REG_ACCESS(WoReg, 0x400B821CU) /**< \brief (CAN1) Mailbox Control Register (MB = 0) */
#define REG_CAN1_MMR1    REG_ACCESS(RwReg, 0x400B8220U) /**< \brief (CAN1) Mailbox Mode Register (MB = 1) */
#define REG_CAN1_MAM1    REG_ACCESS(RwReg, 0x400B8224U) /**< \brief (CAN1) Mailbox Acceptance Mask Register (MB = 1) */
#define REG_CAN1_MID1    REG_ACCESS(RwReg, 0x400B8228U) /**< \brief (CAN1) Mailbox ID Register (MB = 1) */
#define REG_CAN1_MFID1   REG_ACCESS(RoReg, 0x400B822CU) /**< \brief (CAN1) Mailbox Family ID Register (MB = 1) */
#define REG_CAN1_MSR1    REG_ACCESS(RoReg, 0x400B8230U) /**< \brief (CAN1) Mailbox Status Register (MB = 1) */
#define REG_CAN1_MDL1    REG_ACCESS(RwReg, 0x400B8234U) /**< \brief (CAN1) Mailbox Data Low Register (MB = 1) */
#define REG_CAN1_MDH1    REG_ACCESS(RwReg, 0x400B8238U) /**< \brief (CAN1) Mailbox Data High Register (MB = 1) */
#define REG_CAN1_MCR1    REG_ACCESS(WoReg, 0x400B823CU) /**< \brief (CAN1) Mailbox Control Register (MB = 1) */
#define REG_CAN1_MMR2    REG_ACCESS(RwReg, 0x400B8240U) /**< \brief (CAN1) Mailbox Mode Register (MB = 2) */
#define REG_CAN1_MAM2    REG_ACCESS(RwReg, 0x400B8244U) /**< \brief (CAN1) Mailbox Acceptance Mask Register (MB = 2) */
#define REG_CAN1_MID2    REG_ACCESS(RwReg, 0x400B8248U) /**< \brief (CAN1) Mailbox ID Register (MB = 2) */
#define REG_CAN1_MFID2   REG_ACCESS(RoReg, 0x400B824CU) /**< \brief (CAN1) Mailbox Family ID Register (MB = 2) */
#define REG_CAN1_MSR2    REG_ACCESS(RoReg, 0x400B8250U) /**< \brief (CAN1) Mailbox Status Register (MB = 2) */
#define REG_CAN1_MDL2    REG_ACCESS(RwReg, 0x400B8254U) /**< \brief (CAN1) Mailbox Data Low Register (MB = 2) */
#define REG_CAN1_MDH2    REG_ACCESS(RwReg, 0x400B8258U) /**< \brief (CAN1) Mailbox Data High Register (MB = 2) */
#define REG_CAN1_MCR2    REG_ACCESS(WoReg, 0x400B825CU) /**< \brief (CAN1) Mailbox Control Register (MB = 2) */
#define REG_CAN1_MMR3    REG_ACCESS(RwReg, 0x400B8260U) /**< \brief (CAN1) Mailbox Mode Register (MB = 3) */
#define REG_CAN1_MAM3    REG_ACCESS(RwReg, 0x400B8264U) /**< \brief (CAN1) Mailbox Acceptance Mask Register (MB = 3) */
#define REG_CAN1_MID3    REG_ACCESS(RwReg, 0x400B8268U) /**< \brief (CAN1) Mailbox ID Register (MB = 3) */
#define REG_CAN1_MFID3   REG_ACCESS(RoReg, 0x400B826CU) /**< \brief (CAN1) Mailbox Family ID Register (MB = 3) */
#define REG_CAN1_MSR3    REG_ACCESS(RoReg, 0x400B8270U) /**< \brief (CAN1) Mailbox Status Register (MB = 3) */
#define REG_CAN1_MDL3    REG_ACCESS(RwReg, 0x400B8274U) /**< \brief (CAN1) Mailbox Data Low Register (MB = 3) */
#define REG_CAN1_MDH3    REG_ACCESS(RwReg, 0x400B8278U) /**< \brief (CAN1) Mailbox Data High Register (MB = 3) */
#define REG_CAN1_MCR3    REG_ACCESS(WoReg, 0x400B827CU) /**< \brief (CAN1) Mailbox Control Register (MB = 3) */
#define REG_CAN1_MMR4    REG_ACCESS(RwReg, 0x400B8280U) /**< \brief (CAN1) Mailbox Mode Register (MB = 4) */
#define REG_CAN1_MAM4    REG_ACCESS(RwReg, 0x400B8284U) /**< \brief (CAN1) Mailbox Acceptance Mask Register (MB = 4) */
#define REG_CAN1_MID4    REG_ACCESS(RwReg, 0x400B8288U) /**< \brief (CAN1) Mailbox ID Register (MB = 4) */
#define REG_CAN1_MFID4   REG_ACCESS(RoReg, 0x400B828CU) /**< \brief (CAN1) Mailbox Family ID Register (MB = 4) */
#define REG_CAN1_MSR4    REG_ACCESS(RoReg, 0x400B8290U) /**< \brief (CAN1) Mailbox Status Register (MB = 4) */
#define REG_CAN1_MDL4    REG_ACCESS(RwReg, 0x400B8294U) /**< \brief (CAN1) Mailbox Data Low Register (MB = 4) */
#define REG_CAN1_MDH4    REG_ACCESS(RwReg, 0x400B8298U) /**< \brief (CAN1) Mailbox Data High Register (MB = 4) */
#define REG_CAN1_MCR4    REG_ACCESS(WoReg, 0x400B829CU) /**< \brief (CAN1) Mailbox Control Register (MB = 4) */
#define REG_CAN1_MMR5    REG_ACCESS(RwReg, 0x400B82A0U) /**< \brief (CAN1) Mailbox Mode Register (MB = 5) */
#define REG_CAN1_MAM5    REG_ACCESS(RwReg, 0x400B82A4U) /**< \brief (CAN1) Mailbox Acceptance Mask Register (MB = 5) */
#define REG_CAN1_MID5    REG_ACCESS(RwReg, 0x400B82A8U) /**< \brief (CAN1) Mailbox ID Register (MB = 5) */
#define REG_CAN1_MFID5   REG_ACCESS(RoReg, 0x400B82ACU) /**< \brief (CAN1) Mailbox Family ID Register (MB = 5) */
#define REG_CAN1_MSR5    REG_ACCESS(RoReg, 0x400B82B0U) /**< \brief (CAN1) Mailbox Status Register (MB = 5) */
#define REG_CAN1_MDL5    REG_ACCESS(RwReg, 0x400B82B4U) /**< \brief (CAN1) Mailbox Data Low Register (MB = 5) */
#define REG_CAN1_MDH5    REG_ACCESS(RwReg, 0x400B82B8U) /**< \brief (CAN1) Mailbox Data High Register (MB = 5) */
#define REG_CAN1_MCR5    REG_ACCESS(WoReg, 0x400B82BCU) /**< \brief (CAN1) Mailbox Control Register (MB = 5) */
#define REG_CAN1_MMR6    REG_ACCESS(RwReg, 0x400B82C0U) /**< \brief (CAN1) Mailbox Mode Register (MB = 6) */
#define REG_CAN1_MAM6    REG_ACCESS(RwReg, 0x400B82C4U) /**< \brief (CAN1) Mailbox Acceptance Mask Register (MB = 6) */
#define REG_CAN1_MID6    REG_ACCESS(RwReg, 0x400B82C8U) /**< \brief (CAN1) Mailbox ID Register (MB = 6) */
#define REG_CAN1_MFID6   REG_ACCESS(RoReg, 0x400B82CCU) /**< \brief (CAN1) Mailbox Family ID Register (MB = 6) */
#define REG_CAN1_MSR6    REG_ACCESS(RoReg, 0x400B82D0U) /**< \brief (CAN1) Mailbox Status Register (MB = 6) */
#define REG_CAN1_MDL6    REG_ACCESS(RwReg, 0x400B82D4U) /**< \brief (CAN1) Mailbox Data Low Register (MB = 6) */
#define REG_CAN1_MDH6    REG_ACCESS(RwReg, 0x400B82D8U) /**< \brief (CAN1) Mailbox Data High Register (MB = 6) */
#define REG_CAN1_MCR6    REG_ACCESS(WoReg, 0x400B82DCU) /**< \brief (CAN1) Mailbox Control Register (MB = 6) */
#define REG_CAN1_MMR7    REG_ACCESS(RwReg, 0x400B82E0U) /**< \brief (CAN1) Mailbox Mode Register (MB = 7) */
#define REG_CAN1_MAM7    REG_ACCESS(RwReg, 0x400B82E4U) /**< \brief (CAN1) Mailbox Acceptance Mask Register (MB = 7) */
#define REG_CAN1_MID7    REG_ACCESS(RwReg, 0x400B82E8U) /**< \brief (CAN1) Mailbox ID Register (MB = 7) */
#define REG_CAN1_MFID7   REG_ACCESS(RoReg, 0x400B82ECU) /**< \brief (CAN1) Mailbox Family ID Register (MB = 7) */
#define REG_CAN1_MSR7    REG_ACCESS(RoReg, 0x400B82F0U) /**< \brief (CAN1) Mailbox Status Register (MB = 7) */
#define REG_CAN1_MDL7    REG_ACCESS(RwReg, 0x400B82F4U) /**< \brief (CAN1) Mailbox Data Low Register (MB = 7) */
#define REG_CAN1_MDH7    REG_ACCESS(RwReg, 0x400B82F8U) /**< \brief (CAN1) Mailbox Data High Register (MB = 7) */
#define REG_CAN1_MCR7    REG_ACCESS(WoReg, 0x400B82FCU) /**< \brief (CAN1) Mailbox Control Register (MB = 7) */

#endif /* _SAM3XA_CAN1_INSTANCE_ */
