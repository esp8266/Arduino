/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

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

#ifndef _SPI_H_
#define _SPI_H_

#include <avr32/io.h>


//! Time-out value (number of attempts).
#define SPI_TIMEOUT       10000


//! Status codes used by the SPI driver.
typedef enum
{
  SPI_ERROR = -1,
  SPI_OK = 0,
  SPI_ERROR_TIMEOUT = 1,
  SPI_ERROR_ARGUMENT,
  SPI_ERROR_OVERRUN,
  SPI_ERROR_MODE_FAULT,
  SPI_ERROR_OVERRUN_AND_MODE_FAULT
} spi_status_t;

//! Option structure for SPI channels.
typedef struct
{
  //! The SPI channel to set up.
  unsigned char reg;

  //! Preferred baudrate for the SPI.
  unsigned int baudrate;

  //! Number of bits in each character (8 to 16).
  unsigned char bits;

  //! Delay before first clock pulse after selecting slave (in PBA clock periods).
  unsigned char spck_delay;

  //! Delay between each transfer/character (in PBA clock periods).
  unsigned char trans_delay;

  //! Sets this chip to stay active after last transfer to it.
  unsigned char stay_act;

  //! Which SPI mode to use when transmitting.
  unsigned char spi_mode;

  //! Disables the mode fault detection.
  //! With this bit cleared, the SPI master mode will disable itself if another
  //! master tries to address it.
  unsigned char modfdis;
} spi_options_t;


/*! \brief Resets the SPI controller.
 *
 * \param spi Base address of the SPI instance.
 */
extern void spi_reset(volatile avr32_spi_t *spi);

/*! \brief Initializes the SPI in slave mode.
 *
 * \param spi       Base address of the SPI instance.
 * \param bits      Number of bits in each transmitted character (8 to 16).
 * \param spi_mode  Clock polarity and phase.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 */
extern spi_status_t spi_initSlave(volatile avr32_spi_t *spi,
                                  unsigned char bits,
                                  unsigned char spi_mode);

/*! \brief Sets up the SPI in a test mode where the transmitter is connected to
 *         the receiver (local loopback).
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval SPI_OK Success.
 */
extern spi_status_t spi_initTest(volatile avr32_spi_t *spi);

/*! \brief Initializes the SPI in master mode.
 *
 * \param spi     Base address of the SPI instance.
 * \param options Pointer to a structure containing initialization options.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 */
extern spi_status_t spi_initMaster(volatile avr32_spi_t *spi, const spi_options_t *options);

/*! \brief Sets up how and when the slave chips are selected (master mode only).
 *
 * \param spi         Base address of the SPI instance.
 * \param variable_ps Target slave is selected in transfer register for every
 *                    character to transmit.
 * \param pcs_decode  The four chip select lines are decoded externally. Values
 *                    0 to 14 can be given to \ref spi_selectChip.
 * \param delay       Delay in PBA periods between chip selects.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 */
extern spi_status_t spi_selectionMode(volatile avr32_spi_t *spi,
                                      unsigned char variable_ps,
                                      unsigned char pcs_decode,
                                      unsigned char delay);

/*! \brief Selects slave chip.
 *
 * \param spi   Base address of the SPI instance.
 * \param chip  Slave chip number (normal: 0 to 3, extarnally decoded signal: 0
 *              to 14).
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 */
extern spi_status_t spi_selectChip(volatile avr32_spi_t *spi, unsigned char chip);

/*! \brief Unselects slave chip.
 *
 * \param spi   Base address of the SPI instance.
 * \param chip  Slave chip number (normal: 0 to 3, extarnally decoded signal: 0
 *              to 14).
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_TIMEOUT  Time-out.
 *
 * \note Will block program execution until time-out occurs if last transmission
 *       is not complete. Invoke \ref spi_writeEndCheck beforehand if needed.
 */
extern spi_status_t spi_unselectChip(volatile avr32_spi_t *spi, unsigned char chip);

/*! \brief Sets options for a specific slave chip.
 *
 * The baudrate field has to be written before transfer in master mode. Four
 * similar registers exist, one for each slave. When using encoded slave
 * addressing, reg=0 sets options for slaves 0 to 3, reg=1 for slaves 4 to 7 and
 * so on.
 *
 * \param spi     Base address of the SPI instance.
 * \param options Pointer to a structure containing initialization options for
 *                an SPI channel.
 * \param pba_hz  SPI module input clock frequency (PBA clock, Hz).
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 */
extern spi_status_t spi_setupChipReg(volatile avr32_spi_t *spi,
                                     const spi_options_t *options,
                                     unsigned int pba_hz);

