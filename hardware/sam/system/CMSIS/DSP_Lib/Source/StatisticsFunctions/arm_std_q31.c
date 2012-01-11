/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_std_q31.c   
*   
* Description:	Standard deviation of an array of Q31 type.   
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
 * @ingroup groupStats   
 */

/**   
 * @addtogroup STD   
 * @{   
 */


/**   
 * @brief Standard deviation of the elements of a Q31 vector.   
 * @param[in]       *pSrc points to the input vector   
 * @param[in]       blockSize length of the input vector   
 * @param[out]      *pResult standard deviation value returned here   
 * @return none.   
 * @details   
 * <b>Scaling and Overflow Behavior:</b>   
 *   
 *\par   
 * The function is implemented using an internal 64-bit accumulator.   
 * The input is represented in 1.31 format, and intermediate multiplication   
 * yields a 2.62 format.   
 * The accumulator maintains full precision of the intermediate multiplication results,    
 * but provides only a single guard bit.   
 * There is no saturation on intermediate additions.   
 * If the accumulator overflows it wraps around and distorts the result.   
 * In order to avoid overflows completely the input signal must be scaled down by    
 * log2(blockSize) bits, as a total of blockSize additions are performed internally.    
 * Finally, the 2.62 accumulator is right shifted by 31 bits to yield a 1.31 format value.   
 *   
 */


void arm_std_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult)
{
  q63_t sum = 0;                                 /* Accumulator */
  q31_t meanOfSquares, squareOfMean;             /* square of mean and mean of square */
  q31_t mean;                                    /* mean */
  q31_t in;                                      /* input value */
  q31_t t;                                       /* Temporary variable */
  uint32_t blkCnt;                               /* loop counter */


#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  q31_t *pIn;                                    /* Temporary pointer */

  pIn = pSrc;

  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = (A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1])  */
    /* Compute Sum of squares of the input samples   
     * and then store the result in a temporary variable, sum. */
    in = *pSrc++;
    sum += ((q63_t) (in) * (in));
    in = *pSrc++;
    sum += ((q63_t) (in) * (in));
    in = *pSrc++;
    sum += ((q63_t) (in) * (in));
    in = *pSrc++;
    sum += ((q63_t) (in) * (in));

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

  while(blkCnt > 0u)
  {
    /* C = (A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1]) */
    /* Compute Sum of squares of the input samples   
     * and then store the result in a temporary variable, sum. */
    in = *pSrc++;
    sum += ((q63_t) (in) * (in));

    /* Decrement the loop counter */
    blkCnt--;
  }

  t = (q31_t) ((1.0f / (float32_t) (blockSize - 1u)) * 1073741824.0f);

  /* Compute Mean of squares of the input samples   
   * and then store the result in a temporary variable, meanOfSquares. */
  sum = (sum >> 31);
  meanOfSquares = (q31_t) ((sum * t) >> 30);

  /* Reset the accumulator */
  sum = 0;

  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* Reset the input working pointer */
  pSrc = pIn;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */
    /* Compute sum of all input values and then store the result in a temporary variable, sum. */
    sum += *pSrc++;
    sum += *pSrc++;
    sum += *pSrc++;
    sum += *pSrc++;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

  while(blkCnt > 0u)
  {
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */
    /* Compute sum of all input values and then store the result in a temporary variable, sum. */
    sum += *pSrc++;

    /* Decrement the loop counter */
    blkCnt--;
  }

#else

  /* Run the below code for Cortex-M0 */

  q63_t sumOfSquares = 0;                        /* Accumulator */
  /* Loop over blockSize number of values */
  blkCnt = blockSize;

  while(blkCnt > 0u)
  {
    /* C = (A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1]) */
    /* Compute Sum of squares of the input samples    
     * and then store the result in a temporary variable, sumOfSquares. */
    in = *pSrc++;
    sumOfSquares += ((q63_t) (in) * (in));

    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */
    /* Compute sum of all input values and then store the result in a temporary variable, sum. */
    sum += in;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Compute Mean of squares of the input samples    
   * and then store the result in a temporary variable, meanOfSquares. */
  t = (q31_t) ((1.0f / (float32_t) (blockSize - 1u)) * 1073741824.0f);
  sumOfSquares = (sumOfSquares >> 31);
  meanOfSquares = (q31_t) ((sumOfSquares * t) >> 30);

#endif /* #ifndef ARM_MATH_CM0 */

  /* Compute mean of all input values */
  t = (q31_t) ((1.0f / (blockSize * (blockSize - 1u))) * 2147483648.0f);
  mean = (q31_t) (sum);

  /* Compute square of mean */
  squareOfMean = (q31_t) (((q63_t) mean * mean) >> 31);
  squareOfMean = (q31_t) (((q63_t) squareOfMean * t) >> 31);


  /* Compute standard deviation and then store the result to the destination */
  arm_sqrt_q31(meanOfSquares - squareOfMean, pResult);

}

/**   
 * @} end of STD group   
 */
