/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

/** \addtogroup ssc_module Working with SSC
 * The SSC driver provides the interface to configure and use the SSC
 * peripheral.
 *
 * !Usage
 *
 * -# Enable the SSC interface pins.
 * -# Configure the SSC to operate at a specific frequency by calling
 *    SSC_Configure(). This function enables the peripheral clock of the SSC,
 *    but not its PIOs.
 * -# Configure the transmitter and/or the receiver using the
 *    SSC_ConfigureTransmitter() and SSC_ConfigureEmitter() functions.
 * -# Enable the PIOs or the transmitter and/or the received.
 * -# Enable the transmitter and/or the receiver using SSC_EnableTransmitter()
 *    and SSC_EnableReceiver()
 * -# Send data through the transmitter using SSC_Write() and SSC_WriteBuffer()
 * -# Receive data from the receiver using SSC_Read() and SSC_ReadBuffer()
 * -# Disable the transmitter and/or the receiver using SSC_DisableTransmitter()
 *    and SSC_DisableReceiver()
 *
 * For more accurate information, please look at the RTC section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref ssc.c\n
 * \ref ssc.h.\n
*/
/*@{*/
/*@}*/


/**
 * \file
 *
 * Implementation of Synchronous Serial (SSC) controller.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

/*----------------------------------------------------------------------------
 *       Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Configures a SSC peripheral.If the divided clock is not used, the master
 * clock frequency can be set to 0.
 * \note The emitter and transmitter are disabled by this function.
 * \param bitRate  bit rate.
 * \param masterClock  master clock.
 */
void SSC_Configure(uint32_t bitRate, uint32_t masterClock)
{
    /* Enable SSC peripheral clock */
    PMC->PMC_PCER0 = 1 << ID_SSC;

    /* Reset, disable receiver & transmitter */
    SSC->SSC_CR = SSC_CR_RXDIS | SSC_CR_TXDIS | SSC_CR_SWRST;

    SSC->SSC_PTCR = SSC_PTCR_RXTDIS | SSC_PTCR_TXTDIS;
    /* Configure clock frequency */
    if (bitRate != 0) {

        SSC->SSC_CMR = masterClock / (2 * bitRate);
    }
    else {

        SSC->SSC_CMR = 0;
    }
}

/**
 * \brief Configures the transmitter of a SSC peripheral.
 * \param tcmr Transmit Clock Mode Register value.
 * \param tfmr Transmit Frame Mode Register value.
 */
void SSC_ConfigureTransmitter(uint32_t tcmr, uint32_t tfmr)
{
    SSC->SSC_TCMR = tcmr;
    SSC->SSC_TFMR = tfmr;
}

/**
 * \brief Configures the receiver of a SSC peripheral.
 * \param rcmr Receive Clock Mode Register value.
 * \param rfmr Receive Frame Mode Register value.
 */
void SSC_ConfigureReceiver(uint32_t rcmr, uint32_t rfmr)
{
    SSC->SSC_RCMR = rcmr;
    SSC->SSC_RFMR = rfmr;
}

/**
 * \brief Enables the transmitter of a SSC peripheral.
 */
void SSC_EnableTransmitter(void)
{
    SSC->SSC_CR = SSC_CR_TXEN;
}

/**
 * \brief Disables the transmitter of a SSC peripheral.
 */
void SSC_DisableTransmitter(void)
{
    SSC->SSC_CR = SSC_CR_TXDIS;
}

/**
 * \brief Enables the receiver of a SSC peripheral.
 */
void SSC_EnableReceiver(void)
{
    SSC->SSC_CR = SSC_CR_RXEN;
}

/**
 * \brief Disables the receiver of a SSC peripheral.
 */
void SSC_DisableReceiver(void)
{
    SSC->SSC_CR = SSC_CR_RXDIS;
}

/**
 * \brief Enables one or more interrupt sources of a SSC peripheral.
 * \param sources Bitwise OR of selected interrupt sources.
 */
void SSC_EnableInterrupts(uint32_t sources)
{
    SSC->SSC_IER = sources;
}

/**
 * \brief Disables one or more interrupt sources of a SSC peripheral.
 * \param sources Bitwise OR of selected interrupt sources.
 */
void SSC_DisableInterrupts(uint32_t sources)
{
    SSC->SSC_IDR = sources;
}

/**
 * \brief Sends one data frame through a SSC peripheral. If another frame is currently
 * being sent, this function waits for the previous transfer to complete.
 * \param frame Data frame to send.
 */
void SSC_Write(uint32_t frame)
{
    while ((SSC->SSC_SR & SSC_SR_TXRDY) == 0);
    SSC->SSC_THR = frame;
}

/**
 * \brief Waits until one frame is received on a SSC peripheral, and returns it.
 */
uint32_t SSC_Read(void)
{
    while ((SSC->SSC_SR & SSC_SR_RXRDY) == 0);
    return SSC->SSC_RHR;
}

/**
 * \brief Sends the contents of a data buffer a SSC peripheral, using the PDC.
 * \param buffer Data buffer to send.
 * \param length Size of the data buffer.
 * \return 1 if the buffer has been queued for transmission; otherwise returns 0.
 */
uint8_t SSC_WriteBuffer(void *buffer, uint32_t length)
{
    /* Check if first bank is free*/
    if (SSC->SSC_TCR == 0) {

        SSC->SSC_TPR = (uint32_t) buffer;
        SSC->SSC_TCR = length;
        SSC->SSC_PTCR = SSC_PTCR_TXTEN;
        return 1;
    }
    /* Check if second bank is free*/
    else if (SSC->SSC_TNCR == 0) {

        SSC->SSC_TNPR = (uint32_t) buffer;
        SSC->SSC_TNCR = length;
        return 1;
    }
    return 0;
}

/**
 * \brief Reads data coming from a SSC peripheral receiver and stores it into the
 * giving buffer with PDC.
 * \param buffer ata buffer used for reception.
 * \param length Size of the data buffer.
 * \return 1 if the buffer has been queued for reception; otherwise returns 0.
 */
uint8_t SSC_ReadBuffer(void *buffer, uint32_t length)
{
    /* Check if the first bank is free*/
    if (SSC->SSC_RCR == 0) {

        SSC->SSC_RPR = (uint32_t) buffer;
        SSC->SSC_RCR = length;
        SSC->SSC_PTCR = SSC_PTCR_RXTEN;
        return 1;
    }
    /* Check if second bank is free*/
    else if (SSC->SSC_RNCR == 0) {
        SSC->SSC_RNPR = (uint32_t) buffer;
        SSC->SSC_RNCR = length;
        return 1;
    }
    return 0;
}
