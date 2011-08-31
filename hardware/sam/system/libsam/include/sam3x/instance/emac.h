/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_EMAC_INSTANCE_
#define _SAM3XA_EMAC_INSTANCE_

/* ========== Register definition for EMAC peripheral ========== */
#define REG_EMAC_NCR   REG_ACCESS(RwReg, 0x400B0000U) /**< \brief (EMAC) Network Control Register */
#define REG_EMAC_NCFGR REG_ACCESS(RwReg, 0x400B0004U) /**< \brief (EMAC) Network Configuration Register */
#define REG_EMAC_NSR   REG_ACCESS(RoReg, 0x400B0008U) /**< \brief (EMAC) Network Status Register */
#define REG_EMAC_TSR   REG_ACCESS(RwReg, 0x400B0014U) /**< \brief (EMAC) Transmit Status Register */
#define REG_EMAC_RBQP  REG_ACCESS(RwReg, 0x400B0018U) /**< \brief (EMAC) Receive Buffer Queue Pointer Register */
#define REG_EMAC_TBQP  REG_ACCESS(RwReg, 0x400B001CU) /**< \brief (EMAC) Transmit Buffer Queue Pointer Register */
#define REG_EMAC_RSR   REG_ACCESS(RwReg, 0x400B0020U) /**< \brief (EMAC) Receive Status Register */
#define REG_EMAC_ISR   REG_ACCESS(RwReg, 0x400B0024U) /**< \brief (EMAC) Interrupt Status Register */
#define REG_EMAC_IER   REG_ACCESS(WoReg, 0x400B0028U) /**< \brief (EMAC) Interrupt Enable Register */
#define REG_EMAC_IDR   REG_ACCESS(WoReg, 0x400B002CU) /**< \brief (EMAC) Interrupt Disable Register */
#define REG_EMAC_IMR   REG_ACCESS(RoReg, 0x400B0030U) /**< \brief (EMAC) Interrupt Mask Register */
#define REG_EMAC_MAN   REG_ACCESS(RwReg, 0x400B0034U) /**< \brief (EMAC) Phy Maintenance Register */
#define REG_EMAC_PTR   REG_ACCESS(RwReg, 0x400B0038U) /**< \brief (EMAC) Pause Time Register */
#define REG_EMAC_PFR   REG_ACCESS(RwReg, 0x400B003CU) /**< \brief (EMAC) Pause Frames Received Register */
#define REG_EMAC_FTO   REG_ACCESS(RwReg, 0x400B0040U) /**< \brief (EMAC) Frames Transmitted Ok Register */
#define REG_EMAC_SCF   REG_ACCESS(RwReg, 0x400B0044U) /**< \brief (EMAC) Single Collision Frames Register */
#define REG_EMAC_MCF   REG_ACCESS(RwReg, 0x400B0048U) /**< \brief (EMAC) Multiple Collision Frames Register */
#define REG_EMAC_FRO   REG_ACCESS(RwReg, 0x400B004CU) /**< \brief (EMAC) Frames Received Ok Register */
#define REG_EMAC_FCSE  REG_ACCESS(RwReg, 0x400B0050U) /**< \brief (EMAC) Frame Check Sequence Errors Register */
#define REG_EMAC_ALE   REG_ACCESS(RwReg, 0x400B0054U) /**< \brief (EMAC) Alignment Errors Register */
#define REG_EMAC_DTF   REG_ACCESS(RwReg, 0x400B0058U) /**< \brief (EMAC) Deferred Transmission Frames Register */
#define REG_EMAC_LCOL  REG_ACCESS(RwReg, 0x400B005CU) /**< \brief (EMAC) Late Collisions Register */
#define REG_EMAC_ECOL  REG_ACCESS(RwReg, 0x400B0060U) /**< \brief (EMAC) Excessive Collisions Register */
#define REG_EMAC_TUND  REG_ACCESS(RwReg, 0x400B0064U) /**< \brief (EMAC) Transmit Underrun Errors Register */
#define REG_EMAC_CSE   REG_ACCESS(RwReg, 0x400B0068U) /**< \brief (EMAC) Carrier Sense Errors Register */
#define REG_EMAC_RRE   REG_ACCESS(RwReg, 0x400B006CU) /**< \brief (EMAC) Receive Resource Errors Register */
#define REG_EMAC_ROV   REG_ACCESS(RwReg, 0x400B0070U) /**< \brief (EMAC) Receive Overrun Errors Register */
#define REG_EMAC_RSE   REG_ACCESS(RwReg, 0x400B0074U) /**< \brief (EMAC) Receive Symbol Errors Register */
#define REG_EMAC_ELE   REG_ACCESS(RwReg, 0x400B0078U) /**< \brief (EMAC) Excessive Length Errors Register */
#define REG_EMAC_RJA   REG_ACCESS(RwReg, 0x400B007CU) /**< \brief (EMAC) Receive Jabbers Register */
#define REG_EMAC_USF   REG_ACCESS(RwReg, 0x400B0080U) /**< \brief (EMAC) Undersize Frames Register */
#define REG_EMAC_STE   REG_ACCESS(RwReg, 0x400B0084U) /**< \brief (EMAC) SQE Test Errors Register */
#define REG_EMAC_RLE   REG_ACCESS(RwReg, 0x400B0088U) /**< \brief (EMAC) Received Length Field Mismatch Register */
#define REG_EMAC_HRB   REG_ACCESS(RwReg, 0x400B0090U) /**< \brief (EMAC) Hash Register Bottom [31:0] Register */
#define REG_EMAC_HRT   REG_ACCESS(RwReg, 0x400B0094U) /**< \brief (EMAC) Hash Register Top [63:32] Register */
#define REG_EMAC_SA1B  REG_ACCESS(RwReg, 0x400B0098U) /**< \brief (EMAC) Specific Address 1 Bottom Register */
#define REG_EMAC_SA1T  REG_ACCESS(RwReg, 0x400B009CU) /**< \brief (EMAC) Specific Address 1 Top Register */
#define REG_EMAC_SA2B  REG_ACCESS(RwReg, 0x400B00A0U) /**< \brief (EMAC) Specific Address 2 Bottom Register */
#define REG_EMAC_SA2T  REG_ACCESS(RwReg, 0x400B00A4U) /**< \brief (EMAC) Specific Address 2 Top Register */
#define REG_EMAC_SA3B  REG_ACCESS(RwReg, 0x400B00A8U) /**< \brief (EMAC) Specific Address 3 Bottom Register */
#define REG_EMAC_SA3T  REG_ACCESS(RwReg, 0x400B00ACU) /**< \brief (EMAC) Specific Address 3 Top Register */
#define REG_EMAC_SA4B  REG_ACCESS(RwReg, 0x400B00B0U) /**< \brief (EMAC) Specific Address 4 Bottom Register */
#define REG_EMAC_SA4T  REG_ACCESS(RwReg, 0x400B00B4U) /**< \brief (EMAC) Specific Address 4 Top Register */
#define REG_EMAC_TID   REG_ACCESS(RwReg, 0x400B00B8U) /**< \brief (EMAC) Type ID Checking Register */
#define REG_EMAC_USRIO REG_ACCESS(RwReg, 0x400B00C0U) /**< \brief (EMAC) User Input/Output Register */

#endif /* _SAM3XA_EMAC_INSTANCE_ */
