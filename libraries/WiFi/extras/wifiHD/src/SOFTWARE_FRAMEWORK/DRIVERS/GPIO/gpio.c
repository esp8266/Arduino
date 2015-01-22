/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief GPIO driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the GPIO.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a GPIO module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 *****************************************************************************/

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

#include "gpio.h"

//! GPIO module instance.
#define GPIO  AVR32_GPIO


/*! \name Peripheral Bus Interface
 */
//! @{


int gpio_enable_module(const gpio_map_t gpiomap, unsigned int size)
{
  int status = GPIO_SUCCESS;
  unsigned int i;

  for (i = 0; i < size; i++)
  {
    status |= gpio_enable_module_pin(gpiomap->pin, gpiomap->function);
    gpiomap++;
  }

  return status;
}


int gpio_enable_module_pin(unsigned int pin, unsigned int function)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];

  // Enable the correct function.
  switch (function)
  {
  case 0: // A function.
    gpio_port->pmr0c = 1 << (pin & 0x1F);
    gpio_port->pmr1c = 1 << (pin & 0x1F);
#if defined(AVR32_GPIO_210_H_INCLUDED) || defined(AVR32_GPIO_211_H_INCLUDED)
    gpio_port->pmr2c = 1 << (pin & 0x1F);
#endif
    break;

  case 1: // B function.
    gpio_port->pmr0s = 1 << (pin & 0x1F);
    gpio_port->pmr1c = 1 << (pin & 0x1F);
#if defined(AVR32_GPIO_210_H_INCLUDED) || defined(AVR32_GPIO_211_H_INCLUDED)
    gpio_port->pmr2c = 1 << (pin & 0x1F);
#endif
    break;

  case 2: // C function.
    gpio_port->pmr0c = 1 << (pin & 0x1F);
    gpio_port->pmr1s = 1 << (pin & 0x1F);
#if defined(AVR32_GPIO_210_H_INCLUDED) || defined(AVR32_GPIO_211_H_INCLUDED)
    gpio_port->pmr2c = 1 << (pin & 0x1F);
#endif
    break;

  case 3: // D function.
    gpio_port->pmr0s = 1 << (pin & 0x1F);
    gpio_port->pmr1s = 1 << (pin & 0x1F);
#if defined(AVR32_GPIO_210_H_INCLUDED) || defined(AVR32_GPIO_211_H_INCLUDED)
    gpio_port->pmr2c = 1 << (pin & 0x1F);
#endif
    break;

#if defined(AVR32_GPIO_210_H_INCLUDED) || defined(AVR32_GPIO_211_H_INCLUDED)
  case 4: // E function.
    gpio_port->pmr0c = 1 << (pin & 0x1F);
    gpio_port->pmr1c = 1 << (pin & 0x1F);
    gpio_port->pmr2s = 1 << (pin & 0x1F);
    break;
    
  case 5: // F function.
    gpio_port->pmr0s = 1 << (pin & 0x1F);
    gpio_port->pmr1c = 1 << (pin & 0x1F);
    gpio_port->pmr2s = 1 << (pin & 0x1F);
    break;
    
  case 6: // G function.
    gpio_port->pmr0c = 1 << (pin & 0x1F);
    gpio_port->pmr1s = 1 << (pin & 0x1F);
    gpio_port->pmr2s = 1 << (pin & 0x1F);
    break;
    
  case 7: // H function.
    gpio_port->pmr0s = 1 << (pin & 0x1F);
    gpio_port->pmr1s = 1 << (pin & 0x1F);
    gpio_port->pmr2s = 1 << (pin & 0x1F);
    break;
#endif

  default:
    return GPIO_INVALID_ARGUMENT;
  }

  // Disable GPIO control.
  gpio_port->gperc = 1 << (pin & 0x1F);

  return GPIO_SUCCESS;
}


void gpio_enable_gpio(const gpio_map_t gpiomap, unsigned int size)
{
  unsigned int i;

  for (i = 0; i < size; i++)
  {
    gpio_enable_gpio_pin(gpiomap->pin);
    gpiomap++;
  }
}


