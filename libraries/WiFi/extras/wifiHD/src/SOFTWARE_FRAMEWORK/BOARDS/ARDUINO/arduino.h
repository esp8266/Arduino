/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief AT32UC3A EVK1100 board header file.
 *
 * This file contains definitions and services related to the features of the
 * EVK1100 board rev. B and C.
 *
 * To use this board, define BOARD=EVK1100.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 AT32UC3A devices can be used.
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

#ifndef _ARDUINO_H_
#define _ARDUINO_H_

#include "compiler.h"

#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.
#  include "led.h"
#endif  // __AVR32_ABI_COMPILER__


/*! \name Oscillator Definitions
 */
//! @{

// RCOsc has no custom calibration by default. Set the following definition to
// the appropriate value if a custom RCOsc calibration has been applied to your
// part.
//#define FRCOSC          AVR32_PM_RCOSC_FREQUENCY              //!< RCOsc frequency: Hz.

#define FOSC32          32768                                 //!< Osc32 frequency: Hz.
#define OSC32_STARTUP   AVR32_PM_OSCCTRL32_STARTUP_8192_RCOSC //!< Osc32 startup time: RCOsc periods.

#define FOSC0           12000000                              //!< Osc0 frequency: Hz.
#define OSC0_STARTUP    AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC  //!< Osc0 startup time: RCOsc periods.

// Osc1 crystal is not mounted by default. Set the following definitions to the
// appropriate values if a custom Osc1 crystal is mounted on your board.
//#define FOSC1           12000000                              //!< Osc1 frequency: Hz.
//#define OSC1_STARTUP    AVR32_PM_OSCCTRL1_STARTUP_2048_RCOSC  //!< Osc1 startup time: RCOsc periods.

//! @}


//! Number of LEDs.
#define LED_COUNT   0

/*! \name GPIO Connections of LEDs
 */
//! @{
#define LED0_GPIO   AVR32_PIN_PB19
#define LED1_GPIO   AVR32_PIN_PB20
#define LED2_GPIO   AVR32_PIN_PB21
#define DEB_PIN_GPIO AVR32_PIN_PA20
#define DEB2_PIN_GPIO AVR32_PIN_PB00
//! @}

/*! \name PWM Channels of LEDs
 */
//! @{
#define LED0_PWM      0
#define LED1_PWM      1
#define LED2_PWM      2
//! @}

/*! \name PWM Functions of LEDs
 */
//! @{
#define LED0_PWM_FUNCTION   AVR32_PWM_0_FUNCTION
#define LED1_PWM_FUNCTION   AVR32_PWM_1_FUNCTION
#define LED2_PWM_FUNCTION   AVR32_PWM_2_FUNCTION
//! @}

/*! \name Color Identifiers of LEDs to Use with LED Functions
 */
//! @{
#define LED_MONO0_GREEN   LED0
#define LED_MONO1_RED   	LED1
#define LED_MONO2_BLU   	LED2
//! @}

#if 0
/*! \name SPI Connections of the DIP204 LCD
 */
//! @{
#define DIP204_SPI                  (&AVR32_SPI1)
#define DIP204_SPI_NPCS             2
#define DIP204_SPI_SCK_PIN          AVR32_SPI1_SCK_0_0_PIN
#define DIP204_SPI_SCK_FUNCTION     AVR32_SPI1_SCK_0_0_FUNCTION
#define DIP204_SPI_MISO_PIN         AVR32_SPI1_MISO_0_0_PIN
#define DIP204_SPI_MISO_FUNCTION    AVR32_SPI1_MISO_0_0_FUNCTION
#define DIP204_SPI_MOSI_PIN         AVR32_SPI1_MOSI_0_0_PIN
#define DIP204_SPI_MOSI_FUNCTION    AVR32_SPI1_MOSI_0_0_FUNCTION
#define DIP204_SPI_NPCS_PIN         AVR32_SPI1_NPCS_2_0_PIN
#define DIP204_SPI_NPCS_FUNCTION    AVR32_SPI1_NPCS_2_0_FUNCTION
//! @}

/*! \name GPIO and PWM Connections of the DIP204 LCD Backlight
 */
//! @{
#define DIP204_BACKLIGHT_PIN        AVR32_PIN_PB18
#define DIP204_PWM_CHANNEL          6
#define DIP204_PWM_PIN              AVR32_PWM_6_PIN
#define DIP204_PWM_FUNCTION         AVR32_PWM_6_FUNCTION
//! @}
#endif

/*! \name SPI Connections of the AT45DBX Data Flash Memory
 */
//! @{
#define AT45DBX_SPI                 (&AVR32_SPI1)
#define AT45DBX_SPI_NPCS            2
#define AT45DBX_SPI_SCK_PIN         AVR32_SPI1_SCK_0_0_PIN
#define AT45DBX_SPI_SCK_FUNCTION    AVR32_SPI1_SCK_0_0_FUNCTION
#define AT45DBX_SPI_MISO_PIN        AVR32_SPI1_MISO_0_0_PIN
#define AT45DBX_SPI_MISO_FUNCTION   AVR32_SPI1_MISO_0_0_FUNCTION
#define AT45DBX_SPI_MOSI_PIN        AVR32_SPI1_MOSI_0_0_PIN
#define AT45DBX_SPI_MOSI_FUNCTION   AVR32_SPI1_MOSI_0_0_FUNCTION
#define AT45DBX_SPI_NPCS2_PIN       AVR32_SPI1_NPCS_2_0_PIN
#define AT45DBX_SPI_NPCS2_FUNCTION  AVR32_SPI1_NPCS_2_0_FUNCTION
#define AT45DBX_CHIP_RESET  		AVR32_PIN_PA02
//! @}


