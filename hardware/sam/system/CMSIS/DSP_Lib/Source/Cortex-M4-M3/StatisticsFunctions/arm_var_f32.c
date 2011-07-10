/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_var_f32.c  
*  
* Description:	Variance of an array of F32 type.  
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
* ---------------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupStats  
 */ 
 
/**  
 * @defgroup variance  Variance  
 *  
 * Calculates the variance of the elements in the input vector.  
 * The underlying algorithm is used:  
 *  
 * <pre>  
 * 	Result = (sumOfSquares - sum<sup>2</sup> / blockSize) / (blockSize - 1) 
 * 
 *	   where, sumOfSquares = pSrc[0] * pSrc[0] + pSrc[1] * pSrc[1] + ... + pSrc[blockSize-1] * pSrc[blockSize-1] 
 * 
 *	                   sum = pSrc[0] + pSrc[1] + pSrc[2] + ... + pSrc[blockSize-1] 
 * </pre> 
 *  
 * There are separate functions for floating point, Q31, and Q15 data types.  
 */ 
 
/**  
 * @addtogroup variance  
 * @{  
 */ 
 
 
/**  
 * @brief Variance of the elements of a floating-point vector.  
 * @param[in]       *pSrc points to the input vector  
 * @param[in]       blockSize length of the input vector  
 * @param[out]      *pResult variance value returned here  
 * @return none.  
 *  
 */ 
 
 
void arm_var_f32( 
  float32_t * pSrc, 
  uint32_t blockSize, 
  float32_t * pResult) 
{ 
  float32_t sum = (float32_t) 0.0;               /* Accumulator */ 
  float32_t meanOfSquares, mean, in, squareOfMean;      /* Temporary variables */ 
  uint32_t blkCnt;                               /* loop counter */ 
  float32_t *pIn;                                /* Temporary pointer */ 
 
  /* updating temporary pointer */ 
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
    sum += in * in; 
    in = *pSrc++; 
    sum += in * in; 
    in = *pSrc++; 
    sum += in * in; 
    in = *pSrc++; 
    sum += in * in; 
 
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
    sum += in * in; 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Compute Mean of squares of the input samples  
   * and then store the result in a temporary variable, meanOfSquares. */ 
  meanOfSquares = sum / ((float32_t) blockSize - 1.0f); 
 
  /* Reset the accumulator */ 
  sum = 0.0f; 
 
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
  mean = sum / (float32_t) blockSize; 
 
  /* Compute square of mean */ 
  squareOfMean = (mean * mean) * (((float32_t) blockSize) / 
                                  ((float32_t) blockSize - 1.0f)); 
 
  /* Compute variance and then store the result to the destination */ 
  *pResult = meanOfSquares - squareOfMean; 
 
} 
 
/**  
 * @} end of variance group  
 */ 
