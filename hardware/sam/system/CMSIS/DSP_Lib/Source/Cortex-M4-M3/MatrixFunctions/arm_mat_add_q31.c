/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_mat_add_q31.c  
*  
* Description:	Q31 matrix addition  
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
* Version 0.0.5  2010/04/26   
*    incorporated review comments and updated with latest CMSIS layer  
*  
* Version 0.0.3  2010/03/10   
*    Initial version  
* -------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupMatrix  
 */ 
 
/**  
 * @addtogroup MatrixAdd  
 * @{  
 */ 
 
/**  
 * @brief Q31 matrix addition.  
 * @param[in]       *pSrcA points to the first input matrix structure  
 * @param[in]       *pSrcB points to the second input matrix structure  
 * @param[out]      *pDst points to output matrix structure  
 * @return     		The function returns either  
 * <code>ARM_MATH_SIZE_MISMATCH</code> or <code>ARM_MATH_SUCCESS</code> based on the outcome of size checking.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function uses saturating arithmetic.  
 * Results outside of the allowable Q31 range [0x80000000 0x7FFFFFFF] will be saturated.  
 */ 
 
arm_status arm_mat_add_q31( 
  const arm_matrix_instance_q31 * pSrcA, 
  const arm_matrix_instance_q31 * pSrcB, 
  arm_matrix_instance_q31 * pDst) 
{ 
  q31_t *pIn1 = pSrcA->pData;                    /* input data matrix pointer A */ 
  q31_t *pIn2 = pSrcB->pData;                    /* input data matrix pointer B */ 
  q31_t *pOut = pDst->pData;                     /* output data matrix pointer */ 
  uint32_t numSamples;                           /* total number of elements in the matrix  */ 
  uint32_t blkCnt;                               /* loop counters */ 
  arm_status status;                             /* status of matrix addition */ 
 
#ifdef ARM_MATH_MATRIX_CHECK 
  /* Check for matrix mismatch condition */ 
  if((pSrcA->numRows != pSrcB->numRows) || 
     (pSrcA->numCols != pSrcB->numCols) || 
     (pSrcA->numRows != pDst->numRows) || (pSrcA->numCols != pDst->numCols)) 
  { 
    /* Set status as ARM_MATH_SIZE_MISMATCH */ 
    status = ARM_MATH_SIZE_MISMATCH; 
  } 
  else 
#endif 
  { 
    /* Total number of samples in the input matrix */ 
    numSamples = (uint32_t) pSrcA->numRows * pSrcA->numCols; 
 
    /* Loop Unrolling */ 
    blkCnt = numSamples >> 2u; 
 
 
    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
     ** a second loop below computes the remaining 1 to 3 samples. */ 
    while(blkCnt > 0u) 
    { 
      /* C(m,n) = A(m,n) + B(m,n) */ 
      /* Add, saturate and then store the results in the destination buffer. */ 
      *pOut++ = __QADD(*pIn1++, *pIn2++); 
      *pOut++ = __QADD(*pIn1++, *pIn2++); 
      *pOut++ = __QADD(*pIn1++, *pIn2++); 
      *pOut++ = __QADD(*pIn1++, *pIn2++); 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
 
    /* If the numSamples is not a multiple of 4, compute any remaining output samples here.  
     ** No loop unrolling is used. */ 
    blkCnt = numSamples % 0x4u; 
 
    while(blkCnt > 0u) 
    { 
      /* C(m,n) = A(m,n) + B(m,n) */ 
      /* Add, saturate and then store the results in the destination buffer. */ 
      *pOut++ = __QADD(*pIn1++, *pIn2++); 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
 
    /* set status as ARM_MATH_SUCCESS */ 
    status = ARM_MATH_SUCCESS; 
  } 
 
  /* Return to application */ 
  return (status); 
} 
 
/**  
 * @} end of MatrixAdd group  
 */ 
