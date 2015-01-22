/**
 * \file
 *
 * \brief TRNG driver for SAM.
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

/**
 * \defgroup group_sam_drivers_trng TRNG - True Random Number Generator
 *
 * Driver for the TRNG (True Random Number Generator). This driver provides access
 * to the main features of the TRNG controller.
 *
 * \{
 */

#include "../chip.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

#if SAM3XA_SERIES

/* TRNG Security Key Value */
#define TRNG_KEY  0x524E47

/**
 * \brief Enable TRNG.
 *
 * \param p_trng  Pointer to a TRNG instance.
 *
 */
void trng_enable(Trng *p_trng)
{
	p_trng->TRNG_CR = TRNG_CR_ENABLE | TRNG_CR_KEY(TRNG_KEY);
}

/**
 * \brief Disable TRNG.
 *
 * \param p_trng  Pointer to a TRNG instance.
 *
 */
void trng_disable(Trng *p_trng)
{
	p_trng->TRNG_CR = TRNG_CR_KEY(TRNG_KEY);
}

/**
 * \brief Enable TRNG interrupt.
 *
 * \param p_trng  Pointer to a TRNG instance.
 *
 */
void trng_enable_interrupt(Trng *p_trng)
{
	p_trng->TRNG_IER = TRNG_IER_DATRDY;
}

/**
 * \brief Disable TRNG interrupt.
 *
 * \param p_trng  Pointer to a TRNG instance.
 *
 */
void trng_disable_interrupt(Trng *p_trng)
{
	p_trng->TRNG_IDR = TRNG_IER_DATRDY;
}

/**
 * \brief Get TRNG interrupt mask.
 *
 * \param p_trng  Pointer to a TRNG instance.
 *
 * \retval The interrupt mask value.
 */
uint32_t trng_get_interrupt_mask(Trng *p_trng)
{
	return p_trng->TRNG_IMR;
}

/**
 * \brief Get TRNG interrupt status.
 *
 * \param p_trng  Pointer to a TRNG instance.
 *
 * \retval The interrupt status value.
 */
uint32_t trng_get_interrupt_status(Trng *p_trng)
{
	return p_trng->TRNG_ISR;
}

/**
 * \brief Read TRNG output data.
 *
 * \param p_trng  Pointer to a TRNG instance.
 *
 * \retval The output data value.
 */
uint32_t trng_read_output_data(Trng *p_trng)
{
	return p_trng->TRNG_ODATA;
}

#endif // SAM3XA_SERIES

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \}
 */
