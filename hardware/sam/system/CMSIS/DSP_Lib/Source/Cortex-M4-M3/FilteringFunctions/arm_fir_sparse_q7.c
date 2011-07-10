/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_fir_sparse_q7.c  
*  
* Description:	Q7 sparse FIR filter processing function. 
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
*    Production release and review comments incorporated  
*  
* Version 0.0.7  2010/06/10   
*    Misra-C changes done  
* ------------------------------------------------------------------- */ 
#include "arm_math.h" 
 
 
/**  
 * @ingroup groupFilters  
 */ 
 
/**  
 * @addtogroup FIR_Sparse  
 * @{  
 */ 
 
 
/** 
 * @brief Processing function for the Q7 sparse FIR filter. 
 * @param[in]  *S           points to an instance of the Q7 sparse FIR structure. 
 * @param[in]  *pSrc        points to the block of input data. 
 * @param[out] *pDst        points to the block of output data 
 * @param[in]  *pScratchIn  points to a temporary buffer of size blockSize. 
 * @param[in]  *pScratchOut points to a temporary buffer of size blockSize. 
 * @param[in]  blockSize    number of input samples to process per call. 
 * @return none. 
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function is implemented using a 32-bit internal accumulator.  
 * Both coefficients and state variables are represented in 1.7 format and multiplications yield a 2.14 result.  
 * The 2.14 intermediate results are accumulated in a 32-bit accumulator in 18.14 format.  
 * There is no risk of internal overflow with this approach and the full precision of intermediate multiplications is preserved.  
 * The accumulator is then converted to 18.7 format by discarding the low 7 bits. 
 * Finally, the result is truncated to 1.7 format. 
 */ 
 
