/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef _SAM3U_UDPHS_INSTANCE_
#define _SAM3U_UDPHS_INSTANCE_

/* ========== Register definition for UDPHS peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_UDPHS_CTRL                 (0x400A4000U) /**< \brief (UDPHS) UDPHS Control Register */
#define REG_UDPHS_FNUM                 (0x400A4004U) /**< \brief (UDPHS) UDPHS Frame Number Register */
#define REG_UDPHS_IEN                  (0x400A4010U) /**< \brief (UDPHS) UDPHS Interrupt Enable Register */
#define REG_UDPHS_INTSTA               (0x400A4014U) /**< \brief (UDPHS) UDPHS Interrupt Status Register */
#define REG_UDPHS_CLRINT               (0x400A4018U) /**< \brief (UDPHS) UDPHS Clear Interrupt Register */
#define REG_UDPHS_EPTRST               (0x400A401CU) /**< \brief (UDPHS) UDPHS Endpoints Reset Register */
#define REG_UDPHS_TST                  (0x400A40E0U) /**< \brief (UDPHS) UDPHS Test Register */
#define REG_UDPHS_IPNAME1              (0x400A40F0U) /**< \brief (UDPHS) UDPHS Name1 Register */
#define REG_UDPHS_IPNAME2              (0x400A40F4U) /**< \brief (UDPHS) UDPHS Name2 Register */
#define REG_UDPHS_IPFEATURES           (0x400A40F8U) /**< \brief (UDPHS) UDPHS Features Register */
#define REG_UDPHS_EPTCFG0              (0x400A4100U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 0) */
#define REG_UDPHS_EPTCTLENB0           (0x400A4104U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 0) */
#define REG_UDPHS_EPTCTLDIS0           (0x400A4108U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 0) */
#define REG_UDPHS_EPTCTL0              (0x400A410CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 0) */
#define REG_UDPHS_EPTSETSTA0           (0x400A4114U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 0) */
#define REG_UDPHS_EPTCLRSTA0           (0x400A4118U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 0) */
#define REG_UDPHS_EPTSTA0              (0x400A411CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 0) */
#define REG_UDPHS_EPTCFG1              (0x400A4120U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 1) */
#define REG_UDPHS_EPTCTLENB1           (0x400A4124U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 1) */
#define REG_UDPHS_EPTCTLDIS1           (0x400A4128U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 1) */
#define REG_UDPHS_EPTCTL1              (0x400A412CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 1) */
#define REG_UDPHS_EPTSETSTA1           (0x400A4134U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 1) */
#define REG_UDPHS_EPTCLRSTA1           (0x400A4138U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 1) */
#define REG_UDPHS_EPTSTA1              (0x400A413CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 1) */
#define REG_UDPHS_EPTCFG2              (0x400A4140U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 2) */
#define REG_UDPHS_EPTCTLENB2           (0x400A4144U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 2) */
#define REG_UDPHS_EPTCTLDIS2           (0x400A4148U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 2) */
#define REG_UDPHS_EPTCTL2              (0x400A414CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 2) */
#define REG_UDPHS_EPTSETSTA2           (0x400A4154U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 2) */
#define REG_UDPHS_EPTCLRSTA2           (0x400A4158U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 2) */
#define REG_UDPHS_EPTSTA2              (0x400A415CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 2) */
#define REG_UDPHS_EPTCFG3              (0x400A4160U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 3) */
#define REG_UDPHS_EPTCTLENB3           (0x400A4164U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 3) */
#define REG_UDPHS_EPTCTLDIS3           (0x400A4168U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 3) */
#define REG_UDPHS_EPTCTL3              (0x400A416CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 3) */
#define REG_UDPHS_EPTSETSTA3           (0x400A4174U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 3) */
#define REG_UDPHS_EPTCLRSTA3           (0x400A4178U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 3) */
#define REG_UDPHS_EPTSTA3              (0x400A417CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 3) */
#define REG_UDPHS_EPTCFG4              (0x400A4180U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 4) */
#define REG_UDPHS_EPTCTLENB4           (0x400A4184U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 4) */
#define REG_UDPHS_EPTCTLDIS4           (0x400A4188U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 4) */
#define REG_UDPHS_EPTCTL4              (0x400A418CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 4) */
#define REG_UDPHS_EPTSETSTA4           (0x400A4194U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 4) */
#define REG_UDPHS_EPTCLRSTA4           (0x400A4198U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 4) */
#define REG_UDPHS_EPTSTA4              (0x400A419CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 4) */
#define REG_UDPHS_EPTCFG5              (0x400A41A0U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 5) */
#define REG_UDPHS_EPTCTLENB5           (0x400A41A4U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 5) */
#define REG_UDPHS_EPTCTLDIS5           (0x400A41A8U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 5) */
#define REG_UDPHS_EPTCTL5              (0x400A41ACU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 5) */
#define REG_UDPHS_EPTSETSTA5           (0x400A41B4U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 5) */
#define REG_UDPHS_EPTCLRSTA5           (0x400A41B8U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 5) */
#define REG_UDPHS_EPTSTA5              (0x400A41BCU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 5) */
#define REG_UDPHS_EPTCFG6              (0x400A41C0U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 6) */
#define REG_UDPHS_EPTCTLENB6           (0x400A41C4U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 6) */
#define REG_UDPHS_EPTCTLDIS6           (0x400A41C8U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 6) */
#define REG_UDPHS_EPTCTL6              (0x400A41CCU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 6) */
#define REG_UDPHS_EPTSETSTA6           (0x400A41D4U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 6) */
#define REG_UDPHS_EPTCLRSTA6           (0x400A41D8U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 6) */
#define REG_UDPHS_EPTSTA6              (0x400A41DCU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 6) */
#define REG_UDPHS_DMANXTDSC0           (0x400A4300U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 0) */
#define REG_UDPHS_DMAADDRESS0          (0x400A4304U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 0) */
#define REG_UDPHS_DMACONTROL0          (0x400A4308U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 0) */
#define REG_UDPHS_DMASTATUS0           (0x400A430CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 0) */
#define REG_UDPHS_DMANXTDSC1           (0x400A4310U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 1) */
#define REG_UDPHS_DMAADDRESS1          (0x400A4314U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 1) */
#define REG_UDPHS_DMACONTROL1          (0x400A4318U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 1) */
#define REG_UDPHS_DMASTATUS1           (0x400A431CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 1) */
#define REG_UDPHS_DMANXTDSC2           (0x400A4320U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 2) */
#define REG_UDPHS_DMAADDRESS2          (0x400A4324U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 2) */
#define REG_UDPHS_DMACONTROL2          (0x400A4328U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 2) */
#define REG_UDPHS_DMASTATUS2           (0x400A432CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 2) */
#define REG_UDPHS_DMANXTDSC3           (0x400A4330U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 3) */
#define REG_UDPHS_DMAADDRESS3          (0x400A4334U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 3) */
#define REG_UDPHS_DMACONTROL3          (0x400A4338U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 3) */
#define REG_UDPHS_DMASTATUS3           (0x400A433CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 3) */
#define REG_UDPHS_DMANXTDSC4           (0x400A4340U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 4) */
#define REG_UDPHS_DMAADDRESS4          (0x400A4344U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 4) */
#define REG_UDPHS_DMACONTROL4          (0x400A4348U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 4) */
#define REG_UDPHS_DMASTATUS4           (0x400A434CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 4) */
#define REG_UDPHS_DMANXTDSC5           (0x400A4350U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 5) */
#define REG_UDPHS_DMAADDRESS5          (0x400A4354U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 5) */
#define REG_UDPHS_DMACONTROL5          (0x400A4358U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 5) */
#define REG_UDPHS_DMASTATUS5           (0x400A435CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 5) */
#else
#define REG_UDPHS_CTRL        (*(RwReg*)0x400A4000U) /**< \brief (UDPHS) UDPHS Control Register */
#define REG_UDPHS_FNUM        (*(RoReg*)0x400A4004U) /**< \brief (UDPHS) UDPHS Frame Number Register */
#define REG_UDPHS_IEN         (*(RwReg*)0x400A4010U) /**< \brief (UDPHS) UDPHS Interrupt Enable Register */
#define REG_UDPHS_INTSTA      (*(RoReg*)0x400A4014U) /**< \brief (UDPHS) UDPHS Interrupt Status Register */
#define REG_UDPHS_CLRINT      (*(WoReg*)0x400A4018U) /**< \brief (UDPHS) UDPHS Clear Interrupt Register */
#define REG_UDPHS_EPTRST      (*(WoReg*)0x400A401CU) /**< \brief (UDPHS) UDPHS Endpoints Reset Register */
#define REG_UDPHS_TST         (*(RwReg*)0x400A40E0U) /**< \brief (UDPHS) UDPHS Test Register */
#define REG_UDPHS_IPNAME1     (*(RoReg*)0x400A40F0U) /**< \brief (UDPHS) UDPHS Name1 Register */
#define REG_UDPHS_IPNAME2     (*(RoReg*)0x400A40F4U) /**< \brief (UDPHS) UDPHS Name2 Register */
#define REG_UDPHS_IPFEATURES  (*(RoReg*)0x400A40F8U) /**< \brief (UDPHS) UDPHS Features Register */
#define REG_UDPHS_EPTCFG0     (*(RwReg*)0x400A4100U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 0) */
#define REG_UDPHS_EPTCTLENB0  (*(WoReg*)0x400A4104U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 0) */
#define REG_UDPHS_EPTCTLDIS0  (*(WoReg*)0x400A4108U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 0) */
#define REG_UDPHS_EPTCTL0     (*(RoReg*)0x400A410CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 0) */
#define REG_UDPHS_EPTSETSTA0  (*(WoReg*)0x400A4114U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 0) */
#define REG_UDPHS_EPTCLRSTA0  (*(WoReg*)0x400A4118U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 0) */
#define REG_UDPHS_EPTSTA0     (*(RoReg*)0x400A411CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 0) */
#define REG_UDPHS_EPTCFG1     (*(RwReg*)0x400A4120U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 1) */
#define REG_UDPHS_EPTCTLENB1  (*(WoReg*)0x400A4124U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 1) */
#define REG_UDPHS_EPTCTLDIS1  (*(WoReg*)0x400A4128U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 1) */
#define REG_UDPHS_EPTCTL1     (*(RoReg*)0x400A412CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 1) */
#define REG_UDPHS_EPTSETSTA1  (*(WoReg*)0x400A4134U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 1) */
#define REG_UDPHS_EPTCLRSTA1  (*(WoReg*)0x400A4138U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 1) */
#define REG_UDPHS_EPTSTA1     (*(RoReg*)0x400A413CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 1) */
#define REG_UDPHS_EPTCFG2     (*(RwReg*)0x400A4140U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 2) */
#define REG_UDPHS_EPTCTLENB2  (*(WoReg*)0x400A4144U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 2) */
#define REG_UDPHS_EPTCTLDIS2  (*(WoReg*)0x400A4148U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 2) */
#define REG_UDPHS_EPTCTL2     (*(RoReg*)0x400A414CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 2) */
#define REG_UDPHS_EPTSETSTA2  (*(WoReg*)0x400A4154U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 2) */
#define REG_UDPHS_EPTCLRSTA2  (*(WoReg*)0x400A4158U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 2) */
#define REG_UDPHS_EPTSTA2     (*(RoReg*)0x400A415CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 2) */
#define REG_UDPHS_EPTCFG3     (*(RwReg*)0x400A4160U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 3) */
#define REG_UDPHS_EPTCTLENB3  (*(WoReg*)0x400A4164U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 3) */
#define REG_UDPHS_EPTCTLDIS3  (*(WoReg*)0x400A4168U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 3) */
#define REG_UDPHS_EPTCTL3     (*(RoReg*)0x400A416CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 3) */
#define REG_UDPHS_EPTSETSTA3  (*(WoReg*)0x400A4174U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 3) */
#define REG_UDPHS_EPTCLRSTA3  (*(WoReg*)0x400A4178U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 3) */
#define REG_UDPHS_EPTSTA3     (*(RoReg*)0x400A417CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 3) */
#define REG_UDPHS_EPTCFG4     (*(RwReg*)0x400A4180U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 4) */
#define REG_UDPHS_EPTCTLENB4  (*(WoReg*)0x400A4184U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 4) */
#define REG_UDPHS_EPTCTLDIS4  (*(WoReg*)0x400A4188U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 4) */
#define REG_UDPHS_EPTCTL4     (*(RoReg*)0x400A418CU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 4) */
#define REG_UDPHS_EPTSETSTA4  (*(WoReg*)0x400A4194U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 4) */
#define REG_UDPHS_EPTCLRSTA4  (*(WoReg*)0x400A4198U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 4) */
#define REG_UDPHS_EPTSTA4     (*(RoReg*)0x400A419CU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 4) */
#define REG_UDPHS_EPTCFG5     (*(RwReg*)0x400A41A0U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 5) */
#define REG_UDPHS_EPTCTLENB5  (*(WoReg*)0x400A41A4U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 5) */
#define REG_UDPHS_EPTCTLDIS5  (*(WoReg*)0x400A41A8U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 5) */
#define REG_UDPHS_EPTCTL5     (*(RoReg*)0x400A41ACU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 5) */
#define REG_UDPHS_EPTSETSTA5  (*(WoReg*)0x400A41B4U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 5) */
#define REG_UDPHS_EPTCLRSTA5  (*(WoReg*)0x400A41B8U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 5) */
#define REG_UDPHS_EPTSTA5     (*(RoReg*)0x400A41BCU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 5) */
#define REG_UDPHS_EPTCFG6     (*(RwReg*)0x400A41C0U) /**< \brief (UDPHS) UDPHS Endpoint Configuration Register (endpoint = 6) */
#define REG_UDPHS_EPTCTLENB6  (*(WoReg*)0x400A41C4U) /**< \brief (UDPHS) UDPHS Endpoint Control Enable Register (endpoint = 6) */
#define REG_UDPHS_EPTCTLDIS6  (*(WoReg*)0x400A41C8U) /**< \brief (UDPHS) UDPHS Endpoint Control Disable Register (endpoint = 6) */
#define REG_UDPHS_EPTCTL6     (*(RoReg*)0x400A41CCU) /**< \brief (UDPHS) UDPHS Endpoint Control Register (endpoint = 6) */
#define REG_UDPHS_EPTSETSTA6  (*(WoReg*)0x400A41D4U) /**< \brief (UDPHS) UDPHS Endpoint Set Status Register (endpoint = 6) */
#define REG_UDPHS_EPTCLRSTA6  (*(WoReg*)0x400A41D8U) /**< \brief (UDPHS) UDPHS Endpoint Clear Status Register (endpoint = 6) */
#define REG_UDPHS_EPTSTA6     (*(RoReg*)0x400A41DCU) /**< \brief (UDPHS) UDPHS Endpoint Status Register (endpoint = 6) */
#define REG_UDPHS_DMANXTDSC0  (*(RwReg*)0x400A4300U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 0) */
#define REG_UDPHS_DMAADDRESS0 (*(RwReg*)0x400A4304U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 0) */
#define REG_UDPHS_DMACONTROL0 (*(RwReg*)0x400A4308U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 0) */
#define REG_UDPHS_DMASTATUS0  (*(RwReg*)0x400A430CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 0) */
#define REG_UDPHS_DMANXTDSC1  (*(RwReg*)0x400A4310U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 1) */
#define REG_UDPHS_DMAADDRESS1 (*(RwReg*)0x400A4314U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 1) */
#define REG_UDPHS_DMACONTROL1 (*(RwReg*)0x400A4318U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 1) */
#define REG_UDPHS_DMASTATUS1  (*(RwReg*)0x400A431CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 1) */
#define REG_UDPHS_DMANXTDSC2  (*(RwReg*)0x400A4320U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 2) */
#define REG_UDPHS_DMAADDRESS2 (*(RwReg*)0x400A4324U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 2) */
#define REG_UDPHS_DMACONTROL2 (*(RwReg*)0x400A4328U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 2) */
#define REG_UDPHS_DMASTATUS2  (*(RwReg*)0x400A432CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 2) */
#define REG_UDPHS_DMANXTDSC3  (*(RwReg*)0x400A4330U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 3) */
#define REG_UDPHS_DMAADDRESS3 (*(RwReg*)0x400A4334U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 3) */
#define REG_UDPHS_DMACONTROL3 (*(RwReg*)0x400A4338U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 3) */
#define REG_UDPHS_DMASTATUS3  (*(RwReg*)0x400A433CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 3) */
#define REG_UDPHS_DMANXTDSC4  (*(RwReg*)0x400A4340U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 4) */
#define REG_UDPHS_DMAADDRESS4 (*(RwReg*)0x400A4344U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 4) */
#define REG_UDPHS_DMACONTROL4 (*(RwReg*)0x400A4348U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 4) */
#define REG_UDPHS_DMASTATUS4  (*(RwReg*)0x400A434CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 4) */
#define REG_UDPHS_DMANXTDSC5  (*(RwReg*)0x400A4350U) /**< \brief (UDPHS) UDPHS DMA Next Descriptor Address Register (channel = 5) */
#define REG_UDPHS_DMAADDRESS5 (*(RwReg*)0x400A4354U) /**< \brief (UDPHS) UDPHS DMA Channel Address Register (channel = 5) */
#define REG_UDPHS_DMACONTROL5 (*(RwReg*)0x400A4358U) /**< \brief (UDPHS) UDPHS DMA Channel Control Register (channel = 5) */
#define REG_UDPHS_DMASTATUS5  (*(RwReg*)0x400A435CU) /**< \brief (UDPHS) UDPHS DMA Channel Status Register (channel = 5) */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3U_UDPHS_INSTANCE_ */
