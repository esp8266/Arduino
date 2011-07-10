/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_mean_f32.c  
*  
* Description:	Mean value of two floating point arrays.  
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
* ---------------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupStats  
 */ 
 
/**  
 * @defgroup mean Mean  
 *  
 * Calculates the mean of the input vector. Mean is defined as the average of the elements in the vector.  
 * The underlying algorithm is used:  
 *  
 * <pre>  
 * 	Result = (pSrc[0] + pSrc[1] + pSrc[2] + ... + pSrc[blockSize-1]) / blockSize;  
 * </pre>  
 *  
 * There are separate functions for floating-point, Q31, Q15, and Q7 data types.  
 */ 
 
/**  
 * @addtogroup mean  
 * @{  
 */ 
 
 
/**  
 * @brief Mean value of a floating-point vector.  
 * @param[in]       *pSrc points to the input vector  
 * @param[in]       blockSize length of the input vector  
 * @param[out]      *pResult mean value returned here  
 * @return none.  
 */ 
 
 
void arm_mean_f32( 
  float32_t * pSrc, 
  uint32_t blockSize, 
  float32_t * pResult) 
{ 
  float32_t sum = 0.0f;                          /* Temporary result storage */ 
  uint32_t blkCnt;                               /* loop counter */ 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */ 
    sum += *pSrc++; 
    sum += *pSrc++; 
    sum += *pSrc++; 
    sum += *pSrc++; 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */ 
    sum += *pSrc++; 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) / blockSize  */ 
  /* Store the result to the destination */ 
  *pResult = sum / (float32_t) blockSize; 
} 
 
/**  
 * @} end of mean group  
 */ 
