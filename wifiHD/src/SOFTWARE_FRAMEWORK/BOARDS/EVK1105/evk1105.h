/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief AT32UC3A EVK1105 board header file.
 *
 * This file contains definitions and services related to the features of the
 * EVK1105 board rev. B.
 *
 * To use this board, define BOARD=EVK1105.
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

#ifndef _EVK1105_H_
#define _EVK1105_H_

#ifdef EVK1105_REV3
#  include "evk1105_rev3.h"
#else

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

#define FOSC1           11289600                              //!< Osc1 frequency: Hz
#define OSC1_STARTUP    AVR32_PM_OSCCTRL1_STARTUP_2048_RCOSC  //!< Osc1 startup time: RCOsc periods.


//! @}


/*! \name SDRAM Definitions
 */
//! @{

//! Part header file of used SDRAM(s).
#define SDRAM_PART_HDR  "MT48LC16M16A2TG7E/mt48lc16m16a2tg7e.h"

//! Data bus width to use the SDRAM(s) with (16 or 32 bits; always 16 bits on
//! UC3).
#define SDRAM_DBW       16
//! @}


/*! \name USB Definitions
 */
//! @{
//! Multiplexed pin used for USB_ID: AVR32_USBB_USB_ID_x_x.
//! To be selected according to the AVR32_USBB_USB_ID_x_x_PIN and
//! AVR32_USBB_USB_ID_x_x_FUNCTION definitions from <avr32/uc3axxxx.h>.
#define AVR32_USBB_USB_ID_0_2_PIN          21
#define AVR32_USBB_USB_ID_0_2_FUNCTION     2
#define USB_ID                      AVR32_USBB_USB_ID_0_2

//! Multiplexed pin used for USB_VBOF: AVR32_USBB_USB_VBOF_x_x.
//! To be selected according to the AVR32_USBB_USB_VBOF_x_x_PIN and
//! AVR32_USBB_USB_VBOF_x_x_FUNCTION definitions from <avr32/uc3axxxx.h>.
#  define USB_VBOF                    AVR32_USBB_USB_VBOF_0_1


//! Active level of the USB_VBOF output pin.
#  define USB_VBOF_ACTIVE_LEVEL       LOW

//! USB overcurrent detection pin.
#  define USB_OVERCURRENT_DETECT_PIN  AVR32_PIN_PX15

//! @}


//! GPIO connection of the MAC PHY PWR_DOWN/INT signal.
#  define MACB_INTERRUPT_PIN  AVR32_PIN_PA26



//! Number of LEDs.
#define LED_COUNT   4

/*! \name GPIO Connections of LEDs
 */
//! @{
#  define LED0_GPIO   AVR32_PIN_PB27
#  define LED1_GPIO   AVR32_PIN_PB28
#  define LED2_GPIO   AVR32_PIN_PA05
#  define LED3_GPIO   AVR32_PIN_PA06
//! @}

/*! \name Color Identifiers of LEDs to Use with LED Functions
 */
//! @{
#define LED_MONO0_GREEN   LED0
#define LED_MONO1_GREEN   LED1
#define LED_MONO2_GREEN   LED2
#define LED_MONO3_GREEN   LED3
//! @}

/*! \name PWM Channels of LEDs
 */
//! @{
#define LED0_PWM      4
#define LED1_PWM      5
#define LED2_PWM      (-1)
#define LED3_PWM      (-1)
//! @}

/*! \name PWM Functions of LEDs
 */
//! @{
/* TODO: Implement PWM functionality */
#define LED0_PWM_FUNCTION   (-1)//AVR32_PWM_0_FUNCTION
#define LED1_PWM_FUNCTION   (-1)//AVR32_PWM_1_FUNCTION
#define LED2_PWM_FUNCTION   (-1)
#define LED3_PWM_FUNCTION   (-1)
//! @}

//! External interrupt connection of touch sensor.
#define QT1081_EIC_EXTINT_PIN       AVR32_EIC_EXTINT_1_PIN
#define QT1081_EIC_EXTINT_FUNCTION  AVR32_EIC_EXTINT_1_FUNCTION
#define QT1081_EIC_EXTINT_IRQ       AVR32_EIC_IRQ_1
#define QT1081_EIC_EXTINT_INT       AVR32_EIC_INT1
/*! \name Touch sensor low power mode select
 */
#define QT1081_LP_MODE    AVR32_PIN_PB29

/*! \name GPIO Connections of touch buttons
 */
