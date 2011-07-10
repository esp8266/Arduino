/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_max_q7.c  
*  
* Description:	Maximum value of a Q7 vector.  
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
 * @brief Maximum value of a Q7 vector.  
 * @param[in]       *pSrc points to the input vector  
 * @param[in]       blockSize length of the input vector  
 * @param[out]      *pResult maximum value returned here  
 * @param[out]      *pIndex index of maximum value returned here  
  * @return none.  
 */ 
 
void arm_max_q7( 
  q7_t * pSrc, 
  uint32_t blockSize, 
  q7_t * pResult, 
  uint32_t * pIndex) 
{ 
  q7_t res, maxVal, x0, x1, maxVal2, maxVal1;    /* Temporary variables to store the output value. */ 
  uint32_t blkCnt, index1, index2, index3, indx, indxMod;       /* loop counter */ 
 
  /* Initialise the index value to zero. */ 
  indx = 0u; 
 
  /* Load first input value that act as reference value for comparision */ 
  res = *pSrc++; 
 
  /* Loop over blockSize number of values */ 
  blkCnt = (blockSize - 1u) >> 2u; 
 
  while(blkCnt > 0u) 
  { 
    indxMod = blockSize - (blkCnt * 4u); 
 
    /* Load two input values for comparision */ 
    x0 = *pSrc++; 
    x1 = *pSrc++; 
 
    if(x0 < x1) 
    { 
      /* Update the minimum value and its index */ 
      maxVal1 = x1; 
      index1 = indxMod + 1u; 
    } 
    else 
    { 
      /* Update the minimum value and its index */ 
      maxVal1 = x0; 
      index1 = indxMod; 
    } 
 
    /* Load two input values for comparision */ 
    x0 = *pSrc++; 
    x1 = *pSrc++; 
 
    if(x0 < x1) 
    { 
      /* Update the minimum value and its index */ 
      maxVal2 = x1; 
      index2 = indxMod + 3u; 
    } 
    else 
    { 
      /* Update the minimum value and its index */ 
      maxVal2 = x0; 
      index2 = indxMod + 2u; 
    } 
 
    if(maxVal1 < maxVal2) 
    { 
      /* Update the minimum value and its index */ 
      maxVal = maxVal2; 
      index3 = index2; 
    } 
    else 
    { 
      /* Update the minimum value and its index */ 
      maxVal = maxVal1; 
      index3 = index1; 
    } 
 
    if(res < maxVal) 
    { 
      /* Update the minimum value and its index */ 
      res = maxVal; 
      indx = index3; 
    } 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
 
  } 
 
  blkCnt = (blockSize - 1u) % 0x04u; 
 
  while(blkCnt > 0u) 
  { 
    /* Initialize maxVal to the next consecutive values one by one */ 
    maxVal = *pSrc++; 
 
    /* compare for the maximum value */ 
    if(res < maxVal) 
    { 
      /* Update the maximum value and its index */ 
      res = maxVal; 
      indx = blockSize - blkCnt; 
    } 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Store the maximum value and its index into destination pointers */ 
  *pResult = res; 
  *pIndex = indx; 
} 
 
/**  
 * @} end of Max group  
 */ 
