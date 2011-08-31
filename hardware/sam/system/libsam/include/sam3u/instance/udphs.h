/* %ATMEL_LICENCE% */

#ifndef _SAM3U_UDPHS_INSTANCE_
#define _SAM3U_UDPHS_INSTANCE_

/* ========== Register definition for UDPHS peripheral ========== */
#define REG_UDPHS_CTRL        REG_ACCESS(RwReg, 0x400A4000U) /**< \brief (UDPHS) UDPHS Control Register */
#define REG_UDPHS_FNUM        REG_ACCESS(RoReg, 0x400A4004U) /**< \brief (UDPHS) UDPHS Frame Number Register */
#define REG_UDPHS_IEN         REG_ACCESS(RwReg, 0x400A4010U) /**< \brief (UDPHS) UDPHS Interrupt Enable Register */
#define REG_UDPHS_INTSTA      REG_ACCESS(RoReg, 0x400A4014U) /**< \brief (UDPHS) UDPHS Interrupt Status Register */
#define REG_UDPHS_CLRINT      REG_ACCESS(WoReg, 0x400A4018U) /**< \brief (UDPHS) UDPHS Clear Interrupt Register */
#define REG_UDPHS_EPTRST      REG_ACCESS(WoReg, 0x400A401CU) /**< \brief (UDPHS) UDPHS Endpoints Reset Register */
#define REG_UDPHS_TST         REG_ACCESS(RwReg, 0x400A40E0U) /**< \brief (UDPHS) UDPHS Test Register */
#define REG_UDPHS_IPNAME1     REG_ACCESS(RoReg, 0x400A40F0U) /**< \brief (UDPHS) UDPHS Name1 Register */
#define REG_UDPHS_IPNAME2     REG_ACCESS(RoReg, 0x400A40F4U) /**< \brief (UDPHS) UDPHS Name2 Register */
#define REG_UDPHS_IPFEATURES  REG_ACCESS(RoReg, 0x400A40F8U) /**< \brief (UDPHS) UDPHS Features Register */
#define REG_UDPHS_EPTCFG0     REG_ACCESS(RwReg, 0x400A4100U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 0) */
#define REG_UDPHS_EPTCTLENB0  REG_ACCESS(WoReg, 0x400A4104U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 0) */
#define REG_UDPHS_EPTCTLDIS0  REG_ACCESS(WoReg, 0x400A4108U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 0) */
#define REG_UDPHS_EPTCTL0     REG_ACCESS(RoReg, 0x400A410CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 0) */
#define REG_UDPHS_EPTSETSTA0  REG_ACCESS(WoReg, 0x400A4114U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 0) */
#define REG_UDPHS_EPTCLRSTA0  REG_ACCESS(WoReg, 0x400A4118U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 0) */
#define REG_UDPHS_EPTSTA0     REG_ACCESS(RoReg, 0x400A411CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 0) */
#define REG_UDPHS_EPTCFG1     REG_ACCESS(RwReg, 0x400A4120U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 1) */
#define REG_UDPHS_EPTCTLENB1  REG_ACCESS(WoReg, 0x400A4124U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 1) */
#define REG_UDPHS_EPTCTLDIS1  REG_ACCESS(WoReg, 0x400A4128U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 1) */
#define REG_UDPHS_EPTCTL1     REG_ACCESS(RoReg, 0x400A412CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 1) */
#define REG_UDPHS_EPTSETSTA1  REG_ACCESS(WoReg, 0x400A4134U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 1) */
#define REG_UDPHS_EPTCLRSTA1  REG_ACCESS(WoReg, 0x400A4138U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 1) */
#define REG_UDPHS_EPTSTA1     REG_ACCESS(RoReg, 0x400A413CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 1) */
#define REG_UDPHS_EPTCFG2     REG_ACCESS(RwReg, 0x400A4140U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 2) */
#define REG_UDPHS_EPTCTLENB2  REG_ACCESS(WoReg, 0x400A4144U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 2) */
#define REG_UDPHS_EPTCTLDIS2  REG_ACCESS(WoReg, 0x400A4148U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 2) */
#define REG_UDPHS_EPTCTL2     REG_ACCESS(RoReg, 0x400A414CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 2) */
#define REG_UDPHS_EPTSETSTA2  REG_ACCESS(WoReg, 0x400A4154U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 2) */
#define REG_UDPHS_EPTCLRSTA2  REG_ACCESS(WoReg, 0x400A4158U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 2) */
#define REG_UDPHS_EPTSTA2     REG_ACCESS(RoReg, 0x400A415CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 2) */
#define REG_UDPHS_EPTCFG3     REG_ACCESS(RwReg, 0x400A4160U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 3) */
#define REG_UDPHS_EPTCTLENB3  REG_ACCESS(WoReg, 0x400A4164U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 3) */
#define REG_UDPHS_EPTCTLDIS3  REG_ACCESS(WoReg, 0x400A4168U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 3) */
#define REG_UDPHS_EPTCTL3     REG_ACCESS(RoReg, 0x400A416CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 3) */
#define REG_UDPHS_EPTSETSTA3  REG_ACCESS(WoReg, 0x400A4174U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 3) */
#define REG_UDPHS_EPTCLRSTA3  REG_ACCESS(WoReg, 0x400A4178U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 3) */
#define REG_UDPHS_EPTSTA3     REG_ACCESS(RoReg, 0x400A417CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 3) */
#define REG_UDPHS_EPTCFG4     REG_ACCESS(RwReg, 0x400A4180U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 4) */
#define REG_UDPHS_EPTCTLENB4  REG_ACCESS(WoReg, 0x400A4184U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 4) */
#define REG_UDPHS_EPTCTLDIS4  REG_ACCESS(WoReg, 0x400A4188U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 4) */
#define REG_UDPHS_EPTCTL4     REG_ACCESS(RoReg, 0x400A418CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 4) */
#define REG_UDPHS_EPTSETSTA4  REG_ACCESS(WoReg, 0x400A4194U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 4) */
#define REG_UDPHS_EPTCLRSTA4  REG_ACCESS(WoReg, 0x400A4198U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 4) */
#define REG_UDPHS_EPTSTA4     REG_ACCESS(RoReg, 0x400A419CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 4) */
#define REG_UDPHS_EPTCFG5     REG_ACCESS(RwReg, 0x400A41A0U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 5) */
#define REG_UDPHS_EPTCTLENB5  REG_ACCESS(WoReg, 0x400A41A4U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 5) */
#define REG_UDPHS_EPTCTLDIS5  REG_ACCESS(WoReg, 0x400A41A8U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 5) */
#define REG_UDPHS_EPTCTL5     REG_ACCESS(RoReg, 0x400A41ACU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 5) */
#define REG_UDPHS_EPTSETSTA5  REG_ACCESS(WoReg, 0x400A41B4U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 5) */
#define REG_UDPHS_EPTCLRSTA5  REG_ACCESS(WoReg, 0x400A41B8U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 5) */
#define REG_UDPHS_EPTSTA5     REG_ACCESS(RoReg, 0x400A41BCU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 5) */
#define REG_UDPHS_EPTCFG6     REG_ACCESS(RwReg, 0x400A41C0U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 6) */
#define REG_UDPHS_EPTCTLENB6  REG_ACCESS(WoReg, 0x400A41C4U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 6) */
#define REG_UDPHS_EPTCTLDIS6  REG_ACCESS(WoReg, 0x400A41C8U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 6) */
#define REG_UDPHS_EPTCTL6     REG_ACCESS(RoReg, 0x400A41CCU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 6) */
#define REG_UDPHS_EPTSETSTA6  REG_ACCESS(WoReg, 0x400A41D4U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 6) */
#define REG_UDPHS_EPTCLRSTA6  REG_ACCESS(WoReg, 0x400A41D8U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 6) */
#define REG_UDPHS_EPTSTA6     REG_ACCESS(RoReg, 0x400A41DCU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 6) */
#define REG_UDPHS_DMANXTDSC0  REG_ACCESS(RwReg, 0x400A4300U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 0) */
#define REG_UDPHS_DMAADDRESS0 REG_ACCESS(RwReg, 0x400A4304U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 0) */
#define REG_UDPHS_DMACONTROL0 REG_ACCESS(RwReg, 0x400A4308U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 0) */
#define REG_UDPHS_DMASTATUS0  REG_ACCESS(RwReg, 0x400A430CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 0) */
#define REG_UDPHS_DMANXTDSC1  REG_ACCESS(RwReg, 0x400A4310U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 1) */
#define REG_UDPHS_DMAADDRESS1 REG_ACCESS(RwReg, 0x400A4314U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 1) */
#define REG_UDPHS_DMACONTROL1 REG_ACCESS(RwReg, 0x400A4318U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 1) */
#define REG_UDPHS_DMASTATUS1  REG_ACCESS(RwReg, 0x400A431CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 1) */
#define REG_UDPHS_DMANXTDSC2  REG_ACCESS(RwReg, 0x400A4320U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 2) */
#define REG_UDPHS_DMAADDRESS2 REG_ACCESS(RwReg, 0x400A4324U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 2) */
#define REG_UDPHS_DMACONTROL2 REG_ACCESS(RwReg, 0x400A4328U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 2) */
#define REG_UDPHS_DMASTATUS2  REG_ACCESS(RwReg, 0x400A432CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 2) */
#define REG_UDPHS_DMANXTDSC3  REG_ACCESS(RwReg, 0x400A4330U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 3) */
#define REG_UDPHS_DMAADDRESS3 REG_ACCESS(RwReg, 0x400A4334U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 3) */
#define REG_UDPHS_DMACONTROL3 REG_ACCESS(RwReg, 0x400A4338U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 3) */
#define REG_UDPHS_DMASTATUS3  REG_ACCESS(RwReg, 0x400A433CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 3) */
#define REG_UDPHS_DMANXTDSC4  REG_ACCESS(RwReg, 0x400A4340U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 4) */
#define REG_UDPHS_DMAADDRESS4 REG_ACCESS(RwReg, 0x400A4344U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 4) */
#define REG_UDPHS_DMACONTROL4 REG_ACCESS(RwReg, 0x400A4348U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 4) */
#define REG_UDPHS_DMASTATUS4  REG_ACCESS(RwReg, 0x400A434CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 4) */
#define REG_UDPHS_DMANXTDSC5  REG_ACCESS(RwReg, 0x400A4350U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 5) */
#define REG_UDPHS_DMAADDRESS5 REG_ACCESS(RwReg, 0x400A4354U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 5) */
#define REG_UDPHS_DMACONTROL5 REG_ACCESS(RwReg, 0x400A4358U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 5) */
#define REG_UDPHS_DMASTATUS5  REG_ACCESS(RwReg, 0x400A435CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 5) */

#endif /* _SAM3U_UDPHS_INSTANCE_ */