void gpio_enable_gpio_pin(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->oderc = 1 << (pin & 0x1F);
  gpio_port->gpers = 1 << (pin & 0x1F);
}


// The open-drain mode is not synthesized on the current AVR32 products.
// If one day some AVR32 products have this feature, the corresponding part
// numbers should be listed in the #if below.
// Note that other functions are available in this driver to use pins with open
// drain in GPIO mode. The advantage of the open-drain mode functions over these
// other functions is that they can be used not only in GPIO mode but also in
// module mode.
#if 0


void gpio_enable_pin_open_drain(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->odmers = 1 << (pin & 0x1F);
}


void gpio_disable_pin_open_drain(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->odmerc = 1 << (pin & 0x1F);
}


#endif


void gpio_enable_pin_pull_up(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->puers = 1 << (pin & 0x1F);
#if defined(AVR32_GPIO_200_H_INCLUDED) || defined(AVR32_GPIO_210_H_INCLUDED) || defined(AVR32_GPIO_211_H_INCLUDED)
  gpio_port->pderc = 1 << (pin & 0x1F);
#endif
}


void gpio_disable_pin_pull_up(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->puerc = 1 << (pin & 0x1F);
}

#if defined(AVR32_GPIO_200_H_INCLUDED) || defined(AVR32_GPIO_210_H_INCLUDED) || defined(AVR32_GPIO_211_H_INCLUDED)
// Added support of Pull-up Resistor, Pull-down Resistor and Buskeeper Control.

/*! \brief Enables the pull-down resistor of a pin.
 *
 * \param pin The pin number.
 */
void gpio_enable_pin_pull_down(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->puerc = 1 << (pin & 0x1F);
  gpio_port->pders = 1 << (pin & 0x1F);
}

/*! \brief Disables the pull-down resistor of a pin.
 *
 * \param pin The pin number.
 */
void gpio_disable_pin_pull_down(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->pderc = 1 << (pin & 0x1F);
}

/*! \brief Enables the buskeeper functionality on a pin.
 *
 * \param pin The pin number.
 */
void gpio_enable_pin_buskeeper(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->puers = 1 << (pin & 0x1F);
  gpio_port->pders = 1 << (pin & 0x1F);
}

/*! \brief Disables the buskeeper functionality on a pin.
 *
 * \param pin The pin number.
 */
void gpio_disable_pin_buskeeper(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->puerc = 1 << (pin & 0x1F);
  gpio_port->pderc = 1 << (pin & 0x1F);
}

#endif

int gpio_get_pin_value(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  return (gpio_port->pvr >> (pin & 0x1F)) & 1;
}


int gpio_get_gpio_pin_output_value(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  return (gpio_port->ovr >> (pin & 0x1F)) & 1;
}


int gpio_get_gpio_open_drain_pin_output_value(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  return ((gpio_port->oder >> (pin & 0x1F)) & 1) ^ 1;
}


void gpio_set_gpio_pin(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];

  gpio_port->ovrs  = 1 << (pin & 0x1F); // Value to be driven on the I/O line: 1.
  gpio_port->oders = 1 << (pin & 0x1F); // The GPIO output driver is enabled for that pin.
  gpio_port->gpers = 1 << (pin & 0x1F); // The GPIO module controls that pin.
}


void gpio_clr_gpio_pin(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];

  gpio_port->ovrc  = 1 << (pin & 0x1F); // Value to be driven on the I/O line: 0.
  gpio_port->oders = 1 << (pin & 0x1F); // The GPIO output driver is enabled for that pin.
  gpio_port->gpers = 1 << (pin & 0x1F); // The GPIO module controls that pin.
}


void gpio_tgl_gpio_pin(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];

  gpio_port->ovrt  = 1 << (pin & 0x1F); // Toggle the I/O line.
  gpio_port->oders = 1 << (pin & 0x1F); // The GPIO output driver is enabled for that pin.
  gpio_port->gpers = 1 << (pin & 0x1F); // The GPIO module controls that pin.
}


