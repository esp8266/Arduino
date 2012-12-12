/**
 * \file
 *
 * \brief Arduino Due/X Board Definition.
 *
 * Copyright (c) 2011 - 2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef ARDUINO_DUE_X_H_INCLUDED
#define ARDUINO_DUE_X_H_INCLUDED

#include "compiler.h"
#include "system_sam3x.h"
//#include "exceptions.h"

/* ------------------------------------------------------------------------ */

/**
 *  \page arduino_due_x_opfreq "Arduino Due/X - Operating frequencies"
 *  This page lists several definition related to the board operating frequency
 *
 *  \section Definitions
 *  - \ref BOARD_FREQ_*
 *  - \ref BOARD_MCK
 */

/*! Board oscillator settings */
#define BOARD_FREQ_SLCK_XTAL            (32768U)
#define BOARD_FREQ_SLCK_BYPASS          (32768U)
#define BOARD_FREQ_MAINCK_XTAL          (12000000U)
#define BOARD_FREQ_MAINCK_BYPASS        (12000000U)

/*! Master clock frequency */
#define BOARD_MCK                       CHIP_FREQ_CPU_MAX
#define BOARD_NO_32K_XTAL

/** board main clock xtal statup time */
#define BOARD_OSC_STARTUP_US   15625

/* ------------------------------------------------------------------------ */

/**
 * \page arduino_due_x_board_info "Arduino Due/X - Board informations"
 * This page lists several definition related to the board description.
 *
 * \section Definitions
 * - \ref BOARD_NAME
 */

/*! Name of the board */
#define BOARD_NAME "Arduino Due/X"
/*! Board definition */
#define arduinoduex
/*! Family definition (already defined) */
#define sam3x
/*! Core definition */
#define cortexm3

/* ------------------------------------------------------------------------ */

/**
 * \page arduino_due_x_piodef "Arduino Due/X - PIO definitions"
 * This pages lists all the pio definitions. The constants
 * are named using the following convention: PIN_* for a constant which defines
 * a single Pin instance (but may include several PIOs sharing the same
 * controller), and PINS_* for a list of Pin instances.
 *
 */

/**
 * \file
 * ADC
 * - \ref PIN_ADC0_AD1
 * - \ref PINS_ADC
 *
 */

/**
 * \note ADC pins are automatically configured by the ADC peripheral as soon as
 * the corresponding channel is enabled.
 *
 * \note On Arduino Due/X, Channel 1 is labelled A6 on the PCB.
 */

/*! ADC_AD1 pin definition. */
#define PIN_ADC0_AD1 {PIO_PA3X1_AD1, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PINS_ADC_TRIG  PIO_PA11_IDX
#define PINS_ADC_TRIG_FLAG  (PIO_PERIPH_B | PIO_DEFAULT)
/*! Pins ADC */
#define PINS_ADC PIN_ADC0_AD1

/**
 * \file
 * DAC
 *
 */

/**
 * \note DAC pins are automatically configured by the DAC peripheral as soon
 * as the corresponding channel is enabled.
 *
 * \note On Arduino Due/X, channel 0 is labelled A12 and channel 1 is labelled
 * A13 on the PCB.
 */


/**
 * \file
 * LEDs
 *
 */

/* ------------------------------------------------------------------------ */
/* LEDS                                                                     */
/* ------------------------------------------------------------------------ */
/*! Power LED pin definition (ORANGE). L */
#define PIN_POWER_LED   {PIO_PB27, PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
/*! LED #1 pin definition */
#define PIN_USER_LED1   {PIO_PC21, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
/*! LED #2 pin definition */
#define PIN_USER_LED2   {PIO_PC22, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
/*! LED #3 pin definition */
#define PIN_USER_LED3   {PIO_PC23, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}

/*! List of all LEDs definitions. */
#define PINS_LEDS   PIN_USER_LED1, PIN_USER_LED2, PIN_USER_LED3, PIN_POWER_LED

/*! LED #0 "L" pin definition (ORANGE).*/
#define LED_0_NAME      "Orange_LED"
#define LED0_GPIO       (PIO_PB27_IDX)
#define LED0_FLAGS      (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LED0_ACTIVE_LEVEL 0

#define PIN_LED_0       {1 << 27, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_LED_0_MASK  (1 << 27)
#define PIN_LED_0_PIO   PIOB
#define PIN_LED_0_ID    ID_PIOB
#define PIN_LED_0_TYPE  PIO_OUTPUT_0
#define PIN_LED_0_ATTR  PIO_DEFAULT

/*! LED #1 pin definition */
#define LED_1_NAME      "External_LED_on_PWM9_connector_output"
#define LED1_GPIO       (PIO_PC21_IDX)
#define LED1_FLAGS      (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LED1_ACTIVE_LEVEL 0

