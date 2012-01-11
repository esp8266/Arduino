/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_mat_trans_q15.c   
*   
* Description:	Q15 matrix transpose.   
*   
* Target Processor: Cortex-M4/Cortex-M3/Cortex-M0
*  
* Version 1.0.10 2011/7/15 
*    Big Endian support added and Merged M0 and M3/M4 Source code.  
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
 * @addtogroup MatrixTrans   
 * @{   
 */

/*   
 * @brief Q15 matrix transpose.   
 * @param[in]  *pSrc points to the input matrix   
 * @param[out] *pDst points to the output matrix   
 * @return 	The function returns either  <code>ARM_MATH_SIZE_MISMATCH</code>   
 * or <code>ARM_MATH_SUCCESS</code> based on the outcome of size checking.   
 */

arm_status arm_mat_trans_q15(
  const arm_matrix_instance_q15 * pSrc,
  arm_matrix_instance_q15 * pDst)
{
  q15_t *pSrcA = pSrc->pData;                    /* input data matrix pointer */
  q15_t *pOut = pDst->pData;                     /* output data matrix pointer */
  uint16_t nRows = pSrc->numRows;                /* number of nRows */
  uint16_t nColumns = pSrc->numCols;             /* number of nColumns */
  uint16_t col, row = nRows, i = 0u;             /* row and column loop counters */
  arm_status status;                             /* status of matrix transpose */

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  q31_t in;                                      /* variable to hold temporary output  */


#ifdef ARM_MATH_MATRIX_CHECK


  /* Check for matrix mismatch condition */
  if((pSrc->numRows != pDst->numCols) || (pSrc->numCols != pDst->numRows))
  {
    /* Set status as ARM_MATH_SIZE_MISMATCH */
    status = ARM_MATH_SIZE_MISMATCH;
  }
  else
#endif /*      #ifdef ARM_MATH_MATRIX_CHECK    */

  {
    /* Matrix transpose by exchanging the rows with columns */
    /* row loop     */
    do
    {
      /* Apply loop unrolling and exchange the columns with row elements */
      col = nColumns >> 2u;

      /* The pointer pOut is set to starting address of the column being processed */
      pOut = pDst->pData + i;

      /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
       ** a second loop below computes the remaining 1 to 3 samples. */
      while(col > 0u)
      {
        /* Read two elements from the row */
        in = *__SIMD32(pSrcA)++;

        /* Unpack and store one element in the destination */
#ifndef ARM_MATH_BIG_ENDIAN

        *pOut = (q15_t) in;

#else

        *pOut = (q15_t) ((in & (q31_t) 0xffff0000) >> 16);

#endif /*    #ifndef ARM_MATH_BIG_ENDIAN    */

        /* Update the pointer pOut to point to the next row of the transposed matrix */
        pOut += nRows;

        /* Unpack and store the second element in the destination */

#ifndef ARM_MATH_BIG_ENDIAN

        *pOut = (q15_t) ((in & (q31_t) 0xffff0000) >> 16);

#else

        *pOut = (q15_t) in;

#endif /*    #ifndef ARM_MATH_BIG_ENDIAN    */

        /* Update the pointer pOut to point to the next row of the transposed matrix */
        pOut += nRows;

        /* Read two elements from the row */
#ifndef ARM_MATH_BIG_ENDIAN

        in = *__SIMD32(pSrcA)++;

#else

        in = *__SIMD32(pSrcA)++;

#endif /*    #ifndef ARM_MATH_BIG_ENDIAN    */

        /* Unpack and store one element in the destination */
#ifndef ARM_MATH_BIG_ENDIAN

        *pOut = (q15_t) in;

#else

        *pOut = (q15_t) ((in & (q31_t) 0xffff0000) >> 16);

#endif /*    #ifndef ARM_MATH_BIG_ENDIAN    */

        /* Update the pointer pOut to point to the next row of the transposed matrix */
        pOut += nRows;

        /* Unpack and store the second element in the destination */
#ifndef ARM_MATH_BIG_ENDIAN

        *pOut = (q15_t) ((in & (q31_t) 0xffff0000) >> 16);

#else

        *pOut = (q15_t) in;

#endif /*    #ifndef ARM_MATH_BIG_ENDIAN    */

        /* Update the pointer pOut to point to the next row of the transposed matrix */
        pOut += nRows;

        /* Decrement the column loop counter */
        col--;
      }

      /* Perform matrix transpose for last 3 samples here. */
      col = nColumns % 0x4u;

#else

  /* Run the below code for Cortex-M0 */

#ifdef ARM_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if((pSrc->numRows != pDst->numCols) || (pSrc->numCols != pDst->numRows))
  {
    /* Set status as ARM_MATH_SIZE_MISMATCH */
    status = ARM_MATH_SIZE_MISMATCH;
  }
  else
#endif /*    #ifdef ARM_MATH_MATRIX_CHECK    */

  {
    /* Matrix transpose by exchanging the rows with columns */
    /* row loop     */
    do
    {
      /* The pointer pOut is set to starting address of the column being processed */
      pOut = pDst->pData + i;

      /* Initialize column loop counter */
      col = nColumns;

#endif /* #ifndef ARM_MATH_CM0 */

      while(col > 0u)
      {
        /* Read and store the input element in the destination */
        *pOut = *pSrcA++;

        /* Update the pointer pOut to point to the next row of the transposed matrix */
        pOut += nRows;

        /* Decrement the column loop counter */
        col--;
      }

      i++;

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
 * @} end of MatrixTrans group   
 */
