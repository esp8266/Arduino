/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief PDCA driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the PDCA interface on AVR32
 * devices.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a PDCA module.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

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

#ifndef _PDCA_H_
#define _PDCA_H_

#include <avr32/io.h>


//! Size of PDCA transfer: byte.
#define PDCA_TRANSFER_SIZE_BYTE               AVR32_PDCA_BYTE

//! Size of PDCA transfer: half-word.
#define PDCA_TRANSFER_SIZE_HALF_WORD          AVR32_PDCA_HALF_WORD

//! Size of PDCA transfer: word.
#define PDCA_TRANSFER_SIZE_WORD               AVR32_PDCA_WORD

/*! \name PDCA Driver Status Codes
 */
//! @{
#define PDCA_SUCCESS 0
#define PDCA_INVALID_ARGUMENT -1
//! @}

/*! \name PDCA Transfer Status Codes
 */
//! @{
#define PDCA_TRANSFER_ERROR                   AVR32_PDCA_TERR_MASK
#define PDCA_TRANSFER_COMPLETE                AVR32_PDCA_TRC_MASK
#define PDCA_TRANSFER_COUNTER_RELOAD_IS_ZERO  AVR32_PDCA_RCZ_MASK
//! @}


//! PDCA channel options.
typedef struct
{
  //! Memory address.
  volatile  void         *addr          ;
  //! Transfer counter.
            unsigned int  size          ;
  //! Next memory address.
  volatile  void         *r_addr        ;
  //! Next transfer counter.
            unsigned int  r_size        ;
  //! Select peripheral ID.
            unsigned int  pid           ;
  //! Select the size of the transfer (byte, half-word or word).
            unsigned int  transfer_size ;
#if (defined AVR32_PDCA_120_H_INCLUDED ) || (defined AVR32_PDCA_121_H_INCLUDED ) || (defined AVR32_PDCA_122_H_INCLUDED )
// Note: the options in this preprocessor section are only available from the PDCA IP version 1.2.0 on.
  //! Enable (\c 1) or disable (\c 0) the transfer upon event trigger.
            unsigned char etrig         ;
#endif // #ifdef AVR32_PDCA_120_H_INCLUDED
} pdca_channel_options_t;


/*! \brief Get PDCA channel handler
 *
 * \param pdca_ch_number  PDCA channel
 *
 * \return channel handled or PDCA_INVALID_ARGUMENT
 */
extern volatile avr32_pdca_channel_t *pdca_get_handler(unsigned int pdca_ch_number);

/*! \brief Set the channel configuration
 *
 * \param pdca_ch_number PDCA channel
 * \param opt channel option
 */
extern int pdca_init_channel(unsigned int pdca_ch_number, const pdca_channel_options_t *opt);

/*! \brief Get the PDCA channel transfer enable status
 *
 * \param pdca_ch_number PDCA channel
 *
 * \return \c 1 if channel transfer is enabled, else \c 0
 */
extern unsigned int pdca_get_channel_status(unsigned int pdca_ch_number);

/*! \brief Disable the PDCA for the given channel
 *
 * \param pdca_ch_number PDCA channel
 */
extern void pdca_disable(unsigned int pdca_ch_number);

/*! \brief Enable the PDCA for the given channel
 *
 * \param pdca_ch_number PDCA channel
 */
extern void pdca_enable(unsigned int pdca_ch_number);

/*! \brief Get PDCA channel load size (or remaining size if transfer started)
 *
 * \param pdca_ch_number PDCA channel
 *
 * \return size           current size to transfer
 */
extern unsigned int pdca_get_load_size(unsigned int pdca_ch_number);

/*! \brief Set PDCA channel load values
 *
 * \param pdca_ch_number PDCA channel
 * \param addr           address where data to load are stored
 * \param size           size of the data block to load
 */
extern void pdca_load_channel(unsigned int pdca_ch_number, volatile void *addr, unsigned int size);

/*! \brief Get PDCA channel reload size
 *
 * \param pdca_ch_number PDCA channel
 *
 * \return size           current reload size
 */
extern unsigned int pdca_get_reload_size(unsigned int pdca_ch_number);

/*! \brief Set PDCA channel reload values
 *
 * \param pdca_ch_number PDCA channel
 * \param addr           address where data to load are stored
 * \param size           size of the data block to load
 */
extern void pdca_reload_channel(unsigned int pdca_ch_number, volatile void *addr, unsigned int size);

/*! \brief Set the peripheral function to use with the PDCA channel
 *
 * \param pdca_ch_number PDCA channel
 * \param pid the peripheral ID
 */
extern void pdca_set_peripheral_select(unsigned int pdca_ch_number, unsigned int pid);

/*! \brief Set the size of the transfer
 *
 * \param pdca_ch_number PDCA channel
 * \param transfer_size size of the transfer (byte, half-word or word)
 */
extern void pdca_set_transfer_size(unsigned int pdca_ch_number, unsigned int transfer_size);

#if (defined AVR32_PDCA_120_H_INCLUDED ) || (defined AVR32_PDCA_121_H_INCLUDED ) || (defined AVR32_PDCA_122_H_INCLUDED )
// Note: the functions in this preprocessor section are only available from the PDCA IP version 1.2.0 on.

/*! \brief Disable the event-triggered transfer feature
 *
 * \param pdca_ch_number PDCA channel
 */
extern void pdca_disable_event_trigger(unsigned int pdca_ch_number);

/*! \brief Enable the event-triggered transfer feature
 *
 * \param pdca_ch_number PDCA channel
 */
extern void pdca_enable_event_trigger(unsigned int pdca_ch_number);

#endif // #ifdef AVR32_PDCA_120_H_INCLUDED

/*! \brief Disable PDCA transfer error interrupt
 *
 * \param pdca_ch_number PDCA channel
 */
extern void pdca_disable_interrupt_transfer_error(unsigned int pdca_ch_number);

/*! \brief Enable PDCA transfer error interrupt
 *
 * \param pdca_ch_number PDCA channel
 */
extern void pdca_enable_interrupt_transfer_error(unsigned int pdca_ch_number);

/*! \brief Disable PDCA transfer interrupt when completed (ie TCR and TCRR are both zero)
 *
 * \param pdca_ch_number PDCA channel
 */
extern void pdca_disable_interrupt_transfer_complete(unsigned int pdca_ch_number);

/*! \brief Enable PDCA transfer interrupt when completed (ie TCR and TCRR are both zero)
 *
 * \param pdca_ch_number PDCA channel
 */
extern void pdca_enable_interrupt_transfer_complete(unsigned int pdca_ch_number);

/*! \brief Disable PDCA transfer interrupt when TCRR reaches zero
 *
 * \param pdca_ch_number PDCA channel
 */
extern void pdca_disable_interrupt_reload_counter_zero(unsigned int pdca_ch_number);

/*! \brief Enable PDCA transfer interrupt when TCRR reaches zero
 *
 * \param pdca_ch_number PDCA channel
 */
extern void pdca_enable_interrupt_reload_counter_zero(unsigned int pdca_ch_number);

/*! \brief Get PDCA channel transfer status
 *
 * \param pdca_ch_number PDCA channel
 *
 * \return PDCA transfer status with the following bit-masks:\n
 *           - \c PDCA_TRANSFER_ERROR;\n
 *           - \c PDCA_TRANSFER_COMPLETE;\n
 *           - \c PDCA_TRANSFER_COUNTER_RELOAD_IS_ZERO.
 */
extern unsigned long pdca_get_transfer_status(unsigned int pdca_ch_number);


#endif  // _PDCA_H_
