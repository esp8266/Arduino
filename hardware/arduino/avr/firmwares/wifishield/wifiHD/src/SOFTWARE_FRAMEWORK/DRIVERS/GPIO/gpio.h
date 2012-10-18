/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief GPIO header for AVR32 UC3.
 *
 * This file contains basic GPIO driver functions.
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

#ifndef _GPIO_H_
#define _GPIO_H_

#include <avr32/io.h>
#include "compiler.h"

/*! \name Return Values of the GPIO API
 */
//! @{
#define GPIO_SUCCESS            0 //!< Function successfully completed.
#define GPIO_INVALID_ARGUMENT   1 //!< Input parameters are out of range.
//! @}


/*! \name Interrupt Trigger Modes
 */
//! @{
#define GPIO_PIN_CHANGE         0 //!< Interrupt triggered upon pin change.
#define GPIO_RISING_EDGE        1 //!< Interrupt triggered upon rising edge.
#define GPIO_FALLING_EDGE       2 //!< Interrupt triggered upon falling edge.
//! @}


//! A type definition of pins and modules connectivity.
typedef struct
{
  unsigned char pin;              //!< Module pin.
  unsigned char function;         //!< Module function.
} gpio_map_t[];


/*! \name Peripheral Bus Interface
 *
 * Low-speed interface with a non-deterministic number of clock cycles per
 * access.
 *
 * This interface operates with lower clock frequencies (fPB <= fCPU), and its
 * timing is not deterministic since it needs to access a shared bus which may
 * be heavily loaded.
 *
 * \note This interface is immediately available without initialization.
 */
//! @{

/*! \brief Enables specific module modes for a set of pins.
 *
 * \param gpiomap The pin map.
 * \param size The number of pins in \a gpiomap.
 *
 * \return \ref GPIO_SUCCESS or \ref GPIO_INVALID_ARGUMENT.
 */
extern int gpio_enable_module(const gpio_map_t gpiomap, unsigned int size);

/*! \brief Enables a specific module mode for a pin.
 *
 * \param pin The pin number.\n
 *            Refer to the product header file `uc3x.h' (where x is the part
 *            number; e.g. x = a0512) for module pins. E.g., to enable a PWM
 *            channel output, the pin number can be AVR32_PWM_3_PIN for PWM
 *            channel 3.
 * \param function The pin function.\n
 *                 Refer to the product header file `uc3x.h' (where x is the
 *                 part number; e.g. x = a0512) for module pin functions. E.g.,
 *                 to enable a PWM channel output, the pin function can be
 *                 AVR32_PWM_3_FUNCTION for PWM channel 3.
 *
 * \return \ref GPIO_SUCCESS or \ref GPIO_INVALID_ARGUMENT.
 */
extern int gpio_enable_module_pin(unsigned int pin, unsigned int function);

/*! \brief Enables the GPIO mode of a set of pins.
 *
 * \param gpiomap The pin map.
 * \param size The number of pins in \a gpiomap.
 */
extern void gpio_enable_gpio(const gpio_map_t gpiomap, unsigned int size);

/*! \brief Enables the GPIO mode of a pin.
 *
 * \param pin The pin number.\n
 *            Refer to the product header file `uc3x.h' (where x is the part
 *            number; e.g. x = a0512) for pin definitions. E.g., to enable the
 *            GPIO mode of PX21, AVR32_PIN_PX21 can be used. Module pins such as
 *            AVR32_PWM_3_PIN for PWM channel 3 can also be used to release
 *            module pins for GPIO.
 */
extern void gpio_enable_gpio_pin(unsigned int pin);

// The open-drain mode is not synthesized on the current AVR32 products.
// If one day some AVR32 products have this feature, the corresponding part
// numbers should be listed in the #if below.
// Note that other functions are available in this driver to use pins with open
// drain in GPIO mode. The advantage of the open-drain mode functions over these
// other functions is that they can be used not only in GPIO mode but also in
// module mode.
#if 0

/*! \brief Enables the open-drain mode of a pin.
 *
 * \param pin The pin number.
 */
extern void gpio_enable_pin_open_drain(unsigned int pin);

/*! \brief Disables the open-drain mode of a pin.
 *
 * \param pin The pin number.
 */
extern void gpio_disable_pin_open_drain(unsigned int pin);

#endif

/*! \brief Enables the pull-up resistor of a pin.
 *
 * \param pin The pin number.
 */
