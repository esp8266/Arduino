/* Copyright (c) 2006, 2007, 2008  Eric B. Weddington
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

/* $Id: power.h,v 1.11.2.11 2008/11/03 04:13:18 arcanum Exp $ */

#ifndef _AVR_POWER_H_
#define _AVR_POWER_H_   1

#include <avr/io.h>
#include <stdint.h>


/** \file */
/** \defgroup avr_power <avr/power.h>: Power Reduction Management

\code #include <avr/power.h>\endcode

Many AVRs contain a Power Reduction Register (PRR) or Registers (PRRx) that 
allow you to reduce power consumption by disabling or enabling various on-board 
peripherals as needed.

There are many macros in this header file that provide an easy interface
to enable or disable on-board peripherals to reduce power. See the table below.

\note Not all AVR devices have a Power Reduction Register (for example
the ATmega128). On those devices without a Power Reduction Register, these 
macros are not available.

\note Not all AVR devices contain the same peripherals (for example, the LCD
interface), or they will be named differently (for example, USART and 
USART0). Please consult your device's datasheet, or the header file, to 
find out which macros are applicable to your device.

*/


/** \addtogroup avr_power

\anchor avr_powermacros
<small>
<center>
<table border="3">
  <tr>
    <td width="10%"><strong>Power Macro</strong></td>
    <td width="15%"><strong>Description</strong></td>
    <td width="75%"><strong>Applicable for device</strong></td>
  </tr>

  <tr>
    <td>power_adc_enable()</td>
    <td>Enable the Analog to Digital Converter module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B, AT90PWM216, AT90PWM316, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490, ATmega164P, ATmega324P, ATmega644, ATmega48, ATmega88, ATmega168, ATtiny24, ATtiny44, ATtiny84, ATtiny25, ATtiny45, ATtiny85, ATtiny261, ATtiny461, ATtiny861</td>
  </tr>

  <tr>
    <td>power_adc_disable()</td>
    <td>Disable the Analog to Digital Converter module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B, AT90PWM216, AT90PWM316, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490, ATmega164P, ATmega324P, ATmega644, ATmega48, ATmega88, ATmega168, ATtiny24, ATtiny44, ATtiny84, ATtiny25, ATtiny45, ATtiny85, ATtiny261, ATtiny461, ATtiny861</td>
  </tr>

  <tr>
    <td>power_lcd_enable()</td>
    <td>Enable the LCD module.</td>
    <td>ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490</td>
  </tr>

  <tr>
    <td>power_lcd_disable().</td>
    <td>Disable the LCD module.</td>
    <td>ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490</td>
  </tr>

  <tr>
    <td>power_psc0_enable()</td>
    <td>Enable the Power Stage Controller 0 module.</td>
    <td>AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B</td>
  </tr>

  <tr>
    <td>power_psc0_disable()</td>
    <td>Disable the Power Stage Controller 0 module.</td>
    <td>AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B</td>
  </tr>

  <tr>
    <td>power_psc1_enable()</td>
    <td>Enable the Power Stage Controller 1 module.</td>
    <td>AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B</td>
  </tr>

  <tr>
    <td>power_psc1_disable()</td>
    <td>Disable the Power Stage Controller 1 module.</td>
    <td>AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B</td>
  </tr>

  <tr>
    <td>power_psc2_enable()</td>
    <td>Enable the Power Stage Controller 2 module.</td>
    <td>AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B</td>
  </tr>

  <tr>
    <td>power_psc2_disable()</td>
    <td>Disable the Power Stage Controller 2 module.</td>
    <td>AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B</td>
  </tr>

  <tr>
    <td>power_spi_enable()</td>
    <td>Enable the Serial Peripheral Interface module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B, AT90PWM216, AT90PWM316, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490, ATmega164P, ATmega324P, ATmega644, ATmega48, ATmega88, ATmega168</td>
  </tr>

  <tr>
    <td>power_spi_disable()</td>
    <td>Disable the Serial Peripheral Interface module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B, AT90PWM216, AT90PWM316, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490, ATmega164P, ATmega324P, ATmega644, ATmega48, ATmega88, ATmega168</td>
  </tr>

  <tr>
    <td>power_timer0_enable()</td>
    <td>Enable the Timer 0 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, AT90PWM1, AT90PWM216, AT90PWM316, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega164P, ATmega324P, ATmega644, ATmega406, ATmega48, ATmega88, ATmega168, ATtiny24, ATtiny44, ATtiny84, ATtiny25, ATtiny45, ATtiny85, ATtiny261, ATtiny461, ATtiny861</td>
  </tr>

  <tr>
    <td>power_timer0_disable()</td>
    <td>Disable the Timer 0 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B, AT90PWM216, AT90PWM316, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega164P, ATmega324P, ATmega644, ATmega406, ATmega48, ATmega88, ATmega168, ATtiny24, ATtiny44, ATtiny84, ATtiny25, ATtiny45, ATtiny85, ATtiny261, ATtiny461, ATtiny861</td>
  </tr>

  <tr>
    <td>power_timer1_enable()</td>
    <td>Enable the Timer 1 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B, AT90PWM216, AT90PWM316, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490, ATmega164P, ATmega324P, ATmega644, ATmega406, ATmega48, ATmega88, ATmega168, ATtiny24, ATtiny44, ATtiny84, ATtiny25, ATtiny45, ATtiny85, ATtiny261, ATtiny461, ATtiny861</td>
  </tr>

  <tr>
    <td>power_timer1_disable()</td>
    <td>Disable the Timer 1 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B, AT90PWM216, AT90PWM316, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490, ATmega164P, ATmega324P, ATmega644, ATmega406, ATmega48, ATmega88, ATmega168, ATtiny24, ATtiny44, ATtiny84, ATtiny25, ATtiny45, ATtiny85, ATtiny261, ATtiny461, ATtiny861</td>
  </tr>

  <tr>
    <td>power_timer2_enable()</td>
    <td>Enable the Timer 2 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, ATmega164P, ATmega324P, ATmega644, ATmega48, ATmega88, ATmega168</td>
  </tr>

  <tr>
    <td>power_timer2_disable()</td>
    <td>Disable the Timer 2 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, ATmega164P, ATmega324P, ATmega644, ATmega48, ATmega88, ATmega168</td>
  </tr>

  <tr>
    <td>power_timer3_enable()</td>
    <td>Enable the Timer 3 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287</td>
  </tr>

  <tr>
    <td>power_timer3_disable()</td>
    <td>Disable the Timer 3 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287</td>
  </tr>

  <tr>
    <td>power_timer4_enable()</td>
    <td>Enable the Timer 4 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561</td>
  </tr>

  <tr>
    <td>power_timer4_disable()</td>
    <td>Disable the Timer 4 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561</td>
  </tr>

  <tr>
    <td>power_timer5_enable()</td>
    <td>Enable the Timer 5 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561</td>
  </tr>

  <tr>
    <td>power_timer5_disable()</td>
    <td>Disable the Timer 5 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561</td>
  </tr>

  <tr>
    <td>power_twi_enable()</td>
    <td>Enable the Two Wire Interface module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, ATmega164P, ATmega324P, ATmega644, ATmega406, ATmega48, ATmega88, ATmega168</td>
  </tr>

  <tr>
    <td>power_twi_disable()</td>
    <td>Disable the Two Wire Interface module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, ATmega164P, ATmega324P, ATmega644, ATmega406, ATmega48, ATmega88, ATmega168</td>
  </tr>

  <tr>
    <td>power_usart_enable()</td>
    <td>Enable the USART module.</td>
    <td>AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B</td>
  </tr>

  <tr>
    <td>power_usart_disable()</td>
    <td>Disable the USART module.</td>
    <td>AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B</td>
  </tr>

  <tr>
    <td>power_usart0_enable()</td>
    <td>Enable the USART 0 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490, ATmega164P, ATmega324P, ATmega644, ATmega48, ATmega88, ATmega168</td>
  </tr>

  <tr>
    <td>power_usart0_disable()</td>
    <td>Disable the USART 0 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490, ATmega164P, ATmega324P, ATmega644, ATmega48, ATmega88, ATmega168</td>
  </tr>

  <tr>
    <td>power_usart1_enable()</td>
    <td>Enable the USART 1 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, ATmega164P, ATmega324P</td>
  </tr>

  <tr>
    <td>power_usart1_disable()</td>
    <td>Disable the USART 1 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, ATmega164P, ATmega324P</td>
  </tr>

  <tr>
    <td>power_usart2_enable()</td>
    <td>Enable the USART 2 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561</td>
  </tr>

  <tr>
    <td>power_usart2_disable()</td>
    <td>Disable the USART 2 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561</td>
  </tr>

  <tr>
    <td>power_usart3_enable()</td>
    <td>Enable the USART 3 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561</td>
  </tr>

  <tr>
    <td>power_usart3_disable()</td>
    <td>Disable the USART 3 module.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561</td>
  </tr>

  <tr>
    <td>power_usb_enable()</td>
    <td>Enable the USB module.</td>
    <td>AT90USB646, AT90USB647, AT90USB1286, AT90USB1287</td>
  </tr>

  <tr>
    <td>power_usb_disable()</td>
    <td>Disable the USB module.</td>
    <td>AT90USB646, AT90USB647, AT90USB1286, AT90USB1287</td>
  </tr>

  <tr>
    <td>power_usi_enable()</td>
    <td>Enable the Universal Serial Interface module.</td>
    <td>ATtiny24, ATtiny44, ATtiny84, ATtiny25, ATtiny45, ATtiny85, ATtiny261, ATtiny461, ATtiny861</td>
  </tr>

  <tr>
    <td>power_usi_disable()</td>
    <td>Disable the Universal Serial Interface module.</td>
    <td>ATtiny24, ATtiny44, ATtiny84, ATtiny25, ATtiny45, ATtiny85, ATtiny261, ATtiny461, ATtiny861</td>
  </tr>

  <tr>
    <td>power_vadc_enable()</td>
    <td>Enable the Voltage ADC module.</td>
    <td>ATmega406</td>
  </tr>

  <tr>
    <td>power_vadc_disable()</td>
    <td>Disable the Voltage ADC module.</td>
    <td>ATmega406</td>
  </tr>

  <tr>
    <td>power_all_enable()</td>
    <td>Enable all modules.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B, AT90PWM216, AT90PWM316, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490, ATmega164P, ATmega324P, ATmega644, ATmega406, ATmega48, ATmega88, ATmega168, ATtiny24, ATtiny44, ATtiny84, ATtiny25, ATtiny45, ATtiny85, ATtiny261, ATtiny461, ATtiny861</td>
  </tr>

  <tr>
    <td>power_all_disable()</td>
    <td>Disable all modules.</td>
    <td>ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, AT90USB646, AT90USB647, AT90USB1286, AT90USB1287, AT90PWM1, AT90PWM2, AT90PWM2B, AT90PWM3, AT90PWM3B, AT90PWM216, AT90PWM316, ATmega165, ATmega165P, ATmega325, ATmega3250, ATmega645, ATmega6450, ATmega169, ATmega169P, ATmega329, ATmega3290, ATmega649, ATmega6490, ATmega164P, ATmega324P, ATmega644, ATmega406, ATmega48, ATmega88, ATmega168, ATtiny24, ATtiny44, ATtiny84, ATtiny25, ATtiny45, ATtiny85, ATtiny261, ATtiny461, ATtiny861</td>
  </tr>
</table>
</center>
</small>

@} */


