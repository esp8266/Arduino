/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_abs_q31.c  
*  
* Description:	Q31 vector absolute value.  
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
 * @brief Q31 vector absolute value.  
 * @param[in]       *pSrc points to the input buffer  
 * @param[out]      *pDst points to the output buffer  
 * @param[in]       blockSize number of samples in each vector  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function uses saturating arithmetic.  
 * The Q31 value -1 (0x80000000) will be saturated to the maximum allowable positive value 0x7FFFFFFF.  
 */ 
 
void arm_abs_q31( 
  q31_t * pSrc, 
  q31_t * pDst, 
  uint32_t blockSize) 
{ 
  uint32_t blkCnt;                               /* loop counter */ 
  q31_t in;                                      /* Input value */ 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = |A| */ 
    /* Calculate absolute of input (if -1 then saturated to 0x7fffffff) and then store the results in the destination buffer. */ 
    in = *pSrc++; 
    *pDst++ = (in > 0) ? in : ((in == 0x80000000) ? 0x7fffffff : -in); 
    in = *pSrc++; 
    *pDst++ = (in > 0) ? in : ((in == 0x80000000) ? 0x7fffffff : -in); 
    in = *pSrc++; 
    *pDst++ = (in > 0) ? in : ((in == 0x80000000) ? 0x7fffffff : -in); 
    in = *pSrc++; 
    *pDst++ = (in > 0) ? in : ((in == 0x80000000) ? 0x7fffffff : -in); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C = |A| */ 
    /* Calculate absolute value of the input (if -1 then saturated to 0x7fffffff) and then store the results in the destination buffer. */ 
    in = *pSrc++; 
    *pDst++ = (in > 0) ? in : ((in == 0x80000000) ? 0x7fffffff : -in); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of BasicAbs group  
 */ 
