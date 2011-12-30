/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

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
#include <gpio.h>
#include <intc.h>
#include <avr32/io.h>
#include <compiler.h>
#include <string.h>
#include <board.h>
#include "printf-stdarg.h"
#include "platform_spi.h"
#include "delay.h"

/* This define can be removed if the SPI irq line from the SPB104 board is
 * connected to a GPIO on the AVR.
 */
#if EXT_BOARD == SPB104
#define USE_POLL
#endif

#if BOARD == EVK1100
 #define AVR32_SPI AVR32_SPI1
 #define GPIO_IRQ_PIN AVR32_PIN_PB30  /* J16 pin 6, GPIO62 */
 #define GPIO_IRQ AVR32_GPIO_IRQ_7
 #define AVR32_PDCA_PID_TX AVR32_PDCA_PID_SPI1_TX
 #define AVR32_PDCA_PID_RX AVR32_PDCA_PID_SPI1_RX
 #define SPI_CS 1
#elif BOARD == EVK1101 /* only one SPI on 1101 */
 #define GPIO_IRQ_PIN AVR32_PIN_PB00  /* GPIO32 */
 #define GPIO_IRQ AVR32_GPIO_IRQ_4
 #define AVR32_PDCA_PID_TX AVR32_PDCA_PID_SPI_TX
 #define AVR32_PDCA_PID_RX AVR32_PDCA_PID_SPI_RX
 #define SPI_CS 1
#elif BOARD == EVK1105
 #define AVR32_SPI AVR32_SPI0
 #define GPIO_IRQ_PIN AVR32_PIN_PB30  /* J16 pin 6, GPIO62 */
 #define GPIO_IRQ AVR32_GPIO_IRQ_7
 #define AVR32_PDCA_PID_TX AVR32_PDCA_PID_SPI0_TX
 #define AVR32_PDCA_PID_RX AVR32_PDCA_PID_SPI0_RX
 #if EXT_BOARD == SPB105
  #define SPI_CS 2
 #elif EXT_BOARD == SPB104
  #define SPI_CS 1
 #else /* EXT_BOARD */
  #error
 #endif
#elif BOARD == ARDUINO
//defined in arduino.h
#else /* BOARD */
 #error
#endif

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])

__attribute__((__interrupt__)) void avr32_irq_handler(void);

void platform_init(U8 *flags)
{
#ifdef _ASSERT_ENABLE_ /* To silence warning if Assert() macro is empty */
        volatile avr32_pm_t *pm = &AVR32_PM;
#endif
	const gpio_map_t GPIO_MAP = {

                /* EVK1100 pin configuration */
#if BOARD == EVK1100
                { AVR32_SPI1_NPCS_0_0_PIN, AVR32_SPI1_NPCS_0_0_FUNCTION },
		{ AVR32_SPI1_NPCS_1_0_PIN, AVR32_SPI1_NPCS_1_0_FUNCTION },
		{ AVR32_SPI1_MISO_0_0_PIN, AVR32_SPI1_MISO_0_0_FUNCTION },
		{ AVR32_SPI1_MOSI_0_0_PIN, AVR32_SPI1_MOSI_0_0_FUNCTION },
		{ AVR32_SPI1_SCK_0_0_PIN, AVR32_SPI1_SCK_0_0_FUNCTION },

                /* EVK1101 pin configuration */
#elif BOARD == EVK1101
                { AVR32_SPI_NPCS_0_0_PIN, AVR32_SPI_NPCS_0_0_FUNCTION },
		{ AVR32_SPI_NPCS_1_0_PIN, AVR32_SPI_NPCS_1_0_FUNCTION },
		{ AVR32_SPI_MISO_0_0_PIN, AVR32_SPI_MISO_0_0_FUNCTION },
		{ AVR32_SPI_MOSI_0_0_PIN, AVR32_SPI_MOSI_0_0_FUNCTION },
		{ AVR32_SPI_SCK_0_0_PIN, AVR32_SPI_SCK_0_0_FUNCTION },

                /* EVK1105 pin configuration */
#elif BOARD == EVK1105
                { AVR32_SPI0_NPCS_0_0_PIN, AVR32_SPI0_NPCS_0_0_FUNCTION },
#if SPI_CS == 1
		{ AVR32_SPI0_NPCS_1_0_PIN, AVR32_SPI0_NPCS_1_0_FUNCTION },
#elif SPI_CS == 2
                { AVR32_SPI0_NPCS_2_0_PIN, AVR32_SPI0_NPCS_2_0_FUNCTION },
#endif
		{ AVR32_SPI0_MISO_0_0_PIN, AVR32_SPI0_MISO_0_0_FUNCTION },
		{ AVR32_SPI0_MOSI_0_0_PIN, AVR32_SPI0_MOSI_0_0_FUNCTION },
		{ AVR32_SPI0_SCK_0_0_PIN, AVR32_SPI0_SCK_0_0_FUNCTION },
#elif BOARD == ARDUINO
        { AVR32_SPI1_NPCS_0_0_PIN, AVR32_SPI1_NPCS_0_0_FUNCTION },
		{ AVR32_SPI1_MISO_0_0_PIN, AVR32_SPI1_MISO_0_0_FUNCTION },
		{ AVR32_SPI1_MOSI_0_0_PIN, AVR32_SPI1_MOSI_0_0_FUNCTION },
		{ AVR32_SPI1_SCK_0_0_PIN, AVR32_SPI1_SCK_0_0_FUNCTION },
#else
#error
#endif
	};
        U32 i;
        
#ifndef WITH_NO_DMA
	volatile avr32_pdca_channel_t *pdca_tx = &AVR32_PDCA.channel[0];
	volatile avr32_pdca_channel_t *pdca_rx = &AVR32_PDCA.channel[1];
#endif
        
#ifdef USE_POLL
        *flags = SPI_FLAG_POLL;
#else
        *flags = 0;
#endif

        gpio_enable_module(GPIO_MAP, ARRAY_SIZE(GPIO_MAP));

        for (i = 0; i < ARRAY_SIZE(GPIO_MAP); i++)
                gpio_enable_pin_pull_up(GPIO_MAP[i].pin);

        /* input, irq */
        gpio_enable_gpio_pin(GPIO_IRQ_PIN);
        gpio_enable_pin_pull_up(GPIO_IRQ_PIN);

        /* shutdown pin */
        gpio_set_gpio_pin(GPIO_W_SHUTDOWN_PIN);

        delay_ms(10);	//2ms

        /* reset pin */
        gpio_set_gpio_pin(GPIO_W_RESET_PIN);

        /* Note: SPI0 clock enabled at reset in pm->pbamask (see 13.6.3) */
        Assert(pm->pbamask & (1 << 5));

        /* Note: GPIO clock enabled at reset in pm->pbamask (see 13.6.3) */
        Assert(pm->pbamask & (1 << 1));

        /* 22.4.7: "In every port there are four interrupt lines
         * connected to the interrupt controller. Every eigth
         * interrupts in the port are ored together to form an
         * interrupt line."
         *
         * GPIO_IRQ_# = (GPIO_IRQ_PIN / 32) * 4 + (GPIO_IRQ_PIN / 8) % 4
         * 62 => 1 * 4 + 3 = 7
         */
        INTC_register_interrupt(&avr32_irq_handler, GPIO_IRQ, AVR32_INTC_INT0);

#ifndef WITH_NO_DMA
        INTC_register_interrupt(&avr32_irq_handler, AVR32_PDCA_IRQ_0,
                                AVR32_INTC_INT0);
        INTC_register_interrupt(&avr32_irq_handler, AVR32_PDCA_IRQ_1,
                                AVR32_INTC_INT0);
        pdca_tx->IER.terr = 1;
        pdca_rx->IER.terr = 1;
#endif
}

