/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_CAN0_INSTANCE_
#define _SAM3XA_CAN0_INSTANCE_

/* ========== Register definition for CAN0 peripheral ========== */
#define REG_CAN0_MR      REG_ACCESS(RwReg, 0x400B4000U) /**< \brief (CAN0) Mode Register */
#define REG_CAN0_IER     REG_ACCESS(WoReg, 0x400B4004U) /**< \brief (CAN0) Interrupt Enable Register */
#define REG_CAN0_IDR     REG_ACCESS(WoReg, 0x400B4008U) /**< \brief (CAN0) Interrupt Disable Register */
#define REG_CAN0_IMR     REG_ACCESS(RoReg, 0x400B400CU) /**< \brief (CAN0) Interrupt Mask Register */
#define REG_CAN0_SR      REG_ACCESS(RoReg, 0x400B4010U) /**< \brief (CAN0) Status Register */
#define REG_CAN0_BR      REG_ACCESS(RwReg, 0x400B4014U) /**< \brief (CAN0) Baudrate Register */
#define REG_CAN0_TIM     REG_ACCESS(RoReg, 0x400B4018U) /**< \brief (CAN0) Timer Register */
#define REG_CAN0_TIMESTP REG_ACCESS(RoReg, 0x400B401CU) /**< \brief (CAN0) Timestamp Register */
#define REG_CAN0_ECR     REG_ACCESS(RoReg, 0x400B4020U) /**< \brief (CAN0) Error Counter Register */
#define REG_CAN0_TCR     REG_ACCESS(WoReg, 0x400B4024U) /**< \brief (CAN0) Transfer Command Register */
#define REG_CAN0_ACR     REG_ACCESS(WoReg, 0x400B4028U) /**< \brief (CAN0) Abort Command Register */
#define REG_CAN0_MMR0    REG_ACCESS(RwReg, 0x400B4200U) /**< \brief (CAN0) Mailbox Mode Register (MB = 0) */
#define REG_CAN0_MAM0    REG_ACCESS(RwReg, 0x400B4204U) /**< \brief (CAN0) Mailbox Acceptance Mask Register (MB = 0) */
#define REG_CAN0_MID0    REG_ACCESS(RwReg, 0x400B4208U) /**< \brief (CAN0) Mailbox ID Register (MB = 0) */
#define REG_CAN0_MFID0   REG_ACCESS(RoReg, 0x400B420CU) /**< \brief (CAN0) Mailbox Family ID Register (MB = 0) */
#define REG_CAN0_MSR0    REG_ACCESS(RoReg, 0x400B4210U) /**< \brief (CAN0) Mailbox Status Register (MB = 0) */
#define REG_CAN0_MDL0    REG_ACCESS(RwReg, 0x400B4214U) /**< \brief (CAN0) Mailbox Data Low Register (MB = 0) */
#define REG_CAN0_MDH0    REG_ACCESS(RwReg, 0x400B4218U) /**< \brief (CAN0) Mailbox Data High Register (MB = 0) */
#define REG_CAN0_MCR0    REG_ACCESS(WoReg, 0x400B421CU) /**< \brief (CAN0) Mailbox Control Register (MB = 0) */
#define REG_CAN0_MMR1    REG_ACCESS(RwReg, 0x400B4220U) /**< \brief (CAN0) Mailbox Mode Register (MB = 1) */
#define REG_CAN0_MAM1    REG_ACCESS(RwReg, 0x400B4224U) /**< \brief (CAN0) Mailbox Acceptance Mask Register (MB = 1) */
#define REG_CAN0_MID1    REG_ACCESS(RwReg, 0x400B4228U) /**< \brief (CAN0) Mailbox ID Register (MB = 1) */
#define REG_CAN0_MFID1   REG_ACCESS(RoReg, 0x400B422CU) /**< \brief (CAN0) Mailbox Family ID Register (MB = 1) */
#define REG_CAN0_MSR1    REG_ACCESS(RoReg, 0x400B4230U) /**< \brief (CAN0) Mailbox Status Register (MB = 1) */
#define REG_CAN0_MDL1    REG_ACCESS(RwReg, 0x400B4234U) /**< \brief (CAN0) Mailbox Data Low Register (MB = 1) */
#define REG_CAN0_MDH1    REG_ACCESS(RwReg, 0x400B4238U) /**< \brief (CAN0) Mailbox Data High Register (MB = 1) */
#define REG_CAN0_MCR1    REG_ACCESS(WoReg, 0x400B423CU) /**< \brief (CAN0) Mailbox Control Register (MB = 1) */
#define REG_CAN0_MMR2    REG_ACCESS(RwReg, 0x400B4240U) /**< \brief (CAN0) Mailbox Mode Register (MB = 2) */
#define REG_CAN0_MAM2    REG_ACCESS(RwReg, 0x400B4244U) /**< \brief (CAN0) Mailbox Acceptance Mask Register (MB = 2) */
#define REG_CAN0_MID2    REG_ACCESS(RwReg, 0x400B4248U) /**< \brief (CAN0) Mailbox ID Register (MB = 2) */
#define REG_CAN0_MFID2   REG_ACCESS(RoReg, 0x400B424CU) /**< \brief (CAN0) Mailbox Family ID Register (MB = 2) */
#define REG_CAN0_MSR2    REG_ACCESS(RoReg, 0x400B4250U) /**< \brief (CAN0) Mailbox Status Register (MB = 2) */
#define REG_CAN0_MDL2    REG_ACCESS(RwReg, 0x400B4254U) /**< \brief (CAN0) Mailbox Data Low Register (MB = 2) */
#define REG_CAN0_MDH2    REG_ACCESS(RwReg, 0x400B4258U) /**< \brief (CAN0) Mailbox Data High Register (MB = 2) */
#define REG_CAN0_MCR2    REG_ACCESS(WoReg, 0x400B425CU) /**< \brief (CAN0) Mailbox Control Register (MB = 2) */
#define REG_CAN0_MMR3    REG_ACCESS(RwReg, 0x400B4260U) /**< \brief (CAN0) Mailbox Mode Register (MB = 3) */
#define REG_CAN0_MAM3    REG_ACCESS(RwReg, 0x400B4264U) /**< \brief (CAN0) Mailbox Acceptance Mask Register (MB = 3) */
#define REG_CAN0_MID3    REG_ACCESS(RwReg, 0x400B4268U) /**< \brief (CAN0) Mailbox ID Register (MB = 3) */
#define REG_CAN0_MFID3   REG_ACCESS(RoReg, 0x400B426CU) /**< \brief (CAN0) Mailbox Family ID Register (MB = 3) */
#define REG_CAN0_MSR3    REG_ACCESS(RoReg, 0x400B4270U) /**< \brief (CAN0) Mailbox Status Register (MB = 3) */
#define REG_CAN0_MDL3    REG_ACCESS(RwReg, 0x400B4274U) /**< \brief (CAN0) Mailbox Data Low Register (MB = 3) */
#define REG_CAN0_MDH3    REG_ACCESS(RwReg, 0x400B4278U) /**< \brief (CAN0) Mailbox Data High Register (MB = 3) */
#define REG_CAN0_MCR3    REG_ACCESS(WoReg, 0x400B427CU) /**< \brief (CAN0) Mailbox Control Register (MB = 3) */
#define REG_CAN0_MMR4    REG_ACCESS(RwReg, 0x400B4280U) /**< \brief (CAN0) Mailbox Mode Register (MB = 4) */
#define REG_CAN0_MAM4    REG_ACCESS(RwReg, 0x400B4284U) /**< \brief (CAN0) Mailbox Acceptance Mask Register (MB = 4) */
#define REG_CAN0_MID4    REG_ACCESS(RwReg, 0x400B4288U) /**< \brief (CAN0) Mailbox ID Register (MB = 4) */
#define REG_CAN0_MFID4   REG_ACCESS(RoReg, 0x400B428CU) /**< \brief (CAN0) Mailbox Family ID Register (MB = 4) */
#define REG_CAN0_MSR4    REG_ACCESS(RoReg, 0x400B4290U) /**< \brief (CAN0) Mailbox Status Register (MB = 4) */
#define REG_CAN0_MDL4    REG_ACCESS(RwReg, 0x400B4294U) /**< \brief (CAN0) Mailbox Data Low Register (MB = 4) */
#define REG_CAN0_MDH4    REG_ACCESS(RwReg, 0x400B4298U) /**< \brief (CAN0) Mailbox Data High Register (MB = 4) */
#define REG_CAN0_MCR4    REG_ACCESS(WoReg, 0x400B429CU) /**< \brief (CAN0) Mailbox Control Register (MB = 4) */
#define REG_CAN0_MMR5    REG_ACCESS(RwReg, 0x400B42A0U) /**< \brief (CAN0) Mailbox Mode Register (MB = 5) */
#define REG_CAN0_MAM5    REG_ACCESS(RwReg, 0x400B42A4U) /**< \brief (CAN0) Mailbox Acceptance Mask Register (MB = 5) */
#define REG_CAN0_MID5    REG_ACCESS(RwReg, 0x400B42A8U) /**< \brief (CAN0) Mailbox ID Register (MB = 5) */
#define REG_CAN0_MFID5   REG_ACCESS(RoReg, 0x400B42ACU) /**< \brief (CAN0) Mailbox Family ID Register (MB = 5) */
#define REG_CAN0_MSR5    REG_ACCESS(RoReg, 0x400B42B0U) /**< \brief (CAN0) Mailbox Status Register (MB = 5) */
#define REG_CAN0_MDL5    REG_ACCESS(RwReg, 0x400B42B4U) /**< \brief (CAN0) Mailbox Data Low Register (MB = 5) */
#define REG_CAN0_MDH5    REG_ACCESS(RwReg, 0x400B42B8U) /**< \brief (CAN0) Mailbox Data High Register (MB = 5) */
#define REG_CAN0_MCR5    REG_ACCESS(WoReg, 0x400B42BCU) /**< \brief (CAN0) Mailbox Control Register (MB = 5) */
#define REG_CAN0_MMR6    REG_ACCESS(RwReg, 0x400B42C0U) /**< \brief (CAN0) Mailbox Mode Register (MB = 6) */
#define REG_CAN0_MAM6    REG_ACCESS(RwReg, 0x400B42C4U) /**< \brief (CAN0) Mailbox Acceptance Mask Register (MB = 6) */
#define REG_CAN0_MID6    REG_ACCESS(RwReg, 0x400B42C8U) /**< \brief (CAN0) Mailbox ID Register (MB = 6) */
#define REG_CAN0_MFID6   REG_ACCESS(RoReg, 0x400B42CCU) /**< \brief (CAN0) Mailbox Family ID Register (MB = 6) */
#define REG_CAN0_MSR6    REG_ACCESS(RoReg, 0x400B42D0U) /**< \brief (CAN0) Mailbox Status Register (MB = 6) */
#define REG_CAN0_MDL6    REG_ACCESS(RwReg, 0x400B42D4U) /**< \brief (CAN0) Mailbox Data Low Register (MB = 6) */
#define REG_CAN0_MDH6    REG_ACCESS(RwReg, 0x400B42D8U) /**< \brief (CAN0) Mailbox Data High Register (MB = 6) */
#define REG_CAN0_MCR6    REG_ACCESS(WoReg, 0x400B42DCU) /**< \brief (CAN0) Mailbox Control Register (MB = 6) */
#define REG_CAN0_MMR7    REG_ACCESS(RwReg, 0x400B42E0U) /**< \brief (CAN0) Mailbox Mode Register (MB = 7) */
#define REG_CAN0_MAM7    REG_ACCESS(RwReg, 0x400B42E4U) /**< \brief (CAN0) Mailbox Acceptance Mask Register (MB = 7) */
#define REG_CAN0_MID7    REG_ACCESS(RwReg, 0x400B42E8U) /**< \brief (CAN0) Mailbox ID Register (MB = 7) */
#define REG_CAN0_MFID7   REG_ACCESS(RoReg, 0x400B42ECU) /**< \brief (CAN0) Mailbox Family ID Register (MB = 7) */
#define REG_CAN0_MSR7    REG_ACCESS(RoReg, 0x400B42F0U) /**< \brief (CAN0) Mailbox Status Register (MB = 7) */
#define REG_CAN0_MDL7    REG_ACCESS(RwReg, 0x400B42F4U) /**< \brief (CAN0) Mailbox Data Low Register (MB = 7) */
#define REG_CAN0_MDH7    REG_ACCESS(RwReg, 0x400B42F8U) /**< \brief (CAN0) Mailbox Data High Register (MB = 7) */
#define REG_CAN0_MCR7    REG_ACCESS(WoReg, 0x400B42FCU) /**< \brief (CAN0) Mailbox Control Register (MB = 7) */

#endif /* _SAM3XA_CAN0_INSTANCE_ */
