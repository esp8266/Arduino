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
 * DUE Board pin   |  PORT
 * ----------------+--------
 *   0   RX0       |  PA8
 *   1   TX0       |  PA9
 *  13       TIOB0 |  PB27
 *
 * AT24C512BN pin  |  PORT
 * ----------------+--------
 *  SO             |  PA25
 *  SI             |  PA26
 *  SCK            |  PA27
 *  #CS        SS3 |  PB23
 *
 *
 * LEDs            |  PORT
 * ----------------+--------
 * "TX"            |  PA21
 * "RX"            |  PC30
 * "L"             |  PB27
 */


/*
 * Pins descriptions
 */
extern const PinDescription g_APinDescription[]=
{
  // 0 .. x - Digital pins
  // ----------------------
  // 0/1 - UART (Serial)
  { PIOA, PIO_PA8A_URXD,     ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // URXD
  { PIOA, PIO_PA9A_UTXD,     ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // UTXD

  // 2/5 - LEDS
  { PIOB, PIO_PB13,          ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // USER_LED1
  { PIOB, PIO_PB12,          ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // USER_LED2
  { PIOA, PIO_PA12,          ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // USER_LED3
  { PIOA, PIO_PA13,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // POWER_LED

  // 6/7 - USART0 (Serial2)
  { PIOA, PIO_PA10A_RXD0,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // URXD
  { PIOA, PIO_PA11A_TXD0,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // UTXD

  // 8 - UART (Serial) all pins
  { PIOA, PIO_PA8A_URXD|PIO_PA9A_UTXD, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },

  // 9 - USART0 (Serial2) all pins
  { PIOA, PIO_PA11A_TXD0|PIO_PA10A_RXD0, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },

  // 10 - Analog pins
  // ----------------------
  { PIOA, PIO_PA3,           ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC0,   ADC7,   NO_PWM,  NO_TC    }, // AD0

  { NULL, 0,                 0,       PIO_NOT_A_PIN, PIO_DEFAULT, PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC } // END
} ;

/*
 * UART objects
 */
RingBuffer rx_buffer1 ;

UARTClass Serial1( UART, UART_IRQn, ID_UART, &rx_buffer1 ) ;

// IT handlers
void UART_Handler(void)
{
  Serial1.IrqHandler() ;
}

// ----------------------------------------------------------------------------
/*
 * USART objects
 */
RingBuffer rx_buffer2 ;
RingBuffer rx_buffer3 ;

USARTClass Serial2( USART0, USART0_IRQn, ID_USART0, &rx_buffer2 ) ;

// IT handlers
void USART0_Handler( void )
{
  Serial2.IrqHandler() ;
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
  PIO_Configure( g_APinDescription[PINS_UART].pPort,
                  g_APinDescription[PINS_UART].ulPinType,
                  g_APinDescription[PINS_UART].ulPin,
                  g_APinDescription[PINS_UART].ulPinConfiguration ) ;

  // Initialize Serial ports USART
  PIO_Configure( g_APinDescription[PINS_USART0].pPort,
                  g_APinDescription[PINS_USART0].ulPinType,
                  g_APinDescription[PINS_USART0].ulPin,
                  g_APinDescription[PINS_USART0].ulPinConfiguration ) ;

  // Switch off Power LED
  PIO_Configure( g_APinDescription[PIN_LED_RED].pPort,
                  g_APinDescription[PIN_LED_RED].ulPinType,
                  g_APinDescription[PIN_LED_RED].ulPin,
                  g_APinDescription[PIN_LED_RED].ulPinConfiguration ) ;

  PIO_Clear( g_APinDescription[PIN_LED_RED].pPort, g_APinDescription[PIN_LED_RED].ulPin ) ;
}
#ifdef __cplusplus
}
#endif

