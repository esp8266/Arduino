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
  // 0 .. 53 - Digital pins
  // ----------------------
  // 0/1 - UART (Serial)
  { PIOA, PIO_PA8A_URXD,     ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // URXD
  { PIOA, PIO_PA9A_UTXD,     ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // UTXD
  // LEDS, 0..2
  { PIOB, PIO_PB0, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT }, // LED BLUE
  { PIOB, PIO_PB1, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT }, // LED GREEN
  { PIOB, PIO_PB2, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT }, // LED RED

  // USB, 3..3
  { PIOA, PIO_PA0, ID_PIOA, PIO_INPUT, PIO_PULLUP }, // USB_VBUS

  // UART (Serial), 4..6
  { PIOA, PIO_PA11A_URXD, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // URXD
  { PIOA, PIO_PA12A_UTXD, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // UTXD
  { PIOA, PIO_PA11A_URXD|PIO_PA12A_UTXD, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // All UART pins

  // Push buttons, 7..8
  { PIOA, PIO_PA18, ID_PIOA, PIO_INPUT, PIO_PULLUP | PIO_DEGLITCH }, // PB_LEFT
  { PIOA, PIO_PA19, ID_PIOA, PIO_INPUT, PIO_PULLUP | PIO_DEGLITCH }, // PB_RIGHT

  // MCI (SDCARD), 9..19
  { PIOA, PIO_PA25, ID_PIOA, PIO_INPUT, PIO_PULLUP }, // MCI Card Detect
  { PIOA, PIO_PA5A_MCDA0, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP }, // MCDA0
  { PIOA, PIO_PA6A_MCDA1, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP }, // MCDA1
  { PIOA, PIO_PA7A_MCDA2, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP }, // MCDA2
  { PIOA, PIO_PA8A_MCDA3, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP }, // MCDA3
  { PIOC, PIO_PC28B_MCDA4, ID_PIOC, PIO_PERIPH_B, PIO_PULLUP }, // MCDA4
  { PIOC, PIO_PC29B_MCDA5, ID_PIOC, PIO_PERIPH_B, PIO_PULLUP }, // MCDA5
  { PIOC, PIO_PC30B_MCDA6, ID_PIOC, PIO_PERIPH_B, PIO_PULLUP }, // MCDA6
  { PIOC, PIO_PC31B_MCDA7, ID_PIOC, PIO_PERIPH_B, PIO_PULLUP }, // MCDA7
  { PIOA, PIO_PA4A_MCCDA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP }, // MCCDA
  { PIOA, PIO_PA3A_MCCK, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP }, // MCCK

  // SPI, 20..23
  { PIOA, PIO_PA13A_MISO, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // MISO
  { PIOA, PIO_PA14A_MOSI, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // MOSI
  { PIOA, PIO_PA15A_SPCK, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // SPCK
  { PIOC, PIO_PC14B_NPCS2, ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT }, // NPCS2

  // TouchScreen, 24..25
  { PIOA, PIO_PA24, ID_PIOA, PIO_INPUT, PIO_PULLUP }, // IRQ_TSC
  { PIOA, PIO_PA2, ID_PIOA, PIO_INPUT, PIO_PULLUP }, // BUSY_TSC

  // USART1, 26..29
  { PIOA, PIO_PA21A_RXD1, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // RXD1
  { PIOA, PIO_PA20A_TXD1, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // TXD1
  { PIOA, PIO_PA22B_RTS1, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // RTS1
  { PIOA, PIO_PA23B_CTS1, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }, // CTS1

  // LCD, 30..47
  { PIOB, 0xFE01FE00, ID_PIOB, PIO_PERIPH_A, PIO_PULLUP }, // EBI Data bus PB9-16 PB25-31
  { PIOB, PIO_PB23A_NWE, ID_PIOB, PIO_PERIPH_A, PIO_PULLUP }, // EBI_NWE
  { PIOB, PIO_PB19A_NRD, ID_PIOB, PIO_PERIPH_A, PIO_PULLUP }, // EBI_NRD
  { PIOC, PIO_PC16A_NCS2, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP }, // LCD CS
  { PIOB, PIO_PB8, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT }, // LCD RS
  { PIOC, PIO_PC19, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT }, // LCD BackLight

  // 79 .. 84 - "All pins" masks
  // 79 - TWI0 all pins
  { PIOA, PIO_PA17A_TWD0|PIO_PA18A_TWCK0, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },
  // 81 - UART (Serial) all pins
  { PIOA, PIO_PA8A_URXD|PIO_PA9A_UTXD, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },
  // 82 - USART0 (Serial2) all pins
  { PIOA, PIO_PA11A_TXD0|PIO_PA10A_RXD0, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },
  { NULL, 0, 0, PIO_NOT_A_PIN, PIO_DEFAULT } // END
} ;

/*
 * UART objects
 */
RingBuffer rx_buffer1 ;

UARTClass Serial( UART, UART_IRQn, ID_UART, &rx_buffer1 ) ;

// IT handlers
void UART_Handler(void)
{
  Serial.IrqHandler() ;
}

// ----------------------------------------------------------------------------
/*
 * USART objects
 */
RingBuffer rx_buffer2 ;
RingBuffer rx_buffer3 ;

USARTClass Serial2( USART0, USART0_IRQn, ID_USART0, &rx_buffer2 ) ;
USARTClass Serial3( USART1, USART1_IRQn, ID_USART1, &rx_buffer3 ) ;

// IT handlers
void USART0_Handler( void )
{
  Serial2.IrqHandler() ;
}

void USART1_Handler( void )
{
  Serial3.IrqHandler() ;
}

// ----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// Should be made in a better way...
extern void analogOutputInit(void);

/**
 *
 */
extern void init( void )
{
  SystemInit() ;

  // Set Systick to 1ms interval, common to all SAM3 variants
  if ( SysTick_Config( SystemCoreClock / 1000 ) )
  {
    // Capture error
    while ( 1 ) ;
  }

  // Disable watchdog, common to all SAM variants
  WDT_Disable( WDT ) ;

  // Initialize Serial port UART, common to all SAM3 variants
  PIO_Configure(
    g_APinDescription[PINS_UART].pPort,
    g_APinDescription[PINS_UART].ulPinType,
    g_APinDescription[PINS_UART].ulPin,
    g_APinDescription[PINS_UART].ulPinConfiguration);

  // Initialize Serial ports USART
  PIO_Configure(
    g_APinDescription[PINS_USART0].pPort,
    g_APinDescription[PINS_USART0].ulPinType,
    g_APinDescription[PINS_USART0].ulPin,
    g_APinDescription[PINS_USART0].ulPinConfiguration);
  PIO_Configure(
    g_APinDescription[PINS_USART1].pPort,
    g_APinDescription[PINS_USART1].ulPinType,
    g_APinDescription[PINS_USART1].ulPin,
    g_APinDescription[PINS_USART1].ulPinConfiguration);

  // Switch off Power LED
  PIO_Configure( g_APinDescription[PIN_LED_RED].pPort, g_APinDescription[PIN_LED_RED].ulPinType,
                 g_APinDescription[PIN_LED_RED].ulPin, g_APinDescription[PIN_LED_RED].ulPinConfiguration ) ;
  PIO_Clear( g_APinDescription[PIN_LED_RED].pPort, g_APinDescription[PIN_LED_RED].ulPin ) ;

  // Initialize 10bit Analog Controller
  PMC_EnablePeripheral( ID_ADC ) ;
  adc_init( ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP ) ;
  adc_configure_timing( ADC, 15 ) ;
  adc_configure_trigger( ADC, ADC_TRIG_SW ) ;
  adc_disable_interrupt( ADC, 0xFFFFFFFF ) ; /* Disable all adc interrupt. */
  adc_disable_channel( ADC, ADC_ALL_CHANNEL ) ;

  // Initialize 12bit Analog Controller
  PMC_EnablePeripheral( ID_ADC12B ) ;
  adc12_init( ADC12B, SystemCoreClock, ADC12_FREQ_MAX, ADC12_STARTUP_FAST, 1 ) ;
  adc12_configure_timing( ADC12B, 15 ) ;
  adc12_configure_trigger( ADC12B, ADC_TRIG_SW ) ;
  adc12_disable_interrupt( ADC12B, 0xFFFFFFFF ) ; /* Disable all adc interrupt. */
  adc12_disable_channel( ADC12B, ADC_ALL_CHANNEL ) ;

  // Initialize analogOutput module
  analogOutputInit();
}
#ifdef __cplusplus
}
#endif

