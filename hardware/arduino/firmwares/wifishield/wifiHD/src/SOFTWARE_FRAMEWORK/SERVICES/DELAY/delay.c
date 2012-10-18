/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Management of the delays.
 *
 * This file manages the "delays", with or without an OS.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ***************************************************************************/

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

//_____  I N C L U D E S ___________________________________________________

#include "delay.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

//! CPU frequency
#ifndef FREERTOS_USED
static unsigned long s_fcpu_hz;
#endif
#if (defined NUTOS_USED)
extern void NutSleep(unsigned long ms);
#endif

//_____ D E C L A R A T I O N S ____________________________________________

void delay_init(unsigned long fcpu_hz)
{
#ifndef FREERTOS_USED
  s_fcpu_hz = fcpu_hz;
#endif
}


void delay_ms(unsigned long delay)
{
#if (defined FREERTOS_USED)
  vTaskDelay( (portTickType)TASK_DELAY_MS(delay) );
#elif (defined NUTOS_USED)
  NutSleep(delay);
#else
  cpu_delay_ms(delay, s_fcpu_hz);
#endif
}
