/* $asf_license$ */

#ifndef _SAM3N_MATRIX_INSTANCE_
#define _SAM3N_MATRIX_INSTANCE_

/* ========== Register definition for MATRIX peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_MATRIX_MCFG             (0x400E0200U) /**< \brief (MATRIX) Master Configuration Register */
#define REG_MATRIX_SCFG             (0x400E0240U) /**< \brief (MATRIX) Slave Configuration Register */
#define REG_MATRIX_PRAS0            (0x400E0280U) /**< \brief (MATRIX) Priority Register A for Slave 0 */
#define REG_MATRIX_PRAS1            (0x400E0288U) /**< \brief (MATRIX) Priority Register A for Slave 1 */
#define REG_MATRIX_PRAS2            (0x400E0290U) /**< \brief (MATRIX) Priority Register A for Slave 2 */
#define REG_MATRIX_PRAS3            (0x400E0298U) /**< \brief (MATRIX) Priority Register A for Slave 3 */
#define REG_CCFG_SYSIO              (0x400E0314U) /**< \brief (MATRIX) System I/O Configuration register */
#define REG_MATRIX_WPMR             (0x400E03E4U) /**< \brief (MATRIX) Write Protect Mode Register */
#define REG_MATRIX_WPSR             (0x400E03E8U) /**< \brief (MATRIX) Write Protect Status Register */
#else
#define REG_MATRIX_MCFG    (*(RwReg*)0x400E0200U) /**< \brief (MATRIX) Master Configuration Register */
#define REG_MATRIX_SCFG    (*(RwReg*)0x400E0240U) /**< \brief (MATRIX) Slave Configuration Register */
#define REG_MATRIX_PRAS0   (*(RwReg*)0x400E0280U) /**< \brief (MATRIX) Priority Register A for Slave 0 */
#define REG_MATRIX_PRAS1   (*(RwReg*)0x400E0288U) /**< \brief (MATRIX) Priority Register A for Slave 1 */
#define REG_MATRIX_PRAS2   (*(RwReg*)0x400E0290U) /**< \brief (MATRIX) Priority Register A for Slave 2 */
#define REG_MATRIX_PRAS3   (*(RwReg*)0x400E0298U) /**< \brief (MATRIX) Priority Register A for Slave 3 */
#define REG_CCFG_SYSIO     (*(RwReg*)0x400E0314U) /**< \brief (MATRIX) System I/O Configuration register */
#define REG_MATRIX_WPMR    (*(RwReg*)0x400E03E4U) /**< \brief (MATRIX) Write Protect Mode Register */
#define REG_MATRIX_WPSR    (*(RoReg*)0x400E03E8U) /**< \brief (MATRIX) Write Protect Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3N_MATRIX_INSTANCE_ */
