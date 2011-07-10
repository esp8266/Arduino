/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:        arm_mat_scale_f32.c  
*  
* Description:	Multiplies a floating-point matrix by a scalar.  
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
 * @defgroup MatrixScale Matrix Scale  
 *  
 * Multiplies a matrix by a scalar.  This is accomplished by multiplying each element in the  
 * matrix by the scalar.  For example:  
 * \image html MatrixScale.gif "Matrix Scaling of a 3 x 3 matrix"  
 *  
 * The function checks to make sure that the input and output matrices are of the same size.  
 *  
 * In the fixed-point Q15 and Q31 functions, <code>scale</code> is represented by  
 * a fractional multiplication <code>scaleFract</code> and an arithmetic shift <code>shift</code>.  
 * The shift allows the gain of the scaling operation to exceed 1.0.  
 * The overall scale factor applied to the fixed-point data is  
 * <pre>  
 *     scale = scaleFract * 2^shift.  
 * </pre>  
 */ 
 
/**  
 * @addtogroup MatrixScale  
 * @{  
 */ 
 
/**  
 * @brief Floating-point matrix scaling.  
 * @param[in]       *pSrc points to input matrix structure  
 * @param[in]       scale scale factor to be applied   
 * @param[out]      *pDst points to output matrix structure  
 * @return     		The function returns either <code>ARM_MATH_SIZE_MISMATCH</code>   
 * or <code>ARM_MATH_SUCCESS</code> based on the outcome of size checking.  
 *  
 */ 
 
arm_status arm_mat_scale_f32( 
  const arm_matrix_instance_f32 * pSrc, 
  float32_t scale, 
  arm_matrix_instance_f32 * pDst) 
{ 
  float32_t *pIn = pSrc->pData;                  /* input data matrix pointer */ 
  float32_t *pOut = pDst->pData;                 /* output data matrix pointer */ 
  uint32_t numSamples;                           /* total number of elements in the matrix */ 
  uint32_t blkCnt;                               /* loop counters */ 
  arm_status status;                             /* status of matrix scaling     */ 
 
#ifdef ARM_MATH_MATRIX_CHECK 
  /* Check for matrix mismatch condition */ 
  if((pSrc->numRows != pDst->numRows) || (pSrc->numCols != pDst->numCols)) 
  { 
    /* Set status as ARM_MATH_SIZE_MISMATCH */ 
    status = ARM_MATH_SIZE_MISMATCH; 
  } 
  else 
#endif 
  { 
    /* Total number of samples in the input matrix */ 
    numSamples = (uint32_t) pSrc->numRows * pSrc->numCols; 
 
    /* Loop Unrolling */ 
    blkCnt = numSamples >> 2; 
 
    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.  
     ** a second loop below computes the remaining 1 to 3 samples. */ 
    while(blkCnt > 0u) 
    { 
      /* C(m,n) = A(m,n) * scale */ 
      /* Scaling and results are stored in the destination buffer. */ 
      *pOut++ = (*pIn++) * scale; 
      *pOut++ = (*pIn++) * scale; 
      *pOut++ = (*pIn++) * scale; 
      *pOut++ = (*pIn++) * scale; 
 
      /* Decrement the numSamples loop counter */ 
      blkCnt--; 
    } 
 
    /* If the numSamples is not a multiple of 4, compute any remaining output samples here.  
     ** No loop unrolling is used. */ 
    blkCnt = numSamples % 0x4u; 
 
    while(blkCnt > 0u) 
    { 
      /* C(m,n) = A(m,n) * scale */ 
      /* The results are stored in the destination buffer. */ 
      *pOut++ = (*pIn++) * scale; 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
 
    /* Set status as ARM_MATH_SUCCESS */ 
    status = ARM_MATH_SUCCESS; 
  } 
 
  /* Return to application */ 
  return (status); 
} 
 
/**  
 * @} end of MatrixScale group  
 */ 
