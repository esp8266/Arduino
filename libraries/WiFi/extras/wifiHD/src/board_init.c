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

#include <pm.h>
#include <flashc.h>
#include <intc.h>
#include <board.h>
#include <gpio.h>
#ifdef WITH_SDRAM
 #include <sdramc.h>
#endif
#include <board_init.h>
#include <spi.h>

#ifndef NO_SERIAL /* The bootloader does not want serial port
                   * code */
#include <usart.h>

int board_putchar(char c)
{
        int timeout = USART_DEFAULT_TIMEOUT;
        if (c == '\n') {
                do {
                        if (!timeout--)
                                return USART_FAILURE;
                } while (usart_write_char(&CONFIG_CONSOLE_PORT, '\r') !=
                         USART_SUCCESS);
                
                timeout = USART_DEFAULT_TIMEOUT;
        }
        
        do {
                if (!timeout--)
                        return USART_FAILURE;
        } while (usart_write_char(&CONFIG_CONSOLE_PORT, c) != USART_SUCCESS);

        return USART_SUCCESS;
}
#endif /* NO_SERIAL */
/**
 * Initializes the MCU system clocks.
 */
static void
init_sys_clocks(void)
{

        /* if we don't run on OSC0 don't switch to it since we don't know
         * what kind of oscillator we have here
         */
          
#if OSC == 0
        /* switch to OSC0 to speed up the booting */
        pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);
#endif

        
#ifndef USE_PLL
        return;
#endif
        
        /* For audio, ee have to use OSC1 on to generate the correct clockrate
         * for the SSC
         */
#if OSC == 1
        /* start oscillator1 */
        pm_enable_osc1_crystal(&AVR32_PM, FOSC1);
        pm_enable_clk1(&AVR32_PM, OSC1_STARTUP);
#endif

        /* configure pll multipliers */
        pm_pll_setup(&AVR32_PM,
                     0,          /* pll */
                     PLL_MUL,    /* mul */
                     1,          /* div */
                     OSC,        /* osc */
                     16);        /* lockcount */

        /* set PLL operating range and divider (fpll = fvco/2)
         * this gives PLL output = 66 MHz (62.0928 MHz for EVK1105/OSC1)
         */
        pm_pll_set_option(&AVR32_PM,
                          0,  /* pll */
                          1,  /* pll_freq */
                          1,  /* pll_div2 */
                          0); /* pll_wbwdisable. */
  

        /* start PLL0 and wait for the lock */
        pm_pll_enable(&AVR32_PM, 0);
        pm_wait_for_pll0_locked(&AVR32_PM);
        
        /* Set all peripheral clocks torun at master clock rate */
        pm_cksel(&AVR32_PM,
                 0,   /* pbadiv */
                 0,   /* pbasel */
                 0,   /* pbbdiv */
                 0,   /* pbbsel */
                 0,   /* hsbdiv */
                 0);  /* hsbsel */ 
        
        /* Set one waitstate for the flash */
        flashc_set_wait_state(1);
        
        /* Switch to PLL0 as the master clock */
        pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);
}

static void init_exceptions(void)
{
	extern void _evba;
	Set_system_register(AVR32_EVBA, (int)&_evba);
	Enable_global_exception();
}

static void init_hmatrix(void)
{
	union {
		unsigned long                 scfg;
		avr32_hmatrix_scfg_t          SCFG;
	} u_avr32_hmatrix_scfg = { 
		AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH]
	};
	u_avr32_hmatrix_scfg.SCFG.defmstr_type = 
		AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH] = 
		u_avr32_hmatrix_scfg.scfg;
}

static void init_interrupts(void)
{
	INTC_init_interrupts();
	Enable_global_interrupt();
}

