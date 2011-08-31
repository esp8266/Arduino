/* %ATMEL_LICENCE% */

#ifndef _SAM3S_
#define _SAM3S_

#if defined sam3s1a
  #include "SAM3S1A.h"
#elif defined sam3s1b
  #include "SAM3S1B.h"
#elif defined sam3s1c
  #include "SAM3S1C.h"
#elif defined sam3s2a
  #include "SAM3S2A.h"
#elif defined sam3s2b
  #include "SAM3S2B.h"
#elif defined sam3s2c
  #include "SAM3S2C.h"
#elif defined sam3s4a
  #include "SAM3S4A.h"
#elif defined sam3s4b
  #include "SAM3S4B.h"
#elif defined sam3s4c
  #include "SAM3S4C.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3S_ */
