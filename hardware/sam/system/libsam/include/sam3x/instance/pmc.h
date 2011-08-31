/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_PMC_INSTANCE_
#define _SAM3XA_PMC_INSTANCE_

/* ========== Register definition for PMC peripheral ========== */
#define REG_PMC_SCER   REG_ACCESS(WoReg, 0x400E0600U) /**< \brief (PMC) System Clock Enable Register */
#define REG_PMC_SCDR   REG_ACCESS(WoReg, 0x400E0604U) /**< \brief (PMC) System Clock Disable Register */
#define REG_PMC_SCSR   REG_ACCESS(RoReg, 0x400E0608U) /**< \brief (PMC) System Clock Status Register */
#define REG_PMC_PCER0  REG_ACCESS(WoReg, 0x400E0610U) /**< \brief (PMC) Peripheral Clock Enable Register 0 */
#define REG_PMC_PCDR0  REG_ACCESS(WoReg, 0x400E0614U) /**< \brief (PMC) Peripheral Clock Disable Register 0 */
#define REG_PMC_PCSR0  REG_ACCESS(RoReg, 0x400E0618U) /**< \brief (PMC) Peripheral Clock Status Register 0 */
#define REG_CKGR_UCKR  REG_ACCESS(RwReg, 0x400E061CU) /**< \brief (PMC) UTMI Clock Register */
#define REG_CKGR_MOR   REG_ACCESS(RwReg, 0x400E0620U) /**< \brief (PMC) Main Oscillator Register */
#define REG_CKGR_MCFR  REG_ACCESS(RoReg, 0x400E0624U) /**< \brief (PMC) Main Clock Frequency Register */
#define REG_CKGR_PLLAR REG_ACCESS(RwReg, 0x400E0628U) /**< \brief (PMC) PLLA Register */
#define REG_PMC_MCKR   REG_ACCESS(RwReg, 0x400E0630U) /**< \brief (PMC) Master Clock Register */
#define REG_PMC_USB    REG_ACCESS(RwReg, 0x400E0638U) /**< \brief (PMC) USB Clock Register */
#define REG_PMC_PCK    REG_ACCESS(RwReg, 0x400E0640U) /**< \brief (PMC) Programmable Clock 0 Register */
#define REG_PMC_IER    REG_ACCESS(WoReg, 0x400E0660U) /**< \brief (PMC) Interrupt Enable Register */
#define REG_PMC_IDR    REG_ACCESS(WoReg, 0x400E0664U) /**< \brief (PMC) Interrupt Disable Register */
#define REG_PMC_SR     REG_ACCESS(RoReg, 0x400E0668U) /**< \brief (PMC) Status Register */
#define REG_PMC_IMR    REG_ACCESS(RoReg, 0x400E066CU) /**< \brief (PMC) Interrupt Mask Register */
#define REG_PMC_FSMR   REG_ACCESS(RwReg, 0x400E0670U) /**< \brief (PMC) Fast Startup Mode Register */
#define REG_PMC_FSPR   REG_ACCESS(RwReg, 0x400E0674U) /**< \brief (PMC) Fast Startup Polarity Register */
#define REG_PMC_FOCR   REG_ACCESS(WoReg, 0x400E0678U) /**< \brief (PMC) Fault Output Clear Register */
#define REG_PMC_WPMR   REG_ACCESS(RwReg, 0x400E06E4U) /**< \brief (PMC) Write Protect Mode Register */
#define REG_PMC_WPSR   REG_ACCESS(RoReg, 0x400E06E8U) /**< \brief (PMC) Write Protect Status Register */
#define REG_PMC_PCER1  REG_ACCESS(WoReg, 0x400E0700U) /**< \brief (PMC) Peripheral Clock Enable Register 1 */
#define REG_PMC_PCDR1  REG_ACCESS(WoReg, 0x400E0704U) /**< \brief (PMC) Peripheral Clock Disable Register 1 */
#define REG_PMC_PCSR1  REG_ACCESS(RoReg, 0x400E0708U) /**< \brief (PMC) Peripheral Clock Status Register 1 */
#define REG_PMC_PCR    REG_ACCESS(RwReg, 0x400E070CU) /**< \brief (PMC) Peripheral Control Register */

#endif /* _SAM3XA_PMC_INSTANCE_ */