extern void gpio_enable_pin_pull_up(unsigned int pin);

/*! \brief Disables the pull-up resistor of a pin.
 *
 * \param pin The pin number.
 */
extern void gpio_disable_pin_pull_up(unsigned int pin);

#if defined(AVR32_GPIO_200_H_INCLUDED) || defined(AVR32_GPIO_210_H_INCLUDED) || defined(AVR32_GPIO_211_H_INCLUDED)
// Added support of Pull-up Resistor, Pull-down Resistor and Buskeeper Control.

/*! \brief Enables the pull-down resistor of a pin.
 *
 * \param pin The pin number.
 */
extern void gpio_enable_pin_pull_down(unsigned int pin);

/*! \brief Disables the pull-down resistor of a pin.
 *
 * \param pin The pin number.
 */
extern void gpio_disable_pin_pull_down(unsigned int pin);

/*! \brief Enables the buskeeper functionality on a pin.
 *
 * \param pin The pin number.
 */
extern void gpio_enable_pin_buskeeper(unsigned int pin);

/*! \brief Disables the buskeeper functionality on a pin.
 *
 * \param pin The pin number.
 */
extern void gpio_disable_pin_buskeeper(unsigned int pin);

#endif

/*! \brief Returns the value of a pin.
 *
 * \param pin The pin number.
 *
 * \return The pin value.
 */
extern int gpio_get_pin_value(unsigned int pin);

/*! \brief Returns the output value set for a GPIO pin.
 *
 * \param pin The pin number.
 *
 * \return The pin output value.
 *
 * \note This function must be used in conjunction with \ref gpio_set_gpio_pin,
 *       \ref gpio_clr_gpio_pin and \ref gpio_tgl_gpio_pin.
 */
extern int gpio_get_gpio_pin_output_value(unsigned int pin);

/*! \brief Returns the output value set for a GPIO pin using open drain.
 *
 * \param pin The pin number.
 *
 * \return The pin output value.
 *
 * \note This function must be used in conjunction with
 *       \ref gpio_set_gpio_open_drain_pin, \ref gpio_clr_gpio_open_drain_pin
 *       and \ref gpio_tgl_gpio_open_drain_pin.
 */
extern int gpio_get_gpio_open_drain_pin_output_value(unsigned int pin);

/*! \brief Drives a GPIO pin to 1.
 *
 * \param pin The pin number.
 */
extern void gpio_set_gpio_pin(unsigned int pin);

/*! \brief Drives a GPIO pin to 0.
 *
 * \param pin The pin number.
 */
extern void gpio_clr_gpio_pin(unsigned int pin);

/*! \brief Toggles a GPIO pin.
 *
 * \param pin The pin number.
 */
extern void gpio_tgl_gpio_pin(unsigned int pin);

/*! \brief Drives a GPIO pin to 1 using open drain.
 *
 * \param pin The pin number.
 */
extern void gpio_set_gpio_open_drain_pin(unsigned int pin);

/*! \brief Drives a GPIO pin to 0 using open drain.
 *
 * \param pin The pin number.
 */
extern void gpio_clr_gpio_open_drain_pin(unsigned int pin);

/*! \brief Toggles a GPIO pin using open drain.
 *
 * \param pin The pin number.
 */
extern void gpio_tgl_gpio_open_drain_pin(unsigned int pin);

/*! \brief Enables the glitch filter of a pin.
 *
 * When the glitch filter is enabled, a glitch with duration of less than 1
 * clock cycle is automatically rejected, while a pulse with duration of 2 clock
 * cycles or more is accepted. For pulse durations between 1 clock cycle and 2
 * clock cycles, the pulse may or may not be taken into account, depending on
 * the precise timing of its occurrence. Thus for a pulse to be guaranteed
 * visible it must exceed 2 clock cycles, whereas for a glitch to be reliably
 * filtered out, its duration must not exceed 1 clock cycle. The filter
 * introduces 2 clock cycles latency.
 *
 * \param pin The pin number.
 */
extern void gpio_enable_pin_glitch_filter(unsigned int pin);

/*! \brief Disables the glitch filter of a pin.
 *
 * \param pin The pin number.
 */
extern void gpio_disable_pin_glitch_filter(unsigned int pin);

