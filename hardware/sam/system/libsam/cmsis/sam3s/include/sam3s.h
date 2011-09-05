/* %ATMEL_LICENCE% */

#ifndef _SAM3S_
#define _SAM3S_

#if defined __sam3s1a__
  #include "SAM3S1A.h"
#elif defined __sam3s1b__
  #include "SAM3S1B.h"
#elif defined __sam3s1c__
  #include "SAM3S1C.h"
#elif defined __sam3s2a__
  #include "SAM3S2A.h"
#elif defined __sam3s2b__
  #include "SAM3S2B.h"
#elif defined __sam3s2c__
  #include "SAM3S2C.h"
#elif defined __sam3s4a__
  #include "SAM3S4A.h"
#elif defined __sam3s4b__
  #include "SAM3S4B.h"
#elif defined __sam3s4c__
  #include "./SAM3S4C.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3S_ */
