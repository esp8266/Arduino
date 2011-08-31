/* %ATMEL_LICENCE% */

#ifndef _SAM3S_PMC_INSTANCE_
#define _SAM3S_PMC_INSTANCE_

/* ========== Register definition for PMC peripheral ========== */
#define REG_PMC_SCER   REG_ACCESS(WoReg, 0x400E0400U) /**< \brief (PMC) System Clock Enable Register */
#define REG_PMC_SCDR   REG_ACCESS(WoReg, 0x400E0404U) /**< \brief (PMC) System Clock Disable Register */
#define REG_PMC_SCSR   REG_ACCESS(RoReg, 0x400E0408U) /**< \brief (PMC) System Clock Status Register */
#define REG_PMC_PCER0  REG_ACCESS(WoReg, 0x400E0410U) /**< \brief (PMC) Peripheral Clock Enable Register 0 */
#define REG_PMC_PCDR0  REG_ACCESS(WoReg, 0x400E0414U) /**< \brief (PMC) Peripheral Clock Disable Register 0 */
#define REG_PMC_PCSR0  REG_ACCESS(RoReg, 0x400E0418U) /**< \brief (PMC) Peripheral Clock Status Register 0 */
#define REG_CKGR_MOR   REG_ACCESS(RwReg, 0x400E0420U) /**< \brief (PMC) Main Oscillator Register */
#define REG_CKGR_MCFR  REG_ACCESS(RoReg, 0x400E0424U) /**< \brief (PMC) Main Clock Frequency Register */
#define REG_CKGR_PLLAR REG_ACCESS(RwReg, 0x400E0428U) /**< \brief (PMC) PLLA Register */
#define REG_CKGR_PLLBR REG_ACCESS(RwReg, 0x400E042CU) /**< \brief (PMC) PLLB Register */
#define REG_PMC_MCKR   REG_ACCESS(RwReg, 0x400E0430U) /**< \brief (PMC) Master Clock Register */
#define REG_PMC_USB    REG_ACCESS(RwReg, 0x400E0438U) /**< \brief (PMC) USB Clock Register */
#define REG_PMC_PCK    REG_ACCESS(RwReg, 0x400E0440U) /**< \brief (PMC) Programmable Clock 0 Register */
#define REG_PMC_IER    REG_ACCESS(WoReg, 0x400E0460U) /**< \brief (PMC) Interrupt Enable Register */
#define REG_PMC_IDR    REG_ACCESS(WoReg, 0x400E0464U) /**< \brief (PMC) Interrupt Disable Register */
#define REG_PMC_SR     REG_ACCESS(RoReg, 0x400E0468U) /**< \brief (PMC) Status Register */
#define REG_PMC_IMR    REG_ACCESS(RoReg, 0x400E046CU) /**< \brief (PMC) Interrupt Mask Register */
#define REG_PMC_FSMR   REG_ACCESS(RwReg, 0x400E0470U) /**< \brief (PMC) Fast Startup Mode Register */
#define REG_PMC_FSPR   REG_ACCESS(RwReg, 0x400E0474U) /**< \brief (PMC) Fast Startup Polarity Register */
#define REG_PMC_FOCR   REG_ACCESS(WoReg, 0x400E0478U) /**< \brief (PMC) Fault Output Clear Register */
#define REG_PMC_WPMR   REG_ACCESS(RwReg, 0x400E04E4U) /**< \brief (PMC) Write Protect Mode Register */
#define REG_PMC_WPSR   REG_ACCESS(RoReg, 0x400E04E8U) /**< \brief (PMC) Write Protect Status Register */
#define REG_PMC_PCER1  REG_ACCESS(WoReg, 0x400E0500U) /**< \brief (PMC) Peripheral Clock Enable Register 1 */
#define REG_PMC_PCDR1  REG_ACCESS(WoReg, 0x400E0504U) /**< \brief (PMC) Peripheral Clock Disable Register 1 */
#define REG_PMC_PCSR1  REG_ACCESS(RoReg, 0x400E0508U) /**< \brief (PMC) Peripheral Clock Status Register 1 */
#define REG_PMC_OCR    REG_ACCESS(RwReg, 0x400E0510U) /**< \brief (PMC) Oscillator Calibration Register */

#endif /* _SAM3S_PMC_INSTANCE_ */
