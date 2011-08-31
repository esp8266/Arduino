/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_
#define _SAM3XA_

#if defined sam3a2c
  #include "SAM3A2C.h"
#elif defined sam3a4c
  #include "SAM3A4C.h"
#elif defined sam3a8c
  #include "SAM3A8C.h"
#elif defined sam3x2c
  #include "SAM3X2C.h"
#elif defined sam3x2e
  #include "SAM3X2E.h"
#elif defined sam3x2g
  #include "SAM3X2G.h"
#elif defined sam3x2h
  #include "SAM3X2H.h"
#elif defined sam3x4c
  #include "SAM3X4C.h"
#elif defined sam3x4e
  #include "SAM3X4E.h"
#elif defined sam3x4g
  #include "SAM3X4G.h"
#elif defined sam3x4h
  #include "SAM3X4H.h"
#elif defined sam3x8c
  #include "SAM3X8C.h"
#elif defined sam3x8e
  #include "SAM3X8E.h"
#elif defined sam3x8g
  #include "SAM3X8G.h"
#elif defined sam3x8h
  #include "SAM3X8H.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3XA_ */
