/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief SPI driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the SPI interface on AVR32
 * devices.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with an SPI module can be used.
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

#include "spi.h"

#ifdef FREERTOS_USED

#include "FreeRTOS.h"
#include "semphr.h"

#endif


/*! \name SPI Writable Bit-Field Registers
 */
//! @{

typedef union
{
  unsigned long                 cr;
  avr32_spi_cr_t                CR;
} u_avr32_spi_cr_t;

typedef union
{
  unsigned long                 mr;
  avr32_spi_mr_t                MR;
} u_avr32_spi_mr_t;

typedef union
{
  unsigned long                 tdr;
  avr32_spi_tdr_t               TDR;
} u_avr32_spi_tdr_t;

typedef union
{
  unsigned long                 ier;
  avr32_spi_ier_t               IER;
} u_avr32_spi_ier_t;

typedef union
{
  unsigned long                 idr;
  avr32_spi_idr_t               IDR;
} u_avr32_spi_idr_t;

typedef union
{
  unsigned long                 csr;
  avr32_spi_csr0_t              CSR;
} u_avr32_spi_csr_t;

//! @}


#ifdef FREERTOS_USED

//! The SPI mutex.
static xSemaphoreHandle xSPIMutex;

#endif


/*! \brief Calculates the baudrate divider.
 *
 * \param options Pointer to a structure containing initialization options for
 *                an SPI channel.
 * \param pba_hz  SPI module input clock frequency (PBA clock, Hz).
 *
 * \return Divider or error code.
 *   \retval >=0  Success.
 *   \retval  <0  Error.
 */
static int getBaudDiv(const spi_options_t *options, unsigned int pba_hz)
{
  int baudDiv = (pba_hz + options->baudrate / 2) / options->baudrate;

  if (baudDiv <= 0 || baudDiv > 255) {
    return -1;
  }

  return baudDiv;
}


void spi_reset(volatile avr32_spi_t *spi)
{
  spi->cr = AVR32_SPI_CR_SWRST_MASK;
}


spi_status_t spi_initSlave(volatile avr32_spi_t *spi,
                           unsigned char bits,
                           unsigned char spi_mode)
{
  if (spi_mode > 3 ||
      bits < 8 || bits > 16) {
    return SPI_ERROR_ARGUMENT;
  }

  // Reset.
  spi->cr = AVR32_SPI_CR_SWRST_MASK;

  // Will use CSR0 offsets; these are the same for CSR0 to CSR3.
  spi->csr0 = ((spi_mode >> 1) << AVR32_SPI_CSR0_CPOL_OFFSET) |
              (((spi_mode & 0x1) ^ 0x1) << AVR32_SPI_CSR0_NCPHA_OFFSET) |
              ((bits - 8) << AVR32_SPI_CSR0_BITS_OFFSET);

  return SPI_OK;
}


spi_status_t spi_initTest(volatile avr32_spi_t *spi)
{
  // Reset.
  spi->cr = AVR32_SPI_CR_SWRST_MASK;
  spi->mr |= AVR32_SPI_MR_MSTR_MASK | // Master Mode.
             AVR32_SPI_MR_LLB_MASK;   // Local Loopback.

  return SPI_OK;
}


spi_status_t spi_initMaster(volatile avr32_spi_t *spi, const spi_options_t *options)
{
  u_avr32_spi_mr_t u_avr32_spi_mr;

  if (options->modfdis > 1) {
    return SPI_ERROR_ARGUMENT;
  }

  // Reset.
  spi->cr = AVR32_SPI_CR_SWRST_MASK;

  // Master Mode.
  u_avr32_spi_mr.mr = spi->mr;
  u_avr32_spi_mr.MR.mstr = 1;
  u_avr32_spi_mr.MR.modfdis = options->modfdis;
  u_avr32_spi_mr.MR.llb = 0;
  u_avr32_spi_mr.MR.pcs = (1 << AVR32_SPI_MR_PCS_SIZE) - 1;
  spi->mr = u_avr32_spi_mr.mr;

  return SPI_OK;
}


