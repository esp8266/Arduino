/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_mean_q15.c   
*   
* Description:	Mean value of a Q15 vector.  
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
* -------------------------------------------------------------------- */

#include "arm_math.h"

/**   
 * @ingroup groupStats   
 */

/**   
 * @addtogroup mean   
 * @{   
 */

/**   
 * @brief Mean value of a Q15 vector.   
 * @param[in]       *pSrc points to the input vector   
 * @param[in]       blockSize length of the input vector   
 * @param[out]      *pResult mean value returned here   
 * @return none.   
 *   
 * @details   
 * <b>Scaling and Overflow Behavior:</b>   
 * \par   
 * The function is implemented using a 32-bit internal accumulator.   
 * The input is represented in 1.15 format and is accumulated in a 32-bit    
 * accumulator in 17.15 format.    
 * There is no risk of internal overflow with this approach, and the    
 * full precision of intermediate result is preserved.    
 * Finally, the accumulator is saturated and truncated to yield a result of 1.15 format.   
 *   
 */


void arm_mean_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult)
{
  q31_t sum = 0;                                 /* Temporary result storage */
  uint32_t blkCnt;                               /* loop counter */

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */
    sum += *pSrc++;
    sum += *pSrc++;
    sum += *pSrc++;
    sum += *pSrc++;

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
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */
    sum += *pSrc++;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) / blockSize  */
  /* Store the result to the destination */
  *pResult = (q15_t) (sum / blockSize);
}

/**   
 * @} end of mean group   
 */
