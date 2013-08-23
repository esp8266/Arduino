/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief USART driver for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 USART, with support for all
 * modes, settings and clock speeds.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USART module can be used.
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

#include "compiler.h"
#include "usart.h"


//------------------------------------------------------------------------------
/*! \name Private Functions
 */
//! @{


/*! \brief Checks if the USART is in multidrop mode.
 *
 * \param usart Base address of the USART instance.
 *
 * \return \c 1 if the USART is in multidrop mode, otherwise \c 0.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
static __inline__ int usart_mode_is_multidrop(volatile avr32_usart_t *usart)
{
  return ((usart->mr >> AVR32_USART_MR_PAR_OFFSET) & AVR32_USART_MR_PAR_MULTI) == AVR32_USART_MR_PAR_MULTI;
}


/*! \brief Calculates a clock divider (\e CD) and a fractional part (\e FP) for
 *         the USART asynchronous modes to generate a baud rate as close as
 *         possible to the baud rate set point.
 *
 * Baud rate calculation:
 * \f$ Baudrate = \frac{SelectedClock}{Over \times (CD + \frac{FP}{8})} \f$, \e Over being 16 or 8.
 * The maximal oversampling is selected if it allows to generate a baud rate close to the set point.
 *
 * \param usart     Base address of the USART instance.
 * \param baudrate  Baud rate set point.
 * \param pba_hz    USART module input clock frequency (PBA clock, Hz).
 *
 * \retval USART_SUCCESS        Baud rate successfully initialized.
 * \retval USART_INVALID_INPUT  Baud rate set point is out of range for the given input clock frequency.
 */
static int usart_set_async_baudrate(volatile avr32_usart_t *usart, unsigned int baudrate, unsigned long pba_hz)
{
  unsigned int over = (pba_hz >= 16 * baudrate) ? 16 : 8;
  unsigned int cd_fp = ((1 << AVR32_USART_BRGR_FP_SIZE) * pba_hz + (over * baudrate) / 2) / (over * baudrate);
  unsigned int cd = cd_fp >> AVR32_USART_BRGR_FP_SIZE;
  unsigned int fp = cd_fp & ((1 << AVR32_USART_BRGR_FP_SIZE) - 1);

  if (cd < 1 || cd > (1 << AVR32_USART_BRGR_CD_SIZE) - 1)
    return USART_INVALID_INPUT;

  usart->mr = (usart->mr & ~(AVR32_USART_MR_USCLKS_MASK |
                             AVR32_USART_MR_SYNC_MASK |
                             AVR32_USART_MR_OVER_MASK)) |
              AVR32_USART_MR_USCLKS_MCK << AVR32_USART_MR_USCLKS_OFFSET |
              ((over == 16) ? AVR32_USART_MR_OVER_X16 : AVR32_USART_MR_OVER_X8) << AVR32_USART_MR_OVER_OFFSET;

  usart->brgr = cd << AVR32_USART_BRGR_CD_OFFSET |
                fp << AVR32_USART_BRGR_FP_OFFSET;

  return USART_SUCCESS;
}


/*! \brief Calculates a clock divider (\e CD) for the USART synchronous master
 *         modes to generate a baud rate as close as possible to the baud rate
 *         set point.
 *
 * Baud rate calculation:
 * \f$ Baudrate = \frac{SelectedClock}{CD} \f$.
 *
 * \param usart     Base address of the USART instance.
 * \param baudrate  Baud rate set point.
 * \param pba_hz    USART module input clock frequency (PBA clock, Hz).
 *
 * \retval USART_SUCCESS        Baud rate successfully initialized.
 * \retval USART_INVALID_INPUT  Baud rate set point is out of range for the given input clock frequency.
 */
static int usart_set_sync_master_baudrate(volatile avr32_usart_t *usart, unsigned int baudrate, unsigned long pba_hz)
{
  unsigned int cd = (pba_hz + baudrate / 2) / baudrate;

  if (cd < 1 || cd > (1 << AVR32_USART_BRGR_CD_SIZE) - 1)
    return USART_INVALID_INPUT;

  usart->mr = (usart->mr & ~AVR32_USART_MR_USCLKS_MASK) |
              AVR32_USART_MR_USCLKS_MCK << AVR32_USART_MR_USCLKS_OFFSET |
              AVR32_USART_MR_SYNC_MASK;

  usart->brgr = cd << AVR32_USART_BRGR_CD_OFFSET;

  return USART_SUCCESS;
}


