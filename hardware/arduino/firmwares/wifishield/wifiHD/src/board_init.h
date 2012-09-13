/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef STARTUP_H
#define STARTUP_H

#include <board.h>
#include <wl_api.h> /* defines SPB104, SPB105 */

void board_init(void);



/*
 *
 * EVK1100
 * ---------------------------------------------------------------------------
 *
 */
#if BOARD == EVK1100 

/* USART0 physical assignment */
#define BOARD_RS232_0 AVR32_USART1
#define BOARD_RS232_0_TX                                                 \
        { AVR32_USART1_TXD_0_0_PIN, AVR32_USART1_TXD_0_0_FUNCTION }
#define BOARD_RS232_0_RX                                                 \
        { AVR32_USART1_RXD_0_0_PIN, AVR32_USART1_RXD_0_0_FUNCTION }

/* USART1 physical assignment */
#define BOARD_RS232_1 AVR32_USART0
#define BOARD_RS232_1_TX                                                 \
        { AVR32_USART0_TXD_0_0_PIN, AVR32_USART0_TXD_0_0_FUNCTION }
#define BOARD_RS232_1_RX                                                 \
        { AVR32_USART0_RXD_0_0_PIN, AVR32_USART0_RXD_0_0_FUNCTION },

/* Clocks */
#define USE_PLL
#define OSC     0
#define PLL_MUL 10

/* Wifi (SPB104 only) */
#if defined(EXT_BOARD)
#if EXT_BOARD == SPB104
#define WL_SPI AVR32_SPI1 /* Pin 8 NC, so no irq support if using SD-slot */
#define WL_SPI_CLOCK_DIVIDER 3  /* due to adapter */
#define WL_PDCA_PID_TX AVR32_PDCA_PID_SPI1_TX
#define WL_PDCA_PID_RX AVR32_PDCA_PID_SPI1_RX
#define WL_SPI_CS 1
#define WL_SPI_NPCS0 { AVR32_SPI1_NPCS_0_0_PIN, AVR32_SPI1_NPCS_0_0_FUNCTION }
#define WL_SPI_NPCS { AVR32_SPI1_NPCS_1_0_PIN, AVR32_SPI1_NPCS_1_0_FUNCTION }
#define WL_SPI_MISO { AVR32_SPI1_MISO_0_0_PIN, AVR32_SPI1_MISO_0_0_FUNCTION }
#define WL_SPI_MOSI { AVR32_SPI1_MOSI_0_0_PIN, AVR32_SPI1_MOSI_0_0_FUNCTION }
#define WL_SPI_SCK  { AVR32_SPI1_SCK_0_0_PIN, AVR32_SPI1_SCK_0_0_FUNCTION }
#endif
#endif /* EXT_BOARD */






/*
 *
 * EVK1101
 * ---------------------------------------------------------------------------
 *
 */
#elif BOARD == EVK1101

/* USART0 physical assignment */
#define BOARD_RS232_0 AVR32_USART1
#define BOARD_RS232_0_TX                                                 \
        { AVR32_USART1_TXD_0_0_PIN, AVR32_USART1_TXD_0_0_FUNCTION }
#define BOARD_RS232_0_RX                                                 \
        { AVR32_USART1_RXD_0_0_PIN, AVR32_USART1_RXD_0_0_FUNCTION }

/* Clocks */
#define USE_PLL
#define OSC     0
#define PLL_MUL 9

/* Wifi (SPB104 only) */
#if defined(EXT_BOARD)
#if EXT_BOARD == SPB104 /* Pin 8 NC, so no irq support if using SD-slot */
#define WL_SPI AVR32_SPI
#define WL_SPI_CLOCK_DIVIDER 3  /* due to adapter */
#define WL_PDCA_PID_TX AVR32_PDCA_PID_SPI_TX
#define WL_PDCA_PID_RX AVR32_PDCA_PID_SPI_RX
#define WL_SPI_CS 1
#if EXT_BOARD == SPB105
 #define WL_SHUTDOWN_PIN AVR32_PIN_PA06
 #define WL_POWER_PIN AVR32_PIN_PA30
#endif
#define WL_SPI_NPCS0 { AVR32_SPI_NPCS_0_0_PIN, AVR32_SPI_NPCS_0_0_FUNCTION }
#define WL_SPI_NPCS { AVR32_SPI_NPCS_1_0_PIN, AVR32_SPI_NPCS_1_0_FUNCTION }
#define WL_SPI_MISO { AVR32_SPI_MISO_0_0_PIN, AVR32_SPI_MISO_0_0_FUNCTION }
#define WL_SPI_MOSI { AVR32_SPI_MOSI_0_0_PIN, AVR32_SPI_MOSI_0_0_FUNCTION }
#define WL_SPI_SCK { AVR32_SPI_SCK_0_0_PIN, AVR32_SPI_SCK_0_0_FUNCTION }
#endif
#endif /* EXT_BOARD */







