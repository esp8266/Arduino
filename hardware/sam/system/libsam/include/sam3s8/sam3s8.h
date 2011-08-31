/* %ATMEL_LICENCE% */

#ifndef _SAM3S8_
#define _SAM3S8_

#if defined sam3sd8a
  #include "SAM3SD8A.h"
#elif defined sam3sd8b
  #include "SAM3SD8B.h"
#elif defined sam3sd8c
  #include "SAM3SD8C.h"
#elif defined sam3s8a
  #include "SAM3S8A.h"
#elif defined sam3s8b
  #include "SAM3S8B.h"
#elif defined sam3s8c
  #include "SAM3S8C.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3S8_ */
