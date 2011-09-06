/* $asf_license$ */

#ifndef _SAM3N_
#define _SAM3N_

#if defined __SAM3N1A__
  #include "sam3n1a.h"
#elif defined __SAM3N1B__
  #include "sam3n1b.h"
#elif defined __SAM3N1C__
  #include "sam3n1c.h"
#elif defined __SAM3N2A__
  #include "sam3n2a.h"
#elif defined __SAM3N2B__
  #include "sam3n2b.h"
#elif defined __SAM3N2C__
  #include "sam3n2c.h"
#elif defined __SAM3N4A__
  #include "sam3n4a.h"
#elif defined __SAM3N4B__
  #include "sam3n4b.h"
#elif defined __SAM3N4C__
  #include "sam3n4c.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3N_ */
