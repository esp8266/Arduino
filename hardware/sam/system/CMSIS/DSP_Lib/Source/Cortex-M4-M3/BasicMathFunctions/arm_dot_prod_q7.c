/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_dot_prod_q7.c  
*  
* Description:	Q7 dot product.  
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
 * @addtogroup dot_prod  
 * @{  
 */ 
 
/**  
 * @brief Dot product of Q7 vectors.  
 * @param[in]       *pSrcA points to the first input vector  
 * @param[in]       *pSrcB points to the second input vector  
 * @param[in]       blockSize number of samples in each vector  
 * @param[out]      *result output result returned here  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The intermediate multiplications are in 1.7 x 1.7 = 2.14 format and these  
 * results are added to an accumulator in 18.14 format.  
 * Nonsaturating additions are used and there is no danger of wrap around as long as  
 * the vectors are less than 2^18 elements long.  
 * The return result is in 18.14 format.  
 */ 
 
void arm_dot_prod_q7( 
  q7_t * pSrcA, 
  q7_t * pSrcB, 
  uint32_t blockSize, 
  q31_t * result) 
{ 
  q31_t input1, input2;                          /* Temporary variables to store input */ 
  q15_t in1, in2;                                /* Temporary variables to store input */ 
  q31_t sum = 0;                                 /* Temporary variables to store output */ 
  uint32_t blkCnt;                               /* loop counter */ 
 
 
 
  /*loop Unrolling */ 
  blkCnt = blockSize >> 2u; 
 
  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
   ** a second loop below computes the remaining 1 to 3 samples. */ 
  while(blkCnt > 0u) 
  { 
    /* Reading two inputs of SrcA buffer and packing */ 
    in1 = (q15_t) * pSrcA++; 
    in2 = (q15_t) * pSrcA++; 
    input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16); 
 
    /* Reading two inputs of SrcB buffer and packing */ 
    in1 = (q15_t) * pSrcB++; 
    in2 = (q15_t) * pSrcB++; 
    input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16); 
 
    /* C = A[0]* B[0] + A[1]* B[1] + A[2]* B[2] + .....+ A[blockSize-1]* B[blockSize-1] */ 
    /* Perform Dot product of 2 packed inputs using SMLALD and store the result in a temporary variable. */ 
    sum = __SMLAD(input1, input2, sum); 
 
    /* Reading two inputs of SrcA buffer and packing */ 
    in1 = (q15_t) * pSrcA++; 
    in2 = (q15_t) * pSrcA++; 
    input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16); 
 
    /* Reading two inputs of SrcB buffer and packing */ 
    in1 = (q15_t) * pSrcB++; 
    in2 = (q15_t) * pSrcB++; 
    input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16); 
 
    /* C = A[0]* B[0] + A[1]* B[1] + A[2]* B[2] + .....+ A[blockSize-1]* B[blockSize-1] */ 
    /* Perform Dot product of 2 packed inputs using SMLALD and store the result in a temporary variable. */ 
    sum = __SMLAD(input1, input2, sum); 
 
 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.  
   ** No loop unrolling is used. */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* C = A[0]* B[0] + A[1]* B[1] + A[2]* B[2] + .....+ A[blockSize-1]* B[blockSize-1] */ 
    /* Dot product and then store the results in a temporary buffer. */ 
    sum = __SMLAD(*pSrcA++, *pSrcB++, sum); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Store the result in the destination buffer in 18.14 format */ 
  *result = sum; 
} 
 
/**  
 * @} end of dot_prod group  
 */ 
