/* %ATMEL_LICENCE% */

#ifndef _SAM3U_
#define _SAM3U_

#if defined __sam3u1c__
  #include "SAM3U1C.h"
#elif defined __sam3u1e__
  #include "SAM3U1E.h"
#elif defined __sam3u2c__
  #include "SAM3U2C.h"
#elif defined __sam3u2e__
  #include "SAM3U2E.h"
#elif defined __sam3u4c__
  #include "SAM3U4C.h"
#elif defined __sam3u4e__
  #include "SAM3U4E.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3U_ */