/*! \brief Enables the SPI.
 *
 * \param spi Base address of the SPI instance.
 */
extern void spi_enable(volatile avr32_spi_t *spi);

/*! \brief Disables the SPI.
 *
 * Ensures that nothing is transferred while setting up buffers.
 *
 * \param spi Base address of the SPI instance.
 *
 * \warning This may cause data loss if used on a slave SPI.
 */
extern void spi_disable(volatile avr32_spi_t *spi);

/*! \brief Tests if the SPI is enabled.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return \c 1 if the SPI is enabled, otherwise \c 0.
 */
extern int spi_is_enabled(volatile avr32_spi_t *spi);

/*! \brief Checks if there is no data in the transmit register.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval 1  No data in TDR.
 *   \retval 0  Some data in TDR.
 */
extern unsigned char spi_writeRegisterEmptyCheck(volatile avr32_spi_t *spi);

/*! \brief Writes one data word in master fixed peripheral select mode or in
 *         slave mode.
 *
 * \param spi   Base address of the SPI instance.
 * \param data  The data word to write.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_TIMEOUT  Time-out.
 *
 * \note Will block program execution until time-out occurs if transmitter is
 *       busy and transmit buffer is full. Invoke
 *       \ref spi_writeRegisterEmptyCheck beforehand if needed.
 *
 * \note Once the data has been written to the transmit buffer, the end of
 *       transmission is not waited for. Invoke \ref spi_writeEndCheck if
 *       needed.
 */
extern spi_status_t spi_write(volatile avr32_spi_t *spi, unsigned short data);

/*! \brief Selects a slave in master variable peripheral select mode and writes
 *         one data word to it.
 *
 * \param spi       Base address of the SPI instance.
 * \param data      The data word to write.
 * \param pcs       Slave selector (bit 0 -> nCS line 0, bit 1 -> nCS line 1,
 *                  etc.).
 * \param lastxfer  Boolean indicating whether this is the last data word
 *                  transfer.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_TIMEOUT  Time-out.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 *
 * \note Will block program execution until time-out occurs if transmitter is
 *       busy and transmit buffer is full. Invoke
 *       \ref spi_writeRegisterEmptyCheck beforehand if needed.
 *
 * \note Once the data has been written to the transmit buffer, the end of
 *       transmission is not waited for. Invoke \ref spi_writeEndCheck if
 *       needed.
 */
extern spi_status_t spi_variableSlaveWrite(volatile avr32_spi_t *spi,
                                           unsigned short data,
                                           unsigned char pcs,
                                           unsigned char lastxfer);

/*! \brief Checks if all transmissions are complete.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval 1  All transmissions complete.
 *   \retval 0  Transmissions not complete.
 */
extern unsigned char spi_writeEndCheck(volatile avr32_spi_t *spi);

/*! \brief Checks if there is data in the receive register.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval 1  Some data in RDR.
 *   \retval 0  No data in RDR.
 */
extern unsigned char spi_readRegisterFullCheck(volatile avr32_spi_t *spi);

/*! \brief Reads one data word in master mode or in slave mode.
 *
 * \param spi   Base address of the SPI instance.
 * \param data  Pointer to the location where to store the received data word.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_TIMEOUT  Time-out.
 *
 * \note Will block program execution until time-out occurs if no data is
 *       received or last transmission is not complete. Invoke
 *       \ref spi_writeEndCheck or \ref spi_readRegisterFullCheck beforehand if
 *       needed.
 */
extern spi_status_t spi_read(volatile avr32_spi_t *spi, unsigned short *data);

/*! \brief Gets status information from the SPI.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval SPI_OK                           Success.
 *   \retval SPI_ERROR_OVERRUN                Overrun error.
 *   \retval SPI_ERROR_MODE_FAULT             Mode fault (SPI addressed as slave
 *                                            while in master mode).
 *   \retval SPI_ERROR_OVERRUN_AND_MODE_FAULT Overrun error and mode fault.
 */
extern unsigned char spi_getStatus(volatile avr32_spi_t *spi);


#endif  // _SPI_H_
