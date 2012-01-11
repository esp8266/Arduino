/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_scale_q31.c   
*   
* Description:	Multiplies a Q31 vector by a scalar.   
*   
* Target Processor: Cortex-M4/Cortex-M3/Cortex-M0
*  
* Version 1.0.10 2011/7/15 
*    Big Endian support added and Merged M0 and M3/M4 Source code.  
*   
* Version 1.0.3 2010/11/29  
*    Re-organized the CMSIS folders and updated documentation.   
*    
* Version 1.0.2 2010/11/11   
*    Documentation updated.    
*   
* Version 1.0.1 2010/10/05    
*    Production release and review comments incorporated.   
*   
* Version 1.0.0 2010/09/20    
*    Production release and review comments incorporated   
*   
* Version 0.0.7  2010/06/10    
*    Misra-C changes done   
* -------------------------------------------------------------------- */

#include "arm_math.h"

/**   
 * @ingroup groupMath   
 */

/**   
 * @addtogroup scale   
 * @{   
 */

/**   
 * @brief Multiplies a Q31 vector by a scalar.   
 * @param[in]       *pSrc points to the input vector   
 * @param[in]       scaleFract fractional portion of the scale value   
 * @param[in]       shift number of bits to shift the result by   
 * @param[out]      *pDst points to the output vector   
 * @param[in]       blockSize number of samples in the vector   
 * @return none.   
 *   
 * <b>Scaling and Overflow Behavior:</b>   
 * \par   
 * The input data <code>*pSrc</code> and <code>scaleFract</code> are in 1.31 format.   
 * These are multiplied to yield a 2.62 intermediate result and this is shifted with saturation to 1.31 format.   
 */

void arm_scale_q31(
  q31_t * pSrc,
  q31_t scaleFract,
  int8_t shift,
  q31_t * pDst,
  uint32_t blockSize)
{
  int8_t kShift = 31 - shift;                    /* Shift to apply after scaling */
  uint32_t blkCnt;                               /* loop counter */

#ifndef ARM_MATH_CM0

/* Run the below code for Cortex-M4 and Cortex-M3 */

  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = A * scale */
    /* Scale the input and then store the results in the destination buffer. */
    *pDst++ = clip_q63_to_q31(((q63_t) * pSrc++ * scaleFract) >> kShift);
    *pDst++ = clip_q63_to_q31(((q63_t) * pSrc++ * scaleFract) >> kShift);
    *pDst++ = clip_q63_to_q31(((q63_t) * pSrc++ * scaleFract) >> kShift);
    *pDst++ = clip_q63_to_q31(((q63_t) * pSrc++ * scaleFract) >> kShift);

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

#else

  /* Run the below code for Cortex-M0 */

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #ifndef ARM_MATH_CM0 */

  while(blkCnt > 0u)
  {
    /* C = A * scale */
    /* Scale the input and then store the result in the destination buffer. */
    *pDst++ = clip_q63_to_q31(((q63_t) * pSrc++ * scaleFract) >> kShift);

    /* Decrement the loop counter */
    blkCnt--;
  }
}

/**   
 * @} end of scale group   
 */
