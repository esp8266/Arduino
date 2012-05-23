/*
  Copyright (c) 2012 Google, Inc.  All right reserved.

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
 * ADK2 Board pin  |  PORT
 * ----------------+--------
 *   0   RXD0      |  PA10
 *   1   TXD0      |  PA11
 *   2       PWML0 |  PA21
 *   3       PWML1 |  PC4
 *   4       PWML2 |  PC6
 *   5       PWML3 |  PC8
 *   6       PWML4 |  PC21
 *   7       PWML5 |  PC22
 *   8       PWML6 |  PC23
 *   9       PWML7 |  PC24
 *  10       PWMH0 |  PC3
 *  11       PWMH1 |  PC5
 *  12       PWMH2 |  PC7
 *  13       PWMH3 |  PC9 ("L" LED on board)
 *  14   TX3       |  PD4
 *  15   RX3       |  PD5
 *  16   TX2       |  PB20
 *  17   RX2       |  PB21
 *  18   TX1       |  PA13
 *  19   RX1       |  PA12
 *  20   SDA       |  PA17
 *  21   SCL       |  PA18
 *  22             |  PA14
 *  23             |  PA15
 *  24             |  PD0
 *  25             |  PD1
 *  26             |  PD2
 *  27             |  PD3
 *  28             |  PB23
 *  29             |  PB22
 *  30             |  PC28
 *  31             |  PC27
 *  32             |  PC26
 *  33             |  PC25
 *  34             |  PB8
 *  35             |  PB9
 *  36             |  PB6
 *  37             |  PB7
 *  38             |  PB4
 *  39             |  PB5
 *  40             |  PB2
 *  41             |  PB3
 *  42             |  PB0
 *  43             |  PB1
 *  44             |  PA29
 *  45             |  PA19
 *  46             |  PA20
 *  47             |  PA16
 *  48             |  PC29
 *  49             |  PC30
 *  50             |  PA25
 *  51             |  PA26
 *  52             |  PA27
 *  53             |  PA28
 *  A0             |  PA24
 *  A1             |  PA23
 *  A2             |  PA22
 *  A3             |  PA6
 *  A4             |  PA4
 *  A5             |  PA3
 *  A6             |  PA2
 *  A7             |  PB17
 *  A8             |  PB18
 *  A9             |  PB19
 * A10             |  Pxx
 * A11             |  Pxx
 * A12             |  PB15 (DAC0)
 * A13             |  PB16 (DAC1)
 * A14             |  Pxx
 * A15             |  Pxx
 *      SDA1       |  PB12
 *      SCL1       |  PB13
 *
 * LEDs            |  PORT
 * ----------------+--------
 * "TX"            |  (On LPC1343)
 * "RX"            |  (On LPC1343)
 * "L"             |  PC9
 */


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Pins descriptions
 */
