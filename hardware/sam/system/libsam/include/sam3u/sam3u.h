/* %ATMEL_LICENCE% */

#ifndef _SAM3U_
#define _SAM3U_

#if defined sam3u1c
  #include "SAM3U1C.h"
#elif defined sam3u1e
  #include "SAM3U1E.h"
#elif defined sam3u2c
  #include "SAM3U2C.h"
#elif defined sam3u2e
  #include "SAM3U2E.h"
#elif defined sam3u4c
  #include "SAM3U4C.h"
#elif defined sam3u4e
  #include "SAM3U4E.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3U_ */
