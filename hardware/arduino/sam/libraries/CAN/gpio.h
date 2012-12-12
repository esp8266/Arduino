/**
 * \file
 *
 * \brief Common GPIO API.
 *
 * Copyright (c) 2010-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef _GPIO_H_
#define _GPIO_H_

#include <parts.h>


#if (SAM3S || SAM3U || SAM3N || SAM3XA || SAM4S)
# include "sam_gpio.h"
#elif XMEGA
# include "xmega_gpio/xmega_gpio.h"
#elif MEGA || MEGA_RF
# include "mega_gpio/mega_gpio.h"
#else
# error Unsupported chip type
#endif



/**
 * \defgroup gpio_group General Purpose Input/Output
 *
 * This is the common API for GPIO. Additional features are available
 * in the documentation of the specific modules.
 *
 * \section io_group_platform Platform Dependencies
 *
 * The following functions are available on all platforms, but there may
 * be variations in the function signature (i.e. parameters) and
 * behaviour. These functions are typically called by platform-specific
 * parts of drivers, and applications that aren't intended to be
 * portable:
 *   - gpio_pin_is_low()
 *   - gpio_pin_is_high()
 *   - gpio_set_pin_high()
 *   - gpio_set_pin_group_high()
 *   - gpio_set_pin_low()
 *   - gpio_set_pin_group_low()
 *   - gpio_toggle_pin()
 *   - gpio_toggle_pin_group()
 *   - gpio_configure_pin()
 *   - gpio_configure_group()
 */

#endif  /* _GPIO_H_ */
