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
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <wl_spi.h>
#include <printf-stdarg.h>
#include <board_init.h>

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])

__attribute__((__interrupt__)) void avr32_irq_handler(void);
void owl_spi_mdelay(uint32_t ms);

int owl_spi_init(U8 *flags)
{
#ifdef _ASSERT_ENABLE_ /* To silence warning if Assert() macro is empty */
        volatile avr32_pm_t *pm = &AVR32_PM;
#endif
        
        volatile avr32_spi_t *spi = &WL_SPI;
#if WL_SPI_CS == 1
        volatile avr32_spi_csr1_t* CSR = &spi->CSR1;
#elif WL_SPI_CS == 2
        volatile avr32_spi_csr2_t* CSR = &spi->CSR2;
#elif WL_SPI_CS == 3
        volatile avr32_spi_csr3_t* CSR = &spi->CSR3;
#elif SPI_CS == 0
        volatile avr32_spi_csr0_t* CSR = &spi->CSR0;
#endif
        
#ifndef WITH_NO_DMA
	volatile avr32_pdca_channel_t *pdca_tx = &AVR32_PDCA.channel[0];
	volatile avr32_pdca_channel_t *pdca_rx = &AVR32_PDCA.channel[1];
#endif
        
#ifndef WL_IRQ_PIN
        *flags = SPI_FLAG_POLL;
#else
        *flags = 0;
#endif


#ifdef WL_IRQ_PIN
        /* input, irq */
        gpio_enable_gpio_pin(WL_IRQ_PIN);
        gpio_enable_pin_pull_up(WL_IRQ_PIN);
#endif

//#ifdef WL_RESET_PIN
//        /* reset pin */
//        gpio_enable_gpio_pin(WL_RESET_PIN);
//        gpio_set_gpio_pin(WL_RESET_PIN);
//#endif


#ifdef WL_POWER_PIN
        /* power off the device */
        gpio_enable_gpio_pin(WL_POWER_PIN);
        gpio_set_gpio_pin(WL_POWER_PIN);
#endif

#ifdef WL_SHUTDOWN_PIN
        gpio_enable_gpio_pin(WL_SHUTDOWN_PIN);

#ifdef WL_NO_INTERNAL_RESET  /* never defined for SPB104/SPB105 */
        gpio_clr_gpio_pin(WL_SHUTDOWN_PIN);
#endif
        
#ifdef WL_EXTERNAL_RESET
        gpio_enable_gpio_pin(WL_RESET_PIN);
#endif

#endif /* WL_SHUTDOWN_PIN */

#ifdef WL_POWER_PIN
        /* power on the device */
        gpio_clr_gpio_pin(WL_POWER_PIN); 
#endif

#ifdef WL_SHUTDOWN_PIN
        
#ifdef WL_NO_INTERNAL_RESET /* never defined for SPB104/SPB105 */
        owl_spi_mdelay(5);
        gpio_set_gpio_pin(WL_SHUTDOWN_PIN);

#elif WL_EXTERNAL_RESET
        owl_spi_mdelay(5);
        gpio_set_gpio_pin(WL_SHUTDOWN_PIN);

        owl_spi_mdelay(20);
        //delay_ms(10);	//2ms

         /* reset pin */
        gpio_set_gpio_pin(WL_RESET_PIN);

#else

        /* The shutdown pin will go high once the device is powered */
        {
#define SHUTDOWN_TIMEOUT 350
                uint32_t shutdown_timer = 0;
                while (gpio_get_pin_value(WL_SHUTDOWN_PIN) == 0) {
                        if (shutdown_timer > SHUTDOWN_TIMEOUT)
                        {
                        	printk("Timeout WL Shutdown\n");
                        	 return -1;
                        }
                        owl_spi_mdelay(5);
                        shutdown_timer += 5;
                }
        }
#endif /* WL_NO_INTERNAL_RESET */
        
#else
        /* We need to make a guess about the time needed to power the device,
         * this will depend on the hardware design.
         */
        owl_spi_mdelay(5);
#endif /* WL_SHUTDOWN_PIN */
        
        /* Note: SPI0 clock enabled at reset in pm->pbamask (see 13.6.3) */
        Assert(pm->pbamask & (1 << 5));

        /* Note: GPIO clock enabled at reset in pm->pbamask (see 13.6.3) */
        Assert(pm->pbamask & (1 << 1));
#ifdef WL_IRQ_PIN
        /* 22.4.7: "In every port there are four interrupt lines
         * connected to the interrupt controller. Every eigth
         * interrupts in the port are ored together to form an
         * interrupt line."
         *
         * WL_IRQ_# = (WL_IRQ_PIN / 32) * 4 + (WL_IRQ_PIN / 8) % 4
         * 62 => 1 * 4 + 3 = 7
         */
        INTC_register_interrupt(&avr32_irq_handler, WL_IRQ, AVR32_INTC_INT0);
#endif
        
#ifndef WITH_NO_DMA
        INTC_register_interrupt(&avr32_irq_handler, AVR32_PDCA_IRQ_0,
                                AVR32_INTC_INT0);
        INTC_register_interrupt(&avr32_irq_handler, AVR32_PDCA_IRQ_1,
                                AVR32_INTC_INT0);
        pdca_tx->IER.terr = 1;
        pdca_rx->IER.terr = 1;
#endif

#ifdef WL_SPI_CLOCK_DIVIDER
        CSR->scbr = WL_SPI_CLOCK_DIVIDER;
#else
        CSR->scbr = 2;
#endif

        /* Use max width of TDR register, 16 bit transfers */
	CSR->bits = 0x8; 

        /* Make sure that we can hold CS low until transfer is completed, e.g
         * LASTXFER is set in TDR.
         */
        CSR->csaat = 1;

        /* NRG component requires clock polarity high */
        CSR->cpol = 1;

        
#ifdef WL_IRQ_PIN
        /* make sure to clear any pending bits in ifr here. */
        gpio_clear_pin_interrupt_flag(WL_IRQ_PIN);
#endif

        return 0;
}

