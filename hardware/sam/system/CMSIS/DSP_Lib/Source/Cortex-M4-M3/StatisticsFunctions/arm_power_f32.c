/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_power_f32.c  
*  
* Description:	sum of the square of the elements in a floating point array  
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
*  
* Version 0.0.7  2010/06/10   
*    Misra-C changes done  
* ---------------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupStats  
 */ 
 
/**  
 * @defgroup power Power  
 *  
 * Calculates the sum of the squares of the elements in the input vector.  
 * The underlying algorithm is used:  
 *  
 * <pre>  
 * 	Result = pSrc[0] * pSrc[0] + pSrc[1] * pSrc[1] + pSrc[2] * pSrc[2] + ... + pSrc[blockSize-1] * pSrc[blockSize-1];  
 * </pre>  
 * 
 * There are separate functions for floating point, Q31, Q15, and Q7 data types.   
 */ 
 
/**  
 * @addtogroup power  
 * @{  
 */ 
 
 
/**  
 * @brief Sum of the squares of the elements of a floating-point vector.  
 * @param[in]       *pSrc points to the input vector  
 * @param[in]       blockSize length of the input vector  
 * @param[out]      *pResult sum of the squares value returned here  
 * @return none.  
 *  
 */ 
 
 
void arm_power_f32( 
  float32_t * pSrc, 
  uint32_t blockSize, 
  float32_t * pResult) 
{ 
  float32_t sum = 0.0f;                          /* accumulator */ 
  float32_t in;                                  /* Temporary variable to store input value */ 
  uint32_t blkCnt;                               /* loop counter */ 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = A[0] * A[0] + A[1] * A[1] + A[2] * A[2] + ... + A[blockSize-1] * A[blockSize-1] */ 
    /* Compute Power and then store the result in a temporary variable, sum. */ 
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
    /* C = A[0] * A[0] + A[1] * A[1] + A[2] * A[2] + ... + A[blockSize-1] * A[blockSize-1] */ 
    /* compute power and then store the result in a temporary variable, sum. */ 
    in = *pSrc++; 
    sum += in * in; 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Store the result to the destination */ 
  *pResult = sum; 
} 
 
/**  
 * @} end of power group  
 */ 