/*! \brief Enables the interrupt of a pin with the specified settings.
 *
 * \param pin The pin number.
 * \param mode The trigger mode (\ref GPIO_PIN_CHANGE, \ref GPIO_RISING_EDGE or
 *             \ref GPIO_FALLING_EDGE).
 *
 * \return \ref GPIO_SUCCESS or \ref GPIO_INVALID_ARGUMENT.
 */
extern int gpio_enable_pin_interrupt(unsigned int pin, unsigned int mode);

/*! \brief Disables the interrupt of a pin.
 *
 * \param pin The pin number.
 */
extern void gpio_disable_pin_interrupt(unsigned int pin);

/*! \brief Gets the interrupt flag of a pin.
 *
 * \param pin The pin number.
 *
 * \return The pin interrupt flag.
 */
extern int gpio_get_pin_interrupt_flag(unsigned int pin);

/*! \brief Clears the interrupt flag of a pin.
 *
 * \param pin The pin number.
 */
extern void gpio_clear_pin_interrupt_flag(unsigned int pin);

//! @}


#if (defined AVR32_GPIO_LOCAL_ADDRESS)
/*! \name Local Bus Interface
 *
 * High-speed interface with only one clock cycle per access.
 *
 * This interface operates with high clock frequency (fCPU), and its timing is
 * deterministic since it does not need to access a shared bus which may be
 * heavily loaded.
 *
 * \warning To use this interface, the clock frequency of the peripheral bus on
 *          which the GPIO peripheral is connected must be set to the CPU clock
 *          frequency (fPB = fCPU).
 *
 * \note This interface has to be initialized in order to be available.
 */
//! @{