#if defined(__AVR_ATxmega64a1) \
|| defined(__AVR_ATxmega64a3) \
|| defined(__AVR_ATxmega128a1__) \
|| defined(__AVR_ATxmega128a3__) \
|| defined(__AVR_ATxmega256a3__) \
|| defined(__AVR_ATxmega256a3b__)

/*
#define power_aes_enable()  (PR_PR &= (uint8_t)~(PR_AES_bm))
#define power_aes_disable() (PR_PR |= (uint8_t)PR_AES_bm)
*/

#define power_ebi_enable()  (PR_PR &= (uint8_t)~(PR_EBI_bm))
#define power_ebi_disable() (PR_PR |= (uint8_t)PR_EBI_bm)

#define power_rtc_enable()  (PR_PR &= (uint8_t)~(PR_RTC_bm))
#define power_rtc_disable() (PR_PR |= (uint8_t)PR_RTC_bm)

#define power_evsys_enable()    (PR_PR &= (uint8_t)~(PR_EVSYS_bm))
#define power_evsys_disable()   (PR_PR |= (uint8_t)PR_EVSYS_bm)

#define power_dma_enable()    (PR_PR &= (uint8_t)~(PR_DMA_bm))
#define power_dma_disable()   (PR_PR |= (uint8_t)PR_DMA_bm)

#define power_daca_enable()     (PR_PRPA &= (uint8_t)~(PR_DAC_bm))
#define power_daca_disable()    (PR_PRPA |= (uint8_t)PR_DAC_bm)
#define power_dacb_enable()     (PR_PRPB &= (uint8_t)~(PR_DAC_bm))
#define power_dacb_disable()    (PR_PRPB |= (uint8_t)PR_DAC_bm)

#define power_adca_enable()     (PR_PRPA &= (uint8_t)~(PR_ADC_bm))
#define power_adca_disable()    (PR_PRPA |= (uint8_t)PR_ADC_bm)
#define power_adcb_enable()     (PR_PRPB &= (uint8_t)~(PR_ADC_bm))
#define power_adcb_disable()    (PR_PRPB |= (uint8_t)PR_ADC_bm)