#define PIN_LED_1       {1 << 21, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_LED_1_MASK  (1 << 21)
#define PIN_LED_1_PIO   PIOC
#define PIN_LED_1_ID    ID_PIOC
#define PIN_LED_1_TYPE  PIO_OUTPUT_1
#define PIN_LED_1_ATTR  PIO_DEFAULT

/*! LED #2 pin detection */
#define LED2_GPIO       (PIO_PC22_IDX)
#define LED2_FLAGS      (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LED2_ACTIVE_LEVEL 0

#define PIN_LED_2       {1 << 22, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_LED_2_MASK  (1 << 22)
#define PIN_LED_2_PIO   PIOC
#define PIN_LED_2_ID    ID_PIOC
#define PIN_LED_2_TYPE  PIO_OUTPUT_1
#define PIN_LED_2_ATTR  PIO_DEFAULT

/*! LED #3 pin detection */
#define LED3_GPIO       (PIO_PC23_IDX)
#define LED3_FLAGS      (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LED3_ACTIVE_LEVEL 1

#define BOARD_NUM_OF_LED 4
#define PIN_LED_3       {1 << 23, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_LED_3_MASK  (1 << 23)
#define PIN_LED_3_PIO   PIOC
#define PIN_LED_3_ID    ID_PIOC
#define PIN_LED_3_TYPE  PIO_OUTPUT_1
#define PIN_LED_3_ATTR  PIO_DEFAULT

/**
 * \file
 * Push buttons
 * - \ref PIN_PB_LEFT_CLICK
 * - \ref PIN_PB_RIGHT_CLICK
 * - \ref PINS_PUSHBUTTONS
 * - \ref PUSHBUTTON_BP1
 * - \ref PUSHBUTTON_BP2
 *
 */

/* ------------------------------------------------------------------------ */
/* PUSHBUTTONS                                                              */
/* ------------------------------------------------------------------------ */

/**************************changing**********************************/

/** Push button LEFT CLICK definition.
 *  Attributes = pull-up + debounce + interrupt on falling edge. */
#define PIN_PB_LEFT_CLICK    {PIO_PD8, PIOD, ID_PIOD, PIO_INPUT,\
	PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_FALL_EDGE}

/** Push button RIGHT CLICK definition.
 *  Attributes = pull-up + debounce + interrupt on falling edge. */
#define PIN_PB_RIGHT_CLICK    {PIO_PC28, PIOC, ID_PIOC, PIO_INPUT,\
	PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_FALL_EDGE}

/*! List of all push button definitions. */
#define PINS_PUSHBUTTONS    PIN_PB_LEFT_CLICK, PIN_PB_RIGHT_CLICK

/*! Push button #1 index. */
#define PUSHBUTTON_BP1   0
/*! Push button #2 index. */
#define PUSHBUTTON_BP2   1

/*! Push button LEFT CLICK index. */
#define PUSHBUTTON_LEFT     0
/*! Push button RIGHT CLICK index. */
#define PUSHBUTTON_RIGHT    1

/** Push button #0 definition.
 *  Attributes = pull-up + debounce + interrupt on rising edge. */
#define PUSHBUTTON_1_NAME    "External_PB1_on_PWM12_connector_output"

#define GPIO_PUSH_BUTTON_1           (PIO_PD8_IDX)
#define GPIO_PUSH_BUTTON_1_FLAGS\
	(PIO_INPUT | PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)

#define PIN_PUSHBUTTON_1    {PIO_PD8, PIOD, ID_PIOD, PIO_INPUT,\
	PIO_PULLUP }
#define PIN_PUSHBUTTON_1_MASK PIO_PD8
#define PIN_PUSHBUTTON_1_PIO PIOD
#define PIN_PUSHBUTTON_1_ID ID_PIOD
#define PIN_PUSHBUTTON_1_TYPE PIO_INPUT
#define PIN_PUSHBUTTON_1_ATTR (PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)

/** Push button #1 definition.
 *  Attributes = pull-up + debounce + interrupt on falling edge. */
#define PUSHBUTTON_2_NAME    "External_PB2_on_PWM3_connector_output"
#define GPIO_PUSH_BUTTON_2           (PIO_PC28_IDX)
#define GPIO_PUSH_BUTTON_2_FLAGS\
	(PIO_INPUT | PIO_PULLUP)

#define PIN_PUSHBUTTON_2    {PIO_PC28, PIOC, ID_PIOC, PIO_INPUT,\
	PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_FALL_EDGE}
