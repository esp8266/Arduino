/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_mult_q15.c  
*  
* Description:	Q15 vector multiplication.  
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
* Version 0.0.5  2010/04/26   
*    incorporated review comments and updated with latest CMSIS layer  
*  
* Version 0.0.3  2010/03/10   
*    Initial version  
* -------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupMath  
 */ 
 
/**  
 * @addtogroup BasicMult  
 * @{  
 */ 
 
 
/**  
 * @brief           Q15 vector multiplication  
 * @param[in]       *pSrcA points to the first input vector  
 * @param[in]       *pSrcB points to the second input vector  
 * @param[out]      *pDst points to the output vector  
 * @param[in]       blockSize number of samples in each vector  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function uses saturating arithmetic.  
 * Results outside of the allowable Q15 range [0x8000 0x7FFF] will be saturated.  
 */ 
 
void arm_mult_q15( 
  q15_t * pSrcA, 
  q15_t * pSrcB, 
  q15_t * pDst, 
  uint32_t blockSize) 
{ 
  uint32_t blkCnt;                               /* loop counters */ 
 
  /* loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = A * B */ 
    /* Multiply the inputs and store the result in the destination buffer */ 
    *pDst++ = (q15_t) __SSAT((((q31_t) (*pSrcA++) * (*pSrcB++)) >> 15), 16); 
    *pDst++ = (q15_t) __SSAT((((q31_t) (*pSrcA++) * (*pSrcB++)) >> 15), 16); 
    *pDst++ = (q15_t) __SSAT((((q31_t) (*pSrcA++) * (*pSrcB++)) >> 15), 16); 
    *pDst++ = (q15_t) __SSAT((((q31_t) (*pSrcA++) * (*pSrcB++)) >> 15), 16); 
 
    /* Decrement the blockSize loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C = A * B */ 
    /* Multiply the inputs and store the result in the destination buffer */ 
    *pDst++ = (q15_t) __SSAT((((q31_t) (*pSrcA++) * (*pSrcB++)) >> 15), 16); 
 
    /* Decrement the blockSize loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of BasicMult group  
 */ 
