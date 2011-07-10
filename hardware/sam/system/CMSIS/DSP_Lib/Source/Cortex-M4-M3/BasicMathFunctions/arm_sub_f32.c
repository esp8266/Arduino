/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_sub_f32.c  
*  
* Description:	Floating-point vector subtraction.  
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
* ---------------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupMath  
 */ 
 
/**  
 * @defgroup BasicSub Vector Subtraction  
 *  
 * Element-by-element subtraction of two vectors.  
 *  
 * <pre>  
 *     pDst[n] = pSrcA[n] - pSrcB[n],   0 <= n < blockSize.  
 * </pre>  
 *  
 * There are separate functions for floating-point, Q7, Q15, and Q31 data types.  
 */ 
 
/**  
 * @addtogroup BasicSub  
 * @{  
 */ 
 
 
/**  
 * @brief Floating-point vector subtraction.  
 * @param[in]       *pSrcA points to the first input vector  
 * @param[in]       *pSrcB points to the second input vector  
 * @param[out]      *pDst points to the output vector  
 * @param[in]       blockSize number of samples in each vector  
 * @return none.  
 */ 
 
void arm_sub_f32( 
  float32_t * pSrcA, 
  float32_t * pSrcB, 
  float32_t * pDst, 
  uint32_t blockSize) 
{ 
  uint32_t blkCnt;                               /* loop counter */ 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = A - B */ 
    /* Subtract and then store the results in the destination buffer. */ 
    *pDst++ = (*pSrcA++) - (*pSrcB++); 
    *pDst++ = (*pSrcA++) - (*pSrcB++); 
    *pDst++ = (*pSrcA++) - (*pSrcB++); 
    *pDst++ = (*pSrcA++) - (*pSrcB++); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C = A - B */ 
    /* Subtract and then store the results in the destination buffer. */ 
    *pDst++ = (*pSrcA++) - (*pSrcB++); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of BasicSub group  
 */ 
