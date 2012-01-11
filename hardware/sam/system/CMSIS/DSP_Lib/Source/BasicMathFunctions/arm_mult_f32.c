/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_mult_f32.c   
*   
* Description:	Floating-point vector multiplication.   
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
 * @defgroup BasicMult Vector Multiplication   
 *   
 * Element-by-element multiplication of two vectors.   
 *   
 * <pre>   
 *     pDst[n] = pSrcA[n] * pSrcB[n],   0 <= n < blockSize.   
 * </pre>   
 *   
 * There are separate functions for floating-point, Q7, Q15, and Q31 data types.   
 */

/**   
 * @addtogroup BasicMult   
 * @{   
 */

/**   
 * @brief Floating-point vector multiplication.   
 * @param[in]       *pSrcA points to the first input vector   
 * @param[in]       *pSrcB points to the second input vector   
 * @param[out]      *pDst points to the output vector   
 * @param[in]       blockSize number of samples in each vector   
 * @return none.   
 */

void arm_mult_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counters */

#ifndef ARM_MATH_CM0

/* Run the below code for Cortex-M4 and Cortex-M3 */

  /* loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = A * B */
    /* Multiply the inputs and store the results in output buffer */
    *pDst++ = (*pSrcA++) * (*pSrcB++);
    *pDst++ = (*pSrcA++) * (*pSrcB++);
    *pDst++ = (*pSrcA++) * (*pSrcB++);
    *pDst++ = (*pSrcA++) * (*pSrcB++);

    /* Decrement the blockSize loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

#else

  /* Run the below code for Cortex-M0 */

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #ifndef ARM_MATH_CM0 */


  while(blkCnt > 0u)
  {
    /* C = A * B */
    /* Multiply the inputs and store the results in output buffer */
    *pDst++ = (*pSrcA++) * (*pSrcB++);

    /* Decrement the blockSize loop counter */
    blkCnt--;
  }

}

/**   
 * @} end of BasicMult group   
 */
