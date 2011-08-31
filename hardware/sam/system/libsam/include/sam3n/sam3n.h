/* %ATMEL_LICENCE% */

#ifndef _SAM3N_
#define _SAM3N_

#if defined sam3n1a
  #include "SAM3N1A.h"
#elif defined sam3n1b
  #include "SAM3N1B.h"
#elif defined sam3n1c
  #include "SAM3N1C.h"
#elif defined sam3n2a
  #include "SAM3N2A.h"
#elif defined sam3n2b
  #include "SAM3N2B.h"
#elif defined sam3n2c
  #include "SAM3N2C.h"
#elif defined sam3n4a
  #include "SAM3N4A.h"
#elif defined sam3n4b
  #include "SAM3N4B.h"
#elif defined sam3n4c
  #include "SAM3N4C.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3N_ */
