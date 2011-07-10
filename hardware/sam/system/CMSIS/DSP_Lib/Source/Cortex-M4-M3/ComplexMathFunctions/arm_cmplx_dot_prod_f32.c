/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_cmplx_dot_prod_f32.c  
*  
* Description:	Floating-point complex dot product  
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
 * @defgroup cmplx_dot_prod Complex Dot Product  
 *  
 * Computes the dot product of two complex vectors.  
 * The vectors are multiplied element-by-element and then summed.  
 * 
 * The <code>pSrcA</code> points to the first complex input vector and  
 * <code>pSrcB</code> points to the second complex input vector.  
 * <code>numSamples</code> specifies the number of complex samples  
 * and the data in each array is stored in an interleaved fashion  
 * (real, imag, real, imag, ...).  
 * Each array has a total of <code>2*numSamples</code> values.  
 *  
 * The underlying algorithm is used:  
 * <pre>  
 * realResult=0;  
 * imagResult=0;  
 * for(n=0; n<numSamples; n++) {  
 *     realResult += pSrcA[(2*n)+0]*pSrcB[(2*n)+0] - pSrcA[(2*n)+1]*pSrcB[(2*n)+1];  
 *     imagResult += pSrcA[(2*n)+0]*pSrcB[(2*n)+1] + pSrcA[(2*n)+1]*pSrcB[(2*n)+0];  
 * }  
 * </pre>  
 *  
 * There are separate functions for floating-point, Q15, and Q31 data types.  
 */ 
 
/**  
 * @addtogroup cmplx_dot_prod  
 * @{  
 */ 
 
/**  
 * @brief  Floating-point complex dot product  
 * @param  *pSrcA points to the first input vector  
 * @param  *pSrcB points to the second input vector  
 * @param  numSamples number of complex samples in each vector  
 * @param  *realResult real part of the result returned here  
 * @param  *imagResult imaginary part of the result returned here  
 * @return none.  
 */ 
 
void arm_cmplx_dot_prod_f32( 
  float32_t * pSrcA, 
  float32_t * pSrcB, 
  uint32_t numSamples, 
  float32_t * realResult, 
  float32_t * imagResult) 
{ 
  float32_t real_sum = 0.0f, imag_sum = 0.0f;    /* Temporary result storage */ 
  uint32_t blkCnt;                               /* loop counter */ 
 
  /*loop Unrolling */ 
  blkCnt = numSamples >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* CReal = A[0]* B[0] + A[2]* B[2] + A[4]* B[4] + .....+ A[numSamples-2]* B[numSamples-2] */ 
    real_sum += (*pSrcA++) * (*pSrcB++); 
    /* CImag = A[1]* B[1] + A[3]* B[3] + A[5]* B[5] + .....+ A[numSamples-1]* B[numSamples-1] */ 
    imag_sum += (*pSrcA++) * (*pSrcB++); 
 
    real_sum += (*pSrcA++) * (*pSrcB++); 
    imag_sum += (*pSrcA++) * (*pSrcB++); 
 
    real_sum += (*pSrcA++) * (*pSrcB++); 
    imag_sum += (*pSrcA++) * (*pSrcB++); 
 
    real_sum += (*pSrcA++) * (*pSrcB++); 
    imag_sum += (*pSrcA++) * (*pSrcB++); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the numSamples is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = numSamples % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* CReal = A[0]* B[0] + A[2]* B[2] + A[4]* B[4] + .....+ A[numSamples-2]* B[numSamples-2] */ 
    real_sum += (*pSrcA++) * (*pSrcB++); 
    /* CImag = A[1]* B[1] + A[3]* B[3] + A[5]* B[5] + .....+ A[numSamples-1]* B[numSamples-1] */ 
    imag_sum += (*pSrcA++) * (*pSrcB++); 
 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Store the real and imaginary results in the destination buffers */ 
  *realResult = real_sum; 
  *imagResult = imag_sum; 
} 
 
/**  
 * @} end of cmplx_dot_prod group  
 */ 
