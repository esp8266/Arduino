/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_max_q31.c  
*  
* Description:	Maximum value of a Q31 vector.  
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
 * @addtogroup Max  
 * @{  
 */ 
 
 
/**  
 * @brief Maximum value of a Q31 vector.  
 * @param[in]       *pSrc points to the input vector  
 * @param[in]       blockSize length of the input vector  
 * @param[out]      *pResult maximum value returned here  
 * @param[out]      *pIndex index of maximum value returned here  
 * @return none.  
 */ 
 
void arm_max_q31( 
  q31_t * pSrc, 
  uint32_t blockSize, 
  q31_t * pResult, 
  uint32_t * pIndex) 
{ 
  q31_t maxVal, out;                             /* Temporary variables to store the output value. */ 
  uint32_t blkCnt, outIndex;                     /* loop counter */ 
 
  /* Initialise the index value to zero. */ 
  outIndex = 0u; 
  /* Load first input value that act as reference value for comparision */ 
  out = *pSrc++; 
 
  /* Loop over blockSize number of values */ 
  blkCnt = (blockSize - 1u); 
 
  do 
  { 
    /* Initialize maxVal to the next consecutive values one by one */ 
    maxVal = *pSrc++; 
 
    /* compare for the maximum value */ 
    if(out < maxVal) 
    { 
      /* Update the maximum value and its index */ 
      out = maxVal; 
      outIndex = blockSize - blkCnt; 
    } 
 
    blkCnt--; 
 
  } while(blkCnt > 0u); 
 
  /* Store the maximum value and its index into destination pointers */ 
  *pResult = out; 
  *pIndex = outIndex; 
} 
 
/**  
 * @} end of Max group  
 */ 
