/* $asf_license$ */

#ifndef _SAM3XA_RTT_INSTANCE_
#define _SAM3XA_RTT_INSTANCE_

/* ========== Register definition for RTT peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_RTT_MR          (0x400E1A30U) /**< \brief (RTT) Mode Register */
#define REG_RTT_AR          (0x400E1A34U) /**< \brief (RTT) Alarm Register */
#define REG_RTT_VR          (0x400E1A38U) /**< \brief (RTT) Value Register */
#define REG_RTT_SR          (0x400E1A3CU) /**< \brief (RTT) Status Register */
#else
#define REG_RTT_MR (*(RwReg*)0x400E1A30U) /**< \brief (RTT) Mode Register */
#define REG_RTT_AR (*(RwReg*)0x400E1A34U) /**< \brief (RTT) Alarm Register */
#define REG_RTT_VR (*(RoReg*)0x400E1A38U) /**< \brief (RTT) Value Register */
#define REG_RTT_SR (*(RoReg*)0x400E1A3CU) /**< \brief (RTT) Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3XA_RTT_INSTANCE_ */
