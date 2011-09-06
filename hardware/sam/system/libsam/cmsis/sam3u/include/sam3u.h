/* $asf_license$ */

#ifndef _SAM3U_
#define _SAM3U_

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
