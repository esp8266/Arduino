/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_mat_mult_fast_q15.c  
*  
* Description:	 Q15 matrix multiplication (fast variant)  
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
 * @brief Q15 matrix multiplication (fast variant)  
 * @param[in]       *pSrcA points to the first input matrix structure  
 * @param[in]       *pSrcB points to the second input matrix structure  
 * @param[out]      *pDst points to output matrix structure  
 * @param[in]		*pState points to the array for storing intermediate results  
 * @return     		The function returns either  
 * <code>ARM_MATH_SIZE_MISMATCH</code> or <code>ARM_MATH_SUCCESS</code> based on the outcome of size checking.  
 *  
 * @details  
 * <b>Scaling and Overflow Behavior:</b>  
 *  
 * \par  
 * The difference between the function arm_mat_mult_q15() and this fast variant is that  
 * the fast variant use a 32-bit rather than a 64-bit accumulator.  
 * The result of each 1.15 x 1.15 multiplication is truncated to  
 * 2.30 format. These intermediate results are accumulated in a 32-bit register in 2.30  
 * format. Finally, the accumulator is saturated and converted to a 1.15 result.  
 *  
 * \par  
 * The fast version has the same overflow behavior as the standard version but provides  
 * less precision since it discards the low 16 bits of each multiplication result.  
 * In order to avoid overflows completely the input signals must be scaled down.  
 * Scale down one of the input matrices by log2(numColsA) bits to  
 * avoid overflows, as a total of numColsA additions are computed internally for each  
 * output element.  
 *  
 * \par  
 * See <code>arm_mat_mult_q15()</code> for a slower implementation of this function  
 * which uses 64-bit accumulation to provide higher precision.  
 */ 
 
arm_status arm_mat_mult_fast_q15( 
  const arm_matrix_instance_q15 * pSrcA, 
  const arm_matrix_instance_q15 * pSrcB, 
  arm_matrix_instance_q15 * pDst, 
  q15_t * pState) 
{ 
  q31_t sum;                                     /* accumulator */ 
  q31_t in;                                      /* Temporary variable to hold the input value */ 
  q15_t *pSrcBT = pState;                        /* input data matrix pointer for transpose */ 
  q15_t *pInA = pSrcA->pData;                    /* input data matrix pointer A of Q15 type */ 
  q15_t *pInB = pSrcB->pData;                    /* input data matrix pointer B of Q15 type */ 
//  q15_t *pDst = pDst->pData;                     /* output data matrix pointer */  
  q15_t *px;                                     /* Temporary output data matrix pointer */ 
  uint16_t numRowsA = pSrcA->numRows;            /* number of rows of input matrix A    */ 
  uint16_t numColsB = pSrcB->numCols;            /* number of columns of input matrix B */ 
  uint16_t numColsA = pSrcA->numCols;            /* number of columns of input matrix A */ 
  uint16_t numRowsB = pSrcB->numRows;            /* number of rows of input matrix A    */ 
  uint16_t col, i = 0u, row = numRowsB, colCnt;  /* loop counters */ 
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
    /* Matrix transpose */ 
    do 
    { 
      /* Apply loop unrolling and exchange the columns with row elements */ 
      col = numColsB >> 2; 
 
      /* The pointer px is set to starting address of the column being processed */ 
      px = pSrcBT + i; 
 
      /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
       ** a second loop below computes the remaining 1 to 3 samples. */ 
      while(col > 0u) 
      { 
        /* Read two elements from the row */ 
        in = *__SIMD32(pInB)++; 
 
        /* Unpack and store one element in the destination */ 
        *px = (q15_t) in; 
 
        /* Update the pointer px to point to the next row of the transposed matrix */ 
        px += numRowsB; 
 
        /* Unpack and store the second element in the destination */ 
        *px = (q15_t) ((in & (q31_t) 0xffff0000) >> 16); 
 
        /* Update the pointer px to point to the next row of the transposed matrix */ 
        px += numRowsB; 
 
        /* Read two elements from the row */ 
        in = *__SIMD32(pInB)++; 
 
        /* Unpack and store one element in the destination */ 
        *px = (q15_t) in; 
 
        /* Update the pointer px to point to the next row of the transposed matrix */ 
        px += numRowsB; 
 
        /* Unpack and store the second element in the destination */ 
        *px = (q15_t) ((in & (q31_t) 0xffff0000) >> 16); 
 
        /* Update the pointer px to point to the next row of the transposed matrix */ 
        px += numRowsB; 
 
        /* Decrement the column loop counter */ 
        col--; 
      } 
 
      /* If the columns of pSrcB is not a multiple of 4, compute any remaining output samples here.  
       ** No loop unrolling is used. */ 
      col = numColsB % 0x4u; 
 
      while(col > 0u) 
      { 
        /* Read and store the input element in the destination */ 
        *px = *pInB++; 
 
        /* Update the pointer px to point to the next row of the transposed matrix */ 
        px += numRowsB; 
 
        /* Decrement the column loop counter */ 
        col--; 
      } 
 
      i++; 
 
      /* Decrement the row loop counter */ 
      row--; 
 
    } while(row > 0u); 
 
    /* Reset the variables for the usage in the following multiplication process */ 
    row = numRowsA; 
    i = 0u; 
    px = pDst->pData; 
 
    /* The following loop performs the dot-product of each row in pSrcA with each column in pSrcB */ 
    /* row loop */ 
    do 
    { 
      /* For every row wise process, the column loop counter is to be initiated */ 
      col = numColsB; 
 
      /* For every row wise process, the pIn2 pointer is set  
       ** to the starting address of the transposed pSrcB data */ 
      pInB = pSrcBT; 
 
      /* column loop */ 
      do 
      { 
        /* Set the variable sum, that acts as accumulator, to zero */ 
        sum = 0; 
 
        /* Apply loop unrolling and compute 2 MACs simultaneously. */ 
        colCnt = numColsA >> 1; 
 
        /* Initiate the pointer pIn1 to point to the starting address of the column being processed */ 
        pInA = pSrcA->pData + i; 
 
        /* matrix multiplication */ 
        while(colCnt > 0u) 
        { 
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */ 
          sum = __SMLAD(*__SIMD32(pInA)++, *__SIMD32(pInB)++, sum); 
 
          /* Decrement the loop counter */ 
          colCnt--; 
        } 
 
        /* process odd column samples */ 
        if((numColsA & 0x1u) > 0u) 
        { 
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */ 
          sum += (q31_t) * pInA * (*pInB++); 
        } 
 
        /* Saturate and store the result in the destination buffer */ 
        *px = (q15_t) (sum >> 15); 
        px++; 
 
        /* Decrement the column loop counter */ 
        col--; 
 
      } while(col > 0u); 
 
      i = i + numColsA; 
 
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
