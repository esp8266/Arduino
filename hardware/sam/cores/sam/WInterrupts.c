/* %arduino_license% */

#include "WInterrupts.h"

#ifdef __cplusplus
extern "C" {
#endif

/** PIO interrupt handlers array */
/*volatile*/ static voidFuncPtr g_apfn_IntFunc[EXTERNAL_NUM_INTERRUPTS]={ 0 } ;

void attachInterrupt( uint32_t ulInterrupt, void (*pfn_UserFunc)(void), uint32_t ulMode )
{
  if ( ulInterrupt < EXTERNAL_NUM_INTERRUPTS )
  {
    g_apfn_IntFunc[ulInterrupt] = pfn_UserFunc ;

    // Configure the interrupt mode (trigger on low input, any change, rising
    // edge, or falling edge).  The mode constants were chosen to correspond
    // to the configuration bits in the hardware register, so we simply shift
    // the mode into place.

    // Enable the interrupt.

  }
}

void detachInterrupt( uint32_t ulInterrupt )
{
  if ( ulInterrupt < EXTERNAL_NUM_INTERRUPTS )
  {
    /* Disable the interrupt. */


    g_apfn_IntFunc[ulInterrupt] = NULL ;
  }
}

#ifdef __cplusplus
}
#endif
