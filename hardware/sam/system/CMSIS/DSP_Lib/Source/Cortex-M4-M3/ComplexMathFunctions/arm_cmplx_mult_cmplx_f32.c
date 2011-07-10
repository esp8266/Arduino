/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_cmplx_mult_cmplx_f32.c  
*  
* Description:	Floating-point complex-by-complex multiplication  
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
 * @defgroup CmplxByCmplxMult Complex-by-Complex Multiplication  
 *  
 * Multiplies a complex vector by another complex vector and generates a complex result.  
 * The data in the complex arrays is stored in an interleaved fashion  
 * (real, imag, real, imag, ...).  
 * The parameter <code>numSamples</code> represents the number of complex  
 * samples processed.  The complex arrays have a total of <code>2*numSamples</code>  
 * real values.  
 *  
 * The underlying algorithm is used:  
 *  
 * <pre>  
 * for(n=0; n<numSamples; n++) {  
 *     pDst[(2*n)+0] = pSrcA[(2*n)+0] * pSrcB[(2*n)+0] - pSrcA[(2*n)+1] * pSrcB[(2*n)+1];  
 *     pDst[(2*n)+1] = pSrcA[(2*n)+0] * pSrcB[(2*n)+1] + pSrcA[(2*n)+1] * pSrcB[(2*n)+0];  
 * }  
 * </pre>  
 *  
 * There are separate functions for floating-point, Q15, and Q31 data types.  
 */ 
 
/**  
 * @addtogroup CmplxByCmplxMult  
 * @{  
 */ 
 
 
/**  
 * @brief  Floating-point complex-by-complex multiplication  
 * @param[in]  *pSrcA points to the first input vector  
 * @param[in]  *pSrcB points to the second input vector  
 * @param[out]  *pDst  points to the output vector  
 * @param[in]  numSamples number of complex samples in each vector  
 * @return none.  
 */ 
 
void arm_cmplx_mult_cmplx_f32( 
  float32_t * pSrcA, 
  float32_t * pSrcB, 
  float32_t * pDst, 
  uint32_t numSamples) 
{ 
  float32_t a, b, c, d;                          /* Temporary variables to store real and imaginary values */ 
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
 
    /* store the result in the destination buffer. */ 
    *pDst++ = (a * c) - (b * d); 
    *pDst++ = (a * d) + (b * c); 
 
    a = *pSrcA++; 
    b = *pSrcA++; 
    c = *pSrcB++; 
    d = *pSrcB++; 
 
    *pDst++ = (a * c) - (b * d); 
    *pDst++ = (a * d) + (b * c); 
 
    a = *pSrcA++; 
    b = *pSrcA++; 
    c = *pSrcB++; 
    d = *pSrcB++; 
 
    *pDst++ = (a * c) - (b * d); 
    *pDst++ = (a * d) + (b * c); 
 
    a = *pSrcA++; 
    b = *pSrcA++; 
    c = *pSrcB++; 
    d = *pSrcB++; 
 
    *pDst++ = (a * c) - (b * d); 
    *pDst++ = (a * d) + (b * c); 
 
    /* Decrement the numSamples loop counter */ 
    blkCnt--; 
  } 
 
  /* If the numSamples is not a multiple of 4, compute any remaining output samples here.  
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
 
    /* store the result in the destination buffer. */ 
    *pDst++ = (a * c) - (b * d); 
    *pDst++ = (a * d) + (b * c); 
 
    /* Decrement the numSamples loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of CmplxByCmplxMult group  
 */ 
