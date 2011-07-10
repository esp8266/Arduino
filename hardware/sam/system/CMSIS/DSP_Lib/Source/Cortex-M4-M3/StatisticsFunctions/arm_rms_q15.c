/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_rms_q15.c  
*  
* Description:	Processing function for the Q15 RMS  
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
 * @addtogroup RMS  
 * @{  
 */ 
 
/**  
 * @brief Root Mean Square of the elements of a Q15 vector.  
 * @param[in]       *pSrc points to the input vector  
 * @param[in]       blockSize length of the input vector  
 * @param[out]      *pResult rms value returned here  
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
 
void arm_rms_q15( 
  q15_t * pSrc, 
  uint32_t blockSize, 
  q15_t * pResult) 
{ 
  q63_t sum = 0;                                 /* accumulator */ 
  q31_t in;                                      /* temporary variable to store the input value */ 
  q15_t in1;                                     /* temporary variable to store the input value */ 
  uint32_t blkCnt;                               /* loop counter */ 
 
  /* loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = (A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1]) */ 
    /* Compute sum of the squares and then store the results in a temporary variable, sum */ 
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
    /* Compute sum of the squares and then store the results in a temporary variable, sum */ 
    in1 = *pSrc++; 
    sum = __SMLALD(in1, in1, sum); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Truncating and saturating the accumulator to 1.15 format */ 
  sum = __SSAT((q31_t) (sum >> 15), 16); 
 
  in1 = (q15_t) (sum / blockSize); 
 
  /* Store the result in the destination */ 
  arm_sqrt_q15(in1, pResult); 
} 
 
/**  
 * @} end of RMS group  
 */ 
