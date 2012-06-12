/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011-2012, Atmel Corporation
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

/** \addtogroup usart_module Working with USART
 * The USART driver provides the interface to configure and use the USART peripheral.\n
 *
 * The USART supports several kinds of comminication modes such as full-duplex asynchronous/
 * synchronous serial commnunication,RS485 with driver control signal,ISO7816,SPI and Test modes.
 *
 * To start a USART transfer with \ref AT91SAM3S_PDC "PDC" support, the user could follow these steps:
 * <ul>
 * <li> Configure USART with expected mode and baudrate(see \ref USART_Configure), which could be done by:
 * -# Resetting and disabling transmitter and receiver by setting US_CR(Control Register). </li>
 * -# Conifguring the USART in a specific mode by setting USART_MODE bits in US_MR(Mode Register) </li>
 * -# Setting baudrate which is different from mode to mode.
   </li>
 * <li> Enable transmitter or receiver respectively by set US_CR_TXEN or US_CR_RXEN in US_CR.</li>
 * <li> Read from or write to the peripheral with  \ref USART_ReadBuffer or \ref USART_WriteBuffer.
        These operations could be done by polling or interruption. </li>
 * <li> For polling, check the status bit US_CSR_ENDRX/US_CSR_RXBUFF (READ) or US_CSR_ENDTX/
        US_CSR_TXBUFE (WRITE).  </li>
 * <li> For interruption,"enable" the status bit through US_IER and
        realize the hanler with USARTx_IrqHandler according to IRQ vector
        table which is defined in board_cstartup_<toolchain>.c
        To enable the interruption of USART,it should be configured with priority and enabled first through
        NVIC .</li>
 * </ul>
 *
 * For more accurate information, please look at the USART section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref usart.c\n
 * \ref usart.h\n
*/



/**
 * \file
 *
 * Implementation of USART (Universal Synchronous Asynchronous Receiver Transmitter)
 * controller.
 *
 */
/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/
#include "chip.h"

#include <assert.h>
#include <string.h>

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *         Exported functions
 *------------------------------------------------------------------------------*/

/**
 * \brief Configures an USART peripheral with the specified parameters.
 *
 *
 *  \param usart  Pointer to the USART peripheral to configure.
 *  \param mode  Desired value for the USART mode register (see the datasheet).
 *  \param baudrate  Baudrate at which the USART should operate (in Hz).
 *  \param masterClock  Frequency of the system master clock (in Hz).
 */
void USART_Configure(Usart *usart,
                            uint32_t mode,
                            uint32_t baudrate,
                            uint32_t masterClock)
{
    /* Reset and disable receiver & transmitter*/
    usart->US_CR = US_CR_RSTRX | US_CR_RSTTX
                   | US_CR_RXDIS | US_CR_TXDIS;

    /* Configure mode*/
    usart->US_MR = mode;

    /* Configure baudrate*/
    /* Asynchronous, no oversampling*/
    if ( ((mode & US_MR_SYNC) == 0) && ((mode & US_MR_OVER) == 0) )
    {
        usart->US_BRGR = (masterClock / baudrate) / 16;
    }

    if( ((mode & US_MR_USART_MODE_SPI_MASTER) == US_MR_USART_MODE_SPI_MASTER)
     || ((mode & US_MR_SYNC) == US_MR_SYNC))
    {
        if( (mode & US_MR_USCLKS_Msk) == US_MR_USCLKS_MCK)
        {
            usart->US_BRGR = masterClock / baudrate;
        }
        else
        {
            if ( (mode & US_MR_USCLKS_DIV) == US_MR_USCLKS_DIV)
            {
                usart->US_BRGR = masterClock / baudrate / 8;
            }
        }
    }
    /* TODO other modes*/
}
/**
 * \brief Enables or disables the transmitter of an USART peripheral.
 *
 *
 * \param usart  Pointer to an USART peripheral
 * \param enabled  If true, the transmitter is enabled; otherwise it is
 *                disabled.
 */
void USART_SetTransmitterEnabled(Usart *usart, uint8_t enabled)
{
    if (enabled) {

        usart->US_CR = US_CR_TXEN;
    }
    else {

        usart->US_CR = US_CR_TXDIS;
    }
}

/**
 * \brief Enables or disables the receiver of an USART peripheral
 *
 *
 * \param usart  Pointer to an USART peripheral
 * \param enabled  If true, the receiver is enabled; otherwise it is disabled.
 */
void USART_SetReceiverEnabled(Usart *usart,
                                     uint8_t enabled)
{
    if (enabled) {

        usart->US_CR = US_CR_RXEN;
    }
    else {

        usart->US_CR = US_CR_RXDIS;
    }
}

/**
 * \brief Sends one packet of data through the specified USART peripheral. This
 * function operates synchronously, so it only returns when the data has been
 * actually sent.
 *
 *
 * \param usart  Pointer to an USART peripheral.
 * \param data  Data to send including 9nth bit and sync field if necessary (in
 *        the same format as the US_THR register in the datasheet).
 * \param timeOut  Time out value (0 = no timeout).
 */
void USART_Write(
    Usart *usart,
    uint16_t data,
    volatile uint32_t timeOut)
{
    if (timeOut == 0) {

        while ((usart->US_CSR & US_CSR_TXEMPTY) == 0);
    }
    else {

        while ((usart->US_CSR & US_CSR_TXEMPTY) == 0) {

            if (timeOut == 0) {

//                TRACE_ERROR("USART_Write: Timed out.\n\r");
                return;
            }
            timeOut--;
        }
    }

    usart->US_THR = data;
}

