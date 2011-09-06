/*
 %atmel_license%
*/

/**
 * \file
 *
 * \par Purpose
 *
 * This module provides several definitions and methods for using an USART
 * peripheral.
 *
 * \par Usage
 *
 * -# Enable the USART peripheral clock in the PMC.
 * -# Enable the required USART PIOs (see pio.h).
 * -# Configure the UART by calling USART_Configure.
 * -# Enable the transmitter and/or the receiver of the USART using
 *    USART_SetTransmitterEnabled and USART_SetReceiverEnabled.
 * -# Send data through the USART using the USART_Write and
 *    USART_WriteBuffer methods.
 * -# Receive data from the USART using the USART_Read and
 *    USART_ReadBuffer functions; the availability of data can be polled
 *    with USART_IsDataAvailable.
 * -# Disable the transmitter and/or the receiver of the USART with
 *    USART_SetTransmitterEnabled and USART_SetReceiverEnabled.
 */

#ifndef _USART_
#define _USART_

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/

#include "../chip.h"

#include <stdint.h>

/*------------------------------------------------------------------------------
 *         Definitions
 *------------------------------------------------------------------------------*/

/** \section USART_mode USART modes
 * This section lists several common operating modes for an USART peripheral.
 *
 * \b Modes
 * - USART_MODE_ASYNCHRONOUS
 * - USART_MODE_IRDA
 */

/** Basic asynchronous mode, i.e. 8 bits no parity.*/
#define USART_MODE_ASYNCHRONOUS        (US_MR_CHRL_8_BIT | US_MR_PAR_NO)

/** IRDA mode*/
#define USART_MODE_IRDA                (AT91C_US_USMODE_IRDA | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_FILTER)

/** SPI mode*/
#define AT91C_US_USMODE_SPIM     0xE
#define US_SPI_CPOL_0           (0x0<<16)
#define US_SPI_CPHA_0            (0x0<<8)
#define US_SPI_CPOL_1            (0x1<<16)
#define US_SPI_CPHA_1            (0x1<<8)
#define US_SPI_BPMODE_0    (US_SPI_CPOL_0|US_SPI_CPHA_1)
#define US_SPI_BPMODE_1    (US_SPI_CPOL_0|US_SPI_CPHA_0)
#define US_SPI_BPMODE_2    (US_SPI_CPOL_1|US_SPI_CPHA_1)
#define US_SPI_BPMODE_3    (US_SPI_CPOL_1|US_SPI_CPHA_0)

#ifdef __cplusplus
 extern "C" {
#endif

/*------------------------------------------------------------------------------*/
/*         Exported functions                                                   */
/*------------------------------------------------------------------------------*/

extern void USART_Configure( Usart *usart, uint32_t mode, uint32_t baudrate, uint32_t masterClock ) ;
extern uint32_t USART_GetStatus( Usart *usart ) ;
extern void USART_EnableIt( Usart *usart,uint32_t mode ) ;
extern void USART_DisableIt( Usart *usart,uint32_t mode ) ;
extern void USART_SetTransmitterEnabled( Usart *usart, uint8_t enabled ) ;

extern void USART_SetReceiverEnabled( Usart *usart, uint8_t enabled ) ;

extern void USART_Write( Usart *usart, uint16_t data, volatile uint32_t timeOut ) ;

extern uint8_t USART_WriteBuffer( Usart *usart, void *buffer, uint32_t size ) ;

extern uint16_t USART_Read( Usart *usart, volatile uint32_t timeOut ) ;

extern uint8_t USART_ReadBuffer( Usart *usart, void *buffer, uint32_t size ) ;

extern uint8_t USART_IsDataAvailable( Usart *usart ) ;

extern void USART_SetIrdaFilter(Usart *pUsart, uint8_t filter);

extern void USART_PutChar( Usart *usart, uint8_t c ) ;

extern uint32_t USART_IsRxReady( Usart *usart ) ;

extern uint8_t USART_GetChar( Usart *usart ) ;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _USART_ */