void platform_reset(void)
{
        volatile avr32_spi_t *spi = &AVR32_SPI;
#if SPI_CS == 1
        volatile avr32_spi_csr1_t* CSR = &spi->CSR1;
#elif SPI_CS == 2
        volatile avr32_spi_csr2_t* CSR = &spi->CSR2;
#elif SPI_CS == 0
        volatile avr32_spi_csr0_t* CSR = &spi->CSR0;
#endif
        
        /* Disable SPI controller during configuration */
        spi->CR.spidis = 1;

        /* SPI master mode */
        spi->MR.mstr = 1;

        /* fixed peripheral select */
        spi->MR.ps = 0;
#if SPI_CS == 2
        spi->MR.pcs = 0x3; /* cs2 */
#elif SPI_CS == 1
        spi->MR.pcs = 0x1; /* cs1 */
#elif SPI_CS == 0
        spi->MR.pcs = 0x0; /* cs0 */
        spi->MR.modfdis = 0x1; /* disable Mode Fault DEtection */
#endif

        CSR->scbr = 2;

        /* Use max width of TDR register, 16 bit transfers */
	CSR->bits = 0x8; 

        /* Make sure that we can hold CS low until transfer is completed, e.g
         * LASTXFER is set in TDR.
         */
        CSR->csaat = 1;

        /* NRG component requires clock polarity high */
        CSR->cpol = 1;

        /* Enable SPI controller */
        spi->CR.spien = 1;
}

#ifndef WITH_NO_DMA
static void dma_txrx(const U8* in, U8* out, U16 len)
{
	volatile avr32_pdca_channel_t *pdca_tx = &AVR32_PDCA.channel[0];
	volatile avr32_pdca_channel_t *pdca_rx = &AVR32_PDCA.channel[1];
        
        /* setup tx */
        pdca_tx->mar = (U32) in;
        pdca_tx->PSR.pid = AVR32_PDCA_PID_TX;
	pdca_tx->tcr = len / 2;
	pdca_tx->MR.size = 1; /* 2-byte */
        pdca_tx->IER.trc = 1;

        /* setup rx */
        pdca_rx->mar = (U32) out;
	pdca_rx->PSR.pid = AVR32_PDCA_PID_RX;
	pdca_rx->tcr = len / 2;
	pdca_rx->MR.size = 1; /* 2-byte */
        pdca_rx->IER.trc = 1;

        /* start dma's. for some reason rx must be started prior to tx */
	pdca_rx->CR.ten = 1;
	pdca_tx->CR.ten = 1;

        /* blocking wait until transfer is completed */
        while (!(pdca_tx->ISR.trc && pdca_rx->ISR.trc));
}
#endif