void gpio_set_gpio_open_drain_pin(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];

  gpio_port->oderc = 1 << (pin & 0x1F); // The GPIO output driver is disabled for that pin.
  gpio_port->gpers = 1 << (pin & 0x1F); // The GPIO module controls that pin.
}


void gpio_clr_gpio_open_drain_pin(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];

  gpio_port->ovrc  = 1 << (pin & 0x1F); // Value to be driven on the I/O line: 0.
  gpio_port->oders = 1 << (pin & 0x1F); // The GPIO output driver is enabled for that pin.
  gpio_port->gpers = 1 << (pin & 0x1F); // The GPIO module controls that pin.
}


void gpio_tgl_gpio_open_drain_pin(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];

  gpio_port->ovrc  = 1 << (pin & 0x1F); // Value to be driven on the I/O line if the GPIO output driver is enabled: 0.
  gpio_port->odert = 1 << (pin & 0x1F); // The GPIO output driver is toggled for that pin.
  gpio_port->gpers = 1 << (pin & 0x1F); // The GPIO module controls that pin.
}


void gpio_enable_pin_glitch_filter(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->gfers = 1 << (pin & 0x1F);
}


void gpio_disable_pin_glitch_filter(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->gferc = 1 << (pin & 0x1F);
}

/*! \brief Configure the edge detector of an input pin
 *
 * \param pin The pin number.
 * \param mode The edge detection mode (\ref GPIO_PIN_CHANGE, \ref GPIO_RISING_EDGE
 *             or \ref GPIO_FALLING_EDGE).
 *
 * \return \ref GPIO_SUCCESS or \ref GPIO_INVALID_ARGUMENT.
 */
static int gpio_configure_edge_detector(unsigned int pin, unsigned int mode)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  
  // Configure the edge detector.
  switch (mode)
  {
  case GPIO_PIN_CHANGE:
    gpio_port->imr0c = 1 << (pin & 0x1F);
    gpio_port->imr1c = 1 << (pin & 0x1F);
    break;

  case GPIO_RISING_EDGE:
    gpio_port->imr0s = 1 << (pin & 0x1F);
    gpio_port->imr1c = 1 << (pin & 0x1F);
    break;

  case GPIO_FALLING_EDGE:
    gpio_port->imr0c = 1 << (pin & 0x1F);
    gpio_port->imr1s = 1 << (pin & 0x1F);
    break;

  default:
    return GPIO_INVALID_ARGUMENT;
  }

  return GPIO_SUCCESS;
}


int gpio_enable_pin_interrupt(unsigned int pin, unsigned int mode)
{
  volatile avr32_gpio_port_t  *gpio_port = &GPIO.port[pin >> 5];

  // Enable the glitch filter.
  gpio_port->gfers = 1 << (pin & 0x1F);

  // Configure the edge detector.
  if(GPIO_INVALID_ARGUMENT == gpio_configure_edge_detector(pin, mode))
    return(GPIO_INVALID_ARGUMENT);

  // Enable interrupt.
  gpio_port->iers = 1 << (pin & 0x1F);

  return GPIO_SUCCESS;
}


void gpio_disable_pin_interrupt(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->ierc = 1 << (pin & 0x1F);
}


int gpio_get_pin_interrupt_flag(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  return (gpio_port->ifr >> (pin & 0x1F)) & 1;
}


void gpio_clear_pin_interrupt_flag(unsigned int pin)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];
  gpio_port->ifrc = 1 << (pin & 0x1F);
}


//#
//# Peripheral Event System Support.
//#
#if UC3L
int gpio_configure_pin_periph_event_mode(unsigned int pin, unsigned int mode, unsigned int use_igf)
{
  volatile avr32_gpio_port_t *gpio_port = &GPIO.port[pin >> 5];

  if(TRUE == use_igf)
  {
    // Enable the glitch filter.
    gpio_port->gfers = 1 << (pin & 0x1F);
  }
  else
  {
    // Disable the glitch filter.
    gpio_port->gferc = 1 << (pin & 0x1F);
  }

  // Configure the edge detector.
  return(gpio_configure_edge_detector(pin, mode));
}

#endif

//! @}
