/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_dot_prod_f32.c  
*  
* Description:	Floating-point dot product.  
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
 * @defgroup dot_prod Vector Dot Product  
 *  
 * Computes the dot product of two vectors.  
 * The vectors are multiplied element-by-element and then summed.  
 * There are separate functions for floating-point, Q7, Q15, and Q31 data types.  
 */ 
 
/**  
 * @addtogroup dot_prod  
 * @{  
 */ 
 
/**  
 * @brief Dot product of floating-point vectors.  
 * @param[in]       *pSrcA points to the first input vector  
 * @param[in]       *pSrcB points to the second input vector  
 * @param[in]       blockSize number of samples in each vector  
 * @param[out]      *result output result returned here  
 * @return none.  
 */ 
 
 
void arm_dot_prod_f32( 
  float32_t * pSrcA, 
  float32_t * pSrcB, 
  uint32_t blockSize, 
  float32_t * result) 
{ 
  float32_t sum = 0.0f;                          /* Temporary result storage */ 
  uint32_t blkCnt;                               /* loop counter */ 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = A[0]* B[0] + A[1]* B[1] + A[2]* B[2] + .....+ A[blockSize-1]* B[blockSize-1] */ 
    /* Calculate dot product and then store the result in a temporary buffer */ 
    sum += (*pSrcA++) * (*pSrcB++); 
    sum += (*pSrcA++) * (*pSrcB++); 
    sum += (*pSrcA++) * (*pSrcB++); 
    sum += (*pSrcA++) * (*pSrcB++); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C = A[0]* B[0] + A[1]* B[1] + A[2]* B[2] + .....+ A[blockSize-1]* B[blockSize-1] */ 
    /* Calculate dot product and then store the result in a temporary buffer. */ 
    sum += (*pSrcA++) * (*pSrcB++); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Store the result back in the destination buffer */ 
  *result = sum; 
} 
 
/**  
 * @} end of dot_prod group  
 */ 