#define power_aca_enable()      (PR_PRPA &= (uint8_t)~(PR_AC_bm))
#define power_aca_disable()     (PR_PRPA |= (uint8_t)PR_AC_bm)
#define power_acb_enable()      (PR_PRPB &= (uint8_t)~(PR_AC_bm))
#define power_acb_disable()     (PR_PRPB |= (uint8_t)PR_AC_bm)

#define power_twic_enable()     (PR_PRPC &= (uint8_t)~(PR_TWI_bm))
#define power_twic_disable()    (PR_PRPC |= (uint8_t)PR_TWI_bm)
#define power_twid_enable()     (PR_PRPD &= (uint8_t)~(PR_TWI_bm))
#define power_twid_disable()    (PR_PRPD |= (uint8_t)PR_TWI_bm)
#define power_twie_enable()     (PR_PRPE &= (uint8_t)~(PR_TWI_bm))
#define power_twie_disable()    (PR_PRPE |= (uint8_t)PR_TWI_bm)
#define power_twif_enable()     (PR_PRPF &= (uint8_t)~(PR_TWI_bm))
#define power_twif_disable()    (PR_PRPF |= (uint8_t)PR_TWI_bm)

#define power_usartc1_enable()  (PR_PRPC &= (uint8_t)~(PR_USART1_bm))
#define power_usartc1_disable() (PR_PRPC |= (uint8_t)PR_USART1_bm)
#define power_usartd1_enable()  (PR_PRPD &= (uint8_t)~(PR_USART1_bm))
#define power_usartd1_disable() (PR_PRPD |= (uint8_t)PR_USART1_bm)
#define power_usarte1_enable()  (PR_PRPE &= (uint8_t)~(PR_USART1_bm))
#define power_usarte1_disable() (PR_PRPE |= (uint8_t)PR_USART1_bm)
#define power_usartf1_enable()  (PR_PRPF &= (uint8_t)~(PR_USART1_bm))
#define power_usartf1_disable() (PR_PRPF |= (uint8_t)PR_USART1_bm)

#define power_usartc0_enable()  (PR_PRPC &= (uint8_t)~(PR_USART0_bm))
#define power_usartc0_disable() (PR_PRPC |= (uint8_t)PR_USART0_bm)
#define power_usartd0_enable()  (PR_PRPD &= (uint8_t)~(PR_USART0_bm))
#define power_usartd0_disable() (PR_PRPD |= (uint8_t)PR_USART0_bm)
#define power_usarte0_enable()  (PR_PRPE &= (uint8_t)~(PR_USART0_bm))
#define power_usarte0_disable() (PR_PRPE |= (uint8_t)PR_USART0_bm)
#define power_usartf0_enable()  (PR_PRPF &= (uint8_t)~(PR_USART0_bm))
#define power_usartf0_disable() (PR_PRPF |= (uint8_t)PR_USART0_bm)

#define power_spic_enable()     (PR_PRPC &= (uint8_t)~(PR_SPI_bm))
#define power_spic_disable()    (PR_PRPC |= (uint8_t)PR_SPI_bm)
#define power_spid_enable()     (PR_PRPD &= (uint8_t)~(PR_SPI_bm))
#define power_spid_disable()    (PR_PRPD |= (uint8_t)PR_SPI_bm)
#define power_spie_enable()     (PR_PRPE &= (uint8_t)~(PR_SPI_bm))
#define power_spie_disable()    (PR_PRPE |= (uint8_t)PR_SPI_bm)
#define power_spif_enable()     (PR_PRPF &= (uint8_t)~(PR_SPI_bm))
#define power_spif_disable()    (PR_PRPF |= (uint8_t)PR_SPI_bm)

#define power_hiresc_enable()   (PR_PRPC &= (uint8_t)~(PR_HIRES_bm))
#define power_hiresc_disable()  (PR_PRPC |= (uint8_t)PR_HIRES_bm)
#define power_hiresd_enable()   (PR_PRPD &= (uint8_t)~(PR_HIRES_bm))
#define power_hiresd_disable()  (PR_PRPD |= (uint8_t)PR_HIRES_bm)
#define power_hirese_enable()   (PR_PRPE &= (uint8_t)~(PR_HIRES_bm))
#define power_hirese_disable()  (PR_PRPE |= (uint8_t)PR_HIRES_bm)
#define power_hiresf_enable()   (PR_PRPF &= (uint8_t)~(PR_HIRES_bm))
#define power_hiresf_disable()  (PR_PRPF |= (uint8_t)PR_HIRES_bm)

#define power_tc1c_enable()     (PR_PRPC &= (uint8_t)~(PR_TC1_bm))
#define power_tc1c_disable()    (PR_PRPC |= (uint8_t)PR_TC1_bm)
#define power_tc1d_enable()     (PR_PRPD &= (uint8_t)~(PR_TC1_bm))
#define power_tc1d_disable()    (PR_PRPD |= (uint8_t)PR_TC1_bm)
#define power_tc1e_enable()     (PR_PRPE &= (uint8_t)~(PR_TC1_bm))
#define power_tc1e_disable()    (PR_PRPE |= (uint8_t)PR_TC1_bm)
#define power_tc1f_enable()     (PR_PRPF &= (uint8_t)~(PR_TC1_bm))
#define power_tc1f_disable()    (PR_PRPF |= (uint8_t)PR_TC1_bm)

#define power_tc0c_enable()     (PR_PRPC &= (uint8_t)~(PR_TC0_bm))
#define power_tc0c_disable()    (PR_PRPC |= (uint8_t)PR_TC0_bm)
#define power_tc0d_enable()     (PR_PRPD &= (uint8_t)~(PR_TC0_bm))
#define power_tc0d_disable()    (PR_PRPD |= (uint8_t)PR_TC0_bm)
#define power_tc0e_enable()     (PR_PRPE &= (uint8_t)~(PR_TC0_bm))
#define power_tc0e_disable()    (PR_PRPE |= (uint8_t)PR_TC0_bm)
#define power_tc0f_enable()     (PR_PRPF &= (uint8_t)~(PR_TC0_bm))
#define power_tc0f_disable()    (PR_PRPF |= (uint8_t)PR_TC0_bm)

#define power_all_enable() \
do { \
    /* PR_PR &= (uint8_t)~(PR_AES_bm|PR_EBI_bm|PR_RTC_bm|PR_EVSYS_bm|PR_DMA_bm); */ \
    PR_PR &= (uint8_t)~(PR_EBI_bm|PR_RTC_bm|PR_EVSYS_bm|PR_DMA_bm); \
    PR_PRPA &= (uint8_t)~(PR_DAC_bm|PR_ADC_bm|PR_AC_bm); \
    PR_PRPB &= (uint8_t)~(PR_DAC_bm|PR_ADC_bm|PR_AC_bm); \
    PR_PRPC &= (uint8_t)~(PR_TWI_bm|PR_USART1_bm|PR_USART0_bm|PR_SPI_bm|PR_HIRES_bm|PR_TC1_bm|PR_TC0_bm); \
    PR_PRPD &= (uint8_t)~(PR_TWI_bm|PR_USART1_bm|PR_USART0_bm|PR_SPI_bm|PR_HIRES_bm|PR_TC1_bm|PR_TC0_bm); \
    PR_PRPE &= (uint8_t)~(PR_TWI_bm|PR_USART1_bm|PR_USART0_bm|PR_SPI_bm|PR_HIRES_bm|PR_TC1_bm|PR_TC0_bm); \
    PR_PRPF &= (uint8_t)~(PR_TWI_bm|PR_USART1_bm|PR_USART0_bm|PR_SPI_bm|PR_HIRES_bm|PR_TC1_bm|PR_TC0_bm); \
} while(0)


