/* %ATMEL_LICENCE% */

#ifndef _SAM3N_RTC_INSTANCE_
#define _SAM3N_RTC_INSTANCE_

/* ========== Register definition for RTC peripheral ========== */
#define REG_RTC_CR     REG_ACCESS(RwReg, 0x400E1460U) /**< \brief (RTC) Control Register */
#define REG_RTC_MR     REG_ACCESS(RwReg, 0x400E1464U) /**< \brief (RTC) Mode Register */
#define REG_RTC_TIMR   REG_ACCESS(RwReg, 0x400E1468U) /**< \brief (RTC) Time Register */
#define REG_RTC_CALR   REG_ACCESS(RwReg, 0x400E146CU) /**< \brief (RTC) Calendar Register */
#define REG_RTC_TIMALR REG_ACCESS(RwReg, 0x400E1470U) /**< \brief (RTC) Time Alarm Register */
#define REG_RTC_CALALR REG_ACCESS(RwReg, 0x400E1474U) /**< \brief (RTC) Calendar Alarm Register */
#define REG_RTC_SR     REG_ACCESS(RoReg, 0x400E1478U) /**< \brief (RTC) Status Register */
#define REG_RTC_SCCR   REG_ACCESS(WoReg, 0x400E147CU) /**< \brief (RTC) Status Clear Command Register */
#define REG_RTC_IER    REG_ACCESS(WoReg, 0x400E1480U) /**< \brief (RTC) Interrupt Enable Register */
#define REG_RTC_IDR    REG_ACCESS(WoReg, 0x400E1484U) /**< \brief (RTC) Interrupt Disable Register */
#define REG_RTC_IMR    REG_ACCESS(RoReg, 0x400E1488U) /**< \brief (RTC) Interrupt Mask Register */
#define REG_RTC_VER    REG_ACCESS(RoReg, 0x400E148CU) /**< \brief (RTC) Valid Entry Register */
#define REG_RTC_WPMR   REG_ACCESS(RwReg, 0x400E1544U) /**< \brief (RTC) Write Protect Mode Register */

#endif /* _SAM3N_RTC_INSTANCE_ */
