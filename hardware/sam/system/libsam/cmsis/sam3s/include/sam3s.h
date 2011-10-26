/* $asf_license$ */

#ifndef _SAM3S_
#define _SAM3S_

/* Device characteristics */
#define CHIP_FREQ_SLCK_RC_MIN		(20000U)
#define CHIP_FREQ_SLCK_RC			(32000U)
#define CHIP_FREQ_SLCK_RC_MAX		(44000U)
#define CHIP_FREQ_MAINCK_RC_4MHZ	(4000000U)
#define CHIP_FREQ_MAINCK_RC_8MHZ	(8000000U)
#define CHIP_FREQ_MAINCK_RC_12MHZ	(12000000U)
#define CHIP_FREQ_CPU_MAX			(64000000U)

#define CHIP_FLASH_WAIT_STATE		(3U)

#if defined __SAM3S1A__
  #include "sam3s1a.h"
#elif defined __SAM3S1B__
  #include "sam3s1b.h"
#elif defined __SAM3S1C__
  #include "sam3s1c.h"
#elif defined __SAM3S2A__
  #include "sam3s2a.h"
#elif defined __SAM3S2B__
  #include "sam3s2b.h"
#elif defined __SAM3S2C__
  #include "sam3s2c.h"
#elif defined __SAM3S4A__
  #include "sam3s4a.h"
#elif defined __SAM3S4B__
  #include "sam3s4b.h"
#elif defined __SAM3S4C__
  #include "sam3s4c.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3S_ */