#define power_all_disable() \
do { \
    /* PM_PR_PR |= (uint8_t)(PR_AES_bm|PR_EBI_bm|PR_RTC_bm|PR_EVSYS_bm|PR_DMA_bm); */ \
    PR_PR |= (uint8_t)(PR_EBI_bm|PR_RTC_bm|PR_EVSYS_bm|PR_DMA_bm); \
    PR_PRPA |= (uint8_t)(PR_DAC_bm|PR_ADC_bm|PR_AC_bm); \
    PR_PRPB |= (uint8_t)(PR_DAC_bm|PR_ADC_bm|PR_AC_bm); \
    PR_PRPC |= (uint8_t)(PR_TWI_bm|PR_USART1_bm|PR_USART0_bm|PR_SPI_bm|PR_HIRES_bm|PR_TC1_bm|PR_TC0_bm); \
    PR_PRPD |= (uint8_t)(PR_TWI_bm|PR_USART1_bm|PR_USART0_bm|PR_SPI_bm|PR_HIRES_bm|PR_TC1_bm|PR_TC0_bm); \
    PR_PRPE |= (uint8_t)(PR_TWI_bm|PR_USART1_bm|PR_USART0_bm|PR_SPI_bm|PR_HIRES_bm|PR_TC1_bm|PR_TC0_bm); \
    PR_PRPF |= (uint8_t)(PR_TWI_bm|PR_USART1_bm|PR_USART0_bm|PR_SPI_bm|PR_HIRES_bm|PR_TC1_bm|PR_TC0_bm); \
} while(0)


#elif defined(__AVR_ATmega640__) \
|| defined(__AVR_ATmega1280__) \
|| defined(__AVR_ATmega1281__) \
|| defined(__AVR_ATmega2560__) \
|| defined(__AVR_ATmega2561__) 

#define power_adc_enable()      (PRR0 &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR0 |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR0 &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR0 |= (uint8_t)(1 << PRSPI))

#define power_twi_enable()      (PRR0 &= (uint8_t)~(1 << PRTWI))
#define power_twi_disable()     (PRR0 |= (uint8_t)(1 << PRTWI))

#define power_timer0_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR0 |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR0 |= (uint8_t)(1 << PRTIM1))

#define power_timer2_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM2))
#define power_timer2_disable()  (PRR0 |= (uint8_t)(1 << PRTIM2))

#define power_timer3_enable()   (PRR1 &= (uint8_t)~(1 << PRTIM3))
#define power_timer3_disable()  (PRR1 |= (uint8_t)(1 << PRTIM3))

#define power_timer4_enable()   (PRR1 &= (uint8_t)~(1 << PRTIM4))
#define power_timer4_disable()  (PRR1 |= (uint8_t)(1 << PRTIM4))

#define power_timer5_enable()   (PRR1 &= (uint8_t)~(1 << PRTIM5))
#define power_timer5_disable()  (PRR1 |= (uint8_t)(1 << PRTIM5))

#define power_usart0_enable()   (PRR0 &= (uint8_t)~(1 << PRUSART0))
#define power_usart0_disable()  (PRR0 |= (uint8_t)(1 << PRUSART0))

#define power_usart1_enable()   (PRR1 &= (uint8_t)~(1 << PRUSART1))
#define power_usart1_disable()  (PRR1 |= (uint8_t)(1 << PRUSART1))

#define power_usart2_enable()   (PRR1 &= (uint8_t)~(1 << PRUSART2))
#define power_usart2_disable()  (PRR1 |= (uint8_t)(1 << PRUSART2))

#define power_usart3_enable()   (PRR1 &= (uint8_t)~(1 << PRUSART3))
#define power_usart3_disable()  (PRR1 |= (uint8_t)(1 << PRUSART3))

#define power_all_enable() \
do{ \
    PRR0 &= (uint8_t)~((1<<PRADC)|(1<<PRSPI)|(1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)|(1<<PRUSART0)); \
    PRR1 &= (uint8_t)~((1<<PRTIM3)|(1<<PRTIM4)|(1<<PRTIM5)|(1<<PRTIM5)|(1<<PRUSART1)|(1<<PRUSART2)|(1<<PRUSART3)); \
}while(0)

#define power_all_disable() \
do{ \
    PRR0 |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)|(1<<PRUSART0)); \
    PRR1 |= (uint8_t)((1<<PRTIM3)|(1<<PRTIM4)|(1<<PRTIM5)|(1<<PRTIM5)|(1<<PRUSART1)|(1<<PRUSART2)|(1<<PRUSART3)); \
}while(0)


#elif defined(__AVR_AT90USB646__) \
|| defined(__AVR_AT90USB647__) \
|| defined(__AVR_AT90USB1286__) \
|| defined(__AVR_AT90USB1287__)

#define power_adc_enable()      (PRR0 &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR0 |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR0 &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR0 |= (uint8_t)(1 << PRSPI))

#define power_twi_enable()      (PRR0 &= (uint8_t)~(1 << PRTWI))
#define power_twi_disable()     (PRR0 |= (uint8_t)(1 << PRTWI))

#define power_timer0_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR0 |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR0 |= (uint8_t)(1 << PRTIM1))

#define power_timer2_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM2))
#define power_timer2_disable()  (PRR0 |= (uint8_t)(1 << PRTIM2))

#define power_timer3_enable()   (PRR1 &= (uint8_t)~(1 << PRTIM3))
#define power_timer3_disable()  (PRR1 |= (uint8_t)(1 << PRTIM3))

#define power_usart1_enable()   (PRR1 &= (uint8_t)~(1 << PRUSART1))
#define power_usart1_disable()  (PRR1 |= (uint8_t)(1 << PRUSART1))

#define power_usb_enable()      (PRR1 &= (uint8_t)~(1 << PRUSB))
#define power_usb_disable()     (PRR1 |= (uint8_t)(1 << PRUSB))

#define power_all_enable() \
do{ \
    PRR0 &= (uint8_t)~((1<<PRADC)|(1<<PRSPI)|(1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)); \
    PRR1 &= (uint8_t)~((1<<PRTIM3)|(1<<PRUSART1)|(1<<PRUSB)); \
}while(0)