spi_status_t spi_selectionMode(volatile avr32_spi_t *spi,
                               unsigned char variable_ps,
                               unsigned char pcs_decode,
                               unsigned char delay)
{
  u_avr32_spi_mr_t u_avr32_spi_mr;

  if (variable_ps > 1 ||
      pcs_decode > 1) {
    return SPI_ERROR_ARGUMENT;
  }

  u_avr32_spi_mr.mr = spi->mr;
  u_avr32_spi_mr.MR.ps = variable_ps;
  u_avr32_spi_mr.MR.pcsdec = pcs_decode;
  u_avr32_spi_mr.MR.dlybcs = delay;
  spi->mr = u_avr32_spi_mr.mr;

  return SPI_OK;
}


spi_status_t spi_selectChip(volatile avr32_spi_t *spi, unsigned char chip)
{
#ifdef FREERTOS_USED
  while (pdFALSE == xSemaphoreTake(xSPIMutex, 20));
#endif

  // Assert all lines; no peripheral is selected.
  spi->mr |= AVR32_SPI_MR_PCS_MASK;

  if (spi->mr & AVR32_SPI_MR_PCSDEC_MASK) {
    // The signal is decoded; allow up to 15 chips.
    if (chip > 14) {
      return SPI_ERROR_ARGUMENT;
    }

    spi->mr &= ~AVR32_SPI_MR_PCS_MASK | (chip << AVR32_SPI_MR_PCS_OFFSET);
  } else {
    if (chip > 3) {
      return SPI_ERROR_ARGUMENT;
    }

    spi->mr &= ~(1 << (AVR32_SPI_MR_PCS_OFFSET + chip));
  }

  return SPI_OK;
}


spi_status_t spi_unselectChip(volatile avr32_spi_t *spi, unsigned char chip)
{
  unsigned int timeout = SPI_TIMEOUT;

  while (!(spi->sr & AVR32_SPI_SR_TXEMPTY_MASK)) {
    if (!timeout--) {
      return SPI_ERROR_TIMEOUT;
    }
  }

  // Assert all lines; no peripheral is selected.
  spi->mr |= AVR32_SPI_MR_PCS_MASK;

  // Last transfer, so deassert the current NPCS if CSAAT is set.
  spi->cr = AVR32_SPI_CR_LASTXFER_MASK;

#ifdef FREERTOS_USED
  xSemaphoreGive(xSPIMutex);
#endif

  return SPI_OK;
}


spi_status_t spi_setupChipReg(volatile avr32_spi_t *spi,
                              const spi_options_t *options,
                              unsigned int pba_hz)
{
  u_avr32_spi_csr_t u_avr32_spi_csr;

  if (options->spi_mode > 3 ||
      options->stay_act > 1 ||
      options->bits < 8 || options->bits > 16) {
    return SPI_ERROR_ARGUMENT;
  }

  int baudDiv = getBaudDiv(options, pba_hz);

  if (baudDiv < 0) {
    return SPI_ERROR_ARGUMENT;
  }

  // Will use CSR0 offsets; these are the same for CSR0 to CSR3.
  u_avr32_spi_csr.csr = 0;
  u_avr32_spi_csr.CSR.cpol = options->spi_mode >> 1;
  u_avr32_spi_csr.CSR.ncpha = (options->spi_mode & 0x1) ^ 0x1;
  u_avr32_spi_csr.CSR.csaat = options->stay_act;
  u_avr32_spi_csr.CSR.bits = options->bits - 8;
  u_avr32_spi_csr.CSR.scbr = baudDiv;
  u_avr32_spi_csr.CSR.dlybs = options->spck_delay;
  u_avr32_spi_csr.CSR.dlybct = options->trans_delay;

  switch(options->reg) {
    case 0:
      spi->csr0 = u_avr32_spi_csr.csr;
      break;
    case 1:
      spi->csr1 = u_avr32_spi_csr.csr;
      break;
    case 2:
      spi->csr2 = u_avr32_spi_csr.csr;
      break;
    case 3:
      spi->csr3 = u_avr32_spi_csr.csr;
      break;
    default:
      return SPI_ERROR_ARGUMENT;
  }

#ifdef FREERTOS_USED
  if (!xSPIMutex)
  {
    // Create the SPI mutex.
    vSemaphoreCreateBinary(xSPIMutex);
    if (!xSPIMutex)
    {
      while(1);
    }
  }
#endif

  return SPI_OK;
}


