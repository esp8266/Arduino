/* %ATMEL_LICENCE% */

#ifndef _SAM3S_MATRIX_INSTANCE_
#define _SAM3S_MATRIX_INSTANCE_

/* ========== Register definition for MATRIX peripheral ========== */
#define REG_MATRIX_MCFG    REG_ACCESS(RwReg, 0x400E0200U) /**< \brief (MATRIX) Master Configuration Register */
#define REG_MATRIX_SCFG    REG_ACCESS(RwReg, 0x400E0240U) /**< \brief (MATRIX) Slave Configuration Register */
#define REG_MATRIX_PRAS0   REG_ACCESS(RwReg, 0x400E0280U) /**< \brief (MATRIX) Priority Register A for Slave 0 */
#define REG_MATRIX_PRAS1   REG_ACCESS(RwReg, 0x400E0288U) /**< \brief (MATRIX) Priority Register A for Slave 1 */
#define REG_MATRIX_PRAS2   REG_ACCESS(RwReg, 0x400E0290U) /**< \brief (MATRIX) Priority Register A for Slave 2 */
#define REG_MATRIX_PRAS3   REG_ACCESS(RwReg, 0x400E0298U) /**< \brief (MATRIX) Priority Register A for Slave 3 */
#define REG_MATRIX_PRAS4   REG_ACCESS(RwReg, 0x400E02A0U) /**< \brief (MATRIX) Priority Register A for Slave 4 */
#define REG_CCFG_SYSIO     REG_ACCESS(RwReg, 0x400E0314U) /**< \brief (MATRIX) System I/O Configuration register */
#define REG_CCFG_SMCNFCS   REG_ACCESS(RwReg, 0x400E031CU) /**< \brief (MATRIX) SMC Chip Select NAND Flash Assignment Register */
#define REG_MATRIX_WPMR    REG_ACCESS(RwReg, 0x400E03E4U) /**< \brief (MATRIX) Write Protect Mode Register */
#define REG_MATRIX_WPSR    REG_ACCESS(RoReg, 0x400E03E8U) /**< \brief (MATRIX) Write Protect Status Register */

#endif /* _SAM3S_MATRIX_INSTANCE_ */
