/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_offset_q31.c   
*   
* Description:	Q31 vector offset.   
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
 * @addtogroup offset   
 * @{   
 */

/**   
 * @brief  Adds a constant offset to a Q31 vector.   
 * @param[in]  *pSrc points to the input vector   
 * @param[in]  offset is the offset to be added   
 * @param[out]  *pDst points to the output vector   
 * @param[in]  blockSize number of samples in the vector   
 * @return none.   
 *   
 * <b>Scaling and Overflow Behavior:</b>   
 * \par   
 * The function uses saturating arithmetic.   
 * Results outside of the allowable Q31 range [0x80000000 0x7FFFFFFF] are saturated.   
 */

void arm_offset_q31(
  q31_t * pSrc,
  q31_t offset,
  q31_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counter */

#ifndef ARM_MATH_CM0

/* Run the below code for Cortex-M4 and Cortex-M3 */


  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = A + offset */
    /* Add offset and then store the results in the destination buffer. */
    *pDst++ = __QADD(*pSrc++, offset);
    *pDst++ = __QADD(*pSrc++, offset);
    *pDst++ = __QADD(*pSrc++, offset);
    *pDst++ = __QADD(*pSrc++, offset);

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

  while(blkCnt > 0u)
  {
    /* C = A + offset */
    /* Add offset and then store the result in the destination buffer. */
    *pDst++ = __QADD(*pSrc++, offset);

    /* Decrement the loop counter */
    blkCnt--;
  }

#else

  /* Run the below code for Cortex-M0 */

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

  while(blkCnt > 0u)
  {
    /* C = A + offset */
    /* Add offset and then store the result in the destination buffer. */
    *pDst++ = (q31_t) clip_q63_to_q31((q63_t) * pSrc++ + offset);

    /* Decrement the loop counter */
    blkCnt--;
  }

#endif /* #ifndef ARM_MATH_CM0 */

}

/**   
 * @} end of offset group   
 */