void arm_fir_sparse_q7( 
  arm_fir_sparse_instance_q7 * S, 
  q7_t * pSrc, 
  q7_t * pDst, 
  q7_t * pScratchIn, 
  q31_t * pScratchOut, 
  uint32_t blockSize) 
{ 
 
  q7_t *pState = S->pState;                      /* State pointer */ 
  q7_t *pCoeffs = S->pCoeffs;                    /* Coefficient pointer */ 
  q7_t *px;                                      /* Scratch buffer pointer */ 
  q7_t *py = pState;                             /* Temporary pointers for state buffer */ 
  q7_t *pb = pScratchIn;                         /* Temporary pointers for scratch buffer */ 
  q7_t *pOut = pDst;                             /* Destination pointer */ 
  int32_t *pTapDelay = S->pTapDelay;             /* Pointer to the array containing offset of the non-zero tap values. */ 
  uint32_t delaySize = S->maxDelay + blockSize;  /* state length */ 
  uint16_t numTaps = S->numTaps;                 /* Filter order */ 
  int32_t readIndex;                             /* Read index of the state buffer */ 
  uint32_t tapCnt, blkCnt;                       /* loop counters */ 
  q7_t coeff = *pCoeffs++;                       /* Read the coefficient value */ 
  q31_t *pScr2 = pScratchOut;                    /* Working pointer for scratch buffer of output values */ 
  q31_t in; 
  q7_t in1, in2, in3, in4; 
 
  /* BlockSize of Input samples are copied into the state buffer */ 
  /* StateIndex points to the starting position to write in the state buffer */ 
  arm_circularWrite_q7(py, (int32_t) delaySize, &S->stateIndex, 1, pSrc, 1, 
                       blockSize); 
 
  /* Loop over the number of taps. */ 
  tapCnt = numTaps; 
 
  /* Read Index, from where the state buffer should be read, is calculated. */ 
  readIndex = ((int32_t) S->stateIndex - (int32_t) blockSize) - *pTapDelay++; 
 
  /* Wraparound of readIndex */ 
  if(readIndex < 0) 
  { 
    readIndex += (int32_t) delaySize; 
  } 
 
  /* Working pointer for state buffer is updated */ 
  py = pState; 
 
  /* blockSize samples are read from the state buffer */ 
  arm_circularRead_q7(py, (int32_t) delaySize, &readIndex, 1, pb, pb, 
                      (int32_t) blockSize, 1, blockSize); 
 
  /* Working pointer for the scratch buffer of state values */ 
  px = pb; 
 
  /* Working pointer for scratch buffer of output values */ 
  pScratchOut = pScr2; 
 
  /* Loop over the blockSize. Unroll by a factor of 4.  
   * Compute 4 multiplications at a time. */ 
  blkCnt = blockSize >> 2; 
 
  while(blkCnt > 0u) 
  { 
    /* Perform multiplication and store in the scratch buffer */ 
    *pScratchOut++ = ((q31_t) * px++ * coeff); 
    *pScratchOut++ = ((q31_t) * px++ * coeff); 
    *pScratchOut++ = ((q31_t) * px++ * coeff); 
    *pScratchOut++ = ((q31_t) * px++ * coeff); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4,  
   * compute the remaining samples */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    /* Perform multiplication and store in the scratch buffer */ 
    *pScratchOut++ = ((q31_t) * px++ * coeff); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Load the coefficient value and  
   * increment the coefficient buffer for the next set of state values */ 
  coeff = *pCoeffs++; 
 
  /* Read Index, from where the state buffer should be read, is calculated. */ 
  readIndex = ((int32_t) S->stateIndex - (int32_t) blockSize) - *pTapDelay++; 
 
  /* Wraparound of readIndex */ 
  if(readIndex < 0) 
  { 
    readIndex += (int32_t) delaySize; 
  } 
 
  /* Loop over the number of taps. */ 
  tapCnt = (uint32_t) numTaps - 1u; 
 
  while(tapCnt > 0u) 
  { 
    /* Working pointer for state buffer is updated */ 
    py = pState; 
 
    /* blockSize samples are read from the state buffer */ 
    arm_circularRead_q7(py, (int32_t) delaySize, &readIndex, 1, pb, pb, 
                        (int32_t) blockSize, 1, blockSize); 
 
    /* Working pointer for the scratch buffer of state values */ 
    px = pb; 
 
    /* Working pointer for scratch buffer of output values */ 
    pScratchOut = pScr2; 
 
    /* Loop over the blockSize. Unroll by a factor of 4.  
     * Compute 4 MACS at a time. */ 
    blkCnt = blockSize >> 2; 
 
    while(blkCnt > 0u) 
    { 
      /* Perform Multiply-Accumulate */ 
      in = *pScratchOut + ((q31_t) * px++ * coeff); 
      *pScratchOut++ = in; 
      in = *pScratchOut + ((q31_t) * px++ * coeff); 
      *pScratchOut++ = in; 
      in = *pScratchOut + ((q31_t) * px++ * coeff); 
      *pScratchOut++ = in; 
      in = *pScratchOut + ((q31_t) * px++ * coeff); 
      *pScratchOut++ = in; 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
 
    /* If the blockSize is not a multiple of 4,  
     * compute the remaining samples */ 
    blkCnt = blockSize % 0x4u; 
 
    while(blkCnt > 0u) 
    { 
      /* Perform Multiply-Accumulate */ 
      in = *pScratchOut + ((q31_t) * px++ * coeff); 
      *pScratchOut++ = in; 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
 
    /* Load the coefficient value and  
     * increment the coefficient buffer for the next set of state values */ 
    coeff = *pCoeffs++; 
 
    /* Read Index, from where the state buffer should be read, is calculated. */ 
    readIndex = ((int32_t) S->stateIndex - 
                 (int32_t) blockSize) - *pTapDelay++; 
 
    /* Wraparound of readIndex */ 
    if(readIndex < 0) 
    { 
      readIndex += (int32_t) delaySize; 
    } 
 
    /* Decrement the tap loop counter */ 
    tapCnt--; 
  } 
 
  /* All the output values are in pScratchOut buffer.  
     Convert them into 1.15 format, saturate and store in the destination buffer. */ 
  /* Loop over the blockSize. */ 
  blkCnt = blockSize >> 2; 
 
  while(blkCnt > 0u) 
  { 
    in1 = (q7_t) __SSAT(*pScr2++ >> 7, 8); 
    in2 = (q7_t) __SSAT(*pScr2++ >> 7, 8); 
    in3 = (q7_t) __SSAT(*pScr2++ >> 7, 8); 
    in4 = (q7_t) __SSAT(*pScr2++ >> 7, 8); 
 
    *__SIMD32(pOut)++ = __PACKq7(in1, in2, in3, in4); 
 
    /* Decrement the blockSize loop counter */ 
    blkCnt--; 
  } 
 
  /* If the blockSize is not a multiple of 4,  
     remaining samples are processed in the below loop */ 
  blkCnt = blockSize % 0x4u; 
 
  while(blkCnt > 0u) 
  { 
    *pOut++ = (q7_t) __SSAT(*pScr2++ >> 7, 8); 
 
    /* Decrement the blockSize loop counter */ 
    blkCnt--; 
  } 
} 
 
/**  
 * @} end of FIR_Sparse group  
 */ 
