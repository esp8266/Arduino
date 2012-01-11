/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_cmplx_dot_prod_q15.c   
*   
* Description:	Processing function for the Q15 Complex Dot product   
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
 * @addtogroup cmplx_dot_prod   
 * @{   
 */

/**   
 * @brief  Q15 complex dot product   
 * @param  *pSrcA points to the first input vector   
 * @param  *pSrcB points to the second input vector   
 * @param  numSamples number of complex samples in each vector   
 * @param  *realResult real part of the result returned here   
 * @param  *imagResult imaginary part of the result returned here   
 * @return none.   
 *   
 * <b>Scaling and Overflow Behavior:</b>   
 * \par   
 * The function is implemented using an internal 64-bit accumulator.   
 * The intermediate 1.15 by 1.15 multiplications are performed with full precision and yield a 2.30 result.   
 * These are accumulated in a 64-bit accumulator with 34.30 precision.   
 * As a final step, the accumulators are converted to 8.24 format.   
 * The return results <code>realResult</code> and <code>imagResult</code> are in 8.24 format.   
 */

void arm_cmplx_dot_prod_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  uint32_t numSamples,
  q31_t * realResult,
  q31_t * imagResult)
{
  q63_t real_sum = 0, imag_sum = 0;              /* Temporary result storage */

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */
  uint32_t blkCnt;                               /* loop counter */


  /*loop Unrolling */
  blkCnt = numSamples >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* CReal = A[0]* B[0] + A[2]* B[2] + A[4]* B[4] + .....+ A[numSamples-2]* B[numSamples-2] */
    real_sum += ((q31_t) * pSrcA++ * *pSrcB++);

    /* CImag = A[1]* B[1] + A[3]* B[3] + A[5]* B[5] + .....+ A[numSamples-1]* B[numSamples-1] */
    imag_sum += ((q31_t) * pSrcA++ * *pSrcB++);

    real_sum += ((q31_t) * pSrcA++ * *pSrcB++);
    imag_sum += ((q31_t) * pSrcA++ * *pSrcB++);

    real_sum += ((q31_t) * pSrcA++ * *pSrcB++);
    imag_sum += ((q31_t) * pSrcA++ * *pSrcB++);

    real_sum += ((q31_t) * pSrcA++ * *pSrcB++);
    imag_sum += ((q31_t) * pSrcA++ * *pSrcB++);

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the numSamples is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = numSamples % 0x4u;

  while(blkCnt > 0u)
  {
    /* CReal = A[0]* B[0] + A[2]* B[2] + A[4]* B[4] + .....+ A[numSamples-2]* B[numSamples-2] */
    real_sum += ((q31_t) * pSrcA++ * *pSrcB++);
    /* CImag = A[1]* B[1] + A[3]* B[3] + A[5]* B[5] + .....+ A[numSamples-1]* B[numSamples-1] */
    imag_sum += ((q31_t) * pSrcA++ * *pSrcB++);

    /* Decrement the loop counter */
    blkCnt--;
  }

#else

  /* Run the below code for Cortex-M0 */

  while(numSamples > 0u)
  {
    /* CReal = A[0]* B[0] + A[2]* B[2] + A[4]* B[4] + .....+ A[numSamples-2]* B[numSamples-2] */
    real_sum += ((q31_t) * pSrcA++ * *pSrcB++);
    /* CImag = A[1]* B[1] + A[3]* B[3] + A[5]* B[5] + .....+ A[numSamples-1]* B[numSamples-1] */
    imag_sum += ((q31_t) * pSrcA++ * *pSrcB++);

    /* Decrement the loop counter */
    numSamples--;
  }

#endif /* #ifndef ARM_MATH_CM0 */

  /* Store the real and imaginary results in 8.24 format  */
  /* Convert real data in 34.30 to 8.24 by 6 right shifts */
  *realResult = (q31_t) (real_sum) >> 6;
  /* Convert imaginary data in 34.30 to 8.24 by 6 right shifts */
  *imagResult = (q31_t) (imag_sum) >> 6;
}

/**   
 * @} end of cmplx_dot_prod group   
 */
