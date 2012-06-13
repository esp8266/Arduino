/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _LIB_SAM_
#define _LIB_SAM_

/*
 * Core and peripherals registers definitions
 */
#include "sam.h"

/* Define attribute */
#if defined (  __GNUC__  ) /* GCC CS3 */
    #define WEAK __attribute__ ((weak))
#elif defined ( __ICCARM__ ) /* IAR Ewarm 5.41+ */
    #define WEAK __weak
#endif

/* Define NO_INIT attribute */
#if defined (  __GNUC__  )
    #define NO_INIT
#elif defined ( __ICCARM__ )
    #define NO_INIT __no_init
#endif

/*
 * Peripherals
 */
#include "include/adc.h"
#include "include/dacc.h"
#include "include/interrupt_sam_nvic.h"
#include "include/pio.h"
#include "include/pmc.h"
#include "include/pwmc.h"
#include "include/rtc.h"
#include "include/rtt.h"
#include "include/spi.h"
#include "include/tc.h"
#include "include/twi.h"
#include "include/usart.h"
#include "include/wdt.h"

#include "include/timetick.h"
#include "include/USB_device.h"
#include "include/USB_host.h"

#if SAM3XA_SERIES
#include "include/uotghs_device.h"
#include "include/uotghs_host.h"
#endif /* SAM3XA_SERIES */

#endif /* _LIB_SAM_ */