/**
 * \brief Sends the contents of a data buffer through the specified USART peripheral.
 * This function returns immediately (1 if the buffer has been queued, 0
 * otherwise); poll the ENDTX and TXBUFE bits of the USART status register
 * to check for the transfer completion.
 *
 * \param usart  Pointer to an USART peripheral.
 * \param buffer  Pointer to the data buffer to send.
 * \param size  Size of the data buffer (in bytes).
 */
uint8_t USART_WriteBuffer(
    Usart *usart,
    void *buffer,
    uint32_t size)
{
    /* Check if the first PDC bank is free*/
    if ((usart->US_TCR == 0) && (usart->US_TNCR == 0)) {

        usart->US_TPR = (uint32_t) buffer;
        usart->US_TCR = size;
        usart->US_PTCR = US_PTCR_TXTEN;

        return 1;
    }
    /* Check if the second PDC bank is free*/
    else if (usart->US_TNCR == 0) {

        usart->US_TNPR = (uint32_t) buffer;
        usart->US_TNCR = size;

        return 1;
    }
    else {

        return 0;
    }
}


/**
 * \brief  Reads and return a packet of data on the specified USART peripheral. This
 * function operates asynchronously, so it waits until some data has been
 * received.
 *
 * \param usart  Pointer to an USART peripheral.
 * \param timeOut  Time out value (0 -> no timeout).
 */
uint16_t USART_Read(
    Usart *usart,
    volatile uint32_t timeOut)
{
    if (timeOut == 0) {

        while ((usart->US_CSR & US_CSR_RXRDY) == 0);
    }
    else {

        while ((usart->US_CSR & US_CSR_RXRDY) == 0) {

            if (timeOut == 0) {

//                TRACE_ERROR( "USART_Read: Timed out.\n\r" ) ;
                return 0;
            }
            timeOut--;
        }
    }

    return usart->US_RHR;
}

/**
 * \brief  Reads data from an USART peripheral, filling the provided buffer until it
 * becomes full. This function returns immediately with 1 if the buffer has
 * been queued for transmission; otherwise 0.
 *
 * \param usart  Pointer to an USART peripheral.
 * \param buffer  Pointer to the buffer where the received data will be stored.
 * \param size  Size of the data buffer (in bytes).
 */
uint8_t USART_ReadBuffer(Usart *usart,
                                      void *buffer,
                                      uint32_t size)
{
    /* Check if the first PDC bank is free*/
    if ((usart->US_RCR == 0) && (usart->US_RNCR == 0)) {

        usart->US_RPR = (uint32_t) buffer;
        usart->US_RCR = size;
        usart->US_PTCR = US_PTCR_RXTEN;

        return 1;
    }
    /* Check if the second PDC bank is free*/
    else if (usart->US_RNCR == 0) {

        usart->US_RNPR = (uint32_t) buffer;
        usart->US_RNCR = size;

        return 1;
    }
    else {

        return 0;
    }
}

/**
 * \brief  Returns 1 if some data has been received and can be read from an USART;
 * otherwise returns 0.
 *
 * \param usart  Pointer to an Usart instance.
 */
uint8_t USART_IsDataAvailable(Usart *usart)
{
    if ((usart->US_CSR & US_CSR_RXRDY) != 0) {

        return 1;
    }
    else {

        return 0;
    }
}

/**
 * \brief  Sets the filter value for the IRDA demodulator.
 *
 * \param pUsart  Pointer to an Usart instance.
 * \param filter  Filter value.
 */
void USART_SetIrdaFilter(Usart *pUsart, uint8_t filter)
{
    assert( pUsart != NULL ) ;

    pUsart->US_IF = filter;
}

/**
 * \brief  Sends one packet of data through the specified USART peripheral. This
 * function operates synchronously, so it only returns when the data has been
 * actually sent.
 *
 * \param usart  Pointer to an USART peripheral.
 * \param c  Character to send
 */
void USART_PutChar(
    Usart *usart,
    uint8_t c)
{
    /* Wait for the transmitter to be ready*/
    while ((usart->US_CSR & US_CSR_TXEMPTY) == 0);

    /* Send character*/
    usart->US_THR = c;

    /* Wait for the transfer to complete*/
    while ((usart->US_CSR & US_CSR_TXEMPTY) == 0);
}

/**
 * \brief   Return 1 if a character can be read in USART
 */
uint32_t USART_IsRxReady(Usart *usart)
{
    return (usart->US_CSR & US_CSR_RXRDY);
}
/**
 * \brief   Get present status
 */
uint32_t USART_GetStatus(Usart *usart)
{
    return usart->US_CSR;
}
/**
 * \brief   Enable interrupt
 */
void USART_EnableIt(Usart *usart,uint32_t mode)
{
    usart->US_IER = mode;
}
/**
 * \brief   Disable interrupt
 */
void USART_DisableIt(Usart *usart,uint32_t mode)
{
    usart->US_IDR = mode;
}
/**
 * \brief  Reads and returns a character from the USART.
 *
 * \note This function is synchronous (i.e. uses polling).
 * \param usart  Pointer to an USART peripheral.
 * \return Character received.
 */
uint8_t USART_GetChar(Usart *usart)
{
    while ((usart->US_CSR & US_CSR_RXRDY) == 0);
    return usart->US_RHR;
}