#ifndef WITH_NO_DMA
static void dma_txrx(const U8* in, U8* out, U16 len)
{
	volatile avr32_pdca_channel_t *pdca_tx = &AVR32_PDCA.channel[0];
	volatile avr32_pdca_channel_t *pdca_rx = &AVR32_PDCA.channel[1];
        
        /* setup tx */
        pdca_tx->mar = (U32) in;
        pdca_tx->PSR.pid = WL_PDCA_PID_TX;
	pdca_tx->tcr = len / 2;
	pdca_tx->MR.size = 1; /* 2-byte */
        pdca_tx->IER.trc = 1;

        /* setup rx */
        pdca_rx->mar = (U32) out;
	pdca_rx->PSR.pid = WL_PDCA_PID_RX;
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
 *
 * Note that fifo_txrx() doesn't handle the case where len is not a
 * multiple of two bytes properly.
 *
 * However, there is no actual case where len is odd at the same time
 * as the "out" pointer is non-NULL; therefore I think that in practice,
 * we'll not write beyond the end of the "out" array.
 *
 * The extra unknown byte fetched from the in pointer will be discarded
 * by the device since a length field included in the packet header will inform
 * the device of the actual number of valid bytes (this implementation is
 * kind of hidden inside the library).
 */ 
static void fifo_txrx(const U8 *in, U8* out, U16 len)
{
        volatile avr32_spi_t *spi = &WL_SPI;
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

		/* prepare tx data register contents */
                if (in_ptr.u8ptr) {
                        reg.TDR.td |= (in_ptr.u8ptr[0] << 8) | in_ptr.u8ptr[1];
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
                
                if (len >= 2)
                        len -= 2;
                else
                        len = 0;
        }
        
        sr = spi->sr;
        Assert(!(sr & AVR32_SPI_SR_OVRES_MASK));
        Assert(!(sr & AVR32_SPI_SR_MODF_MASK));
}

void owl_spi_txrx(const U8 *in, U8* out, U16 len)
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

void owl_spi_irq(U8 enable)
{
#ifdef WL_IRQ_PIN

        if (enable)
                gpio_enable_pin_interrupt(WL_IRQ_PIN, GPIO_PIN_CHANGE);
        else
                gpio_disable_pin_interrupt(WL_IRQ_PIN);
#endif
}

void owl_spi_cs(U8 enable) 
{
	volatile avr32_spi_t *spi = &WL_SPI; 

        /*
         * PCS = xxx0 => NPCS[3:0] = 1110
         * PCS = xx01 => NPCS[3:0] = 1101
         * PCS = x011 => NPCS[3:0] = 1011
         * PCS = 0111 => NPCS[3:0] = 0111
         * PCS = 1111 => forbidden (no peripheral is selected)
         */
            
	if (enable) 
#if WL_SPI_CS == 2
	        spi->MR.pcs = 0x3; /* cs2 */ 
#elif WL_SPI_CS == 1
                spi->MR.pcs = 0x1; /* cs1 */ 
#elif WL_SPI_CS == 3 
                spi->MR.pcs = 0x7; /* cs3 */ 
#elif WL_SPI_CS == 0
                spi->MR.pcs = 0x0; /* cs0 */
#endif 
	else 
		spi->MR.pcs = 0xf; 
} 

void owl_spi_mdelay(uint32_t ms)
{
        volatile int a = 0;                     
        int i;                                  
        for (i = 0; i < ms * 5000; i++)         
                a++;                                
}      

__attribute__((__interrupt__)) void avr32_irq_handler(void)
{
#ifndef WITH_NO_DMA
	volatile avr32_pdca_channel_t *pdca_tx = &AVR32_PDCA.channel[0];
	volatile avr32_pdca_channel_t *pdca_rx = &AVR32_PDCA.channel[1];
        
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
#endif
        
#ifdef WL_IRQ_PIN
        if (gpio_get_pin_interrupt_flag(WL_IRQ_PIN)) {
		gpio_clear_pin_interrupt_flag(WL_IRQ_PIN);
                wl_spi_irq();
	}
#endif

}
