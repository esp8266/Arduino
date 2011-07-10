/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_cmplx_conj_q15.c  
*  
* Description:	Q15 complex conjugate.  
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
 * @ingroup groupCmplxMath  
 */ 
 
/**  
 * @addtogroup cmplx_conj  
 * @{  
 */ 
 
/**  
 * @brief  Q15 complex conjugate.  
 * @param  *pSrc points to the input vector  
 * @param  *pDst points to the output vector  
 * @param  numSamples number of complex samples in each vector  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function uses saturating arithmetic.  
 * The Q15 value -1 (0x8000) will be saturated to the maximum allowable positive value 0x7FFF.  
 */ 
 
void arm_cmplx_conj_q15( 
  q15_t * pSrc, 
  q15_t * pDst, 
  uint32_t numSamples) 
{ 
  uint32_t blkCnt;                               /* loop counter */ 
 
  /*loop Unrolling */ 
  blkCnt = numSamples >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C[0]+jC[1] = A[0]+ j (-1) A[1] */ 
    /* Calculate Complex Conjugate and then store the results in the destination buffer. */ 
    *pDst++ = *pSrc++; 
    *pDst++ = __SSAT(-*pSrc++, 16); 
    *pDst++ = *pSrc++; 
    *pDst++ = __SSAT(-*pSrc++, 16); 
    *pDst++ = *pSrc++; 
    *pDst++ = __SSAT(-*pSrc++, 16); 
    *pDst++ = *pSrc++; 
    *pDst++ = __SSAT(-*pSrc++, 16); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the numSamples is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = numSamples % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C[0]+jC[1] = A[0]+ j (-1) A[1] */ 
    /* Calculate Complex Conjugate and then store the results in the destination buffer. */ 
    *pDst++ = *pSrc++; 
    *pDst++ = __SSAT(-*pSrc++, 16); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of cmplx_conj group  
 */ 