/*! \brief Enables the local bus interface for GPIO.
 *
 * \note This function must have been called at least once before using other
 *       functions in this interface.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_local_init(void)
{
  Set_system_register(AVR32_CPUCR,
                      Get_system_register(AVR32_CPUCR) | AVR32_CPUCR_LOCEN_MASK);
}

/*! \brief Enables the output driver of a pin.
 *
 * \param pin The pin number.
 *
 * \note \ref gpio_local_init must have been called beforehand.
 *
 * \note This function does not enable the GPIO mode of the pin.
 *       \ref gpio_enable_gpio_pin can be called for this purpose.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_local_enable_pin_output_driver(unsigned int pin)
{
  AVR32_GPIO_LOCAL.port[pin >> 5].oders = 1 << (pin & 0x1F);
}

/*! \brief Disables the output driver of a pin.
 *
 * \param pin The pin number.
 *
 * \note \ref gpio_local_init must have been called beforehand.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_local_disable_pin_output_driver(unsigned int pin)
{
  AVR32_GPIO_LOCAL.port[pin >> 5].oderc = 1 << (pin & 0x1F);
}

/*! \brief Returns the value of a pin.
 *
 * \param pin The pin number.
 *
 * \return The pin value.
 *
 * \note \ref gpio_local_init must have been called beforehand.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ int gpio_local_get_pin_value(unsigned int pin)
{
  return (AVR32_GPIO_LOCAL.port[pin >> 5].pvr >> (pin & 0x1F)) & 1;
}

/*! \brief Drives a GPIO pin to 1.
 *
 * \param pin The pin number.
 *
 * \note \ref gpio_local_init must have been called beforehand.
 *
 * \note This function does not enable the GPIO mode of the pin nor its output
 *       driver. \ref gpio_enable_gpio_pin and
 *       \ref gpio_local_enable_pin_output_driver can be called for this
 *       purpose.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_local_set_gpio_pin(unsigned int pin)
{
  AVR32_GPIO_LOCAL.port[pin >> 5].ovrs = 1 << (pin & 0x1F);
}

/*! \brief Drives a GPIO pin to 0.
 *
 * \param pin The pin number.
 *
 * \note \ref gpio_local_init must have been called beforehand.
 *
 * \note This function does not enable the GPIO mode of the pin nor its output
 *       driver. \ref gpio_enable_gpio_pin and
 *       \ref gpio_local_enable_pin_output_driver can be called for this
 *       purpose.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_local_clr_gpio_pin(unsigned int pin)
{
  AVR32_GPIO_LOCAL.port[pin >> 5].ovrc = 1 << (pin & 0x1F);
}

/*! \brief Toggles a GPIO pin.
 *
 * \param pin The pin number.
 *
 * \note \ref gpio_local_init must have been called beforehand.
 *
 * \note This function does not enable the GPIO mode of the pin nor its output
 *       driver. \ref gpio_enable_gpio_pin and
 *       \ref gpio_local_enable_pin_output_driver can be called for this
 *       purpose.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_local_tgl_gpio_pin(unsigned int pin)
{
  AVR32_GPIO_LOCAL.port[pin >> 5].ovrt = 1 << (pin & 0x1F);
}

/*! \brief Initializes the configuration of a GPIO pin so that it can be used
 *         with GPIO open-drain functions.
 *
 * \note This function must have been called at least once before using
 *       \ref gpio_local_set_gpio_open_drain_pin,
 *       \ref gpio_local_clr_gpio_open_drain_pin or
 *       \ref gpio_local_tgl_gpio_open_drain_pin.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_local_init_gpio_open_drain_pin(unsigned int pin)
{
  AVR32_GPIO_LOCAL.port[pin >> 5].ovrc = 1 << (pin & 0x1F);
}

/*! \brief Drives a GPIO pin to 1 using open drain.
 *
 * \param pin The pin number.
 *
 * \note \ref gpio_local_init and \ref gpio_local_init_gpio_open_drain_pin must
 *       have been called beforehand.
 *
 * \note This function does not enable the GPIO mode of the pin.
 *       \ref gpio_enable_gpio_pin can be called for this purpose.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_local_set_gpio_open_drain_pin(unsigned int pin)
{
  AVR32_GPIO_LOCAL.port[pin >> 5].oderc = 1 << (pin & 0x1F);
}

/*! \brief Drives a GPIO pin to 0 using open drain.
 *
 * \param pin The pin number.
 *
 * \note \ref gpio_local_init and \ref gpio_local_init_gpio_open_drain_pin must
 *       have been called beforehand.
 *
 * \note This function does not enable the GPIO mode of the pin.
 *       \ref gpio_enable_gpio_pin can be called for this purpose.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_local_clr_gpio_open_drain_pin(unsigned int pin)
{
  AVR32_GPIO_LOCAL.port[pin >> 5].oders = 1 << (pin & 0x1F);
}

/*! \brief Toggles a GPIO pin using open drain.
 *
 * \param pin The pin number.
 *
 * \note \ref gpio_local_init and \ref gpio_local_init_gpio_open_drain_pin must
 *       have been called beforehand.
 *
 * \note This function does not enable the GPIO mode of the pin.
 *       \ref gpio_enable_gpio_pin can be called for this purpose.
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_local_tgl_gpio_open_drain_pin(unsigned int pin)
{
  AVR32_GPIO_LOCAL.port[pin >> 5].odert = 1 << (pin & 0x1F);
}

//! @}
#endif // AVR32_GPIO_LOCAL_ADDRESS

#if UC3L
//! @{
/*! \name Peripheral Event System support
 *
 * The GPIO can be programmed to output peripheral events whenever an interrupt
 * condition is detected, such as pin value change, or only when a rising or
 * falling edge is detected.
 *
 */

/*! \brief Enables the peripheral event generation of a pin.
 *
 * \param pin The pin number.
 *
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_enable_pin_periph_event(unsigned int pin)
{
  AVR32_GPIO.port[pin >> 5].oderc = 1 << (pin & 0x1F); // The GPIO output driver is disabled for that pin.
  AVR32_GPIO.port[pin >> 5].evers = 1 << (pin & 0x1F);
}

/*! \brief Disables the peripheral event generation of a pin.
 *
 * \param pin The pin number.
 *
 */
#if (defined __GNUC__)
__attribute__((__always_inline__))
#endif
extern __inline__ void gpio_disable_pin_periph_event(unsigned int pin)
{
  AVR32_GPIO.port[pin >> 5].everc = 1 << (pin & 0x1F);
}

/*! \brief Configure the peripheral event trigger mode of a pin
 *
 * \param pin The pin number.
 * \param mode The trigger mode (\ref GPIO_PIN_CHANGE, \ref GPIO_RISING_EDGE or
 *             \ref GPIO_FALLING_EDGE).
 * \param use_igf use the Input Glitch Filter (TRUE) or not (FALSE).
 *
 * \return \ref GPIO_SUCCESS or \ref GPIO_INVALID_ARGUMENT.
 */
extern int gpio_configure_pin_periph_event_mode(unsigned int pin, unsigned int mode, unsigned int use_igf);

//! @}
#endif


#endif  // _GPIO_H_