#define power_all_disable() \
do{ \
    PRR0 |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)); \
    PRR1 |= (uint8_t)((1<<PRTIM3)|(1<<PRUSART1)|(1<<PRUSB)); \
}while(0)


#elif defined(__AVR_ATmega32U4__)


#define power_adc_enable()      (PRR0 &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR0 |= (uint8_t)(1 << PRADC))

#define power_usart0_enable()   (PRR0 &= (uint8_t)~(1 << PRUSART0))
#define power_usart0_disable()  (PRR0 |= (uint8_t)(1 << PRUSART0))

#define power_spi_enable()      (PRR0 &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR0 |= (uint8_t)(1 << PRSPI))

#define power_twi_enable()      (PRR0 &= (uint8_t)~(1 << PRTWI))
#define power_twi_disable()     (PRR0 |= (uint8_t)(1 << PRTWI))

#define power_timer0_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR0 |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR0 |= (uint8_t)(1 << PRTIM1))

#define power_timer2_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM2))
#define power_timer2_disable()  (PRR0 |= (uint8_t)(1 << PRTIM2))

#define power_timer3_enable()   (PRR1 &= (uint8_t)~(1 << PRTIM3))
#define power_timer3_disable()  (PRR1 |= (uint8_t)(1 << PRTIM3))

#define power_usart1_enable()   (PRR1 &= (uint8_t)~(1 << PRUSART1))
#define power_usart1_disable()  (PRR1 |= (uint8_t)(1 << PRUSART1))

#define power_usb_enable()      (PRR1 &= (uint8_t)~(1 << PRUSB))
#define power_usb_disable()     (PRR1 |= (uint8_t)(1 << PRUSB))

#define power_all_enable() \
do{ \
    PRR0 &= (uint8_t)~((1<<PRADC)|(1<<PRUSART0)|(1<<PRSPI)|(1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)); \
    PRR1 &= (uint8_t)~((1<<PRTIM3)|(1<<PRUSART1)|(1<<PRUSB)); \
}while(0)

#define power_all_disable() \
do{ \
    PRR0 |= (uint8_t)((1<<PRADC)|(1<<PRUSART0)|(1<<PRSPI)|(1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)); \
    PRR1 |= (uint8_t)((1<<PRTIM3)|(1<<PRUSART1)|(1<<PRUSB)); \
}while(0)


#elif defined(__AVR_ATmega32U6__)


#define power_adc_enable()      (PRR0 &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR0 |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR0 &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR0 |= (uint8_t)(1 << PRSPI))

#define power_twi_enable()      (PRR0 &= (uint8_t)~(1 << PRTWI))
#define power_twi_disable()     (PRR0 |= (uint8_t)(1 << PRTWI))

#define power_timer0_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR0 |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR0 |= (uint8_t)(1 << PRTIM1))

#define power_timer2_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM2))
#define power_timer2_disable()  (PRR0 |= (uint8_t)(1 << PRTIM2))

#define power_timer3_enable()   (PRR1 &= (uint8_t)~(1 << PRTIM3))
#define power_timer3_disable()  (PRR1 |= (uint8_t)(1 << PRTIM3))

#define power_usart1_enable()   (PRR1 &= (uint8_t)~(1 << PRUSART1))
#define power_usart1_disable()  (PRR1 |= (uint8_t)(1 << PRUSART1))

#define power_usb_enable()      (PRR1 &= (uint8_t)~(1 << PRUSB))
#define power_usb_disable()     (PRR1 |= (uint8_t)(1 << PRUSB))

#define power_all_enable() \
do{ \
    PRR0 &= (uint8_t)~((1<<PRADC)|(1<<PRSPI)|(1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)); \
    PRR1 &= (uint8_t)~((1<<PRTIM3)|(1<<PRUSART1)|(1<<PRUSB)); \
}while(0)

#define power_all_disable() \
do{ \
    PRR0 |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)); \
    PRR1 |= (uint8_t)((1<<PRTIM3)|(1<<PRUSART1)|(1<<PRUSB)); \
}while(0)


#elif defined(__AVR_AT90PWM1__)

#define power_adc_enable()      (PRR &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR |= (uint8_t)(1 << PRSPI))

#define power_timer0_enable()   (PRR &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR |= (uint8_t)(1 << PRTIM1))

/* Power Stage Controller 0 */
#define power_psc0_enable()     (PRR &= (uint8_t)~(1 << PRPSC0))
#define power_psc0_disable()    (PRR |= (uint8_t)(1 << PRPSC0))

/* Power Stage Controller 1 */
#define power_psc1_enable()     (PRR &= (uint8_t)~(1 << PRPSC1))
#define power_psc1_disable()    (PRR |= (uint8_t)(1 << PRPSC1))

/* Power Stage Controller 2 */
#define power_psc2_enable()     (PRR &= (uint8_t)~(1 << PRPSC2))
#define power_psc2_disable()    (PRR |= (uint8_t)(1 << PRPSC2))

#define power_all_enable()      (PRR &= (uint8_t)~((1<<PRADC)|(1<<PRSPI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRPSC0)|(1<<PRPSC1)|(1<<PRPSC2)))
#define power_all_disable()     (PRR |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRPSC0)|(1<<PRPSC1)|(1<<PRPSC2)))


#elif defined(__AVR_AT90PWM2__) \
|| defined(__AVR_AT90PWM2B__) \
|| defined(__AVR_AT90PWM3__) \
|| defined(__AVR_AT90PWM3B__) \
|| defined(__AVR_AT90PWM216__) \
|| defined(__AVR_AT90PWM316__)

#define power_adc_enable()      (PRR &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR |= (uint8_t)(1 << PRSPI))

#define power_usart_enable()    (PRR &= (uint8_t)~(1 << PRUSART))
#define power_usart_disable()   (PRR |= (uint8_t)(1 << PRUSART))

#define power_timer0_enable()   (PRR &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR |= (uint8_t)(1 << PRTIM1))

/* Power Stage Controller 0 */
#define power_psc0_enable()     (PRR &= (uint8_t)~(1 << PRPSC0))
#define power_psc0_disable()    (PRR |= (uint8_t)(1 << PRPSC0))

/* Power Stage Controller 1 */
#define power_psc1_enable()     (PRR &= (uint8_t)~(1 << PRPSC1))
#define power_psc1_disable()    (PRR |= (uint8_t)(1 << PRPSC1))

/* Power Stage Controller 2 */
#define power_psc2_enable()     (PRR &= (uint8_t)~(1 << PRPSC2))
#define power_psc2_disable()    (PRR |= (uint8_t)(1 << PRPSC2))

#define power_all_enable()      (PRR &= (uint8_t)~((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRPSC0)|(1<<PRPSC1)|(1<<PRPSC2)))
#define power_all_disable()     (PRR |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRPSC0)|(1<<PRPSC1)|(1<<PRPSC2)))


