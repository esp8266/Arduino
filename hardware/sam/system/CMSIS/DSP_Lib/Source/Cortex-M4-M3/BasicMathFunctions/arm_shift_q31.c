/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_shift_q31.c  
*  
* Description:	Shifts the elements of a Q31 vector  
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
* -------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupMath  
 */ 
/**  
 * @defgroup shift Vector Shift  
 *  
 * Shifts the elements of a fixed-point vector by a specified number of bits.  
 * There are separate functions for Q7, Q15, and Q31 data types.  
 * The underlying algorithm used is:  
 *  
 * <pre>  
 *     pDst[n] = pSrc[n] << shift,   0 <= n < blockSize.  
 * </pre>  
 *  
 * If <code>shift</code> is positive then the elements of the vector are shifted to the left.  
 * If <code>shift</code> is negative then the elements of the vector are shifted to the right.  
 */ 
 
/**  
 * @addtogroup shift  
 * @{  
 */ 
 
/**  
 * @brief  Shifts the elements of a Q31 vector a specified number of bits.  
 * @param  *pSrc points to the input vector  
 * @param  shiftBits number of bits to shift.  A positive value shifts left; a negative value shifts right.  
 * @param  *pDst points to the output vector  
 * @param  blockSize number of samples in the vector  
 * @return none.  
 *  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function uses saturating arithmetic.  
 * Results outside of the allowable Q31 range [0x80000000 0x7FFFFFFF] will be saturated.  
 */ 
 
void arm_shift_q31( 
  q31_t * pSrc, 
  int8_t shiftBits, 
  q31_t * pDst, 
  uint32_t blockSize) 
{ 
  uint32_t blkCnt;                               /* loop counter */ 
  uint32_t sign;                                 /* Sign of shiftBits */ 
 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* Getting the sign of shiftBits */ 
  sign = (shiftBits & 0x80000000); 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = A (>> or <<) shiftBits */ 
    /* Shift the input and then store the results in the destination buffer. */ 
    *pDst++ = (sign == 0u) ? clip_q63_to_q31((q63_t) * pSrc++ << shiftBits) : 
                             (*pSrc++ >> -shiftBits); 
    *pDst++ = (sign == 0u) ? clip_q63_to_q31((q63_t) * pSrc++ << shiftBits) : 
                             (*pSrc++ >> -shiftBits); 
    *pDst++ = (sign == 0u) ? clip_q63_to_q31((q63_t) * pSrc++ << shiftBits) : 
                             (*pSrc++ >> -shiftBits); 
    *pDst++ = (sign == 0u) ? clip_q63_to_q31((q63_t) * pSrc++ << shiftBits) : 
                             (*pSrc++ >> -shiftBits); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C = A (>> or <<) shiftBits */ 
    /* Shift the input and then store the result in the destination buffer. */ 
    *pDst++ = (sign == 0u) ? clip_q63_to_q31((q63_t) * pSrc++ << shiftBits) : 
                             (*pSrc++ >> -shiftBits); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
} 
 
/**  
 * @} end of shift group  
 */ 
