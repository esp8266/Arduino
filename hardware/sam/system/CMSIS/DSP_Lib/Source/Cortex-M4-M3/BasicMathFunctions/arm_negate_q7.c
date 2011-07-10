/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_negate_q7.c  
*  
* Description:	Negates Q7 vectors.  
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
 * @addtogroup negate  
 * @{  
 */ 
 
/**  
 * @brief  Negates the elements of a Q7 vector.  
 * @param  *pSrc points to the input vector  
 * @param  *pDst points to the output vector  
 * @param  blockSize number of samples in the vector  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function uses saturating arithmetic.  
 * The Q7 value -1 (0x80) will be saturated to the maximum allowable positive value 0x7F.  
 */ 
 
void arm_negate_q7( 
  q7_t * pSrc, 
  q7_t * pDst, 
  uint32_t blockSize) 
{ 
  uint32_t blkCnt;                               /* loop counter */ 
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
    /* C = ~A */ 
    /* Read four inputs */ 
    in1 = *pSrc++; 
    in2 = *pSrc++; 
    in3 = *pSrc++; 
    in4 = *pSrc++; 
 
    /* Store the Negated results in the destination buffer in a single cycle by packing the results */ 
    *__SIMD32(pDst)++ = 
      __PACKq7(__SSAT(-in1, 8), __SSAT(-in2, 8), __SSAT(-in3, 8), 
               __SSAT(-in4, 8)); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C = ~A */ 
    /* Negate and then store the results in the destination buffer. */ 
    *pDst++ = __SSAT(-*pSrc++, 8); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of negate group  
 */ 