/*
 *
 * EVK1104
 * ---------------------------------------------------------------------------
 *
 */
#elif BOARD == EVK1104 /* EVK1104 */

/* USART0 physical assignment */
#define BOARD_RS232_0 AVR32_USART1
#define BOARD_RS232_0_TX                                                 \
        { AVR32_USART1_TXD_0_0_PIN, AVR32_USART1_TXD_0_0_FUNCTION }
#define BOARD_RS232_0_RX                                                 \
        { AVR32_USART1_RXD_0_0_PIN, AVR32_USART1_RXD_0_0_FUNCTION }

/* Clocks */
#define USE_PLL
#define OSC     0
#define PLL_MUL 9 /* for some reason we cant use 66 MHz */

/* Wifi (SDIO: SPB104 only; SPI: SPB105 only) */
#if defined(EXT_BOARD)
#if EXT_BOARD == SPB105
 #define WL_SPI AVR32_SPI0
 #define WL_PDCA_PID_TX AVR32_PDCA_PID_SPI0_TX
 #define WL_PDCA_PID_RX AVR32_PDCA_PID_SPI0_RX
 #define WL_SPI_CLOCK_DIVIDER 3 /* due to adapter */
 #define WL_SHUTDOWN_PIN AVR32_PIN_PA17 /* Pin 8 on RF-head -> Pin 4 on wifi */
 #define WL_IRQ_PIN AVR32_PIN_PA18 /* Pin 6 on RF-head -> Pin 3 on wifi */
 #define WL_IRQ AVR32_GPIO_IRQ_2
 #define WL_SPI_CS 3
 #define WL_SPI_NPCS { AVR32_SPI0_NPCS_3_1_PIN, AVR32_SPI0_NPCS_3_1_FUNCTION }
 #define WL_SPI_MISO { AVR32_SPI0_MISO_0_0_PIN, AVR32_SPI0_MISO_0_0_FUNCTION }
 #define WL_SPI_MOSI { AVR32_SPI0_MOSI_0_0_PIN, AVR32_SPI0_MOSI_0_0_FUNCTION }
 #define WL_SPI_SCK { AVR32_SPI0_SCK_0_0_PIN, AVR32_SPI0_SCK_0_0_FUNCTION }
#elif EXT_BOARD == SPB104
 #ifdef SDIO_SLOT_A
  #define WL_SDIO_CLK { AVR32_MCI_CLK_0_PIN, AVR32_MCI_CLK_0_FUNCTION }
  #define WL_SDIO_CMD { AVR32_MCI_CMD_0_PIN, AVR32_MCI_CMD_0_FUNCTION }
  #define WL_SDIO_DAT0 { AVR32_MCI_DATA_0_PIN, AVR32_MCI_DATA_0_FUNCTION }
  #define WL_SDIO_DAT1 { AVR32_MCI_DATA_1_PIN, AVR32_MCI_DATA_1_FUNCTION }
  #define WL_SDIO_DAT2 { AVR32_MCI_DATA_2_PIN, AVR32_MCI_DATA_2_FUNCTION }
  #define WL_SDIO_DAT3 { AVR32_MCI_DATA_3_PIN, AVR32_MCI_DATA_3_FUNCTION }
 #else
  #define WL_SDIO_CLK { AVR32_MCI_CLK_0_PIN, AVR32_MCI_CLK_0_FUNCTION }
  #define WL_SDIO_CMD { AVR32_MCI_CMD_1_0_PIN, AVR32_MCI_CMD_1_0_FUNCTION }
  #define WL_SDIO_DAT0 { AVR32_MCI_DATA_8_0_PIN, AVR32_MCI_DATA_8_0_FUNCTION }
  #define WL_SDIO_DAT1 { AVR32_MCI_DATA_9_0_PIN, AVR32_MCI_DATA_9_0_FUNCTION }
  #define WL_SDIO_DAT2 { AVR32_MCI_DATA_10_0_PIN, AVR32_MCI_DATA_10_0_FUNCTION }
  #define WL_SDIO_DAT3 { AVR32_MCI_DATA_11_0_PIN, AVR32_MCI_DATA_11_0_FUNCTION }
 #endif
#endif
#endif /* EXT_BOARD */







/*
 *
 * EVK1105
 * ---------------------------------------------------------------------------
 *
 */
#elif BOARD == EVK1105 /* EVK1105 */


/* USART0 physical assignment */
#define BOARD_RS232_0 AVR32_USART0
#define BOARD_RS232_0_TX                                                 \
        { AVR32_USART0_TXD_0_0_PIN, AVR32_USART0_TXD_0_0_FUNCTION }
#define BOARD_RS232_0_RX                                                 \
        { AVR32_USART0_RXD_0_0_PIN, AVR32_USART0_RXD_0_0_FUNCTION }

/* Clocks */
#define USE_PLL
#define OSC     1
#define PLL_MUL 10

