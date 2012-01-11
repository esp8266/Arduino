/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_shift_q15.c   
*   
* Description:	Shifts the elements of a Q15 vector by a specified number of bits.   
*   
* Target Processor: Cortex-M4/Cortex-M3/Cortex-M0
*  
* Version 1.0.10 2011/7/15 
*    Big Endian support added and Merged M0 and M3/M4 Source code.  
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
 * @addtogroup shift   
 * @{   
 */

/**   
 * @brief  Shifts the elements of a Q15 vector a specified number of bits.   
 * @param[in]  *pSrc points to the input vector   
 * @param[in]  shiftBits number of bits to shift.  A positive value shifts left; a negative value shifts right.   
 * @param[out]  *pDst points to the output vector   
 * @param[in]  blockSize number of samples in the vector   
 * @return none.   
 *   
 * <b>Scaling and Overflow Behavior:</b>   
 * \par   
 * The function uses saturating arithmetic.   
 * Results outside of the allowable Q15 range [0x8000 0x7FFF] will be saturated.   
 */

void arm_shift_q15(
  q15_t * pSrc,
  int8_t shiftBits,
  q15_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counter */
  uint8_t sign;                                  /* Sign of shiftBits */

#ifndef ARM_MATH_CM0

/* Run the below code for Cortex-M4 and Cortex-M3 */

  q15_t in1, in2;                                /* Temporary variables */


  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* Getting the sign of shiftBits */
  sign = (shiftBits & 0x80);

  /* If the shift value is positive then do right shift else left shift */
  if(sign == 0u)
  {
    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
     ** a second loop below computes the remaining 1 to 3 samples. */
    while(blkCnt > 0u)
    {
      /* Read 2 inputs */
      in1 = *pSrc++;
      in2 = *pSrc++;
      /* C = A << shiftBits */
      /* Shift the inputs and then store the results in the destination buffer. */
#ifndef  ARM_MATH_BIG_ENDIAN

      *__SIMD32(pDst)++ = __PKHBT(__SSAT((in1 << shiftBits), 16),
                                  __SSAT((in2 << shiftBits), 16), 16);

#else

      *__SIMD32(pDst)++ = __PKHBT(__SSAT((in2 << shiftBits), 16),
                                  __SSAT((in1 << shiftBits), 16), 16);

#endif /* #ifndef  ARM_MATH_BIG_ENDIAN    */

      in1 = *pSrc++;
      in2 = *pSrc++;

#ifndef  ARM_MATH_BIG_ENDIAN

      *__SIMD32(pDst)++ = __PKHBT(__SSAT((in1 << shiftBits), 16),
                                  __SSAT((in2 << shiftBits), 16), 16);

#else

      *__SIMD32(pDst)++ = __PKHBT(__SSAT((in2 << shiftBits), 16),
                                  __SSAT((in1 << shiftBits), 16), 16);

#endif /* #ifndef  ARM_MATH_BIG_ENDIAN    */

      /* Decrement the loop counter */
      blkCnt--;
    }

    /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
     ** No loop unrolling is used. */
    blkCnt = blockSize % 0x4u;

    while(blkCnt > 0u)
    {
      /* C = A << shiftBits */
      /* Shift and then store the results in the destination buffer. */
      *pDst++ = __SSAT((*pSrc++ << shiftBits), 16);

      /* Decrement the loop counter */
      blkCnt--;
    }
  }
  else
  {
    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
     ** a second loop below computes the remaining 1 to 3 samples. */
    while(blkCnt > 0u)
    {
      /* Read 2 inputs */
      in1 = *pSrc++;
      in2 = *pSrc++;
      /* C = A >> shiftBits */
      /* Shift the inputs and then store the results in the destination buffer. */
#ifndef  ARM_MATH_BIG_ENDIAN

      *__SIMD32(pDst)++ = __PKHBT((in1 >> -shiftBits),
                                  (in2 >> -shiftBits), 16);

#else

      *__SIMD32(pDst)++ = __PKHBT((in2 >> -shiftBits),
                                  (in1 >> -shiftBits), 16);

#endif /* #ifndef  ARM_MATH_BIG_ENDIAN    */

      in1 = *pSrc++;
      in2 = *pSrc++;

#ifndef  ARM_MATH_BIG_ENDIAN

      *__SIMD32(pDst)++ = __PKHBT((in1 >> -shiftBits),
                                  (in2 >> -shiftBits), 16);

#else

      *__SIMD32(pDst)++ = __PKHBT((in2 >> -shiftBits),
                                  (in1 >> -shiftBits), 16);

#endif /* #ifndef  ARM_MATH_BIG_ENDIAN    */

      /* Decrement the loop counter */
      blkCnt--;
    }

    /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
     ** No loop unrolling is used. */
    blkCnt = blockSize % 0x4u;

    while(blkCnt > 0u)
    {
      /* C = A >> shiftBits */
      /* Shift the inputs and then store the results in the destination buffer. */
      *pDst++ = (*pSrc++ >> -shiftBits);

      /* Decrement the loop counter */
      blkCnt--;
    }
  }

#else

  /* Run the below code for Cortex-M0 */

  /* Getting the sign of shiftBits */
  sign = (shiftBits & 0x80);

  /* If the shift value is positive then do right shift else left shift */
  if(sign == 0u)
  {
    /* Initialize blkCnt with number of samples */
    blkCnt = blockSize;

    while(blkCnt > 0u)
    {
      /* C = A << shiftBits */
      /* Shift and then store the results in the destination buffer. */
      *pDst++ = __SSAT(((q31_t) * pSrc++ << shiftBits), 16);

      /* Decrement the loop counter */
      blkCnt--;
    }
  }
  else
  {
    /* Initialize blkCnt with number of samples */
    blkCnt = blockSize;

    while(blkCnt > 0u)
    {
      /* C = A >> shiftBits */
      /* Shift the inputs and then store the results in the destination buffer. */
      *pDst++ = (*pSrc++ >> -shiftBits);

      /* Decrement the loop counter */
      blkCnt--;
    }
  }

#endif /* #ifndef ARM_MATH_CM0 */

}

/**   
 * @} end of shift group   
 */
