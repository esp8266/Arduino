/* $asf_license$ */

#ifndef _SAM3XA_
#define _SAM3XA_

/* Device characteristics */
#define CHIP_FREQ_SLCK_RC_MIN		(20000U)
#define CHIP_FREQ_SLCK_RC			(32000U)
#define CHIP_FREQ_SLCK_RC_MAX		(44000U)
#define CHIP_FREQ_MAINCK_RC_4MHZ	(4000000U)
#define CHIP_FREQ_MAINCK_RC_8MHZ	(8000000U)
#define CHIP_FREQ_MAINCK_RC_12MHZ	(12000000U)
#define CHIP_FREQ_CPU_MAX			(84000000U)

#define CHIP_FLASH_WAIT_STATE		(3U)

#if defined __SAM3A2C__
  #include "sam3a2c.h"
#elif defined __SAM3A4C__
  #include "sam3a4c.h"
#elif defined __SAM3A8C__
  #include "sam3a8c.h"
#elif defined __SAM3X2C__
  #include "sam3x2c.h"
#elif defined __SAM3X2E__
  #include "sam3x2e.h"
#elif defined __SAM3X2G__
  #include "sam3x2g.h"
#elif defined __SAM3X2H__
  #include "sam3x2h.h"
#elif defined __SAM3X4C__
  #include "sam3x4c.h"
#elif defined __SAM3X4E__
  #include "sam3x4e.h"
#elif defined __SAM3X4G__
  #include "sam3x4g.h"
#elif defined __SAM3X4H__
  #include "sam3x4h.h"
#elif defined __SAM3X8C__
  #include "sam3x8c.h"
#elif defined __SAM3X8E__
  #include "sam3x8e.h"
#elif defined __SAM3X8G__
  #include "sam3x8g.h"
#elif defined __SAM3X8H__
  #include "sam3x8h.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3XA_ */
