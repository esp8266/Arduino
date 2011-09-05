/* %ATMEL_LICENCE% */

#ifndef _SAM3S8_
#define _SAM3S8_

#if defined __sam3sd8a__
  #include "SAM3SD8A.h"
#elif defined __sam3sd8b__
  #include "SAM3SD8B.h"
#elif defined __sam3sd8c__
  #include "SAM3SD8C.h"
#elif defined __sam3s8a__
  #include "SAM3S8A.h"
#elif defined __sam3s8b__
  #include "SAM3S8B.h"
#elif defined __sam3s8c__
  #include "SAM3S8C.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3S8_ */
