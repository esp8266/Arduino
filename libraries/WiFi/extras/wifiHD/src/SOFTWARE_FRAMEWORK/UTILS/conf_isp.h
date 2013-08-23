/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief ISP configuration file.
 *
 * This file contains the possible external configuration of the ISP.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USB module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ***************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#ifndef _CONF_ISP_H_
#define _CONF_ISP_H_

#include <avr32/io.h>
#include "compiler.h"


//_____ D E F I N I T I O N S ______________________________________________

#define PRODUCT_MANUFACTURER_ID       0x58
#define PRODUCT_FAMILY_ID             0x20

#define ISP_VERSION                   0x10
#define ISP_ID0                       0x00
#define ISP_ID1                       0x00

#define ISP_CFG1                      (*(volatile U32 *)ISP_CFG1_ADDRESS)
#define ISP_CFG1_ADDRESS              (AVR32_FLASHC_USER_PAGE_ADDRESS + ISP_CFG1_OFFSET)
#define ISP_CFG1_OFFSET               0x000001FC
#define ISP_CFG1_SIZE                 4

#define ISP_CFG1_BOOT_KEY1            16
#define ISP_CFG1_BOOT_KEY1_MASK       0xFFFF0000
#define ISP_CFG1_BOOT_KEY1_OFFSET     16
#define ISP_CFG1_BOOT_KEY1_SIZE       16
#define ISP_CFG1_BOOT_KEY1_VALUE      0xE11E

#define ISP_CFG1_FORCE                9
#define ISP_CFG1_FORCE_MASK           0x00000200
#define ISP_CFG1_FORCE_OFFSET         9
#define ISP_CFG1_FORCE_SIZE           1

#define ISP_CFG1_IO_COND_EN           8
#define ISP_CFG1_IO_COND_EN_MASK      0x00000100
#define ISP_CFG1_IO_COND_EN_OFFSET    8
#define ISP_CFG1_IO_COND_EN_SIZE      1

#define ISP_CFG1_CRC8                 0
#define ISP_CFG1_CRC8_MASK            0x000000FF
#define ISP_CFG1_CRC8_OFFSET          0
#define ISP_CFG1_CRC8_SIZE            8
#define ISP_CFG1_CRC8_POLYNOMIAL      0x107

#define ISP_CFG2                      (*(volatile U32 *)ISP_CFG2_ADDRESS)
#define ISP_CFG2_ADDRESS              (AVR32_FLASHC_USER_PAGE_ADDRESS + ISP_CFG2_OFFSET)
#define ISP_CFG2_OFFSET               0x000001F8
#define ISP_CFG2_SIZE                 4

#define ISP_CFG2_BOOT_KEY             17
#define ISP_CFG2_BOOT_KEY_MASK        0xFFFE0000
#define ISP_CFG2_BOOT_KEY_OFFSET      17
#define ISP_CFG2_BOOT_KEY_SIZE        15
#define ISP_CFG2_BOOT_KEY_VALUE       0x494F

#define ISP_CFG2_IO_COND_LEVEL        16
#define ISP_CFG2_IO_COND_LEVEL_MASK   0x00010000
#define ISP_CFG2_IO_COND_LEVEL_OFFSET 16
#define ISP_CFG2_IO_COND_LEVEL_SIZE   1

#define ISP_CFG2_IO_COND_PIN          8
#define ISP_CFG2_IO_COND_PIN_MASK     0x0000FF00
#define ISP_CFG2_IO_COND_PIN_OFFSET   8
#define ISP_CFG2_IO_COND_PIN_SIZE     8

#define ISP_CFG2_CRC8                 0
#define ISP_CFG2_CRC8_MASK            0x000000FF
#define ISP_CFG2_CRC8_OFFSET          0
#define ISP_CFG2_CRC8_SIZE            8
#define ISP_CFG2_CRC8_POLYNOMIAL      0x107

#define ISP_KEY                       (*(volatile U32 *)ISP_KEY_ADDRESS)
#define ISP_KEY_ADDRESS               (AVR32_SRAM_ADDRESS + ISP_KEY_OFFSET)
#define ISP_KEY_OFFSET                0x00000000
#define ISP_KEY_SIZE                  4
#define ISP_KEY_VALUE                 ('I' << 24 | 'S' << 16 | 'P' << 8 | 'K')

#ifndef ISP_OSC
  #define ISP_OSC                     0
#endif

#define DFU_FRAME_LENGTH              2048

#define PROGRAM_START_ADDRESS         (AVR32_FLASH_ADDRESS + PROGRAM_START_OFFSET)
#define PROGRAM_START_OFFSET          0x00002000


#endif  // _CONF_ISP_H_