void spi_enable(volatile avr32_spi_t *spi)
{
  spi->cr = AVR32_SPI_CR_SPIEN_MASK;
}


void spi_disable(volatile avr32_spi_t *spi)
{
  spi->cr = AVR32_SPI_CR_SPIDIS_MASK;
}


int spi_is_enabled(volatile avr32_spi_t *spi)
{
  return (spi->sr & AVR32_SPI_SR_SPIENS_MASK) != 0;
}


unsigned char spi_writeRegisterEmptyCheck(volatile avr32_spi_t *spi)
{
  return ((spi->sr & AVR32_SPI_SR_TDRE_MASK) != 0);
}


spi_status_t spi_write(volatile avr32_spi_t *spi, unsigned short data)
{
  unsigned int timeout = SPI_TIMEOUT;

  while (!(spi->sr & AVR32_SPI_SR_TDRE_MASK)) {
    if (!timeout--) {
      return SPI_ERROR_TIMEOUT;
    }
  }

  spi->tdr = data << AVR32_SPI_TDR_TD_OFFSET;

  return SPI_OK;
}


spi_status_t spi_variableSlaveWrite(volatile avr32_spi_t *spi, unsigned short data,
                                    unsigned char pcs, unsigned char lastxfer)
{
  unsigned int timeout = SPI_TIMEOUT;

  if (pcs > 14 || lastxfer > 1) {
    return SPI_ERROR_ARGUMENT;
  }

  while (!(spi->sr & AVR32_SPI_SR_TDRE_MASK)) {
    if (!timeout--) {
      return SPI_ERROR_TIMEOUT;
    }
  }

  spi->tdr = (data << AVR32_SPI_TDR_TD_OFFSET) |
             (pcs << AVR32_SPI_TDR_PCS_OFFSET) |
             (lastxfer << AVR32_SPI_TDR_LASTXFER_OFFSET);

  return SPI_OK;
}


unsigned char spi_writeEndCheck(volatile avr32_spi_t *spi)
{
  return ((spi->sr & AVR32_SPI_SR_TXEMPTY_MASK) != 0);
}


unsigned char spi_readRegisterFullCheck(volatile avr32_spi_t *spi)
{
  return ((spi->sr & AVR32_SPI_SR_RDRF_MASK) != 0);
}


spi_status_t spi_read(volatile avr32_spi_t *spi, unsigned short *data)
{
  unsigned int timeout = SPI_TIMEOUT;

  while ((spi->sr & (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) !=
         (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) {
    if (!timeout--) {
      return SPI_ERROR_TIMEOUT;
    }
  }

  *data = spi->rdr >> AVR32_SPI_RDR_RD_OFFSET;

  return SPI_OK;
}


unsigned char spi_getStatus(volatile avr32_spi_t *spi)
{
  spi_status_t ret = SPI_OK;
  unsigned long sr = spi->sr;

  if (sr & AVR32_SPI_SR_OVRES_MASK) {
    ret = SPI_ERROR_OVERRUN;
  }

  if (sr & AVR32_SPI_SR_MODF_MASK) {
    ret += SPI_ERROR_MODE_FAULT;
  }

  if (ret == (SPI_ERROR_OVERRUN + SPI_ERROR_MODE_FAULT)) {
    return SPI_ERROR_OVERRUN_AND_MODE_FAULT;
  }
  else if (ret > 0) {
    return ret;
  } else {
    return SPI_OK;
  }
}
