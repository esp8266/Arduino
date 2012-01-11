/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_abs_q7.c   
*   
* Description:	Q7 vector absolute value.   
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
 * @addtogroup BasicAbs   
 * @{   
 */

/**   
 * @brief Q7 vector absolute value.   
 * @param[in]       *pSrc points to the input buffer   
 * @param[out]      *pDst points to the output buffer   
 * @param[in]       blockSize number of samples in each vector   
 * @return none.   
 *   
 * <b>Scaling and Overflow Behavior:</b>   
 * \par   
 * The function uses saturating arithmetic.   
 * The Q7 value -1 (0x80) will be saturated to the maximum allowable positive value 0x7F.   
 */

void arm_abs_q7(
  q7_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counter */

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */
  q7_t in1;                                      /* Input value1 */
  q7_t in2;                                      /* Input value2 */
  q7_t in3;                                      /* Input value3 */
  q7_t in4;                                      /* Input value4 */


  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = |A| */
    /* Read 4 inputs */
    in1 = *pSrc++;
    in2 = *pSrc++;
    in3 = *pSrc++;
    in4 = *pSrc++;

    /* Store the Absolute result in the destination buffer by packing the 4 values in single cycle */
    *__SIMD32(pDst)++ =
      __PACKq7(((in1 > 0) ? in1 : __SSAT(-in1, 8)),
               ((in2 > 0) ? in2 : __SSAT(-in2, 8)),
               ((in3 > 0) ? in3 : __SSAT(-in3, 8)),
               ((in4 > 0) ? in4 : __SSAT(-in4, 8)));

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

  while(blkCnt > 0u)
  {
    /* C = |A| */
    /* Read the input */
    in1 = *pSrc++;

    /* Store the Absolute result in the destination buffer */
    *pDst++ = (in1 > 0) ? in1 : __SSAT(-in1, 8);

    /* Decrement the loop counter */
    blkCnt--;
  }

#else

  /* Run the below code for Cortex-M0 */

  q7_t in;                                       /* Temporary input varible */

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

  while(blkCnt > 0u)
  {
    /* C = |A| */
    /* Read the input */
    in = *pSrc++;

    /* Store the Absolute result in the destination buffer */
    *pDst++ = (in > 0) ? in : __SSAT(-in, 8);

    /* Decrement the loop counter */
    blkCnt--;
  }

#endif /*   #ifndef ARM_MATH_CM0   */

}

/**   
 * @} end of BasicAbs group   
 */
