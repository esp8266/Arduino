/**
 * \file
 *
 * \brief Embedded Flash Controller (EFC) driver for SAM.
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef EFC_H_INCLUDED
#define EFC_H_INCLUDED

#include "../chip.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/*! \name EFC return codes */
//! @{
typedef enum efc_rc {
	EFC_RC_OK = 0,	    //!< Operation OK
	EFC_RC_YES = 0,	    //!< Yes
	EFC_RC_NO = 1,	    //!< No
	EFC_RC_ERROR = 1,	//!< General error
	EFC_RC_INVALID,	    //!< Invalid argument input
	EFC_RC_NOT_SUPPORT = 0xFFFFFFFF	//!< Operation is not supported
} efc_rc_t;
//! @}

/*! \name EFC command */
//! @{
#define EFC_FCMD_GETD    0x00	//!< Get Flash Descriptor
#define EFC_FCMD_WP      0x01	//!< Write page
#define EFC_FCMD_WPL     0x02	//!< Write page and lock
#define EFC_FCMD_EWP     0x03	//!< Erase page and write page
#define EFC_FCMD_EWPL    0x04	//!< Erase page and write page then lock
#define EFC_FCMD_EA      0x05	//!< Erase all
#if (SAM3SD8_SERIES)
#define EFC_FCMD_EPL     0x06	//!< Erase plane
#endif
#if (SAM4S_SERIES)
#define EFC_FCMD_EPA     0x07	//!< Erase pages
#endif
#define EFC_FCMD_SLB     0x08	//!< Set Lock Bit
#define EFC_FCMD_CLB     0x09	//!< Clear Lock Bit
#define EFC_FCMD_GLB     0x0A	//!< Get Lock Bit
#define EFC_FCMD_SGPB    0x0B	//!< Set GPNVM Bit
#define EFC_FCMD_CGPB    0x0C	//!< Clear GPNVM Bit
#define EFC_FCMD_GGPB    0x0D	//!< Get GPNVM Bit
#define EFC_FCMD_STUI    0x0E	//!< Start unique ID
#define EFC_FCMD_SPUI    0x0F	//!< Stop unique ID
#if (SAM3S_SERIES || SAM3N_SERIES || SAM3XA_SERIES || SAM4S_SERIES)
#define EFC_FCMD_GCALB   0x10	//!< Get CALIB Bit
#endif
#if (SAM4S_SERIES)
#define EFC_FCMD_ES      0x11 //!< Erase sector
#define EFC_FCMD_WUS		 0x12 //!< Write user signature
#define EFC_FCMD_EUS		 0x13	//!< Erase user signature
#define EFC_FCMD_STUS		 0x14	//!< Start read user signature
#define EFC_FCMD_SPUS		 0x15	//!< Stop read user signature
#endif
//! @}

/*! The IAP function entry address */
#define CHIP_FLASH_IAP_ADDRESS  (IROM_ADDR + 8)

/*! \name EFC access mode */
//! @{
#define EFC_ACCESS_MODE_128    0
#define EFC_ACCESS_MODE_64     EEFC_FMR_FAM
//! @}

uint32_t efc_init(Efc *p_efc, uint32_t ul_access_mode, uint32_t ul_fws);
void efc_enable_frdy_interrupt(Efc *p_efc);
void efc_disable_frdy_interrupt(Efc *p_efc);
void efc_set_flash_access_mode(Efc *p_efc, uint32_t ul_mode);
uint32_t efc_get_flash_access_mode(Efc *p_efc);
void efc_set_wait_state(Efc *p_efc, uint32_t ul_fws);
uint32_t efc_get_wait_state(Efc *p_efc);
uint32_t efc_perform_command(Efc *p_efc, uint32_t ul_command,	uint32_t ul_argument);
uint32_t efc_get_status(Efc *p_efc);
uint32_t efc_get_result(Efc *p_efc);
uint32_t efc_perform_read_sequence(Efc *p_efc, uint32_t ul_cmd_st, uint32_t ul_cmd_sp, uint32_t *p_ul_buf, uint32_t ul_size);

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* EFC_H_INCLUDED */