#define PIN_PUSHBUTTON_2_MASK PIO_PC28
#define PIN_PUSHBUTTON_2_PIO PIOC
#define PIN_PUSHBUTTON_2_ID ID_PIOC
#define PIN_PUSHBUTTON_2_TYPE PIO_INPUT
#define PIN_PUSHBUTTON_2_ATTR (PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_FALL_EDGE)



#define PIN_TC0_TIOA1           (PIO_PA2_IDX)
#define PIN_TC0_TIOA1_FLAGS     (PIO_PERIPH_A | PIO_DEFAULT)

#define PIN_TC0_TIOA1_PIO     PIOA
#define PIN_TC0_TIOA1_MASK    PIO_PA2
#define PIN_TC0_TIOA1_ID      ID_PIOA
#define PIN_TC0_TIOA1_TYPE    PIO_PERIPH_A
#define PIN_TC0_TIOA1_ATTR    PIO_DEFAULT


#define PIN_TC0_TIOA0         (PIO_PB25_IDX)
#define PIN_TC0_TIOA0_FLAGS   (PIO_INPUT | PIO_DEFAULT)

#define PIN_TC0_TIOA0_PIO     PIOB
#define PIN_TC0_TIOA0_MASK    PIO_PB25
#define PIN_TC0_TIOA0_ID      ID_PIOB
#define PIN_TC0_TIOA0_TYPE    PIO_INPUT
#define PIN_TC0_TIOA0_ATTR    PIO_DEFAULT

/**
 * \file
 * PWMC
 * - \ref PIN_PWMC_PWMH0
 * - \ref PIN_PWMC_PWML4
 * - \ref PIN_PWMC_PWML5
 * - \ref PIN_PWMC_PWML6
 * - \ref PIN_PWM_LED0
 * - \ref PIN_PWM_LED1
 * - \ref PIN_PWM_LED2
 *
 */

/* ------------------------------------------------------------------------ */
/* PWM                                                                      */
/* ------------------------------------------------------------------------ */
/*! PWMC PWM0 TRIG pin definition: Output High. */
#define PIN_PWMC_PWMH0_TRIG   PIO_PB12_IDX
#define PIN_PWMC_PWMH0_TRIG_FLAG   PIO_PERIPH_B | PIO_DEFAULT

/*! PWMC PWM4 pin definition: Output Low. */
#define PIN_PWMC_PWML4\
        {PIO_PC21B_PWML4, PIOC, ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT}
/*! PWMC PWM5 pin definition: Output Low. */
#define PIN_PWMC_PWML5\
	{PIO_PC22B_PWML5, PIOC, ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT}
/*! PWMC PWM6 pin definition: Output High. */
#define PIN_PWMC_PWML6\
	{PIO_PC23B_PWML6, PIOC, ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT}

/*! PWM pins definition for LED0 */
#define PIN_PWM_LED0 PIN_PWMC_PWML4
/*! PWM pins definition for LED1 */
#define PIN_PWM_LED1 PIN_PWMC_PWML5
/*! PWM pins definition for LED2 */
#define PIN_PWM_LED2 PIN_PWMC_PWML6


/*! PWM channel for LED0 */
#define CHANNEL_PWM_LED0 PWM_CHANNEL_4
/*! PWM channel for LED1 */
#define CHANNEL_PWM_LED1 PWM_CHANNEL_5
/*! PWM channel for LED2 */
#define CHANNEL_PWM_LED2 PWM_CHANNEL_6

/*! PWM "PWM7" LED0 pin definitions.*/
#define PIN_PWM_LED0_GPIO    PIO_PC21_IDX
#define PIN_PWM_LED0_FLAGS   (PIO_PERIPH_B | PIO_DEFAULT)
#define PIN_PWM_LED0_CHANNEL PWM_CHANNEL_4

/*! PWM "PWM8" LED1 pin definitions.*/
#define PIN_PWM_LED1_GPIO    PIO_PC22_IDX
#define PIN_PWM_LED1_FLAGS   (PIO_PERIPH_B | PIO_DEFAULT)
#define PIN_PWM_LED1_CHANNEL PWM_CHANNEL_5

/*! PWM "PWM9" LED2 pin definitions.*/
#define PIN_PWM_LED2_GPIO    PIO_PC23_IDX
#define PIN_PWM_LED2_FLAGS   (PIO_PERIPH_B | PIO_DEFAULT)
#define PIN_PWM_LED2_CHANNEL PWM_CHANNEL_6

/**
 * \file
 * SPI
 *
 */

/* ------------------------------------------------------------------------ */
/* SPI                                                                      */
/* ------------------------------------------------------------------------ */
/*! SPI0 MISO pin definition. */
#define SPI0_MISO_GPIO        (PIO_PA25_IDX)
#define SPI0_MISO_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
/*! SPI0 MOSI pin definition. */
#define SPI0_MOSI_GPIO        (PIO_PA26_IDX)
#define SPI0_MOSI_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
/*! SPI0 SPCK pin definition. */
#define SPI0_SPCK_GPIO        (PIO_PA27_IDX)
#define SPI0_SPCK_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)

