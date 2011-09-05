/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_UOTGHS_INSTANCE_
#define _SAM3XA_UOTGHS_INSTANCE_

/* ========== Register definition for UOTGHS peripheral ========== */
#define REG_UOTGHS_DEVCTRL        REG_ACCESS(RwReg, 0x400AC000U) /**< \brief (UOTGHS) Device General Control Register */
#define REG_UOTGHS_DEVISR         REG_ACCESS(RoReg, 0x400AC004U) /**< \brief (UOTGHS) Device Global Interrupt Status Register */
#define REG_UOTGHS_DEVICR         REG_ACCESS(WoReg, 0x400AC008U) /**< \brief (UOTGHS) Device Global Interrupt Clear Register */
#define REG_UOTGHS_DEVIFR         REG_ACCESS(WoReg, 0x400AC00CU) /**< \brief (UOTGHS) Device Global Interrupt Set Register */
#define REG_UOTGHS_DEVIMR         REG_ACCESS(RoReg, 0x400AC010U) /**< \brief (UOTGHS) Device Global Interrupt Mask Register */
#define REG_UOTGHS_DEVIDR         REG_ACCESS(WoReg, 0x400AC014U) /**< \brief (UOTGHS) Device Global Interrupt Disable Register */
#define REG_UOTGHS_DEVIER         REG_ACCESS(WoReg, 0x400AC018U) /**< \brief (UOTGHS) Device Global Interrupt Enable Register */
#define REG_UOTGHS_DEVEPT         REG_ACCESS(RwReg, 0x400AC01CU) /**< \brief (UOTGHS) Device Endpoint Register */
#define REG_UOTGHS_DEVFNUM        REG_ACCESS(RoReg, 0x400AC020U) /**< \brief (UOTGHS) Device Frame Number Register */
#define REG_UOTGHS_DEVEPTCFG      REG_ACCESS(RwReg, 0x400AC100U) /**< \brief (UOTGHS) Device Endpoint Configuration Register (n = 0) */
#define REG_UOTGHS_DEVEPTISR      REG_ACCESS(RoReg, 0x400AC130U) /**< \brief (UOTGHS) Device Endpoint Status Register (n = 0) */
#define REG_UOTGHS_DEVEPTICR      REG_ACCESS(WoReg, 0x400AC160U) /**< \brief (UOTGHS) Device Endpoint Clear Register (n = 0) */
#define REG_UOTGHS_DEVEPTIFR      REG_ACCESS(WoReg, 0x400AC190U) /**< \brief (UOTGHS) Device Endpoint Set Register (n = 0) */
#define REG_UOTGHS_DEVEPTIMR      REG_ACCESS(RoReg, 0x400AC1C0U) /**< \brief (UOTGHS) Device Endpoint Mask Register (n = 0) */
#define REG_UOTGHS_DEVEPTIER      REG_ACCESS(WoReg, 0x400AC1F0U) /**< \brief (UOTGHS) Device Endpoint Enable Register (n = 0) */
#define REG_UOTGHS_DEVEPTIDR      REG_ACCESS(WoReg, 0x400AC220U) /**< \brief (UOTGHS) Device Endpoint Disable Register (n = 0) */
#define REG_UOTGHS_DEVDMANXTDSC1  REG_ACCESS(RwReg, 0x400AC310U) /**< \brief (UOTGHS) Device DMA Channel Next Descriptor Address Register (n = 1) */
#define REG_UOTGHS_DEVDMAADDRESS1 REG_ACCESS(RwReg, 0x400AC314U) /**< \brief (UOTGHS) Device DMA Channel Address Register (n = 1) */
#define REG_UOTGHS_DEVDMACONTROL1 REG_ACCESS(RwReg, 0x400AC318U) /**< \brief (UOTGHS) Device DMA Channel Control Register (n = 1) */
#define REG_UOTGHS_DEVDMASTATUS1  REG_ACCESS(RwReg, 0x400AC31CU) /**< \brief (UOTGHS) Device DMA Channel Status Register (n = 1) */
#define REG_UOTGHS_DEVDMANXTDSC2  REG_ACCESS(RwReg, 0x400AC320U) /**< \brief (UOTGHS) Device DMA Channel Next Descriptor Address Register (n = 2) */
#define REG_UOTGHS_DEVDMAADDRESS2 REG_ACCESS(RwReg, 0x400AC324U) /**< \brief (UOTGHS) Device DMA Channel Address Register (n = 2) */
#define REG_UOTGHS_DEVDMACONTROL2 REG_ACCESS(RwReg, 0x400AC328U) /**< \brief (UOTGHS) Device DMA Channel Control Register (n = 2) */
#define REG_UOTGHS_DEVDMASTATUS2  REG_ACCESS(RwReg, 0x400AC32CU) /**< \brief (UOTGHS) Device DMA Channel Status Register (n = 2) */
#define REG_UOTGHS_DEVDMANXTDSC3  REG_ACCESS(RwReg, 0x400AC330U) /**< \brief (UOTGHS) Device DMA Channel Next Descriptor Address Register (n = 3) */
#define REG_UOTGHS_DEVDMAADDRESS3 REG_ACCESS(RwReg, 0x400AC334U) /**< \brief (UOTGHS) Device DMA Channel Address Register (n = 3) */
#define REG_UOTGHS_DEVDMACONTROL3 REG_ACCESS(RwReg, 0x400AC338U) /**< \brief (UOTGHS) Device DMA Channel Control Register (n = 3) */
#define REG_UOTGHS_DEVDMASTATUS3  REG_ACCESS(RwReg, 0x400AC33CU) /**< \brief (UOTGHS) Device DMA Channel Status Register (n = 3) */
#define REG_UOTGHS_DEVDMANXTDSC4  REG_ACCESS(RwReg, 0x400AC340U) /**< \brief (UOTGHS) Device DMA Channel Next Descriptor Address Register (n = 4) */
#define REG_UOTGHS_DEVDMAADDRESS4 REG_ACCESS(RwReg, 0x400AC344U) /**< \brief (UOTGHS) Device DMA Channel Address Register (n = 4) */
#define REG_UOTGHS_DEVDMACONTROL4 REG_ACCESS(RwReg, 0x400AC348U) /**< \brief (UOTGHS) Device DMA Channel Control Register (n = 4) */
#define REG_UOTGHS_DEVDMASTATUS4  REG_ACCESS(RwReg, 0x400AC34CU) /**< \brief (UOTGHS) Device DMA Channel Status Register (n = 4) */
#define REG_UOTGHS_DEVDMANXTDSC5  REG_ACCESS(RwReg, 0x400AC350U) /**< \brief (UOTGHS) Device DMA Channel Next Descriptor Address Register (n = 5) */
#define REG_UOTGHS_DEVDMAADDRESS5 REG_ACCESS(RwReg, 0x400AC354U) /**< \brief (UOTGHS) Device DMA Channel Address Register (n = 5) */
#define REG_UOTGHS_DEVDMACONTROL5 REG_ACCESS(RwReg, 0x400AC358U) /**< \brief (UOTGHS) Device DMA Channel Control Register (n = 5) */
#define REG_UOTGHS_DEVDMASTATUS5  REG_ACCESS(RwReg, 0x400AC35CU) /**< \brief (UOTGHS) Device DMA Channel Status Register (n = 5) */
#define REG_UOTGHS_DEVDMANXTDSC6  REG_ACCESS(RwReg, 0x400AC360U) /**< \brief (UOTGHS) Device DMA Channel Next Descriptor Address Register (n = 6) */
#define REG_UOTGHS_DEVDMAADDRESS6 REG_ACCESS(RwReg, 0x400AC364U) /**< \brief (UOTGHS) Device DMA Channel Address Register (n = 6) */
#define REG_UOTGHS_DEVDMACONTROL6 REG_ACCESS(RwReg, 0x400AC368U) /**< \brief (UOTGHS) Device DMA Channel Control Register (n = 6) */
#define REG_UOTGHS_DEVDMASTATUS6  REG_ACCESS(RwReg, 0x400AC36CU) /**< \brief (UOTGHS) Device DMA Channel Status Register (n = 6) */
#define REG_UOTGHS_DEVDMANXTDSC7  REG_ACCESS(RwReg, 0x400AC370U) /**< \brief (UOTGHS) Device DMA Channel Next Descriptor Address Register (n = 7) */
#define REG_UOTGHS_DEVDMAADDRESS7 REG_ACCESS(RwReg, 0x400AC374U) /**< \brief (UOTGHS) Device DMA Channel Address Register (n = 7) */
#define REG_UOTGHS_DEVDMACONTROL7 REG_ACCESS(RwReg, 0x400AC378U) /**< \brief (UOTGHS) Device DMA Channel Control Register (n = 7) */
#define REG_UOTGHS_DEVDMASTATUS7  REG_ACCESS(RwReg, 0x400AC37CU) /**< \brief (UOTGHS) Device DMA Channel Status Register (n = 7) */
#define REG_UOTGHS_HSTCTRL        REG_ACCESS(RwReg, 0x400AC400U) /**< \brief (UOTGHS) Host General Control Register */
#define REG_UOTGHS_HSTISR         REG_ACCESS(RoReg, 0x400AC404U) /**< \brief (UOTGHS) Host Global Interrupt Status Register */
#define REG_UOTGHS_HSTICR         REG_ACCESS(WoReg, 0x400AC408U) /**< \brief (UOTGHS) Host Global Interrupt Clear Register */
#define REG_UOTGHS_HSTIFR         REG_ACCESS(WoReg, 0x400AC40CU) /**< \brief (UOTGHS) Host Global Interrupt Set Register */
#define REG_UOTGHS_HSTIMR         REG_ACCESS(RoReg, 0x400AC410U) /**< \brief (UOTGHS) Host Global Interrupt Mask Register */
#define REG_UOTGHS_HSTIDR         REG_ACCESS(WoReg, 0x400AC414U) /**< \brief (UOTGHS) Host Global Interrupt Disable Register */
#define REG_UOTGHS_HSTIER         REG_ACCESS(WoReg, 0x400AC418U) /**< \brief (UOTGHS) Host Global Interrupt Enable Register */
#define REG_UOTGHS_HSTPIP         REG_ACCESS(RwReg, 0x400AC41CU) /**< \brief (UOTGHS) Host Pipe Register */
#define REG_UOTGHS_HSTFNUM        REG_ACCESS(RwReg, 0x400AC420U) /**< \brief (UOTGHS) Host Frame Number Register */
#define REG_UOTGHS_HSTADDR1       REG_ACCESS(RwReg, 0x400AC424U) /**< \brief (UOTGHS) Host Address 1 Register */
#define REG_UOTGHS_HSTADDR2       REG_ACCESS(RwReg, 0x400AC428U) /**< \brief (UOTGHS) Host Address 2 Register */
#define REG_UOTGHS_HSTADDR3       REG_ACCESS(RwReg, 0x400AC42CU) /**< \brief (UOTGHS) Host Address 3 Register */
#define REG_UOTGHS_HSTPIPCFG      REG_ACCESS(RwReg, 0x400AC500U) /**< \brief (UOTGHS) Host Pipe Configuration Register (n = 0) */
#define REG_UOTGHS_HSTPIPISR      REG_ACCESS(RoReg, 0x400AC530U) /**< \brief (UOTGHS) Host Pipe Status Register (n = 0) */
#define REG_UOTGHS_HSTPIPICR      REG_ACCESS(WoReg, 0x400AC560U) /**< \brief (UOTGHS) Host Pipe Clear Register (n = 0) */
#define REG_UOTGHS_HSTPIPIFR      REG_ACCESS(WoReg, 0x400AC590U) /**< \brief (UOTGHS) Host Pipe Set Register (n = 0) */
#define REG_UOTGHS_HSTPIPIMR      REG_ACCESS(RoReg, 0x400AC5C0U) /**< \brief (UOTGHS) Host Pipe Mask Register (n = 0) */
#define REG_UOTGHS_HSTPIPIER      REG_ACCESS(WoReg, 0x400AC5F0U) /**< \brief (UOTGHS) Host Pipe Enable Register (n = 0) */
#define REG_UOTGHS_HSTPIPIDR      REG_ACCESS(WoReg, 0x400AC620U) /**< \brief (UOTGHS) Host Pipe Disable Register (n = 0) */
#define REG_UOTGHS_HSTPIPINRQ     REG_ACCESS(RwReg, 0x400AC650U) /**< \brief (UOTGHS) Host Pipe IN Request Register (n = 0) */
#define REG_UOTGHS_HSTPIPERR      REG_ACCESS(RwReg, 0x400AC680U) /**< \brief (UOTGHS) Host Pipe Error Register (n = 0) */
#define REG_UOTGHS_HSTDMANXTDSC1  REG_ACCESS(RwReg, 0x400AC710U) /**< \brief (UOTGHS) Host DMA Channel Next Descriptor Address Register (n = 1) */
#define REG_UOTGHS_HSTDMAADDRESS1 REG_ACCESS(RwReg, 0x400AC714U) /**< \brief (UOTGHS) Host DMA Channel Address Register (n = 1) */
#define REG_UOTGHS_HSTDMACONTROL1 REG_ACCESS(RwReg, 0x400AC718U) /**< \brief (UOTGHS) Host DMA Channel Control Register (n = 1) */
#define REG_UOTGHS_HSTDMASTATUS1  REG_ACCESS(RwReg, 0x400AC71CU) /**< \brief (UOTGHS) Host DMA Channel Status Register (n = 1) */
#define REG_UOTGHS_HSTDMANXTDSC2  REG_ACCESS(RwReg, 0x400AC720U) /**< \brief (UOTGHS) Host DMA Channel Next Descriptor Address Register (n = 2) */
#define REG_UOTGHS_HSTDMAADDRESS2 REG_ACCESS(RwReg, 0x400AC724U) /**< \brief (UOTGHS) Host DMA Channel Address Register (n = 2) */
#define REG_UOTGHS_HSTDMACONTROL2 REG_ACCESS(RwReg, 0x400AC728U) /**< \brief (UOTGHS) Host DMA Channel Control Register (n = 2) */
#define REG_UOTGHS_HSTDMASTATUS2  REG_ACCESS(RwReg, 0x400AC72CU) /**< \brief (UOTGHS) Host DMA Channel Status Register (n = 2) */
#define REG_UOTGHS_HSTDMANXTDSC3  REG_ACCESS(RwReg, 0x400AC730U) /**< \brief (UOTGHS) Host DMA Channel Next Descriptor Address Register (n = 3) */
#define REG_UOTGHS_HSTDMAADDRESS3 REG_ACCESS(RwReg, 0x400AC734U) /**< \brief (UOTGHS) Host DMA Channel Address Register (n = 3) */
#define REG_UOTGHS_HSTDMACONTROL3 REG_ACCESS(RwReg, 0x400AC738U) /**< \brief (UOTGHS) Host DMA Channel Control Register (n = 3) */
#define REG_UOTGHS_HSTDMASTATUS3  REG_ACCESS(RwReg, 0x400AC73CU) /**< \brief (UOTGHS) Host DMA Channel Status Register (n = 3) */
#define REG_UOTGHS_HSTDMANXTDSC4  REG_ACCESS(RwReg, 0x400AC740U) /**< \brief (UOTGHS) Host DMA Channel Next Descriptor Address Register (n = 4) */
#define REG_UOTGHS_HSTDMAADDRESS4 REG_ACCESS(RwReg, 0x400AC744U) /**< \brief (UOTGHS) Host DMA Channel Address Register (n = 4) */
#define REG_UOTGHS_HSTDMACONTROL4 REG_ACCESS(RwReg, 0x400AC748U) /**< \brief (UOTGHS) Host DMA Channel Control Register (n = 4) */
#define REG_UOTGHS_HSTDMASTATUS4  REG_ACCESS(RwReg, 0x400AC74CU) /**< \brief (UOTGHS) Host DMA Channel Status Register (n = 4) */
#define REG_UOTGHS_HSTDMANXTDSC5  REG_ACCESS(RwReg, 0x400AC750U) /**< \brief (UOTGHS) Host DMA Channel Next Descriptor Address Register (n = 5) */
#define REG_UOTGHS_HSTDMAADDRESS5 REG_ACCESS(RwReg, 0x400AC754U) /**< \brief (UOTGHS) Host DMA Channel Address Register (n = 5) */
#define REG_UOTGHS_HSTDMACONTROL5 REG_ACCESS(RwReg, 0x400AC758U) /**< \brief (UOTGHS) Host DMA Channel Control Register (n = 5) */
#define REG_UOTGHS_HSTDMASTATUS5  REG_ACCESS(RwReg, 0x400AC75CU) /**< \brief (UOTGHS) Host DMA Channel Status Register (n = 5) */
#define REG_UOTGHS_HSTDMANXTDSC6  REG_ACCESS(RwReg, 0x400AC760U) /**< \brief (UOTGHS) Host DMA Channel Next Descriptor Address Register (n = 6) */
#define REG_UOTGHS_HSTDMAADDRESS6 REG_ACCESS(RwReg, 0x400AC764U) /**< \brief (UOTGHS) Host DMA Channel Address Register (n = 6) */
#define REG_UOTGHS_HSTDMACONTROL6 REG_ACCESS(RwReg, 0x400AC768U) /**< \brief (UOTGHS) Host DMA Channel Control Register (n = 6) */
#define REG_UOTGHS_HSTDMASTATUS6  REG_ACCESS(RwReg, 0x400AC76CU) /**< \brief (UOTGHS) Host DMA Channel Status Register (n = 6) */
#define REG_UOTGHS_HSTDMANXTDSC7  REG_ACCESS(RwReg, 0x400AC770U) /**< \brief (UOTGHS) Host DMA Channel Next Descriptor Address Register (n = 7) */
#define REG_UOTGHS_HSTDMAADDRESS7 REG_ACCESS(RwReg, 0x400AC774U) /**< \brief (UOTGHS) Host DMA Channel Address Register (n = 7) */
#define REG_UOTGHS_HSTDMACONTROL7 REG_ACCESS(RwReg, 0x400AC778U) /**< \brief (UOTGHS) Host DMA Channel Control Register (n = 7) */
#define REG_UOTGHS_HSTDMASTATUS7  REG_ACCESS(RwReg, 0x400AC77CU) /**< \brief (UOTGHS) Host DMA Channel Status Register (n = 7) */
#define REG_UOTGHS_CTRL           REG_ACCESS(RwReg, 0x400AC800U) /**< \brief (UOTGHS) General Control Register */
#define REG_UOTGHS_SR             REG_ACCESS(RoReg, 0x400AC804U) /**< \brief (UOTGHS) General Status Register */
#define REG_UOTGHS_SCR            REG_ACCESS(WoReg, 0x400AC808U) /**< \brief (UOTGHS) General Status Clear Register */
#define REG_UOTGHS_SFR            REG_ACCESS(WoReg, 0x400AC80CU) /**< \brief (UOTGHS) General Status Set Register */
#define REG_UOTGHS_FSM            REG_ACCESS(RoReg, 0x400AC82CU) /**< \brief (UOTGHS) General Finite State Machine Register */

#endif /* _SAM3XA_UOTGHS_INSTANCE_ */
