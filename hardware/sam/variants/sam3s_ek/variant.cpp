/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "variant.h"

/*
 * Pins descriptions
 */
extern const PinDescription g_APinDescription[]=
{
  // LEDS, 0..2
#if defined VARIANT_REV_A
  { PIOC, PIO_PC20, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT }, // LED BLUE
  { PIOC, PIO_PC21, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT }, // LED GREEN
  { PIOC, PIO_PC22, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT }, // LED RED
#elif defined VARIANT_REV_B
  { PIOA, PIO_PA19, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT }, // LED BLUE
  { PIOA, PIO_PA20, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT }, // LED GREEN
  { PIOC, PIO_PC20, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT }, // LED RED
#else
#error "No board revision defined"
#endif

  // Clock 32KHz, 3..4
  { PIOA, PIO_PA7, ID_PIOA, PIO_NOT_A_PIN, PIO_DEFAULT }, // XIN32
  { PIOA, PIO_PA8, ID_PIOA, PIO_NOT_A_PIN, PIO_DEFAULT }, // XOUT32

  // Clock 12MHz, 5..6
  { PIOB, PIO_PB9, ID_PIOB, PIO_NOT_A_PIN, PIO_DEFAULT }, // XIN
  { PIOB, PIO_PB8, ID_PIOB, PIO_NOT_A_PIN, PIO_DEFAULT }, // XOUT

  // JTAG, 7..10
  { PIOB, PIO_PB4, ID_PIOB, PIO_NOT_A_PIN, PIO_DEFAULT }, // TDI
  { PIOB, PIO_PB5, ID_PIOB, PIO_NOT_A_PIN, PIO_DEFAULT }, // TDO
  { PIOB, PIO_PB6, ID_PIOB, PIO_NOT_A_PIN, PIO_DEFAULT }, // TMS
  { PIOB, PIO_PB7, ID_PIOB, PIO_NOT_A_PIN, PIO_DEFAULT }, // TCK

  // USB, 11..13
  { PIOB, PIO_PB10, ID_PIOB, PIO_NOT_A_PIN, PIO_DEFAULT }, // DDM
  { PIOB, PIO_PB11, ID_PIOB, PIO_NOT_A_PIN, PIO_DEFAULT }, // DDP
#if defined VARIANT_REV_A
  { PIOC, PIO_PC23, ID_PIOC, PIO_INPUT, PIO_PULLUP }, // USB_VBUS
#elif defined VARIANT_REV_B
  { PIOC, PIO_PC21, ID_PIOC, PIO_INPUT, PIO_PULLUP }, // USB_VBUS
#else
#error "No board revision defined"
#endif

  // UART0 (Serial), 14..16
  { PIOA, PIO_PA9, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // URXD0
  { PIOA, PIO_PA10, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // UTXD0
  { PIOA, PIO_PA9|PIO_PA10, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // All UART0 pins

  // Push buttons, 17..18
  { PIOB, PIO_PB3, ID_PIOB, PIO_INPUT, PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE }, // PB1
  { PIOC, PIO_PC12, ID_PIOC, PIO_INPUT, PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_FALL_EDGE }, // PB2

  // QTouch Slider, 19..20
  { PIOA, (PIO_PA0 | PIO_PA2 | PIO_PA4), ID_PIOA, PIO_INPUT, PIO_DEFAULT }, // QTSLIDR SNS
  { PIOA, (PIO_PA1 | PIO_PA3 | PIO_PA5), ID_PIOA, PIO_INPUT, PIO_DEFAULT }, // QTSLIDR SNSK

  // QTouch Buttons, 21..22
  { PIOC, (PIO_PC22 | PIO_PC24 | PIO_PC26 | PIO_PC28 | PIO_PC30), ID_PIOC, PIO_INPUT, PIO_DEFAULT }, // KEYS SNS
  { PIOC, (PIO_PC23 | PIO_PC25 | PIO_PC27 | PIO_PC29 | PIO_PC31), ID_PIOC, PIO_INPUT, PIO_DEFAULT }, // KEYS SNSK

  // MCI (SDCARD), 23..30
#if defined VARIANT_REV_A
  { PIOA, PIO_PA15, ID_PIOA, PIO_INPUT, PIO_PULLUP }, // MCI Card Detect
#elif defined VARIANT_REV_B
  { PIOA, PIO_PA6, ID_PIOA, PIO_INPUT, PIO_PULLUP }, // MCI Card Detect
#else
#error "No board revision defined"
#endif
  { PIOA, PIO_PA30, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP }, // MCDA0
  { PIOA, PIO_PA31, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP }, // MCDA1
  { PIOA, PIO_PA26, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP }, // MCDA2
  { PIOA, PIO_PA27, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP }, // MCDA3
  { PIOA, PIO_PA28, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP }, // MCCDA
  { PIOA, PIO_PA29, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP }, // MCCK

  { PIOA, PIO_PA26|PIO_PA27|PIO_PA28|PIO_PA29|PIO_PA30|PIO_PA31, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP }, // All MCI Pins

  // SPI, 31..34
  { PIOA, PIO_PA12A_MISO, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // MISO
  { PIOA, PIO_PA13A_MOSI, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // MOSI
  { PIOA, PIO_PA14A_SPCK, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // SPCK
  { PIOA, PIO_PA11A_NPCS0, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // NPCS0

  // TouchScreen, 35..36
#if defined VARIANT_REV_A
  { PIOA, PIO_PA4, ID_PIOA, PIO_INPUT, PIO_PULLUP }, // IRQ_TSC
  { PIOA, PIO_PA5, ID_PIOA, PIO_INPUT, PIO_PULLUP }, // BUSY_TSC
#elif defined VARIANT_REV_B
  { PIOA, PIO_PA16, ID_PIOA, PIO_INPUT, PIO_PULLUP }, // IRQ_TSC
  { PIOA, PIO_PA17, ID_PIOA, PIO_INPUT, PIO_PULLUP }, // BUSY_TSC
#else
#error "No board revision defined"
#endif

  // USART1, 37..41
  { PIOA, PIO_PA21A_RXD1, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // RXD1
  { PIOA, PIO_PA22A_TXD1, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // TXD1
  { PIOA, PIO_PA24A_RTS1, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // RTS1
  { PIOA, PIO_PA25A_CTS1, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // CTS1
  { PIOA, PIO_PA23A_SCK1, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT }, // COM1 ENABLE (MAX232)

  // LCD, 42..47
  { PIOC, 0xFF, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP }, // EBI Data bus
  { PIOC, PIO_PC8, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP }, // EBI_NWE
  { PIOC, PIO_PC11, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP }, // EBI_NRD
  { PIOC, PIO_PC15, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP }, // LCD CS
  { PIOC, PIO_PC19, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP }, // LCD RS
  { PIOC, PIO_PC13, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT }, // LCD BackLight

  { NULL, 0, 0, PIO_NOT_A_PIN, PIO_DEFAULT } // END
} ;

/*
 * UART objects
 */
RingBuffer rx_buffer1 ;
RingBuffer tx_buffer1 ;
RingBuffer rx_buffer2 ;
RingBuffer tx_buffer2 ;

UARTClass Serial( UART0, UART0_IRQn, ID_UART0, &rx_buffer1, &tx_buffer1 ) ;
UARTClass Serial2( UART1, UART1_IRQn, ID_UART1, &rx_buffer2, &tx_buffer2 ) ;

#ifdef __cplusplus
extern "C" {
#endif

// IT handlers
extern void UART0_IrqHandler( void )
{
  Serial.IrqHandler() ;
}

extern void UART1_IrqHandler( void )
{
  Serial2.IrqHandler() ;
}

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
/*
 * USART objects
 */
RingBuffer rx_buffer3 ;
RingBuffer tx_buffer3 ;
RingBuffer rx_buffer4 ;
RingBuffer tx_buffer4 ;

USARTClass Serial3( USART0, USART0_IRQn, ID_USART0, &rx_buffer3, &tx_buffer3 ) ;
USARTClass Serial4( USART1, USART1_IRQn, ID_USART1, &rx_buffer4, &tx_buffer4 ) ;

#ifdef __cplusplus
extern "C" {
#endif

// IT handlers
extern void USART0_IrqHandler( void )
{
  Serial3.IrqHandler() ;
}

extern void USART1_IrqHandler( void )
{
  Serial4.IrqHandler() ;
}

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 */
extern void init( void )
{
	SystemInit() ;

    /* Set Systick to 1ms interval, common to all SAM3 variants */
	if ( SysTick_Config( SystemCoreClock / 1000 ) )
  {
    /* Capture error */
    while ( 1 ) ;
	}

  /* Disable watchdog, common to all SAM variants */
  WDT_Disable( WDT ) ;

  // Initialize Serial port UART0, common to all SAM3 variants
  PIO_Configure( g_APinDescription[PINS_UART].pPort, g_APinDescription[PINS_UART].ulPinType,
                 g_APinDescription[PINS_UART].ulPin, g_APinDescription[PINS_UART].ulPinConfiguration ) ;

  // Switch off Power LED
  PIO_Configure( g_APinDescription[PIN_LED_RED].pPort, g_APinDescription[PIN_LED_RED].ulPinType,
                 g_APinDescription[PIN_LED_RED].ulPin, g_APinDescription[PIN_LED_RED].ulPinConfiguration ) ;
  PIO_Clear( g_APinDescription[PIN_LED_RED].pPort, g_APinDescription[PIN_LED_RED].ulPin ) ;
}
#ifdef __cplusplus
}
#endif

