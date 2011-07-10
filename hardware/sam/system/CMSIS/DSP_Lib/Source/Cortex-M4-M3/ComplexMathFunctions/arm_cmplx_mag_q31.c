/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_cmplx_mag_q31.c  
*  
* Description:	Q31 complex magnitude  
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
 * @addtogroup cmplx_mag  
 * @{  
 */ 
 
/**  
 * @brief  Q31 complex magnitude  
 * @param  *pSrc points to the complex input vector  
 * @param  *pDst points to the real output vector  
 * @param  numSamples number of complex samples in the input vector  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function implements 1.31 by 1.31 multiplications and finally output is converted into 2.30 format.  
 * Input down scaling is not required.  
 */ 
 
void arm_cmplx_mag_q31( 
  q31_t * pSrc, 
  q31_t * pDst, 
  uint32_t numSamples) 
{ 
  q31_t real, imag;                              /* Temporary variables to hold input values */ 
  q31_t acc0, acc1;                              /* Accumulators */ 
  uint32_t blkCnt;                               /* loop counter */ 
 
 
  /*loop Unrolling */ 
  blkCnt = numSamples >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
 
    /* C[0] = sqrt(A[0] * A[0] + A[1] * A[1]) */ 
    real = *pSrc++; 
    imag = *pSrc++; 
    acc0 = (q31_t) (((q63_t) real * real) >> 33); 
    acc1 = (q31_t) (((q63_t) imag * imag) >> 33); 
    /* store the result in 2.30 format in the destination buffer. */ 
    arm_sqrt_q31(acc0 + acc1, pDst++); 
 
    real = *pSrc++; 
    imag = *pSrc++; 
    acc0 = (q31_t) (((q63_t) real * real) >> 33); 
    acc1 = (q31_t) (((q63_t) imag * imag) >> 33); 
    /* store the result in 2.30 format in the destination buffer. */ 
    arm_sqrt_q31(acc0 + acc1, pDst++); 
 
    real = *pSrc++; 
    imag = *pSrc++; 
    acc0 = (q31_t) (((q63_t) real * real) >> 33); 
    acc1 = (q31_t) (((q63_t) imag * imag) >> 33); 
    /* store the result in 2.30 format in the destination buffer. */ 
    arm_sqrt_q31(acc0 + acc1, pDst++); 
 
    real = *pSrc++; 
    imag = *pSrc++; 
    acc0 = (q31_t) (((q63_t) real * real) >> 33); 
    acc1 = (q31_t) (((q63_t) imag * imag) >> 33); 
    /* store the result in 2.30 format in the destination buffer. */ 
    arm_sqrt_q31(acc0 + acc1, pDst++); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the numSamples is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = numSamples % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C[0] = sqrt(A[0] * A[0] + A[1] * A[1]) */ 
    real = *pSrc++; 
    imag = *pSrc++; 
    acc0 = (q31_t) (((q63_t) real * real) >> 33); 
    acc1 = (q31_t) (((q63_t) imag * imag) >> 33); 
    /* store the result in 2.30 format in the destination buffer. */ 
    arm_sqrt_q31(acc0 + acc1, pDst++); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of cmplx_mag group  
 */ 