//! @{
#define QT1081_TOUCH_SENSOR_0           AVR32_PIN_PB22
#define QT1081_TOUCH_SENSOR_0_PRESSED   1
#define QT1081_TOUCH_SENSOR_1           AVR32_PIN_PB23
#define QT1081_TOUCH_SENSOR_1_PRESSED   1
#define QT1081_TOUCH_SENSOR_2           AVR32_PIN_PB24
#define QT1081_TOUCH_SENSOR_2_PRESSED   1
#define QT1081_TOUCH_SENSOR_3           AVR32_PIN_PB25
#define QT1081_TOUCH_SENSOR_3_PRESSED   1
#define QT1081_TOUCH_SENSOR_4           AVR32_PIN_PB26
#define QT1081_TOUCH_SENSOR_4_PRESSED   1

#define QT1081_TOUCH_SENSOR_ENTER           QT1081_TOUCH_SENSOR_4
#define QT1081_TOUCH_SENSOR_ENTER_PRESSED   QT1081_TOUCH_SENSOR_4_PRESSED
#define QT1081_TOUCH_SENSOR_LEFT            QT1081_TOUCH_SENSOR_3
#define QT1081_TOUCH_SENSOR_LEFT_PRESSED    QT1081_TOUCH_SENSOR_3_PRESSED
#define QT1081_TOUCH_SENSOR_RIGHT           QT1081_TOUCH_SENSOR_2
#define QT1081_TOUCH_SENSOR_RIGHT_PRESSED   QT1081_TOUCH_SENSOR_2_PRESSED
#define QT1081_TOUCH_SENSOR_UP              QT1081_TOUCH_SENSOR_0
#define QT1081_TOUCH_SENSOR_UP_PRESSED      QT1081_TOUCH_SENSOR_0_PRESSED
#define QT1081_TOUCH_SENSOR_DOWN            QT1081_TOUCH_SENSOR_1
#define QT1081_TOUCH_SENSOR_DOWN_PRESSED    QT1081_TOUCH_SENSOR_1_PRESSED
//! @}

/*! \name SPI Connections of the AT45DBX Data Flash Memory
 */
//! @{
#define AT45DBX_SPI                 (&AVR32_SPI0)
#define AT45DBX_SPI_NPCS            0
#define AT45DBX_SPI_SCK_PIN         AVR32_SPI0_SCK_0_0_PIN
#define AT45DBX_SPI_SCK_FUNCTION    AVR32_SPI0_SCK_0_0_FUNCTION
#define AT45DBX_SPI_MISO_PIN        AVR32_SPI0_MISO_0_0_PIN
#define AT45DBX_SPI_MISO_FUNCTION   AVR32_SPI0_MISO_0_0_FUNCTION
#define AT45DBX_SPI_MOSI_PIN        AVR32_SPI0_MOSI_0_0_PIN
#define AT45DBX_SPI_MOSI_FUNCTION   AVR32_SPI0_MOSI_0_0_FUNCTION
#define AT45DBX_SPI_NPCS0_PIN       AVR32_SPI0_NPCS_0_0_PIN
#define AT45DBX_SPI_NPCS0_FUNCTION  AVR32_SPI0_NPCS_0_0_FUNCTION
//! @}

/*! \name GPIO and SPI Connections of the SD/MMC Connector
 */
//! @{
#define SD_MMC_CARD_DETECT_PIN      AVR32_PIN_PA02
#define SD_MMC_WRITE_PROTECT_PIN    AVR32_PIN_PA18
#define SD_MMC_SPI                  (&AVR32_SPI0)
#define SD_MMC_SPI_NPCS             1
#define SD_MMC_SPI_SCK_PIN          AVR32_SPI0_SCK_0_0_PIN
#define SD_MMC_SPI_SCK_FUNCTION     AVR32_SPI0_SCK_0_0_FUNCTION
#define SD_MMC_SPI_MISO_PIN         AVR32_SPI0_MISO_0_0_PIN
#define SD_MMC_SPI_MISO_FUNCTION    AVR32_SPI0_MISO_0_0_FUNCTION
#define SD_MMC_SPI_MOSI_PIN         AVR32_SPI0_MOSI_0_0_PIN
#define SD_MMC_SPI_MOSI_FUNCTION    AVR32_SPI0_MOSI_0_0_FUNCTION
#define SD_MMC_SPI_NPCS_PIN         AVR32_SPI0_NPCS_1_0_PIN
#define SD_MMC_SPI_NPCS_FUNCTION    AVR32_SPI0_NPCS_1_0_FUNCTION
//! @}


