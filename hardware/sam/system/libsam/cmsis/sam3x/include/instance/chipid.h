/* $asf_license$ */

#ifndef _SAM3XA_CHIPID_INSTANCE_
#define _SAM3XA_CHIPID_INSTANCE_

/* ========== Register definition for CHIPID peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_CHIPID_CIDR          (0x400E0940U) /**< \brief (CHIPID) Chip ID Register */
#define REG_CHIPID_EXID          (0x400E0944U) /**< \brief (CHIPID) Chip ID Extension Register */
#else
#define REG_CHIPID_CIDR (*(RoReg*)0x400E0940U) /**< \brief (CHIPID) Chip ID Register */
#define REG_CHIPID_EXID (*(RoReg*)0x400E0944U) /**< \brief (CHIPID) Chip ID Extension Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3XA_CHIPID_INSTANCE_ */
