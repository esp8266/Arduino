/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief AT32UC3A EVK1100 board LEDs support package.
 *
 * This file contains definitions and services related to the LED features of
 * the EVK1100 board.
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

#include <avr32/io.h>
#include "preprocessor.h"
#include "compiler.h"
#include "arduino.h"
#include "led.h"


//! Structure describing LED hardware connections.
typedef const struct
{
  struct
  {
    U32 PORT;     //!< LED GPIO port.
    U32 PIN_MASK; //!< Bit-mask of LED pin in GPIO port.
  } GPIO; //!< LED GPIO descriptor.
  struct
  {
    S32 CHANNEL;  //!< LED PWM channel (< 0 if N/A).
    S32 FUNCTION; //!< LED pin PWM function (< 0 if N/A).
  } PWM;  //!< LED PWM descriptor.
} tLED_DESCRIPTOR;


//! Hardware descriptors of all LEDs.
static tLED_DESCRIPTOR LED_DESCRIPTOR[LED_COUNT] =
{
#define INSERT_LED_DESCRIPTOR(LED_NO, unused)                 \
  {                                                           \
    {LED##LED_NO##_GPIO / 32, 1 << (LED##LED_NO##_GPIO % 32)},\
    {LED##LED_NO##_PWM,       LED##LED_NO##_PWM_FUNCTION    } \
  },
  MREPEAT(LED_COUNT, INSERT_LED_DESCRIPTOR, ~)
#undef INSERT_LED_DESCRIPTOR
};


//! Saved state of all LEDs.
static volatile U32 LED_State = (1 << LED_COUNT) - 1;


U32 LED_Read_Display(void)
{
  return LED_State;
}


void LED_Display(U32 leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor;
  volatile avr32_gpio_port_t *led_gpio_port;

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  LED_State = leds;

  // For all LEDs...
  for (led_descriptor = &LED_DESCRIPTOR[0];
       led_descriptor < LED_DESCRIPTOR + LED_COUNT;
       led_descriptor++)
  {
    // Set the LED to the requested state.
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    if (leds & 1)
    {
      led_gpio_port->ovrc  = led_descriptor->GPIO.PIN_MASK;
    }
    else
    {
      led_gpio_port->ovrs  = led_descriptor->GPIO.PIN_MASK;
    }
    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK;
    leds >>= 1;
  }
}


U32 LED_Read_Display_Mask(U32 mask)
{
  return Rd_bits(LED_State, mask);
}


void LED_Display_Mask(U32 mask, U32 leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
  U8 led_shift;

  // Make sure only existing LEDs are specified.
  mask &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Wr_bits(LED_State, mask, leds);

  // While there are specified LEDs left to manage...
  while (mask)
  {
    // Select the next specified LED and set it to the requested state.
    led_shift = 1 + ctz(mask);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    leds >>= led_shift - 1;
    if (leds & 1)
    {
      led_gpio_port->ovrc  = led_descriptor->GPIO.PIN_MASK;
    }
    else
    {
      led_gpio_port->ovrs  = led_descriptor->GPIO.PIN_MASK;
    }
    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK;
    leds >>= 1;
    mask >>= led_shift;
  }
}


Bool LED_Test(U32 leds)
{
  return Tst_bits(LED_State, leds);
}


void LED_Off(U32 leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
  U8 led_shift;

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Clr_bits(LED_State, leds);

  // While there are specified LEDs left to manage...
  while (leds)
  {
    // Select the next specified LED and turn it off.
    led_shift = 1 + ctz(leds);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    led_gpio_port->ovrs  = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK;
    leds >>= led_shift;
  }
}


void LED_On(U32 leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
  U8 led_shift;

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Set_bits(LED_State, leds);

  // While there are specified LEDs left to manage...
  while (leds)
  {
    // Select the next specified LED and turn it on.
    led_shift = 1 + ctz(leds);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    led_gpio_port->ovrc  = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK;
    leds >>= led_shift;
  }
}


void LED_Toggle(U32 leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
  U8 led_shift;

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Tgl_bits(LED_State, leds);

  // While there are specified LEDs left to manage...
  while (leds)
  {
    // Select the next specified LED and toggle it.
    led_shift = 1 + ctz(leds);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    led_gpio_port->ovrt  = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK;
    leds >>= led_shift;
  }
}


U32 LED_Read_Display_Field(U32 field)
{
  return Rd_bitfield(LED_State, field);
}


void LED_Display_Field(U32 field, U32 leds)
{
  // Move the bit-field to the appropriate position for the bit-mask.
  LED_Display_Mask(field, leds << ctz(field));
}


U8 LED_Get_Intensity(U32 led)
{
  tLED_DESCRIPTOR *led_descriptor;

  // Check that the argument value is valid.
  led = ctz(led);
  led_descriptor = &LED_DESCRIPTOR[led];
  if (led >= LED_COUNT || led_descriptor->PWM.CHANNEL < 0) return 0;

  // Return the duty cycle value if the LED PWM channel is enabled, else 0.
  return (AVR32_PWM.sr & (1 << led_descriptor->PWM.CHANNEL)) ?
           AVR32_PWM.channel[led_descriptor->PWM.CHANNEL].cdty : 0;
}


void LED_Set_Intensity(U32 leds, U8 intensity)
{
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_pwm_channel_t *led_pwm_channel;
  volatile avr32_gpio_port_t *led_gpio_port;
  U8 led_shift;

  // For each specified LED...
  for (leds &= (1 << LED_COUNT) - 1; leds; leds >>= led_shift)
  {
    // Select the next specified LED and check that it has a PWM channel.
    led_shift = 1 + ctz(leds);
    led_descriptor += led_shift;
    if (led_descriptor->PWM.CHANNEL < 0) continue;

    // Initialize or update the LED PWM channel.
    led_pwm_channel = &AVR32_PWM.channel[led_descriptor->PWM.CHANNEL];
    if (!(AVR32_PWM.sr & (1 << led_descriptor->PWM.CHANNEL)))
    {
      led_pwm_channel->cmr = (AVR32_PWM_CPRE_MCK << AVR32_PWM_CPRE_OFFSET) &
                             ~(AVR32_PWM_CALG_MASK |
                               AVR32_PWM_CPOL_MASK |
                               AVR32_PWM_CPD_MASK);
      led_pwm_channel->cprd = 0x000000FF;
      led_pwm_channel->cdty = intensity;
      AVR32_PWM.ena = 1 << led_descriptor->PWM.CHANNEL;
    }
    else
    {
      AVR32_PWM.isr;
      while (!(AVR32_PWM.isr & (1 << led_descriptor->PWM.CHANNEL)));
      led_pwm_channel->cupd = intensity;
    }

    // Switch the LED pin to its PWM function.
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    if (led_descriptor->PWM.FUNCTION & 0x1)
    {
      led_gpio_port->pmr0s = led_descriptor->GPIO.PIN_MASK;
    }
    else
    {
      led_gpio_port->pmr0c = led_descriptor->GPIO.PIN_MASK;
    }
    if (led_descriptor->PWM.FUNCTION & 0x2)
    {
      led_gpio_port->pmr1s = led_descriptor->GPIO.PIN_MASK;
    }
    else
    {
      led_gpio_port->pmr1c = led_descriptor->GPIO.PIN_MASK;
    }
    led_gpio_port->gperc = led_descriptor->GPIO.PIN_MASK;
  }
}
