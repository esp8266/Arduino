/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_mat_mult_q31.c  
*  
* Description:	 Q31 matrix multiplication.  
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
 * @addtogroup MatrixMult  
 * @{  
 */ 
 
/**  
 * @brief Q31 matrix multiplication  
 * @param[in]       *pSrcA points to the first input matrix structure  
 * @param[in]       *pSrcB points to the second input matrix structure  
 * @param[out]      *pDst points to output matrix structure  
 * @return     		The function returns either  
 * <code>ARM_MATH_SIZE_MISMATCH</code> or <code>ARM_MATH_SUCCESS</code> based on the outcome of size checking.  
 *  
 * @details  
 * <b>Scaling and Overflow Behavior:</b>  
 *  
 * \par  
 * The function is implemented using an internal 64-bit accumulator.  
 * The accumulator has a 2.62 format and maintains full precision of the intermediate  
 * multiplication results but provides only a single guard bit. There is no saturation  
 * on intermediate additions. Thus, if the accumulator overflows it wraps around and  
 * distorts the result. The input signals should be scaled down to avoid intermediate  
 * overflows. The input is thus scaled down by log2(numColsA) bits  
 * to avoid overflows, as a total of numColsA additions are performed internally.  
 * The 2.62 accumulator is right shifted by 31 bits and saturated to 1.31 format to yield the final result.  
 *  
 * \par  
 * See <code>arm_mat_mult_fast_q31()</code> for a faster but less precise implementation of this function.  
 *  
 */ 
 
arm_status arm_mat_mult_q31( 
  const arm_matrix_instance_q31 * pSrcA, 
  const arm_matrix_instance_q31 * pSrcB, 
  arm_matrix_instance_q31 * pDst) 
{ 
  q31_t *pIn1 = pSrcA->pData;                    /* input data matrix pointer A */ 
  q31_t *pIn2 = pSrcB->pData;                    /* input data matrix pointer B */ 
  q31_t *pInA = pSrcA->pData;                    /* input data matrix pointer A */ 
//  q31_t *pSrcB = pSrcB->pData;                    /* input data matrix pointer B */  
  q31_t *pOut = pDst->pData;                     /* output data matrix pointer */ 
  q31_t *px;                                     /* Temporary output data matrix pointer */ 
  q63_t sum;                                     /* Accumulator */ 
  uint16_t numRowsA = pSrcA->numRows;            /* number of rows of input matrix A    */ 
  uint16_t numColsB = pSrcB->numCols;            /* number of columns of input matrix B */ 
  uint16_t numColsA = pSrcA->numCols;            /* number of columns of input matrix A */ 
  uint16_t col, i = 0u, j, row = numRowsA, colCnt;      /* loop counters */ 
  arm_status status;                             /* status of matrix multiplication */ 
 
 
#ifdef ARM_MATH_MATRIX_CHECK 
  /* Check for matrix mismatch condition */ 
  if((pSrcA->numCols != pSrcB->numRows) || 
     (pSrcA->numRows != pDst->numRows) || (pSrcB->numCols != pDst->numCols)) 
  { 
    /* Set status as ARM_MATH_SIZE_MISMATCH */ 
    status = ARM_MATH_SIZE_MISMATCH; 
  } 
  else 
#endif 
  { 
    /* The following loop performs the dot-product of each row in pSrcA with each column in pSrcB */ 
    /* row loop */ 
    do 
    { 
      /* Output pointer is set to starting address of the row being processed */ 
      px = pOut + i; 
 
      /* For every row wise process, the column loop counter is to be initiated */ 
      col = numColsB; 
 
      /* For every row wise process, the pIn2 pointer is set  
       ** to the starting address of the pSrcB data */ 
      pIn2 = pSrcB->pData; 
 
      j = 0u; 
 
      /* column loop */ 
      do 
      { 
        /* Set the variable sum, that acts as accumulator, to zero */ 
        sum = 0; 
 
        /* Initiate the pointer pIn1 to point to the starting address of pInA */ 
        pIn1 = pInA; 
 
        /* Apply loop unrolling and compute 4 MACs simultaneously. */ 
        colCnt = numColsA >> 2; 
 
 
        /* matrix multiplication */ 
        while(colCnt > 0u) 
        { 
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */ 
          /* Perform the multiply-accumulates */ 
          sum += (q63_t) * pIn1++ * *pIn2; 
          pIn2 += numColsB; 
 
          sum += (q63_t) * pIn1++ * *pIn2; 
          pIn2 += numColsB; 
 
          sum += (q63_t) * pIn1++ * *pIn2; 
          pIn2 += numColsB; 
 
          sum += (q63_t) * pIn1++ * *pIn2; 
          pIn2 += numColsB; 
 
          /* Decrement the loop counter */ 
          colCnt--; 
        } 
 
        /* If the columns of pSrcA is not a multiple of 4, compute any remaining output samples here.  
         ** No loop unrolling is used. */ 
        colCnt = numColsA % 0x4u; 
 
        while(colCnt > 0u) 
        { 
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */ 
          /* Perform the multiply-accumulates */ 
          sum += (q63_t) * pIn1++ * *pIn2; 
          pIn2 += numColsB; 
 
          /* Decrement the loop counter */ 
          colCnt--; 
        } 
 
        /* Convert the result from 2.30 to 1.31 format and store in destination buffer */ 
        *px++ = (q31_t) (sum >> 31); 
 
        /* Update the pointer pIn2 to point to the  starting address of the next column */ 
        j++; 
        pIn2 = (pSrcB->pData) + j; 
 
        /* Decrement the column loop counter */ 
        col--; 
 
      } while(col > 0u); 
 
      /* Update the pointer pInA to point to the  starting address of the next row */ 
      i = i + numColsB; 
      pInA = pInA + numColsA; 
 
      /* Decrement the row loop counter */ 
      row--; 
 
    } while(row > 0u); 
 
    /* set status as ARM_MATH_SUCCESS */ 
    status = ARM_MATH_SUCCESS; 
  } 
  /* Return to application */ 
  return (status); 
} 
 
/**  
 * @} end of MatrixMult group  
 */ 
