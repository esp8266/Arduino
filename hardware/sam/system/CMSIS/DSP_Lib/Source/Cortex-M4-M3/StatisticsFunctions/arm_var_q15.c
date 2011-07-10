/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_var_q15.c  
*  
* Description:	Variance of an array of Q15 type.  
*  
* Target Processor: Cortex-M4/Cortex-M3
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
 * @addtogroup variance  
 * @{  
 */ 
 
/**  
 * @brief Variance of the elements of a Q15 vector.  
 * @param[in]       *pSrc points to the input vector  
 * @param[in]       blockSize length of the input vector  
 * @param[out]      *pResult variance value returned here  
 * @return none.  
 *  
 * @details  
 * <b>Scaling and Overflow Behavior:</b>  
 *  
 * \par  
 * The function is implemented using a 64-bit internal accumulator.  
 * The input is represented in 1.15 format. 
 * Intermediate multiplication yields a 2.30 format, and this  
 * result is added without saturation to a 64-bit accumulator in 34.30 format.  
 * With 33 guard bits in the accumulator, there is no risk of overflow, and the  
 * full precision of the intermediate multiplication is preserved.  
 * Finally, the 34.30 result is truncated to 34.15 format by discarding the lower   
 * 15 bits, and then saturated to yield a result in 1.15 format.  
 *  
 */ 
 
 
void arm_var_q15( 
  q15_t * pSrc, 
  uint32_t blockSize, 
  q31_t * pResult) 
{ 
  q63_t sum = 0;                                 /* Accumulator */ 
  q31_t meanOfSquares, squareOfMean;             /* Mean of square and square of mean */ 
  q15_t mean;                                    /* mean */ 
  q31_t in;                                      /* Input variable */ 
  q15_t in1;                                     /* Temporary variable */ 
  uint32_t blkCnt;                               /* loop counter */ 
  q15_t t;                                       /* Temporary variable */ 
  q15_t *pIn;									 /* Temporary pointer */ 
 
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
    in = *__SIMD32(pSrc)++; 
    sum = __SMLALD(in, in, sum); 
    in = *__SIMD32(pSrc)++; 
    sum = __SMLALD(in, in, sum); 
 
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
    in1 = *pSrc++; 
    sum = __SMLALD(in1, in1, sum); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Compute Mean of squares of the input samples  
   * and then store the result in a temporary variable, meanOfSquares. */ 
  t = (q15_t) ((1.0f / (float32_t) (blockSize - 1u)) * 16384); 
  sum = __SSAT((sum >> 15u), 16u); 
 
  meanOfSquares = (q31_t) ((sum * t) >> 14u); 
 
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
 
  /* Compute mean of all input values */ 
  t = (q15_t) ((1.0f / (float32_t) (blockSize * (blockSize - 1u))) * 32768); 
  mean = __SSAT(sum, 16u); 
 
  /* Compute square of mean */ 
  squareOfMean = ((q31_t) mean * mean) >> 15; 
  squareOfMean = (q31_t) (((q63_t) squareOfMean * t) >> 15); 
 
  /* Compute variance and then store the result to the destination */ 
  *pResult = (meanOfSquares - squareOfMean); 
 
} 
 
/**  
 * @} end of variance group  
 */ 
