/* $asf_license$ */

#ifndef _SAM3S8_
#define _SAM3S8_

#if defined __SAM3SD8A__
  #include "sam3sd8a.h"
#elif defined __SAM3SD8B__
  #include "sam3sd8b.h"
#elif defined __SAM3SD8C__
  #include "sam3sd8c.h"
#elif defined __SAM3S8A__
  #include "sam3s8a.h"
#elif defined __SAM3S8B__
  #include "sam3s8b.h"
#elif defined __SAM3S8C__
  #include "sam3s8c.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3S8_ */
