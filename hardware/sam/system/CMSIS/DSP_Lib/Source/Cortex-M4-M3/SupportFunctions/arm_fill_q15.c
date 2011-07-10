/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_fill_q15.c  
*  
* Description:	Processing function for the Q15 fill  
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
 * @ingroup groupSupport  
 */ 
 
/**  
 * @addtogroup Fill  
 * @{  
 */ 
 
/**  
 * @brief Fills a constant value into a Q15 vector.  
 * @param[in]       value input value to be filled 
 * @param[out]      *pDst points to output vector  
 * @param[in]       blockSize length of the output vector 
 * @return none.  
 *  
 */ 
 
void arm_fill_q15( 
  q15_t value, 
  q15_t * pDst, 
  uint32_t blockSize) 
{ 
  uint32_t blkCnt;                               /* loop counter */ 
  q31_t packedValue;                             /* value packed to 32 bits */ 
 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* Packing two 16 bit values to 32 bit value in order to use SIMD */ 
  packedValue = __PKHBT(value, value, 16u); 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = value */ 
    /* Fill the value in the destination buffer */ 
    *__SIMD32(pDst)++ = packedValue; 
    *__SIMD32(pDst)++ = packedValue; 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C = value */ 
    /* Fill the value in the destination buffer */ 
    *pDst++ = value; 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of Fill group  
 */ 
