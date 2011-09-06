/*
 %atmel_license%
*/

#ifndef _LIB_SAM3S_
#define _LIB_SAM3S_

/*
 * Core and peripherals registers definitions
 */
#include "include/sam3.h"

/** Define MAX number of Interrupts: (IRQn_Type+1) + 8 for CM3 core */
#define EXTERNAL_NUM_INTERRUPTS (UDP_IRQn+1+8)

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

#endif /* _LIB_SAM3S_ */
