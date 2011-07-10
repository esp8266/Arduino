/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_abs_q15.c  
*  
* Description:	Q15 vector absolute value.  
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
 * @addtogroup BasicAbs  
 * @{  
 */ 
 
/**  
 * @brief Q15 vector absolute value.  
 * @param[in]       *pSrc points to the input buffer  
 * @param[out]      *pDst points to the output buffer  
 * @param[in]       blockSize number of samples in each vector  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function uses saturating arithmetic.  
 * The Q15 value -1 (0x8000) will be saturated to the maximum allowable positive value 0x7FFF.  
 */ 
 
void arm_abs_q15( 
  q15_t * pSrc, 
  q15_t * pDst, 
  uint32_t blockSize) 
{ 
  uint32_t blkCnt;                               /* loop counter */ 
  q15_t in1;                                     /* Input value1 */ 
  q15_t in2;                                     /* Input value2 */ 
 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = |A| */ 
    /* Read two inputs */ 
    in1 = *pSrc++; 
    in2 = *pSrc++; 
    /* Store the Absolute result in the destination buffer by packing the two values, in a single cycle */ 
    *__SIMD32(pDst)++ = 
      __PKHBT(((in1 > 0) ? in1 : __SSAT(-in1, 16)), 
              ((in2 > 0) ? in2 : __SSAT(-in2, 16)), 16); 
 
    in1 = *pSrc++; 
    in2 = *pSrc++; 
    *__SIMD32(pDst)++ = 
      __PKHBT(((in1 > 0) ? in1 : __SSAT(-in1, 16)), 
              ((in2 > 0) ? in2 : __SSAT(-in2, 16)), 16); 
 
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
 
    /* Calculate absolute value of input and then store the result in the destination buffer. */ 
    *pDst++ = (in1 > 0) ? in1 : __SSAT(-in1, 16); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of BasicAbs group  
 */ 
