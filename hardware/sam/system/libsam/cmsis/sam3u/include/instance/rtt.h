/* $asf_license$ */

#ifndef _SAM3U_RTT_INSTANCE_
#define _SAM3U_RTT_INSTANCE_

/* ========== Register definition for RTT peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_RTT_MR          (0x400E1230U) /**< \brief (RTT) Mode Register */
#define REG_RTT_AR          (0x400E1234U) /**< \brief (RTT) Alarm Register */
#define REG_RTT_VR          (0x400E1238U) /**< \brief (RTT) Value Register */
#define REG_RTT_SR          (0x400E123CU) /**< \brief (RTT) Status Register */
#else
#define REG_RTT_MR (*(RwReg*)0x400E1230U) /**< \brief (RTT) Mode Register */
#define REG_RTT_AR (*(RwReg*)0x400E1234U) /**< \brief (RTT) Alarm Register */
#define REG_RTT_VR (*(RoReg*)0x400E1238U) /**< \brief (RTT) Value Register */
#define REG_RTT_SR (*(RoReg*)0x400E123CU) /**< \brief (RTT) Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3U_RTT_INSTANCE_ */