/*! \name TWI expansion
 */
//! @{
#define EXPANSION_TWI                   (&AVR32_TWI)
#define EXPANSION_RESET                 AVR32_PIN_PX16
#define EXPANSION_TWI_SCL_PIN           AVR32_TWI_SCL_0_0_PIN
#define EXPANSION_TWI_SCL_FUNCTION      AVR32_TWI_SCL_0_0_FUNCTION
#define EXPANSION_TWI_SDA_PIN           AVR32_TWI_SDA_0_0_PIN
#define EXPANSION_TWI_SDA_FUNCTION      AVR32_TWI_SDA_0_0_FUNCTION
//! @}

/*! \name Wireless expansion
 */

#define WEXPANSION_EXTINT_PIN           AVR32_EIC_EXTINT_8_PIN
#define WEXPANSION_EXTINT_FUNCTION      AVR32_EIC_EXTINT_8_FUNCTION
#define WEXPANSION_GPIO1                AVR32_PIN_PB30
#define WEXPANSION_GPIO2                AVR32_PIN_PB31

#define WEXPANSION_SPI                  (&AVR32_SPI0)
#define WEXPANSION_SPI_NPCS             2
#define WEXPANSION_SPI_SCK_PIN          AVR32_SPI0_SCK_0_0_PIN
#define WEXPANSION_SPI_SCK_FUNCTION     AVR32_SPI0_SCK_0_0_FUNCTION
#define WEXPANSION_SPI_MISO_PIN         AVR32_SPI0_MISO_0_0_PIN
#define WEXPANSION_SPI_MISO_FUNCTION    AVR32_SPI0_MISO_0_0_FUNCTION
#define WEXPANSION_SPI_MOSI_PIN         AVR32_SPI0_MOSI_0_0_PIN
#define WEXPANSION_SPI_MOSI_FUNCTION    AVR32_SPI0_MOSI_0_0_FUNCTION
#define WEXPANSION_SPI_NPCS_PIN         AVR32_SPI0_NPCS_2_0_PIN
#define WEXPANSION_SPI_NPCS_FUNCTION    AVR32_SPI0_NPCS_2_0_FUNCTION

//! @}

/*! \name ET024006DHU TFT display
 */
//! @{

#define ET024006DHU_TE_PIN              AVR32_PIN_PX19
#define ET024006DHU_RESET_PIN           AVR32_PIN_PX22
#define ET024006DHU_BL_PIN              AVR32_PWM_6_PIN
#define ET024006DHU_BL_FUNCTION         AVR32_PWM_6_FUNCTION
#define ET024006DHU_DNC_PIN             AVR32_EBI_ADDR_21_1_PIN
#define ET024006DHU_DNC_FUNCTION        AVR32_EBI_ADDR_21_1_FUNCTION
#define ET024006DHU_EBI_NCS_PIN         AVR32_EBI_NCS_0_1_PIN
#define ET024006DHU_EBI_NCS_FUNCTION    AVR32_EBI_NCS_0_1_FUNCTION

//! @}
/*! \name Optional SPI connection to the TFT
 */
//! @{

#define ET024006DHU_SPI                  (&AVR32_SPI0)
#define ET024006DHU_SPI_NPCS             3
#define ET024006DHU_SPI_SCK_PIN          AVR32_SPI0_SCK_0_0_PIN
#define ET024006DHU_SPI_SCK_FUNCTION     AVR32_SPI0_SCK_0_0_FUNCTION
#define ET024006DHU_SPI_MISO_PIN         AVR32_SPI0_MISO_0_0_PIN
#define ET024006DHU_SPI_MISO_FUNCTION    AVR32_SPI0_MISO_0_0_FUNCTION
#define ET024006DHU_SPI_MOSI_PIN         AVR32_SPI0_MOSI_0_0_PIN
#define ET024006DHU_SPI_MOSI_FUNCTION    AVR32_SPI0_MOSI_0_0_FUNCTION
#define ET024006DHU_SPI_NPCS_PIN         AVR32_SPI1_NPCS_3_0_PIN
#define ET024006DHU_SPI_NPCS_FUNCTION    AVR32_SPI1_NPCS_3_0_FUNCTION

//! @}


/*! \name Audio amplifier connection to the DAC
 */
//! @{

#define TPA6130_ABDAC             (&AVR32_ABDAC)