/* access data using byte pointers since we might get unaligned
 * data from lwip. The cpu will issue a data abort if we try
 * to access data which is not properly aligned. See data sheet.
 */ 
static void fifo_txrx(const U8 *in, U8* out, U16 len)
{
        volatile avr32_spi_t *spi = &AVR32_SPI;
        UnionCPtr in_ptr;
        UnionPtr out_ptr;
        U32 sr;

        Assert(len);

        in_ptr.u8ptr = in;
        out_ptr.u8ptr = out;

        while (len) {
                U16 rdr;
		union {
			avr32_spi_tdr_t TDR;
			U32 tdr;
		} reg = { { 0 } };
                
                while (!spi->SR.tdre);
                while (!spi->SR.txempty);

                volatile U16 data= (in_ptr.u8ptr[0] << 8) | in_ptr.u8ptr[1];
		/* prepare tx data register contents */
                if (in_ptr.u8ptr) {
                        reg.TDR.td |= data; //(in_ptr.u8ptr[0] << 8) | in_ptr.u8ptr[1];
                        in_ptr.u16ptr++;
                }
                else
                        reg.TDR.td |= 0xffff;
                
		/* perform tx */
                spi->tdr = reg.tdr;

		/* wait until rx is ready */
                while (!spi->SR.rdrf);

		/* fetch rx data */
                rdr = spi->RDR.rd;
                if (out_ptr.u8ptr) {
                        out_ptr.u8ptr[0] = (rdr >> 8) & 0xff;
                        out_ptr.u8ptr[1] = rdr & 0xff;
                        out_ptr.u16ptr++;
                }
                
		/* adjust remaining length. handle the case where len is not 
		 * a multiple of 2 bytes.
		 */
                if (len >= 2)
                        len -= 2;
                else
                        len = 0;
        }
        
        sr = spi->sr;
        Assert(!(sr & AVR32_SPI_SR_OVRES_MASK));
        Assert(!(sr & AVR32_SPI_SR_MODF_MASK));
}

void platform_txrx(const U8 *in, U8* out, U16 len)
{
#ifndef WITH_NO_DMA
        static uint8_t buf[MAX_BLOCK_LEN];

        /* unaligned data or odd number of bytes, then skip dma */
        if ((U32) in % 4 || (U32) out % 4 || len % 2) {
                fifo_txrx(in, out, len);
        } else {
                if (in == NULL) {
                        memset(buf, 0xff, len);
                        in = buf;
                } else if (out == NULL) {
                        out = buf;
                }
                dma_txrx(in, out, len);
        }
#else
        fifo_txrx(in, out, len);
#endif        
}

void platform_spi_irq(U8 enable)
{
#ifdef USE_POLL
        return;
#endif

        if (enable)
                gpio_enable_pin_interrupt(GPIO_IRQ_PIN, GPIO_PIN_CHANGE);
        else
                gpio_disable_pin_interrupt(GPIO_IRQ_PIN);
}

void platform_spi_cs(U8 enable) 
{
	volatile avr32_spi_t *spi = &AVR32_SPI; 
        
	if (enable) {
#if SPI_CS == 2 
	        spi->MR.pcs = 0x3; /* cs2 */ 
#elif SPI_CS == 1 
                spi->MR.pcs = 0x1; /* cs1 */ 
#elif SPI_CS == 0
                spi->MR.pcs = 0x0; /* cs1 */
#endif 
	}else
		spi->MR.pcs = 0xf; 
} 

__attribute__((__interrupt__)) void avr32_irq_handler(void)
{
	uint8_t irq_status = 0;
#ifndef WITH_NO_DMA
	volatile avr32_pdca_channel_t *pdca_tx = &AVR32_PDCA.channel[0];
	volatile avr32_pdca_channel_t *pdca_rx = &AVR32_PDCA.channel[1];
#endif

        if (gpio_get_pin_interrupt_flag(GPIO_IRQ_PIN)) {
		gpio_clear_pin_interrupt_flag(GPIO_IRQ_PIN);
		irq_status |= SPI_IRQ_RX;
	}
#ifndef WITH_NO_DMA
        
        /* tx xfer complete */
	if (pdca_tx->IMR.trc && pdca_tx->ISR.trc) {
                pdca_tx->IDR.trc = 1;
                pdca_tx->CR.tdis = 1;  /* disable tx xfer */
	}

        /* rx xfer complete */
	if (pdca_rx->IMR.trc && pdca_rx->ISR.trc) {
                pdca_rx->IDR.trc = 1;
                pdca_rx->CR.tdis = 1;  /* disable rx xfer */
	}

#if 0
        /* tx and rx complete */
        if (pdca_tx->ISR.trc && pdca_rx->ISR.trc)
                irq_status |= SPI_IRQ_XFER_COMPLETE;
#endif
                
#endif
	if (irq_status)
		spi_irq_handler(irq_status);
}
