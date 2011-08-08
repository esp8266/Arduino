/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:        arm_mat_init_q31.c   
*   
* Description:	Q31 matrix initialization.   
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
* -------------------------------------------------------------------------- */


#include "arm_math.h"

/**   
 * @ingroup groupMatrix   
 */

/**   
 * @defgroup MatrixInit Matrix Initialization   
 *   
 */

/**   
 * @addtogroup MatrixInit   
 * @{   
 */

  /**   
   * @brief  Q31 matrix initialization.   
   * @param[in,out] *S             points to an instance of the floating-point matrix structure.   
   * @param[in]     nRows          number of rows in the matrix.   
   * @param[in]     nColumns       number of columns in the matrix.   
   * @param[in]     *pData	   points to the matrix data array.   
   * @return        none   
   */

void arm_mat_init_q31(
  arm_matrix_instance_q31 * S,
  uint16_t nRows,
  uint16_t nColumns,
  q31_t * pData)
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