#define TPA6130_DATA0_PIN         AVR32_ABDAC_DATA_0_1_PIN
#define TPA6130_DATA0_FUNCTION    AVR32_ABDAC_DATA_0_1_FUNCTION
#define TPA6130_DATAN0_PIN        AVR32_ABDAC_DATAN_0_1_PIN
#define TPA6130_DATAN0_FUNCTION   AVR32_ABDAC_DATAN_0_1_FUNCTION
#define TPA6130_DATA1_PIN         AVR32_ABDAC_DATA_1_1_PIN
#define TPA6130_DATA1_FUNCTION    AVR32_ABDAC_DATA_1_1_FUNCTION
#define TPA6130_DATAN1_PIN        AVR32_ABDAC_DATAN_1_1_PIN
#define TPA6130_DATAN1_FUNCTION   AVR32_ABDAC_DATAN_1_1_FUNCTION

#define TPA6130_ABDAC_PDCA_PID       AVR32_PDCA_PID_ABDAC_TX
#define TPA6130_ABDAC_PDCA_CHANNEL   0
#define TPA6130_ABDAC_PDCA_IRQ       AVR32_PDCA_IRQ_0
#define TPA6130_ABDAC_PDCA_INT_LEVEL AVR32_INTC_INT3

#define TPA6130_TWI                  (&AVR32_TWI)
#define TPA6130_TWI_SCL_PIN          AVR32_TWI_SCL_0_0_PIN
#define TPA6130_TWI_SCL_FUNCTION     AVR32_TWI_SCL_0_0_FUNCTION
#define TPA6130_TWI_SDA_PIN          AVR32_TWI_SDA_0_0_PIN
#define TPA6130_TWI_SDA_FUNCTION     AVR32_TWI_SDA_0_0_FUNCTION

//! }@
/*! \name TI TLV320AIC23B sound chip
 */
//! @{
#define TLV320_SSC                           (&AVR32_SSC)
#define TLV320_SSC_TX_CLOCK_PIN              AVR32_SSC_TX_CLOCK_0_PIN
#define TLV320_SSC_TX_CLOCK_FUNCTION         AVR32_SSC_TX_CLOCK_0_FUNCTION
#define TLV320_SSC_TX_DATA_PIN               AVR32_SSC_TX_DATA_0_PIN
#define TLV320_SSC_TX_DATA_FUNCTION          AVR32_SSC_TX_DATA_0_FUNCTION
#define TLV320_SSC_TX_FRAME_SYNC_PIN         AVR32_SSC_TX_FRAME_SYNC_0_PIN
#define TLV320_SSC_TX_FRAME_SYNC_FUNCTION    AVR32_SSC_TX_FRAME_SYNC_0_FUNCTION

#define TLV320_TWI                   (&AVR32_TWI)
#define TLV320_TWI_SCL_PIN           AVR32_TWI_SCL_0_0_PIN
#define TLV320_TWI_SCL_FUNCTION      AVR32_TWI_SCL_0_0_FUNCTION
#define TLV320_TWI_SDA_PIN           AVR32_TWI_SDA_0_0_PIN
#define TLV320_TWI_SDA_FUNCTION      AVR32_TWI_SDA_0_0_FUNCTION

#define TLV320_PM_GCLK_PIN         AVR32_PM_GCLK_0_0_PIN
#define TLV320_PM_GCLK_FUNCTION    AVR32_PM_GCLK_0_0_FUNCTION
//! @}

////! \name SPI: Apple Authentication Chip Hardware Connections
////! @{
#define IPOD_AUTH_CHIP_SPI                  (&AVR32_SPI0)
#define IPOD_AUTH_CHIP_SPI_IRQ              AVR32_SPI0_IRQ
#define IPOD_AUTH_CHIP_SPI_NPCS             2
#define IPOD_AUTH_CHIP_SPI_SCK_PIN          AVR32_SPI0_SCK_0_0_PIN
#define IPOD_AUTH_CHIP_SPI_SCK_FUNCTION     AVR32_SPI0_SCK_0_0_FUNCTION
#define IPOD_AUTH_CHIP_SPI_MISO_PIN         AVR32_SPI0_MISO_0_0_PIN
#define IPOD_AUTH_CHIP_SPI_MISO_FUNCTION    AVR32_SPI0_MISO_0_0_FUNCTION
#define IPOD_AUTH_CHIP_SPI_MOSI_PIN         AVR32_SPI0_MOSI_0_0_PIN
#define IPOD_AUTH_CHIP_SPI_MOSI_FUNCTION    AVR32_SPI0_MOSI_0_0_FUNCTION
#define IPOD_AUTH_CHIP_SPI_NPCS_PIN         AVR32_SPI0_NPCS_2_0_PIN
#define IPOD_AUTH_CHIP_SPI_NPCS_FUNCTION    AVR32_SPI0_NPCS_2_0_FUNCTION
#define IPOD_AUTH_CHIP_SPI_N_RESET_PIN      AVR32_PIN_PB30
#define IPOD_AUTH_CHIP_SPI_CP_READY_PIN     AVR32_PIN_PB31
//! }@

