/* $asf_license$ */

#ifndef _SAM3U_MATRIX_INSTANCE_
#define _SAM3U_MATRIX_INSTANCE_

/* ========== Register definition for MATRIX peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_MATRIX_MCFG              (0x400E0200U) /**< \brief (MATRIX) Master Configuration Register */
#define REG_MATRIX_SCFG              (0x400E0240U) /**< \brief (MATRIX) Slave Configuration Register */
#define REG_MATRIX_PRAS0             (0x400E0280U) /**< \brief (MATRIX) Priority Register A for Slave 0 */
#define REG_MATRIX_PRAS1             (0x400E0288U) /**< \brief (MATRIX) Priority Register A for Slave 1 */
#define REG_MATRIX_PRAS2             (0x400E0290U) /**< \brief (MATRIX) Priority Register A for Slave 2 */
#define REG_MATRIX_PRAS3             (0x400E0298U) /**< \brief (MATRIX) Priority Register A for Slave 3 */
#define REG_MATRIX_PRAS4             (0x400E02A0U) /**< \brief (MATRIX) Priority Register A for Slave 4 */
#define REG_MATRIX_PRAS5             (0x400E02A8U) /**< \brief (MATRIX) Priority Register A for Slave 5 */
#define REG_MATRIX_PRAS6             (0x400E02B0U) /**< \brief (MATRIX) Priority Register A for Slave 6 */
#define REG_MATRIX_PRAS7             (0x400E02B8U) /**< \brief (MATRIX) Priority Register A for Slave 7 */
#define REG_MATRIX_PRAS8             (0x400E02C0U) /**< \brief (MATRIX) Priority Register A for Slave 8 */
#define REG_MATRIX_PRAS9             (0x400E02C8U) /**< \brief (MATRIX) Priority Register A for Slave 9 */
#define REG_MATRIX_MRCR              (0x400E0300U) /**< \brief (MATRIX) Master Remap Control Register */
#define REG_MATRIX_WPMR              (0x400E03E4U) /**< \brief (MATRIX) Write Protect Mode Register */
#define REG_MATRIX_WPSR              (0x400E03E8U) /**< \brief (MATRIX) Write Protect Status Register */
#else
#define REG_MATRIX_MCFG     (*(RwReg*)0x400E0200U) /**< \brief (MATRIX) Master Configuration Register */
#define REG_MATRIX_SCFG     (*(RwReg*)0x400E0240U) /**< \brief (MATRIX) Slave Configuration Register */
#define REG_MATRIX_PRAS0    (*(RwReg*)0x400E0280U) /**< \brief (MATRIX) Priority Register A for Slave 0 */
#define REG_MATRIX_PRAS1    (*(RwReg*)0x400E0288U) /**< \brief (MATRIX) Priority Register A for Slave 1 */
#define REG_MATRIX_PRAS2    (*(RwReg*)0x400E0290U) /**< \brief (MATRIX) Priority Register A for Slave 2 */
#define REG_MATRIX_PRAS3    (*(RwReg*)0x400E0298U) /**< \brief (MATRIX) Priority Register A for Slave 3 */
#define REG_MATRIX_PRAS4    (*(RwReg*)0x400E02A0U) /**< \brief (MATRIX) Priority Register A for Slave 4 */
#define REG_MATRIX_PRAS5    (*(RwReg*)0x400E02A8U) /**< \brief (MATRIX) Priority Register A for Slave 5 */
#define REG_MATRIX_PRAS6    (*(RwReg*)0x400E02B0U) /**< \brief (MATRIX) Priority Register A for Slave 6 */
#define REG_MATRIX_PRAS7    (*(RwReg*)0x400E02B8U) /**< \brief (MATRIX) Priority Register A for Slave 7 */
#define REG_MATRIX_PRAS8    (*(RwReg*)0x400E02C0U) /**< \brief (MATRIX) Priority Register A for Slave 8 */
#define REG_MATRIX_PRAS9    (*(RwReg*)0x400E02C8U) /**< \brief (MATRIX) Priority Register A for Slave 9 */
#define REG_MATRIX_MRCR     (*(RwReg*)0x400E0300U) /**< \brief (MATRIX) Master Remap Control Register */
#define REG_MATRIX_WPMR     (*(RwReg*)0x400E03E4U) /**< \brief (MATRIX) Write Protect Mode Register */
#define REG_MATRIX_WPSR     (*(RoReg*)0x400E03E8U) /**< \brief (MATRIX) Write Protect Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3U_MATRIX_INSTANCE_ */
