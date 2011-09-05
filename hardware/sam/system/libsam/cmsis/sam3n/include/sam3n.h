/* %ATMEL_LICENCE% */

#ifndef _SAM3N_
#define _SAM3N_

#if defined __sam3n1a__
  #include "SAM3N1A.h"
#elif defined __sam3n1b__
  #include "SAM3N1B.h"
#elif defined __sam3n1c__
  #include "SAM3N1C.h"
#elif defined __sam3n2a__
  #include "SAM3N2A.h"
#elif defined __sam3n2b__
  #include "SAM3N2B.h"
#elif defined __sam3n2c__
  #include "SAM3N2C.h"
#elif defined __sam3n4a__
  #include "SAM3N4A.h"
#elif defined __sam3n4b__
  #include "SAM3N4B.h"
#elif defined __sam3n4c__
  #include "SAM3N4C.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3N_ */
