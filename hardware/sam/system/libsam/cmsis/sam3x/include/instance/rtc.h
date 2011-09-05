/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_RTC_INSTANCE_
#define _SAM3XA_RTC_INSTANCE_

/* ========== Register definition for RTC peripheral ========== */
#define REG_RTC_CR     REG_ACCESS(RwReg, 0x400E1A60U) /**< \brief (RTC) Control Register */
#define REG_RTC_MR     REG_ACCESS(RwReg, 0x400E1A64U) /**< \brief (RTC) Mode Register */
#define REG_RTC_TIMR   REG_ACCESS(RwReg, 0x400E1A68U) /**< \brief (RTC) Time Register */
#define REG_RTC_CALR   REG_ACCESS(RwReg, 0x400E1A6CU) /**< \brief (RTC) Calendar Register */
#define REG_RTC_TIMALR REG_ACCESS(RwReg, 0x400E1A70U) /**< \brief (RTC) Time Alarm Register */
#define REG_RTC_CALALR REG_ACCESS(RwReg, 0x400E1A74U) /**< \brief (RTC) Calendar Alarm Register */
#define REG_RTC_SR     REG_ACCESS(RoReg, 0x400E1A78U) /**< \brief (RTC) Status Register */
#define REG_RTC_SCCR   REG_ACCESS(WoReg, 0x400E1A7CU) /**< \brief (RTC) Status Clear Command Register */
#define REG_RTC_IER    REG_ACCESS(WoReg, 0x400E1A80U) /**< \brief (RTC) Interrupt Enable Register */
#define REG_RTC_IDR    REG_ACCESS(WoReg, 0x400E1A84U) /**< \brief (RTC) Interrupt Disable Register */
#define REG_RTC_IMR    REG_ACCESS(RoReg, 0x400E1A88U) /**< \brief (RTC) Interrupt Mask Register */
#define REG_RTC_VER    REG_ACCESS(RoReg, 0x400E1A8CU) /**< \brief (RTC) Valid Entry Register */
#define REG_RTC_WPMR   REG_ACCESS(RwReg, 0x400E1B44U) /**< \brief (RTC) Write Protect Mode Register */

#endif /* _SAM3XA_RTC_INSTANCE_ */
