/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_cmplx_conj_q31.c   
*   
* Description:	Q31 complex conjugate.   
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
*    Production release and review comments incorporated.   
* ---------------------------------------------------------------------------- */

#include "arm_math.h"

/**   
 * @ingroup groupCmplxMath   
 */

/**   
 * @addtogroup cmplx_conj   
 * @{   
 */

/**   
 * @brief  Q31 complex conjugate.   
 * @param  *pSrc points to the input vector   
 * @param  *pDst points to the output vector   
 * @param  numSamples number of complex samples in each vector   
 * @return none.   
 *   
 * <b>Scaling and Overflow Behavior:</b>   
 * \par   
 * The function uses saturating arithmetic.   
 * The Q31 value -1 (0x80000000) will be saturated to the maximum allowable positive value 0x7FFFFFFF.   
 */

void arm_cmplx_conj_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t numSamples)
{

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */
  uint32_t blkCnt;                               /* loop counter */
  q31_t in;                                      /* Input value */

  /*loop Unrolling */
  blkCnt = numSamples >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C[0]+jC[1] = A[0]+ j (-1) A[1] */
    /* Calculate Complex Conjugate and then store the results in the destination buffer. */
    /* Saturated to 0x7fffffff if the input is -1(0x80000000) */
    *pDst++ = *pSrc++;
    in = *pSrc++;
    *pDst++ = (in == 0x80000000) ? 0x7fffffff : -in;
    *pDst++ = *pSrc++;
    in = *pSrc++;
    *pDst++ = (in == 0x80000000) ? 0x7fffffff : -in;
    *pDst++ = *pSrc++;
    in = *pSrc++;
    *pDst++ = (in == 0x80000000) ? 0x7fffffff : -in;
    *pDst++ = *pSrc++;
    in = *pSrc++;
    *pDst++ = (in == 0x80000000) ? 0x7fffffff : -in;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the numSamples is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = numSamples % 0x4u;

  while(blkCnt > 0u)
  {
    /* C[0]+jC[1] = A[0]+ j (-1) A[1] */
    /* Calculate Complex Conjugate and then store the results in the destination buffer. */
    /* Saturated to 0x7fffffff if the input is -1(0x80000000) */
    *pDst++ = *pSrc++;
    in = *pSrc++;
    *pDst++ = (in == 0x80000000) ? 0x7fffffff : -in;

    /* Decrement the loop counter */
    blkCnt--;
  }

#else

  /* Run the below code for Cortex-M0 */

  while(numSamples > 0u)
  {
    /* realOut + j (imagOut) = realIn+ j (-1) imagIn */
    /* Calculate Complex Conjugate and then store the results in the destination buffer. */
    *pDst++ = *pSrc++;
    *pDst++ = -*pSrc++;

    /* Decrement the loop counter */
    numSamples--;
  }

#endif /* #ifndef ARM_MATH_CM0 */

}

/**   
 * @} end of cmplx_conj group   
 */
