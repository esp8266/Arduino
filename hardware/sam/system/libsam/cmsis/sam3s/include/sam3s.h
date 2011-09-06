/* $asf_license$ */

#ifndef _SAM3S_
#define _SAM3S_

#if defined __SAM3S1A__
  #include "sam3s1a.h"
#elif defined __SAM3S1B__
  #include "sam3s1b.h"
#elif defined __SAM3S1C__
  #include "sam3s1c.h"
#elif defined __SAM3S2A__
  #include "sam3s2a.h"
#elif defined __SAM3S2B__
  #include "sam3s2b.h"
#elif defined __SAM3S2C__
  #include "sam3s2c.h"
#elif defined __SAM3S4A__
  #include "sam3s4a.h"
#elif defined __SAM3S4B__
  #include "sam3s4b.h"
#elif defined __SAM3S4C__
  #include "sam3s4c.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3S_ */