/*! SPI0 chip select 0 pin definition. (Only one configuration is possible) */
#define SPI0_NPCS0_GPIO            (PIO_PA28_IDX)
#define SPI0_NPCS0_FLAGS           (PIO_PERIPH_A | PIO_DEFAULT)
/*! SPI0 chip select 1 pin definition. (multiple configurations are possible) */
#define SPI0_NPCS1_PA29_GPIO       (PIO_PA29_IDX)
#define SPI0_NPCS1_PA29_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
#define SPI0_NPCS1_PB20_GPIO       (PIO_PB20_IDX)
#define SPI0_NPCS1_PB20_FLAGS      (PIO_PERIPH_B | PIO_DEFAULT)
/*! SPI0 chip select 2 pin definition. (multiple configurations are possible) */
#define SPI0_NPCS2_PA30_GPIO       (PIO_PA30_IDX)
#define SPI0_NPCS2_PA30_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
#define SPI0_NPCS2_PB21_GPIO       (PIO_PB21_IDX)
#define SPI0_NPCS2_PB21_FLAGS      (PIO_PERIPH_B | PIO_DEFAULT)
/*! SPI0 chip select 3 pin definition. (multiple configurations are possible) */
#define SPI0_NPCS3_PA31_GPIO       (PIO_PA31_IDX)
#define SPI0_NPCS3_PA31_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
#define SPI0_NPCS3_PB23_GPIO       (PIO_PB23_IDX)
#define SPI0_NPCS3_PB23_FLAGS      (PIO_PERIPH_B | PIO_DEFAULT)

/*! SPI1 MISO pin definition. */
#define SPI1_MISO_GPIO             (PIO_PE28_IDX)
#define SPI1_MISO_FLAGS            (PIO_PERIPH_A | PIO_DEFAULT)
/*! SPI1 MOSI pin definition. */
#define SPI1_MOSI_GPIO             (PIO_PE29_IDX)
#define SPI1_MOSI_FLAGS            (PIO_PERIPH_A | PIO_DEFAULT)
/*! SPI1 SPCK pin definition. */
#define SPI1_SPCK_GPIO             (PIO_PE30_IDX)
#define SPI1_SPCK_FLAGS            (PIO_PERIPH_A | PIO_DEFAULT)
/*! SPI1 chip select 0 pin definition. (Only one configuration is possible) */
#define SPI1_NPCS0_GPIO            (PIO_PE31_IDX)
#define SPI1_NPCS0_FLAGS           (PIO_PERIPH_A | PIO_DEFAULT)
/*! SPI1 chip select 1 pin definition. (Only one configuration is possible) */
#define SPI1_NPCS1_GPIO            (PIO_PF0_IDX)
#define SPI1_NPCS1_FLAGS           (PIO_PERIPH_A | PIO_DEFAULT)
/*! SPI1 chip select 2 pin definition. (Only one configuration is possible) */
#define SPI1_NPCS2_GPIO            (PIO_PF1_IDX)
#define SPI1_NPCS2_FLAGS           (PIO_PERIPH_A | PIO_DEFAULT)
/*! SPI1 chip select 3 pin definition. (Only one configuration is possible) */
#define SPI1_NPCS3_GPIO            (PIO_PF2_IDX)
#define SPI1_NPCS3_FLAGS           (PIO_PERIPH_A | PIO_DEFAULT)

/**
 * \file
 * SSC
 * - \ref PIN_SSC_TD
 * - \ref PIN_SSC_TK
 * - \ref PIN_SSC_TF
 * - \ref PIN_SSC_RD
 * - \ref PIN_SSC_RK
 * - \ref PIN_SSC_RF
 *
 */
/* ------------------------------------------------------------------------ */
/* SSC                                                                      */
/* ------------------------------------------------------------------------ */
/** SSC pin Transmitter Data (TD) */
#define PIN_SSC_TD        (PIO_PA16_IDX)
#define PIN_SSC_TD_FLAGS  (PIO_PERIPH_B | PIO_DEFAULT)
/** SSC pin Transmitter Clock (TK) */
#define PIN_SSC_TK        (PIO_PA14_IDX)
#define PIN_SSC_TK_FLAGS  (PIO_PERIPH_B | PIO_DEFAULT)
/** SSC pin Transmitter FrameSync (TF) */
#define PIN_SSC_TF        (PIO_PA15_IDX)
#define PIN_SSC_TF_FLAGS  (PIO_PERIPH_B | PIO_DEFAULT)
/** SSC pin Receiver Data (RD) */
#define PIN_SSC_RD        (PIO_PB18_IDX)
#define PIN_SSC_RD_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
/** SSC pin Receiver Clock (RK) */
#define PIN_SSC_RK        (PIO_PB19_IDX)
#define PIN_SSC_RK_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
/** SSC pin Receiver FrameSync (RF) */
#define PIN_SSC_RF        (PIO_PB17_IDX)
#define PIN_SSC_RF_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)