/* Wifi SPB104/SPB105 */
#if defined(EXT_BOARD)
 #define WL_SPI AVR32_SPI0
 #define WL_PDCA_PID_TX AVR32_PDCA_PID_SPI0_TX
 #define WL_PDCA_PID_RX AVR32_PDCA_PID_SPI0_RX
 #if EXT_BOARD == SPB105
  #define WL_SPI_CLOCK_DIVIDER 3  /* due to adapter */
  #define WL_SHUTDOWN_PIN AVR32_PIN_PB31 /* Pin 8 on RF-head -> Pin 4 on wifi */
  #define WL_IRQ_PIN AVR32_PIN_PB30 /* Pin 6 on RF-head -> Pin 3 on wifi */
  #define WL_IRQ AVR32_GPIO_IRQ_7
  #define WL_SPI_CS 2
 #elif EXT_BOARD == SPB104
  #define WL_SPI_CLOCK_DIVIDER 3  /* due to adapter */
  #define WL_SPI_CS 1
 #endif
 #define WL_SPI_NPCS0 { AVR32_SPI0_NPCS_0_0_PIN, AVR32_SPI0_NPCS_0_0_FUNCTION }
 #if WL_SPI_CS == 1
  #define WL_SPI_NPCS { AVR32_SPI0_NPCS_1_0_PIN, AVR32_SPI0_NPCS_1_0_FUNCTION }
 #elif WL_SPI_CS == 2
  #define WL_SPI_NPCS { AVR32_SPI0_NPCS_2_0_PIN, AVR32_SPI0_NPCS_2_0_FUNCTION }
 #endif
 #define WL_SPI_MISO { AVR32_SPI0_MISO_0_0_PIN, AVR32_SPI0_MISO_0_0_FUNCTION }
 #define WL_SPI_MOSI { AVR32_SPI0_MOSI_0_0_PIN, AVR32_SPI0_MOSI_0_0_FUNCTION }
 #define WL_SPI_SCK  { AVR32_SPI0_SCK_0_0_PIN, AVR32_SPI0_SCK_0_0_FUNCTION }
#endif /* EXT_BOARD */

/*
 *
 * ARDUINO
 * ---------------------------------------------------------------------------
 *
 */
#elif BOARD == ARDUINO /* ARDUINO */


/* USART0 physical assignment */
#define BOARD_RS232_1 AVR32_USART1
#define BOARD_RS232_1_TX                                                 \
        { AVR32_USART1_TXD_0_0_PIN, AVR32_USART1_TXD_0_0_FUNCTION }
#define BOARD_RS232_1_RX                                                 \
        { AVR32_USART1_RXD_0_0_PIN, AVR32_USART1_RXD_0_0_FUNCTION }

/* Clocks */
#define USE_PLL
#define OSC     0
#define PLL_MUL 8

#define WL_SPI AVR32_SPI1
#define WL_PDCA_PID_TX AVR32_PDCA_PID_SPI1_TX
#define WL_PDCA_PID_RX AVR32_PDCA_PID_SPI1_RX

#define WL_SHUTDOWN_PIN 	AVR32_PIN_PA09
#define WL_IRQ_PIN 			AVR32_PIN_PA03
/*
* WL_IRQ_# = (WL_IRQ_PIN / 32) * 4 + (WL_IRQ_PIN / 8) % 4
   * 3 => 0 * 4 + 0 = 0
*/
#define WL_IRQ 				AVR32_GPIO_IRQ_0
#define WL_SPI_CS 			0
#define WL_RESET_PIN		AVR32_PIN_PA07
#define WL_EXTERNAL_RESET 	1


#define WL_SPI_NPCS { AVR32_SPI1_NPCS_0_0_PIN, AVR32_SPI1_NPCS_0_0_FUNCTION }
#define WL_SPI_MISO { AVR32_SPI1_MISO_0_0_PIN, AVR32_SPI1_MISO_0_0_FUNCTION }
#define WL_SPI_MOSI { AVR32_SPI1_MOSI_0_0_PIN, AVR32_SPI1_MOSI_0_0_FUNCTION }
#define WL_SPI_SCK  { AVR32_SPI1_SCK_0_0_PIN, AVR32_SPI1_SCK_0_0_FUNCTION }

#endif /* EVKxxxx */









#if OSC == 0
# define FOSC FOSC0 /* 12 MHz */
#else
# define FOSC FOSC1 /* 11.2896 MHz */
#endif

#ifdef USE_PLL
# define FMCK_HZ ((FOSC * (PLL_MUL + 1)) / 2)
#else
# define FMCK_HZ FOSC
#endif

#define FCPU_HZ FMCK_HZ
#define FHSB_HZ FCPU_HZ
#define FPBB_HZ FMCK_HZ
#define FPBA_HZ FMCK_HZ


#ifndef CONFIG_CONSOLE_PORT
#define CONFIG_CONSOLE_PORT BOARD_RS232_1
#endif

#endif
