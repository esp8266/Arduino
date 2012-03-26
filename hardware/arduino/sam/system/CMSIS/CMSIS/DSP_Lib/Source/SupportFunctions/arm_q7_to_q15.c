/* ----------------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_q7_to_q15.c   
*   
* Description:	Converts the elements of the Q7 vector to Q15 vector.   
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
* ---------------------------------------------------------------------------- */

#include "arm_math.h"

/**   
 * @ingroup groupSupport   
 */

/**   
 * @addtogroup q7_to_x   
 * @{   
 */




/**   
 * @brief Converts the elements of the Q7 vector to Q15 vector.   
 * @param[in]       *pSrc points to the Q7 input vector   
 * @param[out]      *pDst points to the Q15 output vector  
 * @param[in]       blockSize length of the input vector   
 * @return none.   
 *   
 * \par Description:   
 *   
 * The equation used for the conversion process is:   
 *  
 * <pre>   
 * 	pDst[n] = (q15_t) pSrc[n] << 8;   0 <= n < blockSize.   
 * </pre>   
 *  
 */


void arm_q7_to_q15(
  q7_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize)
{
  q7_t *pIn = pSrc;                              /* Src pointer */
  uint32_t blkCnt;                               /* loop counter */

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = (q15_t) A << 8 */
    /* convert from q7 to q15 and then store the results in the destination buffer */
    *pDst++ = (q15_t) * pIn++ << 8;
    *pDst++ = (q15_t) * pIn++ << 8;
    *pDst++ = (q15_t) * pIn++ << 8;
    *pDst++ = (q15_t) * pIn++ << 8;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

#else

  /* Run the below code for Cortex-M0 */

  /* Loop over blockSize number of values */
  blkCnt = blockSize;

#endif /* #ifndef ARM_MATH_CM0 */

  while(blkCnt > 0u)
  {
    /* C = (q15_t) A << 8 */
    /* convert from q7 to q15 and then store the results in the destination buffer */
    *pDst++ = (q15_t) * pIn++ << 8;

    /* Decrement the loop counter */
    blkCnt--;
  }

}

/**   
 * @} end of q7_to_x group   
 */