/*! \name GPIO and SPI Connections of the SD/MMC Connector
 */
//! @{
//#define SD_MMC_CARD_DETECT_PIN      AVR32_PIN_PA02
//#define SD_MMC_WRITE_PROTECT_PIN    AVR32_PIN_PA07
#define SD_MMC_SPI                  (&AVR32_SPI1)
#define SD_MMC_SPI_NPCS             1
#define SD_MMC_SPI_SCK_PIN          AVR32_SPI1_SCK_0_0_PIN
#define SD_MMC_SPI_SCK_FUNCTION     AVR32_SPI1_SCK_0_0_FUNCTION
#define SD_MMC_SPI_MISO_PIN         AVR32_SPI1_MISO_0_0_PIN
#define SD_MMC_SPI_MISO_FUNCTION    AVR32_SPI1_MISO_0_0_FUNCTION
#define SD_MMC_SPI_MOSI_PIN         AVR32_SPI1_MOSI_0_0_PIN
#define SD_MMC_SPI_MOSI_FUNCTION    AVR32_SPI1_MOSI_0_0_FUNCTION
#define SD_MMC_SPI_NPCS_PIN         AVR32_SPI1_NPCS_1_0_PIN
#define SD_MMC_SPI_NPCS_FUNCTION    AVR32_SPI1_NPCS_1_0_FUNCTION
//! @}

/*	Timer Counter to generate clock for WiFi chip*/
#  define WIFI_TC                    (&AVR32_TC)
#  define WIFI_TC_CHANNEL_ID         0
#  define WIFI_TC_CHANNEL_PIN        AVR32_TC_A0_0_0_PIN
#  define WIFI_TC_CHANNEL_FUNCTION   AVR32_TC_A0_0_0_FUNCTION
// Note that TC_A0_0_0 pin is pin 6 (PB23) on AT32UC3A1512 QFP100.

/* Pin related to WiFi chip communication */
#ifndef USE_POLL
 #define USE_POLL
#endif
 #define SPI_CS 					0
 #define AVR32_SPI 					AVR32_SPI1
 #define GPIO_IRQ_PIN 				AVR32_PIN_PA03
 #define GPIO_IRQ 					AVR32_GPIO_IRQ_7
 #define GPIO_W_RESET_PIN 			AVR32_PIN_PA07
 #define GPIO_W_SHUTDOWN_PIN 		AVR32_PIN_PA09

/* Pin related to shield communication */
 #define ARDUINO_HANDSHAKE_PIN 		AVR32_PIN_PA25
 #define ARDUINO_EXTINT_PIN 		AVR32_PIN_PA04		//not used

 #define AVR32_PDCA_PID_TX 			AVR32_PDCA_PID_SPI1_TX
 #define AVR32_PDCA_PID_RX 			AVR32_PDCA_PID_SPI1_RX


#if 0
/*! \name TWI Connections of the Spare TWI Connector
 */
//! @{
#define SPARE_TWI                   (&AVR32_TWI)
#define SPARE_TWI_SCL_PIN           AVR32_TWI_SCL_0_0_PIN
#define SPARE_TWI_SCL_FUNCTION      AVR32_TWI_SCL_0_0_FUNCTION
#define SPARE_TWI_SDA_PIN           AVR32_TWI_SDA_0_0_PIN
#define SPARE_TWI_SDA_FUNCTION      AVR32_TWI_SDA_0_0_FUNCTION
//! @}


/*! \name SPI Connections of the Spare SPI Connector
 */
//! @{
#define SPARE_SPI                   (&AVR32_SPI0)
#define SPARE_SPI_NPCS              0
#define SPARE_SPI_SCK_PIN           AVR32_SPI0_SCK_0_0_PIN
#define SPARE_SPI_SCK_FUNCTION      AVR32_SPI0_SCK_0_0_FUNCTION
#define SPARE_SPI_MISO_PIN          AVR32_SPI0_MISO_0_0_PIN
#define SPARE_SPI_MISO_FUNCTION     AVR32_SPI0_MISO_0_0_FUNCTION
#define SPARE_SPI_MOSI_PIN          AVR32_SPI0_MOSI_0_0_PIN
#define SPARE_SPI_MOSI_FUNCTION     AVR32_SPI0_MOSI_0_0_FUNCTION
#define SPARE_SPI_NPCS_PIN          AVR32_SPI0_NPCS_0_0_PIN
#define SPARE_SPI_NPCS_FUNCTION     AVR32_SPI0_NPCS_0_0_FUNCTION
//! @}
#endif

#endif  // _ARDUINO_H_