/**
 * \file
 * PCK0
 * - \ref PIN_PCK0
 *
 */

/* ------------------------------------------------------------------------ */
/* PCK                                                                      */
/* ------------------------------------------------------------------------ */
/*! PCK0 */
#define PIN_PCK0        (PIO_PA1_IDX)
#define PIN_PCK0_FLAGS  (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_PCK_0_MASK  PIO_PA1
#define PIN_PCK_0_PIO   PIOA
#define PIN_PCK_0_ID    ID_PIOA
#define PIN_PCK_0_TYPE  PIO_PERIPH_B
#define PIN_PCK_0_ATTR  PIO_DEFAULT
/**
 * \file
 * UART
 * - \ref PINS_UART
 *
 */

/* ------------------------------------------------------------------------ */
/* UART                                                                     */
/* ------------------------------------------------------------------------ */
/*! UART pins (UTXD0 and URXD0) definitions, PA8,9. (labeled RX0->0 and TX0->1)*/
#define PINS_UART        (PIO_PA8A_URXD | PIO_PA9A_UTXD)
#define PINS_UART_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)

#define PINS_UART_MASK (PIO_PA8A_URXD | PIO_PA9A_UTXD)
#define PINS_UART_PIO  PIOA
#define PINS_UART_ID   ID_PIOA
#define PINS_UART_TYPE PIO_PERIPH_A
#define PINS_UART_ATTR PIO_DEFAULT

/**
 * \file
 * USART0
 * - \ref PIN_USART0_RXD
 * - \ref PIN_USART0_TXD
 */
