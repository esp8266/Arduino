/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_mat_trans_f32.c  
*  
* Description:	Floating-point matrix transpose.  
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
 
/**  
 * @defgroup MatrixTrans Matrix Transpose  
 *  
 * Tranposes a matrix.  
 * Transposing an <code>M x N</code> matrix flips it around the center diagonal and results in an <code>N x M</code> matrix.  
 * \image html MatrixTranspose.gif "Transpose of a 3 x 3 matrix"  
 */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupMatrix  
 */ 
 
/**  
 * @addtogroup MatrixTrans  
 * @{  
 */ 
 
/**  
  * @brief Floating-point matrix transpose.  
  * @param[in]  *pSrc points to the input matrix  
  * @param[out] *pDst points to the output matrix  
  * @return 	The function returns either  <code>ARM_MATH_SIZE_MISMATCH</code>  
  * or <code>ARM_MATH_SUCCESS</code> based on the outcome of size checking.  
  */ 
 
 
arm_status arm_mat_trans_f32( 
  const arm_matrix_instance_f32 * pSrc, 
  arm_matrix_instance_f32 * pDst) 
{ 
  float32_t *pIn = pSrc->pData;                  /* input data matrix pointer */ 
  float32_t *pOut = pDst->pData;                 /* output data matrix pointer */ 
  float32_t *px;                                 /* Temporary output data matrix pointer */ 
  uint16_t nRows = pSrc->numRows;                /* number of rows */ 
  uint16_t nColumns = pSrc->numCols;             /* number of columns */ 
  uint16_t blkCnt, i = 0u, row = nRows;          /* loop counters */ 
  arm_status status;                             /* status of matrix transpose  */ 
 
 
#ifdef ARM_MATH_MATRIX_CHECK 
  /* Check for matrix mismatch condition */ 
  if((pSrc->numRows != pDst->numCols) || (pSrc->numCols != pDst->numRows)) 
  { 
    /* Set status as ARM_MATH_SIZE_MISMATCH */ 
    status = ARM_MATH_SIZE_MISMATCH; 
  } 
  else 
#endif 
  { 
    /* Matrix transpose by exchanging the rows with columns */ 
    /* row loop     */ 
    do 
    { 
      /* Loop Unrolling */ 
      blkCnt = nColumns >> 2; 
 
      /* The pointer px is set to starting address of the column being processed */ 
      px = pOut + i; 
 
      /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
       ** a second loop below computes the remaining 1 to 3 samples. */ 
      while(blkCnt > 0u)        /* column loop */ 
      { 
        /* Read and store the input element in the destination */ 
        *px = *pIn++; 
 
        /* Update the pointer px to point to the next row of the transposed matrix */ 
        px += nRows; 
 
        /* Read and store the input element in the destination */ 
        *px = *pIn++; 
 
        /* Update the pointer px to point to the next row of the transposed matrix */ 
        px += nRows; 
 
        /* Read and store the input element in the destination */ 
        *px = *pIn++; 
 
        /* Update the pointer px to point to the next row of the transposed matrix */ 
        px += nRows; 
 
        /* Read and store the input element in the destination */ 
        *px = *pIn++; 
 
        /* Update the pointer px to point to the next row of the transposed matrix */ 
        px += nRows; 
 
        /* Decrement the column loop counter */ 
        blkCnt--; 
      } 
 
      /* Perform matrix transpose for last 3 samples here. */ 
      blkCnt = nColumns % 0x4u; 
 
      while(blkCnt > 0u) 
      { 
        /* Read and store the input element in the destination */ 
        *px = *pIn++; 
 
        /* Update the pointer px to point to the next row of the transposed matrix */ 
        px += nRows; 
 
        /* Decrement the column loop counter */ 
        blkCnt--; 
      } 
 
      i++; 
 
      /* Decrement the row loop counter */ 
      row--; 
 
    } while(row > 0u);          /* row loop end  */ 
 
    /* Set status as ARM_MATH_SUCCESS */ 
    status = ARM_MATH_SUCCESS; 
  } 
 
  /* Return to application */ 
  return (status); 
} 
 
/**  
 * @} end of MatrixTrans group  
 */ 