/*! \name Connections of the iPOD Authentication Coprocessor
 */
//! @{
        
#define IPOD_AUTH_CHIP_TWI                  (&AVR32_TWI)
#define IPOD_AUTH_CHIP_TWI_SCL_PIN          AVR32_TWI_SCL_0_0_PIN
#define IPOD_AUTH_CHIP_TWI_SCL_FUNCTION     AVR32_TWI_SCL_0_0_FUNCTION
#define IPOD_AUTH_CHIP_TWI_SDA_PIN          AVR32_TWI_SDA_0_0_PIN
#define IPOD_AUTH_CHIP_TWI_SDA_FUNCTION     AVR32_TWI_SDA_0_0_FUNCTION
#define IPOD_AUTH_CHIP_TWI_N_RESET_PIN      AVR32_PIN_PX16

//! @}

/*! \name USART connection to the UC3B board controller
 */
//! @{

#define USART0_RXD_PIN          AVR32_USART0_RXD_0_0_PIN
#define USART0_RXD_FUNCTION     AVR32_USART0_RXD_0_0_FUNCTION
#define USART0_TXD_PIN          AVR32_USART0_TXD_0_0_PIN
#define USART0_TXD_FUNCTION     AVR32_USART0_TXD_0_0_FUNCTION
#define USART0_RTS_PIN          AVR32_USART0_RTS_0_0_PIN
#define USART0_RTS_FUNCTION     AVR32_USART0_RTS_0_0_FUNCTION
#define USART0_CTS_PIN          AVR32_USART0_CTS_0_0_PIN
#define USART0_CTS_FUNCTION     AVR32_USART0_CTS_0_0_FUNCTION

//! @}

#define ADC_VEXT_PIN         AVR32_ADC_AD_7_PIN
#define ADC_VEXT_FUNCTION    AVR32_ADC_AD_7_FUNCTION

/*! \name LCD Connections of the ET024006DHU display
 */
//! @{
#define ET024006DHU_SMC_USE_NCS           0
#define ET024006DHU_SMC_COMPONENT_CS      "smc_et024006dhu.h"

#define ET024006DHU_EBI_DATA_0    AVR32_EBI_DATA_0
#define ET024006DHU_EBI_DATA_1    AVR32_EBI_DATA_1
#define ET024006DHU_EBI_DATA_2    AVR32_EBI_DATA_2
#define ET024006DHU_EBI_DATA_3    AVR32_EBI_DATA_3
#define ET024006DHU_EBI_DATA_4    AVR32_EBI_DATA_4
#define ET024006DHU_EBI_DATA_5    AVR32_EBI_DATA_5
#define ET024006DHU_EBI_DATA_6    AVR32_EBI_DATA_6
#define ET024006DHU_EBI_DATA_7    AVR32_EBI_DATA_7
#define ET024006DHU_EBI_DATA_8    AVR32_EBI_DATA_8
#define ET024006DHU_EBI_DATA_9    AVR32_EBI_DATA_9
#define ET024006DHU_EBI_DATA_10   AVR32_EBI_DATA_10
#define ET024006DHU_EBI_DATA_11   AVR32_EBI_DATA_11
#define ET024006DHU_EBI_DATA_12   AVR32_EBI_DATA_12
#define ET024006DHU_EBI_DATA_13   AVR32_EBI_DATA_13
#define ET024006DHU_EBI_DATA_14   AVR32_EBI_DATA_14
#define ET024006DHU_EBI_DATA_15   AVR32_EBI_DATA_15

#define ET024006DHU_EBI_ADDR_21   AVR32_EBI_ADDR_21_1

#define ET024006DHU_EBI_NWE       AVR32_EBI_NWE0_0
#define ET024006DHU_EBI_NRD       AVR32_EBI_NRD_0
#define ET024006DHU_EBI_NCS       AVR32_EBI_NCS_0_1
//! @}


#endif  // !EVK1105_REVA

#endif  // _EVK1105_H_
