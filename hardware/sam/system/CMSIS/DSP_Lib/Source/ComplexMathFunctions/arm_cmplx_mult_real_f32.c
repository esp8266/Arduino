/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_cmplx_mult_real_f32.c   
*   
* Description:	Floating-point complex by real multiplication   
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
* -------------------------------------------------------------------- */

#include "arm_math.h"

/**   
 * @ingroup groupCmplxMath   
 */

/**   
 * @defgroup CmplxByRealMult Complex-by-Real Multiplication   
 *   
 * Multiplies a complex vector by a real vector and generates a complex result.   
 * The data in the complex arrays is stored in an interleaved fashion   
 * (real, imag, real, imag, ...).   
 * The parameter <code>numSamples</code> represents the number of complex   
 * samples processed.  The complex arrays have a total of <code>2*numSamples</code>   
 * real values while the real array has a total of <code>numSamples</code>   
 * real values.   
 *   
 * The underlying algorithm is used:   
 *   
 * <pre>   
 * for(n=0; n<numSamples; n++) {   
 *     pCmplxDst[(2*n)+0] = pSrcCmplx[(2*n)+0] * pSrcReal[n];   
 *     pCmplxDst[(2*n)+1] = pSrcCmplx[(2*n)+1] * pSrcReal[n];   
 * }   
 * </pre>   
 *   
 * There are separate functions for floating-point, Q15, and Q31 data types.   
 */

/**   
 * @addtogroup CmplxByRealMult   
 * @{   
 */


/**   
 * @brief  Floating-point complex-by-real multiplication   
 * @param[in]  *pSrcCmplx points to the complex input vector   
 * @param[in]  *pSrcReal points to the real input vector   
 * @param[out]  *pCmplxDst points to the complex output vector   
 * @param[in]  numSamples number of samples in each vector   
 * @return none.   
 */

void arm_cmplx_mult_real_f32(
  float32_t * pSrcCmplx,
  float32_t * pSrcReal,
  float32_t * pCmplxDst,
  uint32_t numSamples)
{
  float32_t in;                                  /* Temporary variable to store input value */

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */
  uint32_t blkCnt;                               /* loop counters */

  /* loop Unrolling */
  blkCnt = numSamples >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C[2 * i] = A[2 * i] * B[i].            */
    /* C[2 * i + 1] = A[2 * i + 1] * B[i].        */
    in = *pSrcReal++;
    /* store the result in the destination buffer. */
    *pCmplxDst++ = (*pSrcCmplx++) * (in);
    *pCmplxDst++ = (*pSrcCmplx++) * (in);

    in = *pSrcReal++;
    *pCmplxDst++ = (*pSrcCmplx++) * (in);
    *pCmplxDst++ = (*pSrcCmplx++) * (in);

    in = *pSrcReal++;
    *pCmplxDst++ = (*pSrcCmplx++) * (in);
    *pCmplxDst++ = (*pSrcCmplx++) * (in);

    in = *pSrcReal++;
    *pCmplxDst++ = (*pSrcCmplx++) * (in);
    *pCmplxDst++ = (*pSrcCmplx++) * (in);

    /* Decrement the numSamples loop counter */
    blkCnt--;
  }

  /* If the numSamples is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = numSamples % 0x4u;

  while(blkCnt > 0u)
  {
    /* C[2 * i] = A[2 * i] * B[i].            */
    /* C[2 * i + 1] = A[2 * i + 1] * B[i].        */
    in = *pSrcReal++;
    /* store the result in the destination buffer. */
    *pCmplxDst++ = (*pSrcCmplx++) * (in);
    *pCmplxDst++ = (*pSrcCmplx++) * (in);

    /* Decrement the numSamples loop counter */
    blkCnt--;
  }

#else

  /* Run the below code for Cortex-M0 */

  while(numSamples > 0u)
  {
    /* realOut = realA * realB.            */
    /* imagOut = imagA * realB.                */
    in = *pSrcReal++;
    /* store the result in the destination buffer. */
    *pCmplxDst++ = (*pSrcCmplx++) * (in);
    *pCmplxDst++ = (*pSrcCmplx++) * (in);

    /* Decrement the numSamples loop counter */
    numSamples--;
  }

#endif /* #ifndef ARM_MATH_CM0 */

}

/**   
 * @} end of CmplxByRealMult group   
 */