#elif defined(__AVR_ATmega165__) \
|| defined(__AVR_ATmega165P__) \
|| defined(__AVR_ATmega325__) \
|| defined(__AVR_ATmega3250__) \
|| defined(__AVR_ATmega645__) \
|| defined(__AVR_ATmega6450__)

#define power_adc_enable()      (PRR &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR |= (uint8_t)(1 << PRSPI))

#define power_usart0_enable()   (PRR &= (uint8_t)~(1 << PRUSART0))
#define power_usart0_disable()  (PRR |= (uint8_t)(1 << PRUSART0))

#define power_timer1_enable()   (PRR &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR |= (uint8_t)(1 << PRTIM1))

#define power_all_enable()      (PRR &= (uint8_t)~((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRTIM1)))
#define power_all_disable()     (PRR |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRTIM1)))


#elif defined(__AVR_ATmega169__) \
|| defined(__AVR_ATmega169P__) \
|| defined(__AVR_ATmega329__) \
|| defined(__AVR_ATmega3290__) \
|| defined(__AVR_ATmega649__) \
|| defined(__AVR_ATmega6490__)

#define power_adc_enable()      (PRR &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR |= (uint8_t)(1 << PRSPI))

#define power_usart0_enable()   (PRR &= (uint8_t)~(1 << PRUSART0))
#define power_usart0_disable()  (PRR |= (uint8_t)(1 << PRUSART0))

#define power_timer1_enable()   (PRR &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR |= (uint8_t)(1 << PRTIM1))

#define power_lcd_enable()      (PRR &= (uint8_t)~(1 << PRLCD))
#define power_lcd_disable()     (PRR |= (uint8_t)(1 << PRLCD))

#define power_all_enable()      (PRR &= (uint8_t)~((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRTIM1)|(1<<PRLCD)))
#define power_all_disable()     (PRR |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRTIM1)|(1<<PRLCD)))


#elif defined(__AVR_ATmega164P__) \
|| defined(__AVR_ATmega324P__)

#define power_adc_enable()      (PRR0 &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR0 |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR0 &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR0 |= (uint8_t)(1 << PRSPI))

#define power_usart0_enable()   (PRR0 &= (uint8_t)~(1 << PRUSART0))
#define power_usart0_disable()  (PRR0 |= (uint8_t)(1 << PRUSART0))

#define power_usart1_enable()   (PRR0 &= (uint8_t)~(1 << PRUSART1))
#define power_usart1_disable()  (PRR0 |= (uint8_t)(1 << PRUSART1))

#define power_timer0_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR0 |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR0 |= (uint8_t)(1 << PRTIM1))

#define power_timer2_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM2))
#define power_timer2_disable()  (PRR0 |= (uint8_t)(1 << PRTIM2))

#define power_twi_enable()      (PRR0 &= (uint8_t)~(1 << PRTWI))
#define power_twi_disable()     (PRR0 |= (uint8_t)(1 << PRTWI))

#define power_all_enable()      (PRR0 &= (uint8_t)~((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRUSART1)|(1<<PRTIM0)|(1<<PRTIM2)|(1<<PRTWI)))
#define power_all_disable()     (PRR0 |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRUSART1)|(1<<PRTIM0)|(1<<PRTIM2)|(1<<PRTWI)))


#elif defined(__AVR_ATmega644__) \
|| defined(__AVR_ATmega644P__)

#define power_adc_enable()      (PRR0 &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR0 |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR0 &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR0 |= (uint8_t)(1 << PRSPI))

#define power_usart0_enable()   (PRR0 &= (uint8_t)~(1 << PRUSART0))
#define power_usart0_disable()  (PRR0 |= (uint8_t)(1 << PRUSART0))

#define power_timer0_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR0 |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR0 |= (uint8_t)(1 << PRTIM1))

#define power_timer2_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM2))
#define power_timer2_disable()  (PRR0 |= (uint8_t)(1 << PRTIM2))

#define power_twi_enable()      (PRR0 &= (uint8_t)~(1 << PRTWI))
#define power_twi_disable()     (PRR0 |= (uint8_t)(1 << PRTWI))

#define power_all_enable()      (PRR0 &= (uint8_t)~((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)|(1<<PRTWI)))
#define power_all_disable()     (PRR0 |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)|(1<<PRTWI)))


#elif defined(__AVR_ATmega406__)

#define power_twi_enable()      (PRR0 &= (uint8_t)~(1 << PRTWI))
#define power_twi_disable()     (PRR0 |= (uint8_t)(1 << PRTWI))

#define power_timer0_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR0 |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR0 |= (uint8_t)(1 << PRTIM1))

/* Voltage ADC */
#define power_vadc_enable()     (PRR0 &= (uint8_t)~(1 << PRVADC))
#define power_vadc_disable()    (PRR0 |= (uint8_t)(1 << PRVADC))

#define power_all_enable()      (PRR0 &= (uint8_t)~((1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRVADC)))
#define power_all_disable()     (PRR0 |= (uint8_t)((1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRVADC)))


#elif defined(__AVR_ATmega48__) \
|| defined(__AVR_ATmega48P__) \
|| defined(__AVR_ATmega88__) \
|| defined(__AVR_ATmega88P__) \
|| defined(__AVR_ATmega168__) \
|| defined(__AVR_ATmega168P__) \
|| defined(__AVR_ATmega328P__) \
|| defined(__AVR_ATtiny48__) \
|| defined(__AVR_ATtiny88__)

#define power_adc_enable()      (PRR &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR |= (uint8_t)(1 << PRSPI))

#define power_usart0_enable()   (PRR &= (uint8_t)~(1 << PRUSART0))
#define power_usart0_disable()  (PRR |= (uint8_t)(1 << PRUSART0))

#define power_timer0_enable()   (PRR &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR |= (uint8_t)(1 << PRTIM1))

#define power_timer2_enable()   (PRR &= (uint8_t)~(1 << PRTIM2))
#define power_timer2_disable()  (PRR |= (uint8_t)(1 << PRTIM2))

#define power_twi_enable()      (PRR &= (uint8_t)~(1 << PRTWI))
#define power_twi_disable()     (PRR |= (uint8_t)(1 << PRTWI))

#define power_all_enable()      (PRR &= (uint8_t)~((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)|(1<<PRTWI)))
#define power_all_disable()     (PRR |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)|(1<<PRTWI)))


#elif defined(__AVR_ATtiny24__) \
|| defined(__AVR_ATtiny44__) \
|| defined(__AVR_ATtiny84__) \
|| defined(__AVR_ATtiny25__) \
|| defined(__AVR_ATtiny45__) \
|| defined(__AVR_ATtiny85__) \
|| defined(__AVR_ATtiny261__) \
|| defined(__AVR_ATtiny461__) \
|| defined(__AVR_ATtiny861__) \
|| defined(__AVR_ATtiny43U__)

#define power_adc_enable()      (PRR &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR |= (uint8_t)(1 << PRADC))

