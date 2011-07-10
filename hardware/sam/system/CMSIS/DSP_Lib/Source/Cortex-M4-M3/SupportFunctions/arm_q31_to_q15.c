/* ----------------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_q31_to_q15.c  
*  
* Description:	Processing function for the Conversion from Q31 to Q15  
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
 * @addtogroup q31_to_x  
 * @{  
 */ 
 
/**  
 * @brief Converts the elements of the Q31 vector to Q15 vector.  
 * @param[in]       *pSrc points to the Q31 input vector  
 * @param[out]      *pDst points to the Q15 output vector 
 * @param[in]       blockSize length of the input vector  
 * @return none.  
 *   
 * \par Description:  
 *  
 * The equation used for the conversion process is:  
 * 
 * <pre>  
 * 	pDst[n] = (q15_t) pSrc[n] >> 16;   0 <= n < blockSize.  
 * </pre>  
 * 
 */ 
 
 
void arm_q31_to_q15( 
  q31_t * pSrc, 
  q15_t * pDst, 
  uint32_t blockSize) 
{ 
  q31_t *pIn = pSrc;                             /* Src pointer */ 
  uint32_t blkCnt;                               /* loop counter */ 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = (q15_t) A >> 16 */ 
    /* convert from q31 to q15 and then store the results in the destination buffer */ 
    *pDst++ = (q15_t) (*pIn++ >> 16); 
    *pDst++ = (q15_t) (*pIn++ >> 16); 
    *pDst++ = (q15_t) (*pIn++ >> 16); 
    *pDst++ = (q15_t) (*pIn++ >> 16); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C = (q15_t) A >> 16 */ 
    /* convert from q31 to q15 and then store the results in the destination buffer */ 
    *pDst++ = (q15_t) (*pIn++ >> 16); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
} 
 
/**  
 * @} end of q31_to_x group  
 */ 
