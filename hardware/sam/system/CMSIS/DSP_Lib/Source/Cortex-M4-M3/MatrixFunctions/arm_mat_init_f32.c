/* ----------------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:        arm_mat_init_f32.c  
*  
* Description:	Floating-point matrix initialization.  
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
* -------------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupMatrix  
 */ 
 
/**  
 * @defgroup MatrixInit Matrix Initialization  
 *  
 * Initializes the underlying matrix data structure.  
 * The functions set the <code>numRows</code>,  
 * <code>numCols</code>, and <code>pData</code> fields  
 * of the matrix data structure.  
 */ 
 
/**  
 * @addtogroup MatrixInit  
 * @{  
 */ 
 
/**  
   * @brief  Floating-point matrix initialization.  
   * @param[in,out] *S             points to an instance of the floating-point matrix structure.  
   * @param[in]     nRows          number of rows in the matrix.  
   * @param[in]     nColumns       number of columns in the matrix.  
   * @param[in]     *pData	   points to the matrix data array.  
   * @return        none  
   */ 
 
void arm_mat_init_f32( 
  arm_matrix_instance_f32 * S, 
  uint16_t nRows, 
  uint16_t nColumns, 
  float32_t * pData) 
{ 
  /* Assign Number of Rows */ 
  S->numRows = nRows; 
 
  /* Assign Number of Columns */ 
  S->numCols = nColumns; 
 
  /* Assign Data pointer */ 
  S->pData = pData; 
} 
 
/**  
 * @} end of MatrixInit group  
 */ 
