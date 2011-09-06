/* $asf_license$ */

#ifndef _SAM3S8_RTC_INSTANCE_
#define _SAM3S8_RTC_INSTANCE_

/* ========== Register definition for RTC peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_RTC_CR              (0x400E1460U) /**< \brief (RTC) Control Register */
#define REG_RTC_MR              (0x400E1464U) /**< \brief (RTC) Mode Register */
#define REG_RTC_TIMR            (0x400E1468U) /**< \brief (RTC) Time Register */
#define REG_RTC_CALR            (0x400E146CU) /**< \brief (RTC) Calendar Register */
#define REG_RTC_TIMALR          (0x400E1470U) /**< \brief (RTC) Time Alarm Register */
#define REG_RTC_CALALR          (0x400E1474U) /**< \brief (RTC) Calendar Alarm Register */
#define REG_RTC_SR              (0x400E1478U) /**< \brief (RTC) Status Register */
#define REG_RTC_SCCR            (0x400E147CU) /**< \brief (RTC) Status Clear Command Register */
#define REG_RTC_IER             (0x400E1480U) /**< \brief (RTC) Interrupt Enable Register */
#define REG_RTC_IDR             (0x400E1484U) /**< \brief (RTC) Interrupt Disable Register */
#define REG_RTC_IMR             (0x400E1488U) /**< \brief (RTC) Interrupt Mask Register */
#define REG_RTC_VER             (0x400E148CU) /**< \brief (RTC) Valid Entry Register */
#else
#define REG_RTC_CR     (*(RwReg*)0x400E1460U) /**< \brief (RTC) Control Register */
#define REG_RTC_MR     (*(RwReg*)0x400E1464U) /**< \brief (RTC) Mode Register */
#define REG_RTC_TIMR   (*(RwReg*)0x400E1468U) /**< \brief (RTC) Time Register */
#define REG_RTC_CALR   (*(RwReg*)0x400E146CU) /**< \brief (RTC) Calendar Register */
#define REG_RTC_TIMALR (*(RwReg*)0x400E1470U) /**< \brief (RTC) Time Alarm Register */
#define REG_RTC_CALALR (*(RwReg*)0x400E1474U) /**< \brief (RTC) Calendar Alarm Register */
#define REG_RTC_SR     (*(RoReg*)0x400E1478U) /**< \brief (RTC) Status Register */
#define REG_RTC_SCCR   (*(WoReg*)0x400E147CU) /**< \brief (RTC) Status Clear Command Register */
#define REG_RTC_IER    (*(WoReg*)0x400E1480U) /**< \brief (RTC) Interrupt Enable Register */
#define REG_RTC_IDR    (*(WoReg*)0x400E1484U) /**< \brief (RTC) Interrupt Disable Register */
#define REG_RTC_IMR    (*(RoReg*)0x400E1488U) /**< \brief (RTC) Interrupt Mask Register */
#define REG_RTC_VER    (*(RoReg*)0x400E148CU) /**< \brief (RTC) Valid Entry Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3S8_RTC_INSTANCE_ */
