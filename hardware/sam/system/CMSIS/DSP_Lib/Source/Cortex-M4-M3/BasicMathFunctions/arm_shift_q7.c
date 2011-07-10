/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_shift_q7.c  
*  
* Description:	Processing function for the Q7 Shifting  
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
 * @addtogroup shift  
 * @{  
 */ 
 
 
/**  
 * @brief  Shifts the elements of a Q7 vector a specified number of bits.  
 * @param  *pSrc points to the input vector  
 * @param  shiftBits number of bits to shift.  A positive value shifts left; a negative value shifts right.  
 * @param  *pDst points to the output vector  
 * @param  blockSize number of samples in the vector  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function uses saturating arithmetic.  
 * Results outside of the allowable Q7 range [0x8 0x7F] will be saturated.  
 */ 
 
void arm_shift_q7( 
  q7_t * pSrc, 
  int8_t shiftBits, 
  q7_t * pDst, 
  uint32_t blockSize) 
{ 
  uint32_t blkCnt;                               /* loop counter */ 
  uint8_t sign;                                  /* Sign of shiftBits */ 
  q7_t in1;                                      /* Input value1 */ 
  q7_t in2;                                      /* Input value2 */ 
  q7_t in3;                                      /* Input value3 */ 
  q7_t in4;                                      /* Input value4 */ 
 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* Getting the sign of shiftBits */ 
  sign = (shiftBits & 0x80); 
 
  /* If the shift value is positive then do right shift else left shift */ 
  if(sign == 0u) 
  { 
    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
     ** a second loop below computes the remaining 1 to 3 samples. */ 
    while(blkCnt > 0u) 
    { 
      /* C = A << shiftBits */ 
      /* Read 4 inputs */ 
      in1 = *pSrc++; 
      in2 = *pSrc++; 
      in3 = *pSrc++; 
      in4 = *pSrc++; 
 
      /* Store the Shifted result in the destination buffer in single cycle by packing the outputs */ 
      *__SIMD32(pDst)++ = __PACKq7(__SSAT((in1 << shiftBits), 8), 
                                   __SSAT((in2 << shiftBits), 8), 
                                   __SSAT((in3 << shiftBits), 8), 
                                   __SSAT((in4 << shiftBits), 8)); 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
 
    /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
     ** No loop unrolling is used. */ 
    blkCnt = blockSize % 0x4u; 
 
    while(blkCnt > 0u) 
    { 
      /* C = A << shiftBits */ 
      /* Shift the input and then store the result in the destination buffer. */ 
      *pDst++ = (q7_t) __SSAT((*pSrc++ << shiftBits), 8); 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
  } 
  else 
  { 
    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
     ** a second loop below computes the remaining 1 to 3 samples. */ 
    while(blkCnt > 0u) 
    { 
      /* C = A >> shiftBits */ 
      /* Read 4 inputs */ 
      in1 = *pSrc++; 
      in2 = *pSrc++; 
      in3 = *pSrc++; 
      in4 = *pSrc++; 
 
      /* Store the Shifted result in the destination buffer in single cycle by packing the outputs */ 
      *__SIMD32(pDst)++ = __PACKq7((in1 >> -shiftBits), (in2 >> -shiftBits), 
                                   (in3 >> -shiftBits), (in4 >> -shiftBits)); 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
 
    /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
     ** No loop unrolling is used. */ 
    blkCnt = blockSize % 0x4u; 
 
    while(blkCnt > 0u) 
    { 
      /* C = A >> shiftBits */ 
      /* Shift the input and then store the result in the destination buffer. */ 
      *pDst++ = (*pSrc++ >> -shiftBits); 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
  } 
} 
 
/**  
 * @} end of shift group  
 */ 
