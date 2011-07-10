/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_offset_q7.c  
*  
* Description:	Q7 vector offset.  
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
 * @addtogroup offset  
 * @{  
 */ 
 
/**  
 * @brief  Adds a constant offset to a Q7 vector.  
 * @param  *pSrc points to the input vector  
 * @param  offset is the offset to be added  
 * @param  *pDst points to the output vector  
 * @param  blockSize number of samples in the vector  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function uses saturating arithmetic.  
 * Results outside of the allowable Q7 range [0x80 0x7F] are saturated.  
 */ 
 
void arm_offset_q7( 
  q7_t * pSrc, 
  q7_t offset, 
  q7_t * pDst, 
  uint32_t blockSize) 
{ 
  uint32_t blkCnt;                               /* loop counter */ 
  q31_t offset_packed;                           /* Offset packed to 32 bit */ 
 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* Offset is packed to 32 bit in order to use SIMD32 for addition */ 
  offset_packed = __PACKq7(offset, offset, offset, offset); 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = A + offset */ 
    /* Add offset and then store the results in the destination bufferfor 4 samples at a time. */ 
    *__SIMD32(pDst)++ = __QADD8(*__SIMD32(pSrc)++, offset_packed); 
 
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
    *pDst++ = (q7_t) __SSAT(*pSrc++ + offset, 8); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of offset group  
 */ 
