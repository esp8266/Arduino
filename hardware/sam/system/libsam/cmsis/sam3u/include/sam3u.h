/* $asf_license$ */

#ifndef _SAM3U_
#define _SAM3U_

/* Device characteristics */
#define CHIP_FREQ_SLCK_RC_MIN		(20000U)
#define CHIP_FREQ_SLCK_RC			(32000U)
#define CHIP_FREQ_SLCK_RC_MAX		(44000U)
#define CHIP_FREQ_MAINCK_RC_4MHZ	(4000000U)
#define CHIP_FREQ_MAINCK_RC_8MHZ	(8000000U)
#define CHIP_FREQ_MAINCK_RC_12MHZ	(12000000U)
#define CHIP_FREQ_CPU_MAX			(96000000U)

#define CHIP_FLASH_WAIT_STATE		(3U)

#if defined __SAM3U1C__
  #include "sam3u1c.h"
#elif defined __SAM3U1E__
  #include "sam3u1e.h"
#elif defined __SAM3U2C__
  #include "sam3u2c.h"
#elif defined __SAM3U2E__
  #include "sam3u2e.h"
#elif defined __SAM3U4C__
  #include "sam3u4c.h"
#elif defined __SAM3U4E__
  #include "sam3u4e.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3U_ */
