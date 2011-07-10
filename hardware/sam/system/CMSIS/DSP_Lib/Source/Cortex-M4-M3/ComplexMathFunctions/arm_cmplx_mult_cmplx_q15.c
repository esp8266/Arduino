/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_cmplx_mult_cmplx_q15.c  
*  
* Description:	Q15 complex-by-complex multiplication  
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
 * @ingroup groupCmplxMath  
 */ 
 
/**  
 * @addtogroup CmplxByCmplxMult  
 * @{  
 */ 
 
/**  
 * @brief  Q15 complex-by-complex multiplication  
 * @param[in]  *pSrcA points to the first input vector  
 * @param[in]  *pSrcB points to the second input vector  
 * @param[out]  *pDst  points to the output vector  
 * @param[in]  numSamples number of complex samples in each vector  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function implements 1.15 by 1.15 multiplications and finally output is converted into 3.13 format.  
 */ 
 
void arm_cmplx_mult_cmplx_q15( 
  q15_t * pSrcA, 
  q15_t * pSrcB, 
  q15_t * pDst, 
  uint32_t numSamples) 
{ 
  q15_t a, b, c, d;                              /* Temporary variables to store real and imaginary values */ 
  uint32_t blkCnt;                               /* loop counters */ 
 
  /* loop Unrolling */ 
  blkCnt = numSamples >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C[2 * i] = A[2 * i] * B[2 * i] - A[2 * i + 1] * B[2 * i + 1].  */ 
    /* C[2 * i + 1] = A[2 * i] * B[2 * i + 1] + A[2 * i + 1] * B[2 * i].  */ 
    a = *pSrcA++; 
    b = *pSrcA++; 
    c = *pSrcB++; 
    d = *pSrcB++; 
 
    /* store the result in 3.13 format in the destination buffer. */ 
    *pDst++ = 
      (q15_t) (q31_t) (((q31_t) a * c) >> 17) - (((q31_t) b * d) >> 17); 
    /* store the result in 3.13 format in the destination buffer. */ 
    *pDst++ = 
      (q15_t) (q31_t) (((q31_t) a * d) >> 17) + (((q31_t) b * c) >> 17); 
 
    a = *pSrcA++; 
    b = *pSrcA++; 
    c = *pSrcB++; 
    d = *pSrcB++; 
 
    /* store the result in 3.13 format in the destination buffer. */ 
    *pDst++ = 
      (q15_t) (q31_t) (((q31_t) a * c) >> 17) - (((q31_t) b * d) >> 17); 
    /* store the result in 3.13 format in the destination buffer. */ 
    *pDst++ = 
      (q15_t) (q31_t) (((q31_t) a * d) >> 17) + (((q31_t) b * c) >> 17); 
 
    a = *pSrcA++; 
    b = *pSrcA++; 
    c = *pSrcB++; 
    d = *pSrcB++; 
 
    /* store the result in 3.13 format in the destination buffer. */ 
    *pDst++ = 
      (q15_t) (q31_t) (((q31_t) a * c) >> 17) - (((q31_t) b * d) >> 17); 
    /* store the result in 3.13 format in the destination buffer. */ 
    *pDst++ = 
      (q15_t) (q31_t) (((q31_t) a * d) >> 17) + (((q31_t) b * c) >> 17); 
 
    a = *pSrcA++; 
    b = *pSrcA++; 
    c = *pSrcB++; 
    d = *pSrcB++; 
 
    /* store the result in 3.13 format in the destination buffer. */ 
    *pDst++ = 
      (q15_t) (q31_t) (((q31_t) a * c) >> 17) - (((q31_t) b * d) >> 17); 
    /* store the result in 3.13 format in the destination buffer. */ 
    *pDst++ = 
      (q15_t) (q31_t) (((q31_t) a * d) >> 17) + (((q31_t) b * c) >> 17); 
 
    /* Decrement the blockSize loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = numSamples % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C[2 * i] = A[2 * i] * B[2 * i] - A[2 * i + 1] * B[2 * i + 1].  */ 
    /* C[2 * i + 1] = A[2 * i] * B[2 * i + 1] + A[2 * i + 1] * B[2 * i].  */ 
    a = *pSrcA++; 
    b = *pSrcA++; 
    c = *pSrcB++; 
    d = *pSrcB++; 
 
    /* store the result in 3.13 format in the destination buffer. */ 
    *pDst++ = 
      (q15_t) (q31_t) (((q31_t) a * c) >> 17) - (((q31_t) b * d) >> 17); 
    /* store the result in 3.13 format in the destination buffer. */ 
    *pDst++ = 
      (q15_t) (q31_t) (((q31_t) a * d) >> 17) + (((q31_t) b * c) >> 17); 
 
    /* Decrement the blockSize loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of CmplxByCmplxMult group  
 */ 