extern const PinDescription g_APinDescription[]=
{
  // 0 .. 53 - Digital pins
  // ----------------------
  // 0/1 - USART0 (Serial)
  { PIOA, PIO_PA10A_RXD0,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // RXD0
  { PIOA, PIO_PA11A_TXD0,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // TXD0

  // 2
  { PIOA, PIO_PA21B_PWML0,   ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH0, NO_TC    }, // PWML0
  { PIOC, PIO_PC4B_PWML1,    ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH1, NO_TC    }, // PWML1
  { PIOC, PIO_PC6B_PWML2,    ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH2, NO_TC    }, // PWML2

  // 5
  { PIOC, PIO_PC8B_PWML3,    ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH3, NO_TC    }, // PWML3
  { PIOC, PIO_PC21B_PWML4,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM, NO_TC     }, // PWML4
  { PIOC, PIO_PC22B_PWML5,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM, NO_TC     }, // PWML5
  { PIOC, PIO_PC23B_PWML6,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM, NO_TC     }, // PWML6
  { PIOC, PIO_PC24B_PWML7,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM, NO_TC     }, // PWML7
  // 10
  { PIOC, PIO_PC3B_PWMH0,    ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH0, NO_TC    }, // PWMH0
  { PIOC, PIO_PC5B_PWMH1,    ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH1, NO_TC    }, // PWMH1
  { PIOC, PIO_PC7B_PWMH2,    ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH2, NO_TC    }, // PWMH2

  // 13 - AMBER LED
  { PIOC, PIO_PC9B_PWMH3,    ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH3,  NO_TC   }, // PWMH3

  // 14/15 - USART3 (Serial4)
  { PIOD, PIO_PD4B_TXD3,     ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // TXD3
  { PIOD, PIO_PD5B_RXD3,     ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // RXD3

  // 16/17 - USART2 (Serial3)
  { PIOB, PIO_PB20A_TXD2,    ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // TXD2
  { PIOB, PIO_PB21A_RXD2,    ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // RXD2

  // 18/19 - USART1 (Serial2)
  { PIOA, PIO_PA13A_TXD1,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // TXD1
  { PIOA, PIO_PA12A_RXD1,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // RXD1

  // 20/21 - TWI0
  { PIOA, PIO_PA17A_TWD0,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // TWD0 - SDA0
  { PIOA, PIO_PA18A_TWCK0,   ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // TWCK0 - SCL0

  // 22
  { PIOA, PIO_PA14,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 22
  { PIOA, PIO_PA15,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 23
  { PIOD, PIO_PD0,           ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 24
  { PIOD, PIO_PD1,           ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 25

  // 26
  { PIOD, PIO_PD2,           ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 26
  { PIOD, PIO_PD3,           ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 27
  { PIOB, PIO_PB23,          ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 28
  { PIOB, PIO_PB22,          ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 29

  // 30
  { PIOC, PIO_PC28,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 30
  { PIOC, PIO_PC27,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 31
  { PIOC, PIO_PC26,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 32
  { PIOC, PIO_PC25,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 33

  // 34
  { PIOB, PIO_PB8,           ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 34
  { PIOB, PIO_PB9,           ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 35
  { PIOB, PIO_PB6,           ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 36
  { PIOB, PIO_PB7,           ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 37

  // 38
  { PIOB, PIO_PB4,           ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 38
  { PIOB, PIO_PB5,           ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 39
  { PIOB, PIO_PB2,           ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 40
  { PIOB, PIO_PB3,           ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 41

  // 42
  { PIOB, PIO_PB0,           ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 42
  { PIOB, PIO_PB1,           ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 43
  { PIOA, PIO_PA29,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 44
  { PIOA, PIO_PA19,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 45

  // 46
  { PIOA, PIO_PA20,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 46
  { PIOA, PIO_PA16,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 47
  { PIOC, PIO_PC29,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 48
  { PIOC, PIO_PC30,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 49

  // 50
  { PIOA, PIO_PA25A_SPI0_MISO,  ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 50
  { PIOA, PIO_PA26A_SPI0_MOSI,  ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 51
  { PIOA, PIO_PA27A_SPI0_SPCK,  ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 52
  { PIOA, PIO_PA28A_SPI0_NPCS0, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // PIN 53

  // 54 .. 63 - Analog pins
  // ----------------------
  { PIOA, PIO_PA24X1_AD6,    ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC0,   ADC6,   NO_PWM,  NO_TC    }, // AD0
  { PIOA, PIO_PA23X1_AD5,    ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC1,   ADC5,   NO_PWM,  NO_TC    }, // AD1
  { PIOA, PIO_PA22X1_AD4,    ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC2,   ADC4,   NO_PWM,  NO_TC    }, // AD2
  { PIOA, PIO_PA6X1_AD3,     ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC3,   ADC3,   NO_PWM,  NO_TC    }, // AD3
  // 58
  { PIOA, PIO_PA4X1_AD2,     ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC4,   ADC2,   NO_PWM,  NO_TC    }, // AD4
  { PIOA, PIO_PA3X1_AD1,     ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC5,   ADC1,   NO_PWM,  NO_TC    }, // AD5
  { PIOA, PIO_PA2X1_AD0,     ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC6,   ADC0,   NO_PWM,  TC0_CHA1 }, // AD6
  { PIOB, PIO_PB17X1_AD10,   ID_PIOB, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC7,   ADC10,  NO_PWM,  NO_TC    }, // AD7
  // 62
  { PIOB, PIO_PB18X1_AD11,   ID_PIOB, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC8,   ADC11,  NO_PWM,  NO_TC    }, // AD8
  { PIOB, PIO_PB19X1_AD12,   ID_PIOB, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC9,   ADC12,  NO_PWM,  NO_TC    }, // AD9

  // 64/65 - TWI1
  { PIOB, PIO_PB12A_TWD1,    ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // TWD1 - SDA1
  { PIOB, PIO_PB13A_TWCK1,   ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // TWCK1 - SCL1

  // 66/67 - Debug UART
  { PIOA, PIO_PA8A_URXD,     ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM, NO_TC     }, // UART RXD
  { PIOA, PIO_PA9A_UTXD,     ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NO_PWM, NO_TC     }, // UART TXD

  // 68 .. 74 - "All pins" masks

  // 68 - TWI0 all pins
  { PIOA, PIO_PA17A_TWD0|PIO_PA18A_TWCK0, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },
  // 69 - TWI1 all pins
  { PIOB, PIO_PB12A_TWD1|PIO_PB13A_TWCK1, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },
  // 70 - UART (Serial) all pins
  { PIOA, PIO_PA8A_URXD|PIO_PA9A_UTXD, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },
  // 71 - USART0 (Serial2) all pins
  { PIOA, PIO_PA10A_RXD0|PIO_PA11A_TXD0, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },
  // 72 - USART1 (Serial3) all pins
  { PIOA, PIO_PA12A_RXD1|PIO_PA13A_TXD1, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },
  // 73 - USART2 (Serial4) all pins
  { PIOB, PIO_PB21A_RXD2|PIO_PB20A_TXD2, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },
  // 74 - USART3 (Serial5) all pins
  { PIOD, PIO_PD5B_RXD3|PIO_PD4B_TXD3, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NO_PWM, NO_TC },

  // 75 - USB
  { PIOB, PIO_PB11A_UOTGID|PIO_PB10A_UOTGVBOF, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,NO_ADC, NO_ADC, NO_PWM,  NO_TC    }, // ID - VBOF

  // END
  { NULL, 0, 0, PIO_NOT_A_PIN, PIO_DEFAULT, 0, NO_ADC, NO_ADC, NO_PWM, NO_TC }
} ;

#ifdef __cplusplus
}
#endif

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
RingBuffer rx_buffer4 ;
RingBuffer rx_buffer5 ;

USARTClass Serial2( USART0, USART0_IRQn, ID_USART0, &rx_buffer2 ) ;
USARTClass Serial3( USART1, USART1_IRQn, ID_USART1, &rx_buffer3 ) ;
USARTClass Serial4( USART2, USART2_IRQn, ID_USART2, &rx_buffer4 ) ;
USARTClass Serial5( USART3, USART3_IRQn, ID_USART3, &rx_buffer5 ) ;

// IT handlers
void USART0_Handler( void )
{
  Serial2.IrqHandler() ;
}

void USART1_Handler( void )
{
  Serial3.IrqHandler() ;
}

void USART2_Handler( void )
{
  Serial4.IrqHandler() ;
}

void USART3_Handler( void )
{
  Serial5.IrqHandler() ;
}

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
  PIO_Configure(
    g_APinDescription[PINS_USART2].pPort,
    g_APinDescription[PINS_USART2].ulPinType,
    g_APinDescription[PINS_USART2].ulPin,
    g_APinDescription[PINS_USART2].ulPinConfiguration);
  PIO_Configure(
    g_APinDescription[PINS_USART3].pPort,
    g_APinDescription[PINS_USART3].ulPinType,
    g_APinDescription[PINS_USART3].ulPin,
    g_APinDescription[PINS_USART3].ulPinConfiguration);

  // Initialize USB
  PIO_Configure(
    g_APinDescription[PINS_USB].pPort,
    g_APinDescription[PINS_USB].ulPinType,
    g_APinDescription[PINS_USB].ulPin,
    g_APinDescription[PINS_USB].ulPinConfiguration);

  // Initialize 10bit Analog Controller
  pmc_enable_periph_clk( ID_ADC ) ;
  adc_init( ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST ) ;
  adc_configure_timing(ADC, 0, ADC_SETTLING_TIME_3, 1);
  adc_configure_trigger(ADC, ADC_TRIG_SW, 0); // Disable hardware trigger.
  adc_disable_interrupt( ADC, 0xFFFFFFFF ) ; // Disable all ADC interrupts.
  adc_disable_all_channel( ADC ) ;

  // Initialize analogOutput module
  analogOutputInit();
}

#ifdef __cplusplus
}
#endif

