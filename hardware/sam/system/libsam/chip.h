#ifndef _LIB_SAM3S_
#define _LIB_SAM3S_

/*
 * Peripherals registers definitions
 */
#if defined sam3s4
#elif defined sam3s2
#elif defined sam3s1
#else
    #warning Library does not support the specified chip, specifying sam3s4.
    #define sam3s4
#endif
#include "include/SAM3S.h"

/** Define MAX number of Interrupts: (IRQn_Type+1) + 8 for CM3 core */
#define EXTERNAL_NUM_INTERRUPTS (UDP_IRQn+1+8)

/* Define attribute */
#if defined (  __GNUC__  ) /* GCC CS3 */
    #define WEAK __attribute__ ((weak))
#endif

/* Define NO_INIT attribute */
#if defined (  __GNUC__  )
    #define NO_INIT
#endif


/*
 * Core
 */

#include "include/exceptions.h"

/*
 * Peripherals
 */
#include "include/acc.h"
#include "include/adc.h"
#include "include/async.h"
#include "include/crccu.h"
#include "include/dacc.h"
#include "include/efc.h"
#include "include/flashd.h"
#include "include/pio.h"
//#include "include/pio_it.h"
#include "include/pio_capture.h"
#include "include/pmc.h"
#include "include/pwmc.h"
#include "include/rtc.h"
#include "include/rtt.h"
#include "include/spi.h"
#include "include/spi_pdc.h"
#include "include/ssc.h"
#include "include/tc.h"
#include "include/twi.h"
#include "include/twid.h"
#include "include/usart.h"
#include "include/wdt.h"

#include "include/timetick.h"

#endif /* _LIB_SAM3S_ */