/*! \brief Selects the SCK pin as the source of baud rate for the USART
 *         synchronous slave modes.
 *
 * \param usart Base address of the USART instance.
 *
 * \retval USART_SUCCESS  Baud rate successfully initialized.
 */
static int usart_set_sync_slave_baudrate(volatile avr32_usart_t *usart)
{
  usart->mr = (usart->mr & ~AVR32_USART_MR_USCLKS_MASK) |
              AVR32_USART_MR_USCLKS_SCK << AVR32_USART_MR_USCLKS_OFFSET |
              AVR32_USART_MR_SYNC_MASK;

  return USART_SUCCESS;
}


/*! \brief Calculates a clock divider (\e CD) for the USART ISO7816 mode to
 *         generate an ISO7816 clock as close as possible to the clock set point.
 *
 * ISO7816 clock calculation:
 * \f$ Clock = \frac{SelectedClock}{CD} \f$.
 *
 * \param usart   Base address of the USART instance.
 * \param clock   ISO7816 clock set point.
 * \param pba_hz  USART module input clock frequency (PBA clock, Hz).
 *
 * \retval USART_SUCCESS        ISO7816 clock successfully initialized.
 * \retval USART_INVALID_INPUT  ISO7816 clock set point is out of range for the given input clock frequency.
 */
static int usart_set_iso7816_clock(volatile avr32_usart_t *usart, unsigned int clock, unsigned long pba_hz)
{
  unsigned int cd = (pba_hz + clock / 2) / clock;

  if (cd < 1 || cd > (1 << AVR32_USART_BRGR_CD_SIZE) - 1)
    return USART_INVALID_INPUT;

  usart->mr = (usart->mr & ~(AVR32_USART_MR_USCLKS_MASK |
                             AVR32_USART_MR_SYNC_MASK |
                             AVR32_USART_MR_OVER_MASK)) |
              AVR32_USART_MR_USCLKS_MCK << AVR32_USART_MR_USCLKS_OFFSET |
              AVR32_USART_MR_OVER_X16 << AVR32_USART_MR_OVER_OFFSET;

  usart->brgr = cd << AVR32_USART_BRGR_CD_OFFSET;

  return USART_SUCCESS;
}


#if defined(AVR32_USART_400_H_INCLUDED) || \
    defined(AVR32_USART_410_H_INCLUDED) || \
    defined(AVR32_USART_420_H_INCLUDED) || \
    defined(AVR32_USART_440_H_INCLUDED) || \
    defined(AVR32_USART_602_H_INCLUDED)


/*! \brief Calculates a clock divider (\e CD) for the USART SPI master mode to
 *         generate a baud rate as close as possible to the baud rate set point.
 *
 * Baud rate calculation:
 * \f$ Baudrate = \frac{SelectedClock}{CD} \f$.
 *
 * \param usart     Base address of the USART instance.
 * \param baudrate  Baud rate set point.
 * \param pba_hz    USART module input clock frequency (PBA clock, Hz).
 *
 * \retval USART_SUCCESS        Baud rate successfully initialized.
 * \retval USART_INVALID_INPUT  Baud rate set point is out of range for the given input clock frequency.
 */
static int usart_set_spi_master_baudrate(volatile avr32_usart_t *usart, unsigned int baudrate, unsigned long pba_hz)
{
  unsigned int cd = (pba_hz + baudrate / 2) / baudrate;

  if (cd < 4 || cd > (1 << AVR32_USART_BRGR_CD_SIZE) - 1)
    return USART_INVALID_INPUT;

  usart->mr = (usart->mr & ~AVR32_USART_MR_USCLKS_MASK) |
              AVR32_USART_MR_USCLKS_MCK << AVR32_USART_MR_USCLKS_OFFSET;

  usart->brgr = cd << AVR32_USART_BRGR_CD_OFFSET;

  return USART_SUCCESS;
}


