/* %ATMEL_LICENCE% */

#ifndef _SAM3N_PWM_INSTANCE_
#define _SAM3N_PWM_INSTANCE_

/* ========== Register definition for PWM peripheral ========== */
#define REG_PWM_MR    REG_ACCESS(RwReg, 0x40020000U) /**< \brief (PWM) PWM Mode Register */
#define REG_PWM_ENA   REG_ACCESS(WoReg, 0x40020004U) /**< \brief (PWM) PWM Enable Register */
#define REG_PWM_DIS   REG_ACCESS(WoReg, 0x40020008U) /**< \brief (PWM) PWM Disable Register */
#define REG_PWM_SR    REG_ACCESS(RoReg, 0x4002000CU) /**< \brief (PWM) PWM Status Register */
#define REG_PWM_IER   REG_ACCESS(WoReg, 0x40020010U) /**< \brief (PWM) PWM Interrupt Enable Register */
#define REG_PWM_IDR   REG_ACCESS(WoReg, 0x40020014U) /**< \brief (PWM) PWM Interrupt Disable Register */
#define REG_PWM_IMR   REG_ACCESS(RoReg, 0x40020018U) /**< \brief (PWM) PWM Interrupt Mask Register */
#define REG_PWM_ISR   REG_ACCESS(RoReg, 0x4002001CU) /**< \brief (PWM) PWM Interrupt Status Register */
#define REG_PWM_CMR0  REG_ACCESS(RwReg, 0x40020200U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 0) */
#define REG_PWM_CDTY0 REG_ACCESS(RwReg, 0x40020204U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 0) */
#define REG_PWM_CPRD0 REG_ACCESS(RwReg, 0x40020208U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 0) */
#define REG_PWM_CCNT0 REG_ACCESS(RoReg, 0x4002020CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 0) */
#define REG_PWM_CUPD0 REG_ACCESS(WoReg, 0x40020210U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 0) */
#define REG_PWM_CMR1  REG_ACCESS(RwReg, 0x40020220U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 1) */
#define REG_PWM_CDTY1 REG_ACCESS(RwReg, 0x40020224U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 1) */
#define REG_PWM_CPRD1 REG_ACCESS(RwReg, 0x40020228U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 1) */
#define REG_PWM_CCNT1 REG_ACCESS(RoReg, 0x4002022CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 1) */
#define REG_PWM_CUPD1 REG_ACCESS(WoReg, 0x40020230U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 1) */
#define REG_PWM_CMR2  REG_ACCESS(RwReg, 0x40020240U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 2) */
#define REG_PWM_CDTY2 REG_ACCESS(RwReg, 0x40020244U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 2) */
#define REG_PWM_CPRD2 REG_ACCESS(RwReg, 0x40020248U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 2) */
#define REG_PWM_CCNT2 REG_ACCESS(RoReg, 0x4002024CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 2) */
#define REG_PWM_CUPD2 REG_ACCESS(WoReg, 0x40020250U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 2) */
#define REG_PWM_CMR3  REG_ACCESS(RwReg, 0x40020260U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 3) */
#define REG_PWM_CDTY3 REG_ACCESS(RwReg, 0x40020264U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 3) */
#define REG_PWM_CPRD3 REG_ACCESS(RwReg, 0x40020268U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 3) */
#define REG_PWM_CCNT3 REG_ACCESS(RoReg, 0x4002026CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 3) */
#define REG_PWM_CUPD3 REG_ACCESS(WoReg, 0x40020270U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 3) */

#endif /* _SAM3N_PWM_INSTANCE_ */