static void init_spi(void)
{
#if defined(WL_SPI)
        int i;
#endif
        

#if defined(AT45DBX_SPI)
        static const gpio_map_t AT45DBX_SPI_GPIO_MAP = {
                { AT45DBX_SPI_SCK_PIN, AT45DBX_SPI_SCK_FUNCTION },
                { AT45DBX_SPI_MISO_PIN, AT45DBX_SPI_MISO_FUNCTION },
                { AT45DBX_SPI_MOSI_PIN, AT45DBX_SPI_MOSI_FUNCTION },
                { AT45DBX_SPI_NPCS2_PIN, AT45DBX_SPI_NPCS2_FUNCTION },
        };
#endif

        
#if defined(WL_SPI) 
	const gpio_map_t WL_SPI_GPIO_MAP = {
#if defined(WL_SPI_NPCS0)
            WL_SPI_NPCS0,
#endif
            WL_SPI_NPCS, WL_SPI_MISO, WL_SPI_MOSI, WL_SPI_SCK
        };
#endif

#if defined(WL_SPI) || defined(AT45DBX_SPI)
        spi_options_t spiOptions = {
                .modfdis = 1 /* only param used by spi_initMaster() */
        };
#endif

#if defined(AT45DBX_SPI)
        gpio_enable_module(AT45DBX_SPI_GPIO_MAP,
                           sizeof(AT45DBX_SPI_GPIO_MAP) /
                           sizeof(AT45DBX_SPI_GPIO_MAP[0]));
        spi_initMaster(AT45DBX_SPI, &spiOptions);
        spi_selectionMode(AT45DBX_SPI, 0, 0, 0);
#endif

#if defined(WL_SPI)
        /* same pins might be initialized twice here */
        gpio_enable_module(WL_SPI_GPIO_MAP,
                           sizeof(WL_SPI_GPIO_MAP) /
                           sizeof(WL_SPI_GPIO_MAP[0]));
        for (i = 0; i < sizeof(WL_SPI_GPIO_MAP)/sizeof(WL_SPI_GPIO_MAP[0]); i++)
                gpio_enable_pin_pull_up(WL_SPI_GPIO_MAP[i].pin);

        /* same SPI controller might be initialized again */
        spi_initMaster(&WL_SPI, &spiOptions);
        spi_selectionMode(&WL_SPI, 0, 0, 0);
#endif

#if defined(AT45DBX_SPI)
        spi_enable(AT45DBX_SPI);

        /* put up flash reset pin */
        gpio_set_gpio_pin(AT45DBX_CHIP_RESET);
#endif

#if defined(WL_SPI)
        spi_enable(&WL_SPI);
#endif
}


static void init_rs232(void)
{
#ifndef NO_SERIAL
#if defined(BOARD_RS232_0)
        const gpio_map_t BOARD_RS232_0_GPIO_MAP = {
                BOARD_RS232_0_TX,
                BOARD_RS232_0_RX,
#if defined(BOARD_RS232_0_RTS) && defined (BOARD_RS232_0_CTS)
                BOARD_RS232_0_RTS,
                BOARD_RS232_0_CTS
#endif
                
        };
#endif

#if defined(BOARD_RS232_1)
        const gpio_map_t BOARD_RS232_1_GPIO_MAP = {
                BOARD_RS232_1_TX,
                BOARD_RS232_1_RX
#if defined(BOARD_RS232_1_RTS) && defined (BOARD_RS232_1_CTS)
                BOARD_RS232_1_RTS,
                BOARD_RS232_1_CTS
#endif
        };
#endif

#if defined(BOARD_RS232_0)
	gpio_enable_module(BOARD_RS232_0_GPIO_MAP,
                           sizeof(BOARD_RS232_0_GPIO_MAP) / 
                           sizeof(BOARD_RS232_0_GPIO_MAP[0]));
#endif

#if defined(BOARD_RS232_1)
	gpio_enable_module(BOARD_RS232_1_GPIO_MAP,
                           sizeof(BOARD_RS232_1_GPIO_MAP) / 
                           sizeof(BOARD_RS232_1_GPIO_MAP[0]));
#endif
#endif /* NO_SERIAL */
}

static void init_printk(void)
{
#ifndef NO_SERIAL
#if defined(CONFIG_CONSOLE_PORT)
	const usart_options_t usart_options = {
		.baudrate = 57600,
		.charlength = 8,
		.paritytype = USART_NO_PARITY,
		.stopbits = USART_1_STOPBIT,
		.channelmode = USART_NORMAL_CHMODE
	};
	usart_init_rs232(&CONFIG_CONSOLE_PORT, &usart_options, FPBA_HZ);
#endif
#endif /* NO_SERIAL */
}

void board_init(void)
{
        
    init_exceptions();
    init_hmatrix();
    init_sys_clocks();
    init_interrupts();

    init_rs232();
    init_printk();
        
#ifdef WITH_SDRAM
    sdramc_init(FHSB_HZ);
#endif
    init_spi();
}
