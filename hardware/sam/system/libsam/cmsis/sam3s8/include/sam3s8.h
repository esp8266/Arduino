/* $asf_license$ */

#ifndef _SAM3S8_
#define _SAM3S8_

/* Device characteristics */
#define CHIP_FREQ_SLCK_RC_MIN		(20000U)
#define CHIP_FREQ_SLCK_RC			(32000U)
#define CHIP_FREQ_SLCK_RC_MAX		(44000U)
#define CHIP_FREQ_MAINCK_RC_4MHZ	(4000000U)
#define CHIP_FREQ_MAINCK_RC_8MHZ	(8000000U)
#define CHIP_FREQ_MAINCK_RC_12MHZ	(12000000U)
#define CHIP_FREQ_CPU_MAX			(64000000U)

#define CHIP_FLASH_WAIT_STATE		(3U)

#if defined __SAM3SD8A__
  #include "sam3sd8a.h"
#elif defined __SAM3SD8B__
  #include "sam3sd8b.h"
#elif defined __SAM3SD8C__
  #include "sam3sd8c.h"
#elif defined __SAM3S8A__
  #include "sam3s8a.h"
#elif defined __SAM3S8B__
  #include "sam3s8b.h"
#elif defined __SAM3S8C__
  #include "sam3s8c.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3S8_ */
