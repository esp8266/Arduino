/* $asf_license$ */

#ifndef _SAM3N_
#define _SAM3N_

/* Device characteristics */
#define CHIP_FREQ_SLCK_RC_MIN		(20000U)
#define CHIP_FREQ_SLCK_RC			(32000U)
#define CHIP_FREQ_SLCK_RC_MAX		(44000U)
#define CHIP_FREQ_MAINCK_RC_4MHZ	(4000000U)
#define CHIP_FREQ_MAINCK_RC_8MHZ	(8000000U)
#define CHIP_FREQ_MAINCK_RC_12MHZ	(12000000U)
#define CHIP_FREQ_CPU_MAX			(48000000U)

#define CHIP_FLASH_WAIT_STATE		(3U)

#if defined __SAM3N1A__
  #include "sam3n1a.h"
#elif defined __SAM3N1B__
  #include "sam3n1b.h"
#elif defined __SAM3N1C__
  #include "sam3n1c.h"
#elif defined __SAM3N2A__
  #include "sam3n2a.h"
#elif defined __SAM3N2B__
  #include "sam3n2b.h"
#elif defined __SAM3N2C__
  #include "sam3n2c.h"
#elif defined __SAM3N4A__
  #include "sam3n4a.h"
#elif defined __SAM3N4B__
  #include "sam3n4b.h"
#elif defined __SAM3N4C__
  #include "sam3n4c.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3N_ */
