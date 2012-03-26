/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_cmplx_dot_prod_q31.c   
*   
* Description:	Q31 complex dot product   
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
 * @brief  Q31 complex dot product   
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
 * The intermediate 1.31 by 1.31 multiplications are performed with 64-bit precision and then shifted to 16.48 format.   
 * The internal real and imaginary accumulators are in 16.48 format and provide 15 guard bits.   
 * Additions are nonsaturating and no overflow will occur as long as <code>numSamples</code> is less than 32768.   
 * The return results <code>realResult</code> and <code>imagResult</code> are in 16.48 format.   
 * Input down scaling is not required.   
 */

void arm_cmplx_dot_prod_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  uint32_t numSamples,
  q63_t * realResult,
  q63_t * imagResult)
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
    /* Convert real data in 2.62 to 16.48 by 14 right shifts */
    real_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;
    /* CImag = A[1]* B[1] + A[3]* B[3] + A[5]* B[5] + .....+ A[numSamples-1]* B[numSamples-1] */
    /* Convert imag data in 2.62 to 16.48 by 14 right shifts */
    imag_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;

    real_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;
    imag_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;

    real_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;
    imag_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;

    real_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;
    imag_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;


    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the numSamples  is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = numSamples % 0x4u;

  while(blkCnt > 0u)
  {
    /* CReal = A[0]* B[0] + A[2]* B[2] + A[4]* B[4] + .....+ A[numSamples-2]* B[numSamples-2] */
    real_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;
    /* CImag = A[1]* B[1] + A[3]* B[3] + A[5]* B[5] + .....+ A[numSamples-1]* B[numSamples-1] */
    imag_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;

    /* Decrement the loop counter */
    blkCnt--;
  }

#else

  /* Run the below code for Cortex-M0 */

  while(numSamples > 0u)
  {
    /* outReal = realA[0]* realB[0] + realA[2]* realB[2] + realA[4]* realB[4] + .....+ realA[numSamples-2]* realB[numSamples-2] */
    real_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;
    /* outImag = imagA[1]* imagB[1] + imagA[3]* imagB[3] + imagA[5]* imagB[5] + .....+ imagA[numSamples-1]* imagB[numSamples-1] */
    imag_sum += (q63_t) * pSrcA++ * (*pSrcB++) >> 14;

    /* Decrement the loop counter */
    numSamples--;
  }

#endif /* #ifndef ARM_MATH_CM0 */

  /* Store the real and imaginary results in 16.48 format  */
  *realResult = real_sum;
  *imagResult = imag_sum;
}

/**   
 * @} end of cmplx_dot_prod group   
 */