/*! \brief Selects the SCK pin as the source of baud rate for the USART SPI
 *         slave mode.
 *
 * \param usart Base address of the USART instance.
 *
 * \retval USART_SUCCESS  Baud rate successfully initialized.
 */
static int usart_set_spi_slave_baudrate(volatile avr32_usart_t *usart)
{
  usart->mr = (usart->mr & ~AVR32_USART_MR_USCLKS_MASK) |
              AVR32_USART_MR_USCLKS_SCK << AVR32_USART_MR_USCLKS_OFFSET;

  return USART_SUCCESS;
}


#endif  // USART rev. >= 4.0.0


//! @}


//------------------------------------------------------------------------------
/*! \name Initialization Functions
 */
//! @{


void usart_reset(volatile avr32_usart_t *usart)
{
  Bool global_interrupt_enabled = Is_global_interrupt_enabled();

  // Disable all USART interrupts.
  // Interrupts needed should be set explicitly on every reset.
  if (global_interrupt_enabled) Disable_global_interrupt();
  usart->idr = 0xFFFFFFFF;
  usart->csr;
  if (global_interrupt_enabled) Enable_global_interrupt();

  // Reset mode and other registers that could cause unpredictable behavior after reset.
  usart->mr = 0;
  usart->rtor = 0;
  usart->ttgr = 0;

  // Shutdown TX and RX (will be re-enabled when setup has successfully completed),
  // reset status bits and turn off DTR and RTS.
  usart->cr = AVR32_USART_CR_RSTRX_MASK   |
              AVR32_USART_CR_RSTTX_MASK   |
              AVR32_USART_CR_RSTSTA_MASK  |
              AVR32_USART_CR_RSTIT_MASK   |
              AVR32_USART_CR_RSTNACK_MASK |
#ifndef AVR32_USART_440_H_INCLUDED
// Note: Modem Signal Management DTR-DSR-DCD-RI are not included in USART rev.440.
              AVR32_USART_CR_DTRDIS_MASK  |
#endif
              AVR32_USART_CR_RTSDIS_MASK;
}


