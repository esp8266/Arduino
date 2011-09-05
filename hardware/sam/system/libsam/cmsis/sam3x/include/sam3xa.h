/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_
#define _SAM3XA_

#if defined __sam3a2c__
  #include "SAM3A2C.h"
#elif defined __sam3a4c__
  #include "SAM3A4C.h"
#elif defined __sam3a8c__
  #include "SAM3A8C.h"
#elif defined __sam3x2c__
  #include "SAM3X2C.h"
#elif defined __sam3x2e__
  #include "SAM3X2E.h"
#elif defined __sam3x2g__
  #include "SAM3X2G.h"
#elif defined __sam3x2h__
  #include "SAM3X2H.h"
#elif defined __sam3x4c__
  #include "SAM3X4C.h"
#elif defined __sam3x4e__
  #include "SAM3X4E.h"
#elif defined __sam3x4g__
  #include "SAM3X4G.h"
#elif defined __sam3x4h__
  #include "SAM3X4H.h"
#elif defined __sam3x8c__
  #include "SAM3X8C.h"
#elif defined __sam3x8e__
  #include "SAM3X8E.h"
#elif defined __sam3x8g__
  #include "SAM3X8G.h"
#elif defined __sam3x8h__
  #include "SAM3X8H.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3XA_ */