#define power_timer0_enable()   (PRR &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR |= (uint8_t)(1 << PRTIM1))

/* Universal Serial Interface */
#define power_usi_enable()      (PRR &= (uint8_t)~(1 << PRUSI))
#define power_usi_disable()     (PRR |= (uint8_t)(1 << PRUSI))

#define power_all_enable()      (PRR &= (uint8_t)~((1<<PRADC)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRUSI)))
#define power_all_disable()     (PRR |= (uint8_t)((1<<PRADC)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRUSI)))


#elif defined(__AVR_ATmega1284P__)


#define power_adc_enable()      (PRR0 &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR0 |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR0 &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR0 |= (uint8_t)(1 << PRSPI))

#define power_twi_enable()      (PRR0 &= (uint8_t)~(1 << PRTWI))
#define power_twi_disable()     (PRR0 |= (uint8_t)(1 << PRTWI))

#define power_timer0_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR0 |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR0 |= (uint8_t)(1 << PRTIM1))

#define power_timer2_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM2))
#define power_timer2_disable()  (PRR0 |= (uint8_t)(1 << PRTIM2))

#define power_timer3_enable()   (PRR1 &= (uint8_t)~(1 << PRTIM3))
#define power_timer3_disable()  (PRR1 |= (uint8_t)(1 << PRTIM3))

#define power_usart0_enable()   (PRR0 &= (uint8_t)~(1 << PRUSART0))
#define power_usart0_disable()  (PRR0 |= (uint8_t)(1 << PRUSART0))

#define power_usart1_enable()   (PRR0 &= (uint8_t)~(1 << PRUSART1))
#define power_usart1_disable()  (PRR0 |= (uint8_t)(1 << PRUSART1))

#define power_all_enable() \
do{ \
    PRR0 &= (uint8_t)~((1<<PRADC)|(1<<PRSPI)|(1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)|(1<<PRUSART0)|(1<<PRUSART1)); \
    PRR1 &= (uint8_t)~(1<<PRTIM3); \
}while(0)

#define power_all_disable() \
do{ \
    PRR0 |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)|(1<<PRUSART0)|(1<<PRUSART1)); \
    PRR1 |= (uint8_t)(1<<PRTIM3); \
}while(0)


#elif defined(__AVR_ATmega32HVB__)


#define power_twi_enable()      (PRR0 &= (uint8_t)~(1 << PRTWI))
#define power_twi_disable()     (PRR0 |= (uint8_t)(1 << PRTWI))

#define power_timer0_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR0 |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR0 |= (uint8_t)(1 << PRTIM1))

/* Voltage ADC */
#define power_vadc_enable()     (PRR0 &= (uint8_t)~(1 << PRVADC))
#define power_vadc_disable()    (PRR0 |= (uint8_t)(1 << PRVADC))

#define power_spi_enable()      (PRR0 &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR0 |= (uint8_t)(1 << PRSPI))

#define power_vrm_enable()      (PRR0 &= (uint8_t)~(1 << PRVRM))
#define power_vrm_disable()     (PRR0 |= (uint8_t)(1 << PRVRM))

#define power_all_enable()      (PRR0 &= (uint8_t)~((1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRVADC)|(1<<PRSPI)|(1<<PRVRM)))
#define power_all_disable()     (PRR0 |= (uint8_t)((1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRVADC)|(1<<PRSPI)|(1<<PRVRM)))


#elif defined(__AVR_ATmega32M1__) \
|| defined(__AVR_ATmega32C1__)

#define power_adc_enable()      (PRR &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR |= (uint8_t)(1 << PRADC))

#define power_lin_enable()      (PRR &= (uint8_t)~(1 << PRLIN))
#define power_lin_disable()     (PRR |= (uint8_t)(1 << PRLIN))

#define power_spi_enable()      (PRR &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR |= (uint8_t)(1 << PRSPI))

#define power_timer0_enable()   (PRR &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR |= (uint8_t)(1 << PRTIM1))

#define power_psc_enable()      (PRR &= (uint8_t)~(1 << PRPSC))
#define power_psc_disable()     (PRR |= (uint8_t)(1 << PRPSC))

#define power_can_enable()      (PRR &= (uint8_t)~(1 << PRCAN))
#define power_can_disable()     (PRR |= (uint8_t)(1 << PRCAN))

#define power_all_enable()      (PRR &= (uint8_t)~((1<<PRADC)|(1<<PRLIN)|(1<<PRSPI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRPSC)|(1<<PRCAN)))
#define power_all_disable()     (PRR |= (uint8_t)((1<<PRADC)|(1<<PRLIN)|(1<<PRSPI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRPSC)|(1<<PRCAN)))


#elif defined(__AVR_ATtiny167__)


#define power_adc_enable()      (PRR &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR |= (uint8_t)(1 << PRADC))

#define power_usi_enable()      (PRR &= (uint8_t)~(1 << PRUSI))
#define power_usi_disable()     (PRR |= (uint8_t)(1 << PRUSI))

#define power_timer0_enable()   (PRR &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR |= (uint8_t)(1 << PRTIM1))

#define power_spi_enable()      (PRR &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR |= (uint8_t)(1 << PRSPI))

#define power_lin_enable()      (PRR &= (uint8_t)~(1 << PRLIN))
#define power_lin_disable()     (PRR |= (uint8_t)(1 << PRLIN))

#define power_all_enable()      (PRR &= (uint8_t)~((1<<PRADC)|(1<<PRUSI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRSPI)|(1<<PRLIN)))
#define power_all_disable()     (PRR |= (uint8_t)((1<<PRADC)|(1<<PRUSI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRSPI)|(1<<PRLIN)))


#elif defined(__AVR_AT90USB82__) \
|| defined(__AVR_AT90USB162__)


#define power_spi_enable()      (PRR0 &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR0 |= (uint8_t)(1 << PRSPI))

#define power_timer0_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR0 |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR0 &= (uint8_t)~(1 << PRTIM1))
#define power_timer2_disable()  (PRR0 |= (uint8_t)(1 << PRTIM1))

#define power_usb_enable()      (PRR1 &= (uint8_t)~(1 << PRUSB))
#define power_usb_disable()     (PRR1 |= (uint8_t)(1 << PRUSB))

#define power_usart1_enable()   (PRR1 &= (uint8_t)~(1 << PRUSART1))
#define power_usart1_disable()  (PRR1 |= (uint8_t)(1 << PRUSART1))

#define power_all_enable() \
do{ \
    PRR0 &= (uint8_t)~((1<<PRSPI)|(1<<PRTIM0)|(1<<PRTIM1)); \
    PRR1 &= (uint8_t)~((1<<PRUSB)|(1<<PRUSART1)); \
}while(0)

#define power_all_disable() \
do{ \
    PRR0 |= (uint8_t)((1<<PRSPI)|(1<<PRTIM0)|(1<<PRTIM1)); \
    PRR1 |= (uint8_t)((1<<PRUSB)|(1<<PRUSART1)); \
}while(0)


