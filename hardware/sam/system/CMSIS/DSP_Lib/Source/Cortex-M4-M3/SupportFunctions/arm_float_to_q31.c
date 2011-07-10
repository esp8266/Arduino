/* ----------------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_float_to_q31.c  
*  
* Description:	Processing function for the Conversion from float to Q31  
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
 * @ingroup groupSupport  
 */ 
 
/**  
 * @defgroup float_to_x  Convert 32-bit floating point value  
 */ 
 
/**  
 * @addtogroup float_to_x  
 * @{  
 */ 
 
/**  
 * @brief Converts the elements of the floating-point vector to Q31 vector.  
 * @param[in]       *pSrc points to the floating-point input vector  
 * @param[out]      *pDst points to the Q31 output vector 
 * @param[in]       blockSize length of the input vector  
 * @return none.  
 *  
 *\par Description:  
 * \par 
 * The equation used for the conversion process is:  
 * 
 * <pre>  
 * 	pDst[n] = (q31_t)(pSrc[n] * 2147483648);   0 <= n < blockSize.  
 * </pre>  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function uses saturating arithmetic.  
 * Results outside of the allowable Q31 range[0x80000000 0x7FFFFFFF] will be saturated.  
 * 
 * \note In order to apply rounding, the library should be rebuilt with the ROUNDING macro   
 * defined in the preprocessor section of project options.   
 */ 
 
 
void arm_float_to_q31( 
  float32_t * pSrc, 
  q31_t * pDst, 
  uint32_t blockSize) 
{ 
  float32_t *pIn = pSrc;                         /* Src pointer */ 
  uint32_t blkCnt;                               /* loop counter */ 
 
#ifdef ARM_MATH_ROUNDING 
 
  float32_t in; 
 
#endif 
 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
#ifdef ARM_MATH_ROUNDING 
    /* C = A * 32768 */ 
    /* convert from float to Q31 and then store the results in the destination buffer */ 
    in = *pIn++; 
    in = (in * 2147483648.0f); 
    in += in > 0 ? 0.5 : -0.5; 
    *pDst++ = clip_q63_to_q31((q63_t) (in)); 
 
    in = *pIn++; 
    in = (in * 2147483648.0f); 
    in += in > 0 ? 0.5 : -0.5; 
    *pDst++ = clip_q63_to_q31((q63_t) (in)); 
 
    in = *pIn++; 
    in = (in * 2147483648.0f); 
    in += in > 0 ? 0.5 : -0.5; 
    *pDst++ = clip_q63_to_q31((q63_t) (in)); 
 
    in = *pIn++; 
    in = (in * 2147483648.0f); 
    in += in > 0 ? 0.5 : -0.5; 
    *pDst++ = clip_q63_to_q31((q63_t) (in)); 
 
#else 
 
    /* C = A * 2147483648 */ 
    /* convert from float to Q31 and then store the results in the destination buffer */ 
    *pDst++ = clip_q63_to_q31((q63_t) (*pIn++ * 2147483648.0f)); 
    *pDst++ = clip_q63_to_q31((q63_t) (*pIn++ * 2147483648.0f)); 
    *pDst++ = clip_q63_to_q31((q63_t) (*pIn++ * 2147483648.0f)); 
    *pDst++ = clip_q63_to_q31((q63_t) (*pIn++ * 2147483648.0f)); 
 
#endif 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
#ifdef ARM_MATH_ROUNDING 
 
    /* C = A * 2147483648 */ 
    /* convert from float to Q31 and then store the results in the destination buffer */ 
    in = *pIn++; 
    in = (in * 2147483648.0f); 
    in += in > 0 ? 0.5 : -0.5; 
    *pDst++ = clip_q63_to_q31((q63_t) (in)); 
 
#else 
 
    /* C = A * 2147483648 */ 
    /* convert from float to Q31 and then store the results in the destination buffer */ 
    *pDst++ = clip_q63_to_q31((q63_t) (*pIn++ * 2147483648.0f)); 
 
#endif 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of float_to_x group  
 */ 