int usart_init_rs232(volatile avr32_usart_t *usart, const usart_options_t *opt, long pba_hz)
{
  // Reset the USART and shutdown TX and RX.
  usart_reset(usart);

  // Check input values.
  if (!opt || // Null pointer.
      opt->charlength < 5 || opt->charlength > 9 ||
      opt->paritytype > 7 ||
      opt->stopbits > 2 + 255 ||
      opt->channelmode > 3 ||
      usart_set_async_baudrate(usart, opt->baudrate, pba_hz) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  if (opt->charlength == 9)
  {
    // Character length set to 9 bits. MODE9 dominates CHRL.
    usart->mr |= AVR32_USART_MR_MODE9_MASK;
  }
  else
  {
    // CHRL gives the character length (- 5) when MODE9 = 0.
    usart->mr |= (opt->charlength - 5) << AVR32_USART_MR_CHRL_OFFSET;
  }

  usart->mr |= opt->paritytype << AVR32_USART_MR_PAR_OFFSET |
               opt->channelmode << AVR32_USART_MR_CHMODE_OFFSET;

  if (opt->stopbits > USART_2_STOPBITS)
  {
    // Set two stop bits
    usart->mr |= AVR32_USART_MR_NBSTOP_2 << AVR32_USART_MR_NBSTOP_OFFSET;
    // and a timeguard period gives the rest.
    usart->ttgr = opt->stopbits - USART_2_STOPBITS;
  }
  else
    // Insert 1, 1.5 or 2 stop bits.
    usart->mr |= opt->stopbits << AVR32_USART_MR_NBSTOP_OFFSET;

  // Set normal mode.
  usart->mr = (usart->mr & ~AVR32_USART_MR_MODE_MASK) |
              AVR32_USART_MR_MODE_NORMAL << AVR32_USART_MR_MODE_OFFSET;

  // Setup complete; enable communication.
  // Enable input and output.
  usart->cr = AVR32_USART_CR_RXEN_MASK |
              AVR32_USART_CR_TXEN_MASK;

  return USART_SUCCESS;
}


int usart_init_rs232_tx_only(volatile avr32_usart_t *usart, const usart_options_t *opt, long pba_hz)
{
  // Reset the USART and shutdown TX and RX.
  usart_reset(usart);

  // Check input values.
  if (!opt || // Null pointer.
      opt->charlength < 5 || opt->charlength > 9 ||
      opt->paritytype > 7 ||
      opt->stopbits == 1 || opt->stopbits > 2 + 255 ||
      opt->channelmode > 3 ||
      usart_set_sync_master_baudrate(usart, opt->baudrate, pba_hz) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  if (opt->charlength == 9)
  {
    // Character length set to 9 bits. MODE9 dominates CHRL.
    usart->mr |= AVR32_USART_MR_MODE9_MASK;
  }
  else
  {
    // CHRL gives the character length (- 5) when MODE9 = 0.
    usart->mr |= (opt->charlength - 5) << AVR32_USART_MR_CHRL_OFFSET;
  }

  usart->mr |= opt->paritytype << AVR32_USART_MR_PAR_OFFSET |
               opt->channelmode << AVR32_USART_MR_CHMODE_OFFSET;

  if (opt->stopbits > USART_2_STOPBITS)
  {
    // Set two stop bits
    usart->mr |= AVR32_USART_MR_NBSTOP_2 << AVR32_USART_MR_NBSTOP_OFFSET;
    // and a timeguard period gives the rest.
    usart->ttgr = opt->stopbits - USART_2_STOPBITS;
  }
  else
    // Insert 1 or 2 stop bits.
    usart->mr |= opt->stopbits << AVR32_USART_MR_NBSTOP_OFFSET;

  // Set normal mode.
  usart->mr = (usart->mr & ~AVR32_USART_MR_MODE_MASK) |
              AVR32_USART_MR_MODE_NORMAL << AVR32_USART_MR_MODE_OFFSET;

  // Setup complete; enable communication.
  // Enable only output as input is not possible in synchronous mode without
  // transferring clock.
  usart->cr = AVR32_USART_CR_TXEN_MASK;

  return USART_SUCCESS;
}


int usart_init_hw_handshaking(volatile avr32_usart_t *usart, const usart_options_t *opt, long pba_hz)
{
  // First: Setup standard RS232.
  if (usart_init_rs232(usart, opt, pba_hz) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  // Set hardware handshaking mode.
  usart->mr = (usart->mr & ~AVR32_USART_MR_MODE_MASK) |
              AVR32_USART_MR_MODE_HARDWARE << AVR32_USART_MR_MODE_OFFSET;

  return USART_SUCCESS;
}


int usart_init_modem(volatile avr32_usart_t *usart, const usart_options_t *opt, long pba_hz)
{
  // First: Setup standard RS232.
  if (usart_init_rs232(usart, opt, pba_hz) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  // Set modem mode.
  usart->mr = (usart->mr & ~AVR32_USART_MR_MODE_MASK) |
              AVR32_USART_MR_MODE_MODEM << AVR32_USART_MR_MODE_OFFSET;

  return USART_SUCCESS;
}


int usart_init_sync_master(volatile avr32_usart_t *usart, const usart_options_t *opt, long pba_hz)
{
  // Reset the USART and shutdown TX and RX.
  usart_reset(usart);

  // Check input values.
  if (!opt || // Null pointer.
      opt->charlength < 5 || opt->charlength > 9 ||
      opt->paritytype > 7 ||
      opt->stopbits == 1 || opt->stopbits > 2 + 255 ||
      opt->channelmode > 3 ||
      usart_set_sync_master_baudrate(usart, opt->baudrate, pba_hz) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  if (opt->charlength == 9)
  {
    // Character length set to 9 bits. MODE9 dominates CHRL.
    usart->mr |= AVR32_USART_MR_MODE9_MASK;
  }
  else
  {
    // CHRL gives the character length (- 5) when MODE9 = 0.
    usart->mr |= (opt->charlength - 5) << AVR32_USART_MR_CHRL_OFFSET;
  }

  usart->mr |= opt->paritytype << AVR32_USART_MR_PAR_OFFSET |
               opt->channelmode << AVR32_USART_MR_CHMODE_OFFSET;

  if (opt->stopbits > USART_2_STOPBITS)
  {
    // Set two stop bits
    usart->mr |= AVR32_USART_MR_NBSTOP_2 << AVR32_USART_MR_NBSTOP_OFFSET;
    // and a timeguard period gives the rest.
    usart->ttgr = opt->stopbits - USART_2_STOPBITS;
  }
  else
    // Insert 1 or 2 stop bits.
    usart->mr |= opt->stopbits << AVR32_USART_MR_NBSTOP_OFFSET;

  // Set normal mode.
  usart->mr = (usart->mr & ~AVR32_USART_MR_MODE_MASK) |
              AVR32_USART_MR_MODE_NORMAL << AVR32_USART_MR_MODE_OFFSET |
              AVR32_USART_MR_CLKO_MASK;

  // Setup complete; enable communication.
  // Enable input and output.
  usart->cr = AVR32_USART_CR_RXEN_MASK |
              AVR32_USART_CR_TXEN_MASK;

  return USART_SUCCESS;
}


int usart_init_sync_slave(volatile avr32_usart_t *usart, const usart_options_t *opt, long pba_hz)
{
  // Reset the USART and shutdown TX and RX.
  usart_reset(usart);

  // Check input values.
  if (!opt || // Null pointer.
      opt->charlength < 5 || opt->charlength > 9 ||
      opt->paritytype > 7 ||
      opt->stopbits == 1 || opt->stopbits > 2 + 255 ||
      opt->channelmode > 3 ||
      usart_set_sync_slave_baudrate(usart) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  if (opt->charlength == 9)
  {
    // Character length set to 9 bits. MODE9 dominates CHRL.
    usart->mr |= AVR32_USART_MR_MODE9_MASK;
  }
  else
  {
    // CHRL gives the character length (- 5) when MODE9 = 0.
    usart->mr |= (opt->charlength - 5) << AVR32_USART_MR_CHRL_OFFSET;
  }

  usart->mr |= opt->paritytype << AVR32_USART_MR_PAR_OFFSET |
               opt->channelmode << AVR32_USART_MR_CHMODE_OFFSET;

  if (opt->stopbits > USART_2_STOPBITS)
  {
    // Set two stop bits
    usart->mr |= AVR32_USART_MR_NBSTOP_2 << AVR32_USART_MR_NBSTOP_OFFSET;
    // and a timeguard period gives the rest.
    usart->ttgr = opt->stopbits - USART_2_STOPBITS;
  }
  else
    // Insert 1 or 2 stop bits.
    usart->mr |= opt->stopbits << AVR32_USART_MR_NBSTOP_OFFSET;

  // Set normal mode.
  usart->mr = (usart->mr & ~AVR32_USART_MR_MODE_MASK) |
              AVR32_USART_MR_MODE_NORMAL << AVR32_USART_MR_MODE_OFFSET;

  // Setup complete; enable communication.
  // Enable input and output.
  usart->cr = AVR32_USART_CR_RXEN_MASK |
              AVR32_USART_CR_TXEN_MASK;

  return USART_SUCCESS;
}


int usart_init_rs485(volatile avr32_usart_t *usart, const usart_options_t *opt, long pba_hz)
{
  // First: Setup standard RS232.
  if (usart_init_rs232(usart, opt, pba_hz) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  // Set RS485 mode.
  usart->mr = (usart->mr & ~AVR32_USART_MR_MODE_MASK) |
              AVR32_USART_MR_MODE_RS485 << AVR32_USART_MR_MODE_OFFSET;

  return USART_SUCCESS;
}


int usart_init_IrDA(volatile avr32_usart_t *usart, const usart_options_t *opt,
                    long pba_hz, unsigned char irda_filter)
{
  // First: Setup standard RS232.
  if (usart_init_rs232(usart, opt, pba_hz) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  // Set IrDA filter.
  usart->ifr = irda_filter;

  // Set IrDA mode and activate filtering of input.
  usart->mr = (usart->mr & ~AVR32_USART_MR_MODE_MASK) |
              AVR32_USART_MODE_IRDA << AVR32_USART_MR_MODE_OFFSET |
              AVR32_USART_MR_FILTER_MASK;

  return USART_SUCCESS;
}


int usart_init_iso7816(volatile avr32_usart_t *usart, const usart_iso7816_options_t *opt, int t, long pba_hz)
{
  // Reset the USART and shutdown TX and RX.
  usart_reset(usart);

  // Check input values.
  if (!opt || // Null pointer.
      opt->paritytype > 1)
    return USART_INVALID_INPUT;

  if (t == 0)
  {
    // Set USART mode to ISO7816, T=0.
    // The T=0 protocol always uses 2 stop bits.
    usart->mr = AVR32_USART_MR_MODE_ISO7816_T0 << AVR32_USART_MR_MODE_OFFSET |
                AVR32_USART_MR_NBSTOP_2 << AVR32_USART_MR_NBSTOP_OFFSET |
                opt->bit_order << AVR32_USART_MR_MSBF_OFFSET; // Allow MSBF in T=0.
  }
  else if (t == 1)
  {
    // Only LSB first in the T=1 protocol.
    // max_iterations field is only used in T=0 mode.
    if (opt->bit_order != 0 ||
        opt->max_iterations != 0)
      return USART_INVALID_INPUT;

    // Set USART mode to ISO7816, T=1.
    // The T=1 protocol always uses 1 stop bit.
    usart->mr = AVR32_USART_MR_MODE_ISO7816_T1 << AVR32_USART_MR_MODE_OFFSET |
                AVR32_USART_MR_NBSTOP_1 << AVR32_USART_MR_NBSTOP_OFFSET;
  }
  else
    return USART_INVALID_INPUT;

  if (usart_set_iso7816_clock(usart, opt->iso7816_hz, pba_hz) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  // Set FIDI register: bit rate = selected clock/FI_DI_ratio/16.
  usart->fidi = opt->fidi_ratio;

  // Set ISO7816 spesific options in the MODE register.
  usart->mr |= opt->paritytype << AVR32_USART_MR_PAR_OFFSET |
               AVR32_USART_MR_CLKO_MASK | // Enable clock output.
               opt->inhibit_nack << AVR32_USART_MR_INACK_OFFSET |
               opt->dis_suc_nack << AVR32_USART_MR_DSNACK_OFFSET |
               opt->max_iterations << AVR32_USART_MR_MAX_ITERATION_OFFSET;

  // Setup complete; enable the receiver by default.
  usart_iso7816_enable_receiver(usart);

  return USART_SUCCESS;
}


#if defined(AVR32_USART_400_H_INCLUDED) || \
    defined(AVR32_USART_410_H_INCLUDED) || \
    defined(AVR32_USART_420_H_INCLUDED) || \
    defined(AVR32_USART_440_H_INCLUDED) || \
    defined(AVR32_USART_602_H_INCLUDED)


int usart_init_lin_master(volatile avr32_usart_t *usart, unsigned long baudrate, long pba_hz)
{
  // Reset the USART and shutdown TX and RX.
  usart_reset(usart);

  // Check input values.
  if (usart_set_async_baudrate(usart, baudrate, pba_hz) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  usart->mr |= AVR32_USART_MR_MODE_LIN_MASTER << AVR32_USART_MR_MODE_OFFSET;  // LIN master mode.

  // Setup complete; enable communication.
  // Enable input and output.
  usart->cr = AVR32_USART_CR_RXEN_MASK |
              AVR32_USART_CR_TXEN_MASK;

  return USART_SUCCESS;
}


int usart_init_lin_slave(volatile avr32_usart_t *usart, unsigned long baudrate, long pba_hz)
{
  // Reset the USART and shutdown TX and RX.
  usart_reset(usart);

  // Check input values.
  if (usart_set_async_baudrate(usart, baudrate, pba_hz) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  usart->mr |= AVR32_USART_MR_MODE_LIN_SLAVE << AVR32_USART_MR_MODE_OFFSET; // LIN slave mode.

  // Setup complete; enable communication.
  // Enable input and output.
  usart->cr = AVR32_USART_CR_RXEN_MASK |
              AVR32_USART_CR_TXEN_MASK;

  return USART_SUCCESS;
}


int usart_init_spi_master(volatile avr32_usart_t *usart, const usart_spi_options_t *opt, long pba_hz)
{
  // Reset the USART and shutdown TX and RX.
  usart_reset(usart);

  // Check input values.
  if (!opt || // Null pointer.
      opt->charlength < 5 || opt->charlength > 9 ||
      opt->spimode > 3 ||
      opt->channelmode > 3 ||
      usart_set_spi_master_baudrate(usart, opt->baudrate, pba_hz) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  if (opt->charlength == 9)
  {
    // Character length set to 9 bits. MODE9 dominates CHRL.
    usart->mr |= AVR32_USART_MR_MODE9_MASK;
  }
  else
  {
    // CHRL gives the character length (- 5) when MODE9 = 0.
    usart->mr |= (opt->charlength - 5) << AVR32_USART_MR_CHRL_OFFSET;
  }

  usart->mr |= AVR32_USART_MR_MODE_SPI_MASTER << AVR32_USART_MR_MODE_OFFSET | // SPI master mode.
               ((opt->spimode & 0x1) ^ 0x1) << AVR32_USART_MR_SYNC_OFFSET |   // SPI clock phase.
               opt->channelmode << AVR32_USART_MR_CHMODE_OFFSET |             // Channel mode.
               (opt->spimode >> 1) << AVR32_USART_MR_MSBF_OFFSET |            // SPI clock polarity.
               AVR32_USART_MR_CLKO_MASK;                                      // Drive SCK pin.

  // Setup complete; enable communication.
  // Enable input and output.
  usart->cr = AVR32_USART_CR_RXEN_MASK |
              AVR32_USART_CR_TXEN_MASK;

  return USART_SUCCESS;
}


int usart_init_spi_slave(volatile avr32_usart_t *usart, const usart_spi_options_t *opt, long pba_hz)
{
  // Reset the USART and shutdown TX and RX.
  usart_reset(usart);

  // Check input values.
  if (!opt || // Null pointer.
      opt->charlength < 5 || opt->charlength > 9 ||
      opt->spimode > 3 ||
      opt->channelmode > 3 ||
      usart_set_spi_slave_baudrate(usart) == USART_INVALID_INPUT)
    return USART_INVALID_INPUT;

  if (opt->charlength == 9)
  {
    // Character length set to 9 bits. MODE9 dominates CHRL.
    usart->mr |= AVR32_USART_MR_MODE9_MASK;
  }
  else
  {
    // CHRL gives the character length (- 5) when MODE9 = 0.
    usart->mr |= (opt->charlength - 5) << AVR32_USART_MR_CHRL_OFFSET;
  }

  usart->mr |= AVR32_USART_MR_MODE_SPI_SLAVE << AVR32_USART_MR_MODE_OFFSET |  // SPI slave mode.
               ((opt->spimode & 0x1) ^ 0x1) << AVR32_USART_MR_SYNC_OFFSET |   // SPI clock phase.
               opt->channelmode << AVR32_USART_MR_CHMODE_OFFSET |             // Channel mode.
               (opt->spimode >> 1) << AVR32_USART_MR_MSBF_OFFSET;             // SPI clock polarity.

  // Setup complete; enable communication.
  // Enable input and output.
  usart->cr = AVR32_USART_CR_RXEN_MASK |
              AVR32_USART_CR_TXEN_MASK;

  return USART_SUCCESS;
}


#endif  // USART rev. >= 4.0.0


//! @}


//------------------------------------------------------------------------------
#if defined(AVR32_USART_400_H_INCLUDED) || \
    defined(AVR32_USART_410_H_INCLUDED) || \
    defined(AVR32_USART_420_H_INCLUDED) || \
    defined(AVR32_USART_440_H_INCLUDED) || \
    defined(AVR32_USART_602_H_INCLUDED)


/*! \name SPI Control Functions
 */
//! @{


int usart_spi_selectChip(volatile avr32_usart_t *usart)
{
  // Force the SPI chip select.
  usart->cr = AVR32_USART_CR_RTSEN_MASK;

  return USART_SUCCESS;
}


int usart_spi_unselectChip(volatile avr32_usart_t *usart)
{
  int timeout = USART_DEFAULT_TIMEOUT;

  do
  {
    if (!timeout--) return USART_FAILURE;
  } while (!usart_tx_empty(usart));

  // Release the SPI chip select.
  usart->cr = AVR32_USART_CR_RTSDIS_MASK;

  return USART_SUCCESS;
}


//! @}


#endif  // USART rev. >= 4.0.0


//------------------------------------------------------------------------------
/*! \name Transmit/Receive Functions
 */
//! @{


int usart_send_address(volatile avr32_usart_t *usart, int address)
{
  // Check if USART is in multidrop / RS485 mode.
  if (!usart_mode_is_multidrop(usart)) return USART_MODE_FAULT;

  // Prepare to send an address.
  usart->cr = AVR32_USART_CR_SENDA_MASK;

  // Write the address to TX.
  usart_bw_write_char(usart, address);

  return USART_SUCCESS;
}


int usart_write_char(volatile avr32_usart_t *usart, int c)
{
  if (usart_tx_ready(usart))
  {
    usart->thr = (c << AVR32_USART_THR_TXCHR_OFFSET) & AVR32_USART_THR_TXCHR_MASK;
    return USART_SUCCESS;
  }
  else
    return USART_TX_BUSY;
}


int usart_putchar(volatile avr32_usart_t *usart, int c)
{
  int timeout = USART_DEFAULT_TIMEOUT;

  if (c == '\n')
  {
    do
    {
      if (!timeout--) return USART_FAILURE;
    } while (usart_write_char(usart, '\r') != USART_SUCCESS);

    timeout = USART_DEFAULT_TIMEOUT;
  }

  do
  {
    if (!timeout--) return USART_FAILURE;
  } while (usart_write_char(usart, c) != USART_SUCCESS);

  return USART_SUCCESS;
}


int usart_read_char(volatile avr32_usart_t *usart, int *c)
{
  // Check for errors: frame, parity and overrun. In RS485 mode, a parity error
  // would mean that an address char has been received.
  if (usart->csr & (AVR32_USART_CSR_OVRE_MASK |
                    AVR32_USART_CSR_FRAME_MASK |
                    AVR32_USART_CSR_PARE_MASK))
    return USART_RX_ERROR;

  // No error; if we really did receive a char, read it and return SUCCESS.
  if (usart_test_hit(usart))
  {
    *c = (usart->rhr & AVR32_USART_RHR_RXCHR_MASK) >> AVR32_USART_RHR_RXCHR_OFFSET;
    return USART_SUCCESS;
  }
  else
    return USART_RX_EMPTY;
}


int usart_getchar(volatile avr32_usart_t *usart)
{
  int c, ret;

  while ((ret = usart_read_char(usart, &c)) == USART_RX_EMPTY);

  if (ret == USART_RX_ERROR)
    return USART_FAILURE;

  return c;
}


void usart_write_line(volatile avr32_usart_t *usart, const char *string)
{
  while (*string != '\0')
    usart_putchar(usart, *string++);
}


int usart_get_echo_line(volatile avr32_usart_t *usart)
{
  int rx_char;
  int retval = USART_SUCCESS;

  while (1)
  {
    rx_char = usart_getchar(usart);
    if (rx_char == USART_FAILURE)
    {
      usart_write_line(usart, "Error!!!\n");
      retval = USART_FAILURE;
      break;
    }
    if (rx_char == '\x03')
    {
      retval = USART_FAILURE;
      break;
    }
    usart_putchar(usart, rx_char);
    if (rx_char == '\r')
    {
      usart_putchar(usart, '\n');
      break;
    }
  }

  return retval;
}


//! @}