#endif


#if defined(__AVR_ATmega640__) \
|| defined(__AVR_ATmega1280__) \
|| defined(__AVR_ATmega1281__) \
|| defined(__AVR_ATmega2560__) \
|| defined(__AVR_ATmega2561__) \
|| defined(__AVR_AT90USB646__) \
|| defined(__AVR_AT90USB647__) \
|| defined(__AVR_AT90USB82__) \
|| defined(__AVR_AT90USB1286__) \
|| defined(__AVR_AT90USB1287__) \
|| defined(__AVR_AT90USB162__) \
|| defined(__AVR_AT90CAN32__) \
|| defined(__AVR_AT90CAN64__) \
|| defined(__AVR_AT90CAN128__) \
|| defined(__AVR_AT90PWM1__) \
|| defined(__AVR_AT90PWM2__) \
|| defined(__AVR_AT90PWM2B__) \
|| defined(__AVR_AT90PWM3__) \
|| defined(__AVR_AT90PWM3B__) \
|| defined(__AVR_AT90PWM216__) \
|| defined(__AVR_AT90PWM316__) \
|| defined(__AVR_ATmega32M1__) \
|| defined(__AVR_ATmega1284P__) \
|| defined(__AVR_ATmega162__) \
|| defined(__AVR_ATmega165__) \
|| defined(__AVR_ATmega165P__) \
|| defined(__AVR_ATmega325__) \
|| defined(__AVR_ATmega3250__) \
|| defined(__AVR_ATmega32HVB__) \
|| defined(__AVR_ATmega645__) \
|| defined(__AVR_ATmega6450__) \
|| defined(__AVR_ATmega169__) \
|| defined(__AVR_ATmega169P__) \
|| defined(__AVR_ATmega329__) \
|| defined(__AVR_ATmega3290__) \
|| defined(__AVR_ATmega649__) \
|| defined(__AVR_ATmega6490__) \
|| defined(__AVR_ATmega48__) \
|| defined(__AVR_ATmega48P__) \
|| defined(__AVR_ATmega88__) \
|| defined(__AVR_ATmega88P__) \
|| defined(__AVR_ATmega168__) \
|| defined(__AVR_ATmega168P__) \
|| defined(__AVR_ATmega328P__) \
|| defined(__AVR_ATmega164P__) \
|| defined(__AVR_ATmega324P__) \
|| defined(__AVR_ATmega644__) \
|| defined(__AVR_ATmega644P__) \
|| defined(__AVR_ATtiny48__) \
|| defined(__AVR_ATtiny167__) \
|| defined(__AVR_ATmega32U4__) \
|| defined(__AVR_ATmega32C1__) \
|| defined(__DOXYGEN__)


/** \addtogroup avr_power

Some of the newer AVRs contain a System Clock Prescale Register (CLKPR) that
allows you to decrease the system clock frequency and the power consumption
when the need for processing power is low. Below are two macros and an
enumerated type that can be used to interface to the Clock Prescale Register.

\note Not all AVR devices have a Clock Prescale Register. On those devices
without a Clock Prescale Register, these macros are not available.
*/


/** \addtogroup avr_power
\code 
typedef enum
{
    clock_div_1 = 0,
    clock_div_2 = 1,
    clock_div_4 = 2,
    clock_div_8 = 3,
    clock_div_16 = 4,
    clock_div_32 = 5,
    clock_div_64 = 6,
    clock_div_128 = 7,
    clock_div_256 = 8
} clock_div_t;
\endcode
Clock prescaler setting enumerations.

*/
typedef enum
{
    clock_div_1 = 0,
    clock_div_2 = 1,
    clock_div_4 = 2,
    clock_div_8 = 3,
    clock_div_16 = 4,
    clock_div_32 = 5,
    clock_div_64 = 6,
    clock_div_128 = 7,
    clock_div_256 = 8
} clock_div_t;


/** \addtogroup avr_power
\code clock_prescale_set(x) \endcode
Set the clock prescaler register select bits, selecting a system clock division setting. They type of x is clock_div_t.

*/
#define clock_prescale_set(x) \
{ \
        uint8_t tmp = _BV(CLKPCE); \
        __asm__ __volatile__ ( \
                "in __tmp_reg__,__SREG__" "\n\t" \
                "cli" "\n\t" \
                "sts %1, %0" "\n\t" \
                "sts %1, %2" "\n\t" \
                "out __SREG__, __tmp_reg__" \
                : /* no outputs */ \
                : "d" (tmp), \
                  "M" (_SFR_MEM_ADDR(CLKPR)), \
                  "d" (x) \
                : "r0"); \
}

/** \addtogroup avr_power
\code clock_prescale_get() \endcode
Gets and returns the clock prescaler register setting. The return type is clock_div_t.

*/
#define clock_prescale_get()  (clock_div_t)(CLKPR & (uint8_t)((1<<CLKPS0)|(1<<CLKPS1)|(1<<CLKPS2)|(1<<CLKPS3)))


#elif defined(__AVR_ATtiny24__) \
|| defined(__AVR_ATtiny44__) \
|| defined(__AVR_ATtiny84__) \
|| defined(__AVR_ATtiny25__) \
|| defined(__AVR_ATtiny45__) \
|| defined(__AVR_ATtiny85__) \
|| defined(__AVR_ATtiny261__) \
|| defined(__AVR_ATtiny461__) \
|| defined(__AVR_ATtiny861__) \
|| defined(__AVR_ATtiny2313__) \
|| defined(__AVR_ATtiny13__) \
|| defined(__AVR_ATtiny13A__) \
|| defined(__AVR_ATtiny43U__) \

typedef enum
{
    clock_div_1 = 0,
    clock_div_2 = 1,
    clock_div_4 = 2,
    clock_div_8 = 3,
    clock_div_16 = 4,
    clock_div_32 = 5,
    clock_div_64 = 6,
    clock_div_128 = 7,
    clock_div_256 = 8
} clock_div_t;


#define clock_prescale_set(x) \
{ \
        uint8_t tmp = _BV(CLKPCE); \
        __asm__ __volatile__ ( \
                "in __tmp_reg__,__SREG__" "\n\t" \
                "cli" "\n\t" \
                "out %1, %0" "\n\t" \
                "out %1, %2" "\n\t" \
                "out __SREG__, __tmp_reg__" \
                : /* no outputs */ \
                : "d" (tmp), \
                  "I" (_SFR_IO_ADDR(CLKPR)), \
                  "d" (x) \
                : "r0"); \
}


#define clock_prescale_get()  (clock_div_t)(CLKPR & (uint8_t)((1<<CLKPS0)|(1<<CLKPS1)|(1<<CLKPS2)|(1<<CLKPS3)))


#endif




#endif /* _AVR_POWER_H_ */
