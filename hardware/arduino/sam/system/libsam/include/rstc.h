/**
 * \file
 *
 * \brief Reset Controller (RSTC) driver for SAM.
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

#ifndef RSTC_H_INCLUDED
#define RSTC_H_INCLUDED

#include "../chip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Definitions of Reset Controller Status */
/** Reset cause */
#define RSTC_GENERAL_RESET   (0 << RSTC_SR_RSTTYP_Pos)
#define RSTC_BACKUP_RESET    (1 << RSTC_SR_RSTTYP_Pos)
#define RSTC_WATCHDOG_RESET  (2 << RSTC_SR_RSTTYP_Pos)
#define RSTC_SOFTWARE_RESET  (3 << RSTC_SR_RSTTYP_Pos)
#define RSTC_USER_RESET      (4 << RSTC_SR_RSTTYP_Pos)
/** NRST Pin Level */
#define RSTC_NRST_LOW   (LOW << 16)
#define RSTC_NRST_HIGH  (HIGH << 16)

void rstc_set_external_reset(Rstc* p_rstc, const uint32_t ul_length);
void rstc_enable_user_reset(Rstc* p_rstc);
void rstc_disable_user_reset(Rstc* p_rstc);
void rstc_enable_user_reset_interrupt(Rstc* p_rstc);
void rstc_disable_user_reset_interrupt(Rstc* p_rstc);
void rstc_start_software_reset(Rstc* p_rstc);
void rstc_reset_extern(Rstc *p_rstc);
uint32_t rstc_get_status(Rstc* p_rstc);
uint32_t rstc_get_reset_cause(Rstc* p_rstc);

#ifdef __cplusplus
}
#endif

#endif /* RSTC_H_INCLUDED */