/* ------------------------------------------------------------------------ */
/* USART0                                                                   */
/* ------------------------------------------------------------------------ */
/*! USART0 pin RX  (labeled RX1 19)*/
#define PIN_USART0_RXD\
	{PIO_PA10A_RXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_RXD_IDX        (PIO_PA10_IDX)
#define PIN_USART0_RXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)

/*! USART0 pin TX  (labeled TX1 18) */
#define PIN_USART0_TXD\
	{PIO_PA11A_TXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_TXD_IDX        (PIO_PA11_IDX)
#define PIN_USART0_TXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)

/**
 * \file
 * USART1
 * - \ref PIN_USART1_RXD
 * - \ref PIN_USART1_TXD
 */
/* ------------------------------------------------------------------------ */
/* USART1                                                                   */
/* ------------------------------------------------------------------------ */
/*! USART1 pin RX (labeled RX2 17) */
#define PIN_USART1_RXD\
	{PIO_PA12A_RXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_RXD_IDX        (PIO_PA12_IDX)
#define PIN_USART1_RXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
/*! USART1 pin TX (labeled TX2 16) */
#define PIN_USART1_TXD\
	{PIO_PA13A_TXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_TXD_IDX        (PIO_PA13_IDX)
#define PIN_USART1_TXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
/**
 * \file
 * USART3
 * - \ref PIN_USART3_RXD
 * - \ref PIN_USART3_TXD
 */

/* ------------------------------------------------------------------------ */
/* USART3                                                                   */
/* ------------------------------------------------------------------------ */
/*! USART3 pin RX (labeled RX3 15) */
#define PIN_USART3_RXD\
	{PIO_PD5B_RXD3, PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_USART3_RXD_IDX        (PIO_PD5_IDX)
#define PIN_USART3_RXD_FLAGS      (PIO_PERIPH_B | PIO_DEFAULT)
/*! USART3 pin TX (labeled RX3 14) */
#define PIN_USART3_TXD\
	{PIO_PD4B_TXD3, PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_USART3_TXD_IDX        (PIO_PD4_IDX)
#define PIN_USART3_TXD_FLAGS      (PIO_PERIPH_B | PIO_DEFAULT)
/**
 * \file
 * USB
 * - \ref PIN_USBOTG_VBOF
 * - \ref PIN_USB_FAULT
 *
 */

/* ------------------------------------------------------------------------ */
/* USB                                                                      */
/* ------------------------------------------------------------------------ */
/*! USB OTG VBus On/Off: Bus Power Control Port. */
#define PIN_UOTGHS_VBOF  { PIO_PB10, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_PULLUP }
/*! USB OTG Identification: Mini Connector Identification Port. */
#define PIN_UOTGHS_ID    { PIO_PB11, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_PULLUP }

/*! Multiplexed pin used for USB_ID: */
#define USB_ID                      PIO_PB11_IDX
#define USB_ID_GPIO                 (PIO_PB11_IDX)
#define USB_ID_FLAGS                (PIO_PERIPH_A | PIO_DEFAULT)
/*! Multiplexed pin used for USB_VBOF: */
#define USB_VBOF                    PIO_PB10_IDX
#define USB_VBOF_GPIO               (PIO_PB10_IDX)
#define USB_VBOF_FLAGS              (PIO_PERIPH_A | PIO_DEFAULT)
/*! Active level of the USB_VBOF output pin. */
#define USB_VBOF_ACTIVE_LEVEL       LOW
/* ------------------------------------------------------------------------ */


/**
 * \file
 * CAN
 * \ref PIN_CAN0_TRANSCEIVER_RXEN
 * \ref PIN_CAN0_TRANSCEIVER_RS
 * \ref PIN_CAN0_TXD
 * \ref PIN_CAN0_RXD
 * \ref PINS_CAN0
 *
 * \ref PIN_CAN1_TRANSCEIVER_RXEN
 * \ref PIN_CAN1_TRANSCEIVER_RS
 * \ref PIN_CAN1_TXD
 * \ref PIN_CAN1_RXD
 * \ref PINS_CAN1
 */

/* ------------------------------------------------------------------------ */
/* CAN                                                                      */
/* ------------------------------------------------------------------------ */
/** CAN0 transceiver PIN RS. */
#define PIN_CAN0_TR_RS_IDX        PIO_PB20_IDX
#define PIN_CAN0_TR_RS_FLAGS      (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)

/** CAN0 transceiver PIN EN. */
#define PIN_CAN0_TR_EN_IDX        PIO_PB21_IDX
#define PIN_CAN0_TR_EN_FLAGS      (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)

/** CAN0 PIN RX. */
#define PIN_CAN0_RX_IDX           (PIO_PA1_IDX)
#define PIN_CAN0_RX_FLAGS         (PIO_PERIPH_A | PIO_DEFAULT)

/** CAN0 PIN TX. */
#define PIN_CAN0_TX_IDX           (PIO_PA0_IDX)
#define PIN_CAN0_TX_FLAGS         (PIO_PERIPH_A | PIO_DEFAULT)

/** CAN1 transceiver PIN RS. */
#define PIN_CAN1_TR_RS_IDX        PIO_PE15_IDX
#define PIN_CAN1_TR_RS_FLAGS      (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)

/** CAN1 transceiver PIN EN. */
#define PIN_CAN1_TR_EN_IDX        PIO_PE16_IDX
#define PIN_CAN1_TR_EN_FLAGS      (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)

/** CAN1 PIN RX. */
#define PIN_CAN1_RX_IDX           (PIO_PB15_IDX)
#define PIN_CAN1_RX_FLAGS         (PIO_PERIPH_A | PIO_DEFAULT)

/** CAN1 PIN TX. */
#define PIN_CAN1_TX_IDX           (PIO_PB14_IDX)
#define PIN_CAN1_TX_FLAGS         (PIO_PERIPH_A | PIO_DEFAULT)

/**

/**
 * \file
 * TWI
 */
/* ------------------------------------------------------------------------ */
/* TWI                                                                      */
/* ------------------------------------------------------------------------ */
/*! TWI0 pins definition */
#define TWI0_DATA_GPIO   PIO_PA17_IDX
#define TWI0_DATA_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
#define TWI0_CLK_GPIO    PIO_PA18_IDX
#define TWI0_CLK_FLAGS   (PIO_PERIPH_A | PIO_DEFAULT)

/*! TWI1 pins definition */
#define TWI1_DATA_GPIO   PIO_PB12_IDX
#define TWI1_DATA_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
#define TWI1_CLK_GPIO    PIO_PB13_IDX
#define TWI1_CLK_FLAGS   (PIO_PERIPH_A | PIO_DEFAULT)

/* ------------------------------------------------------------------------ */
/**
 * \file
 * NMA7341
 * - \NMA7341L_CHANNEL
 * - \PIN_NMA7341L
 */


/** Definition of MMA7341L x,y,z axis channel number */
#define MMA7341L_ADC_CHANNEL_X  2
#define MMA7341L_ADC_CHANNEL_Y  6
#define MMA7341L_ADC_CHANNEL_Z  7

/** MMA7341L mode set pin definition. */
#define PIN_MMA7341L_MODE                PIO_PC13_IDX
#define PIN_MMA7341L_MODE_FLAG       PIO_OUTPUT_1 | PIO_DEFAULT

/** MMA7341L X,Y,Z axis pin definition. */
#define PIN_MMA7341L_X_AXIS                PIO_PB3_IDX
#define PIN_MMA7341L_X_AXIS_FLAG       PIO_INPUT | PIO_DEFAULT
#define PIN_MMA7341L_Y_AXIS                PIO_PC17_IDX
#define PIN_MMA7341L_Y_AXIS_FLAG       PIO_INPUT | PIO_DEFAULT
#define PIN_MMA7341L_Z_AXIS                PIO_PC18_IDX
#define PIN_MMA7341L_Z_AXIS_FLAG       PIO_INPUT | PIO_DEFAULT
/* ------------------------------------------------------------------------ */
/**
 * \file
 * TouchScreen
 *
 * - \ref PIN_TSC_IRQ
 * - \ref PIN_TSC_BUSY
 * - \ref BOARD_TSC_SPI_BASE
 * - \ref BOARD_TSC_SPI_ID
 * - \ref BOARD_TSC_SPI_PINS
 * - \ref BOARD_TSC_NPCS
 * - \ref BOARD_TSC_NPCS_PIN
 *
 */

/* ------------------------------------------------------------------------ */
/* Touchscreen                                                              */
/* ------------------------------------------------------------------------ */
/*! Touchscreen controller IRQ pin definition. */
#define PIN_TSC_IRQ  {PIO_PA31, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP}
/*! Touchscreen controller Busy pin definition. */
#define PIN_TSC_BUSY {PIO_PA30, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP}

/*! Chip select pin connected to the touchscreen controller. */
/* We use PIO mode for chip select to meet ADS7843's timing specification */
#define BOARD_TSC_NPCS_PIN\
	{PIO_PA28A_SPI0_NPCS0, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_PULLUP}

/**
 * \file
 * EMAC
 * - BOARD_EMAC_PHY_ADDR: Phy MAC address
 * - BOARD_EMAC_MODE_RMII: Enable RMII connection with the PHY
 */
/*! EMAC pins */
#define PIN_EEMAC_EREFCK      PIO_PB0_IDX
#define PIN_EMAC_ETXEN        PIO_PB1_IDX
#define PIN_EMAC_ETX0         PIO_PB2_IDX
#define PIN_EMAC_ETX1         PIO_PB3_IDX
#define PIN_EMAC_ECRSDV       PIO_PB4_IDX
#define PIN_EMAC_ERX0         PIO_PB5_IDX
#define PIN_EMAC_ERX1         PIO_PB6_IDX
#define PIN_EMAC_ERXER        PIO_PB7_IDX
#define PIN_EMAC_EMDC         PIO_PB8_IDX
#define PIN_EMAC_EMDIO        PIO_PB9_IDX
#define PIN_EMAC_FLAGS        PIO_PERIPH_A | PIO_DEFAULT

/*! EMAC PHY address */
#define BOARD_EMAC_PHY_ADDR  0
/*! EMAC RMII mode */
#define BOARD_EMAC_MODE_RMII 1

/* ------------------------------------------------------------------------ */
/* NAND FLASH                                                               */
/* ------------------------------------------------------------------------ */
/* Chip select number for nand */
#define BOARD_NAND_CS      0

/*! Address for transferring command bytes to the nandflash. */
#define BOARD_NF_COMMAND_ADDR   0x60400000
/*! Address for transferring address bytes to the nandflash. */
#define BOARD_NF_ADDRESS_ADDR   0x60200000
/*! Address for transferring data bytes to the nandflash. */
#define BOARD_NF_DATA_ADDR      0x60000000
/* Bus width for NAND */
#define CONF_NF_BUSWIDTH    8
/* SMC NFC using five address cycle */
#define CONF_NF_NEED_FIVE_ADDRESS_CYCLES  1
/* Access timing for NAND */
#define CONF_NF_SETUP_TIMING (SMC_SETUP_NWE_SETUP(0) \
		| SMC_SETUP_NCS_WR_SETUP(0) \
		| SMC_SETUP_NRD_SETUP(0) \
		| SMC_SETUP_NCS_RD_SETUP(0))
#define CONF_NF_PULSE_TIMING (SMC_PULSE_NWE_PULSE(2) \
		| SMC_PULSE_NCS_WR_PULSE(3) \
		| SMC_PULSE_NRD_PULSE(2) \
		| SMC_PULSE_NCS_RD_PULSE(3))
#define CONF_NF_CYCLE_TIMING (SMC_CYCLE_NWE_CYCLE(3) \
		| SMC_CYCLE_NRD_CYCLE(3))
#define CONF_NF_TIMING (SMC_TIMINGS_TCLR(1) \
		| SMC_TIMINGS_TADL(6) \
		| SMC_TIMINGS_TAR(4) \
		| SMC_TIMINGS_TRR(2) \
		| SMC_TIMINGS_TWB(9) \
		| SMC_TIMINGS_RBNSEL(7) \
		| (SMC_TIMINGS_NFSEL))
/* Support DMA */
#define CONF_NF_USE_DMA
#ifdef CONF_NF_USE_DMA
/* DMA channel used for NF */
#define CONF_NF_DMA_CHANNEL    0
#endif

/* ------------------------------------------------------------------------ */
/* SDRAM                                                                    */
/* ------------------------------------------------------------------------ */
/*!  Board SDRAM size for MT48LC16M16A2 */
#define BOARD_SDRAM_SIZE        (32 * 1024 * 1024)  /* 32 MB */

/*!  List of all SDRAM pins definitions */
#define PIO_SDRAM_SDCKE   PIO_PD13
#define PIO_SDRAM_SDCS    PIO_PD12
#define PIO_SDRAM_RAS     PIO_PD15
#define PIO_SDRAM_CAS     PIO_PD16
#define PIO_SDRAM_BA0     PIO_PD6
#define PIO_SDRAM_BA1     PIO_PD7
#define PIO_SDRAM_SDWE    PIO_PD14
//#define PIO_SDRAM_NBS0    PIO_PC21
#define PIO_SDRAM_NBS1    PIO_PD10
#define PIO_SDRAM_DATA    (0xffff << 2)    /*PIO_PC2--PIO_PC17 */
//#define PIO_SDRAM_SDA0_A7 (0xff << 23)    /*PIO_PC23--PIO_PC30 */
#define PIO_SDRAM_SDA8    PIO_PD22
#define PIO_SDRAM_SDA9    PIO_PD23
#define PIO_SDRAM_SDA11   PIO_PD25
#define PIO_SDRAM_SDA12   PIO_PD4
#define PIO_SDRAM_SDA10   PIO_PD11

/*!  List of all SDRAM pins definitions */
#define PINS_SDRAM_PIOC\
	{ PIO_SDRAM_DATA | PIO_SDRAM_NBS0 | PIO_SDRAM_SDA0_A7,\
	PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP }

#define PINS_SDRAM_PIOD\
	{ PIO_SDRAM_SDCKE | PIO_SDRAM_SDCS |\
	PIO_SDRAM_RAS | PIO_SDRAM_CAS |\
	PIO_SDRAM_BA0 | PIO_SDRAM_BA1 |\
	PIO_SDRAM_SDWE | PIO_SDRAM_NBS1 |\
	PIO_SDRAM_SDA10 |\
	PIO_SDRAM_SDA8 | PIO_SDRAM_SDA9 |\
	PIO_SDRAM_SDA11 | PIO_SDRAM_SDA12,\
	PIOD, ID_PIOD, PIO_PERIPH_A, PIO_PULLUP }

/* PIO18 is used as SDRAM Enable on EK-REVB board */
#define PINS_SDRAM_EN\
	{ (1 << 18), PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT }

#define PINS_SDRAM        PINS_SDRAM_PIOC, PINS_SDRAM_PIOD, PINS_SDRAM_EN

/*!  SDRAM bus width */
#define BOARD_SDRAM_BUSWIDTH    16

/* SDRAMC clock speed */
#define SDRAMC_CLK (BOARD_MCK)

/**
 * \file
 * \section NorFlash
 * - \ref BOARD_NORFLASH_ADDR
 *
 */

/* ------------------------------------------------------------------------ */
/* NOR FLASH                                                                */
/* ------------------------------------------------------------------------ */
/*! Address for transferring command bytes to the norflash. */
#define BOARD_NORFLASH_ADDR     0x60000000

/*! TWI ID for EEPROM application to use */
#define BOARD_ID_TWI_EEPROM         ID_TWI0
/*! TWI Base for TWI EEPROM application to use */
#define BOARD_BASE_TWI_EEPROM       TWI0

/*! USART RX pin for application */
#define BOARD_PIN_USART_RXD        PIN_USART0_RXD
/*! USART TX pin for application */
#define BOARD_PIN_USART_TXD        PIN_USART0_TXD
/*! USART Base for application */
#define BOARD_USART_BASE           USART0
/*! USART ID for application */
#define BOARD_ID_USART             ID_USART0
/*! USART1 Base for application */
#define BOARD_USART1_BASE          USART1
/*! USART1 ID for application */
#define BOARD_ID_USART1            ID_USART1
/*! USART3 Base for application */
#define BOARD_USART3_BASE          USART3
/*! USART3 ID for application */
#define BOARD_ID_USART3            ID_USART3

#define CONSOLE_UART               UART
#define CONSOLE_UART_ID            ID_UART

#endif /* ARDUINO_DUE_X_H_INCLUDED */
