/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:        arm_fir_q7.c  
*  
* Description:  Q7 FIR filter processing function.  
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
* 	 incorporated review comments and updated with latest CMSIS layer  
*  
* Version 0.0.3  2010/03/10   
*    Initial version  
* -------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupFilters  
 */ 
 
/**  
 * @addtogroup FIR  
 * @{  
 */ 
 
/**  
 * @param[in]   *S points to an instance of the Q7 FIR filter structure.  
 * @param[in]   *pSrc points to the block of input data.  
 * @param[out]  *pDst points to the block of output data.  
 * @param[in]   blockSize number of samples to process per call.  
 * @return 	none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function is implemented using a 32-bit internal accumulator.  
 * Both coefficients and state variables are represented in 1.7 format and multiplications yield a 2.14 result.  
 * The 2.14 intermediate results are accumulated in a 32-bit accumulator in 18.14 format.  
 * There is no risk of internal overflow with this approach and the full precision of intermediate multiplications is preserved.  
 * The accumulator is converted to 18.7 format by discarding the low 7 bits.  
 * Finally, the result is truncated to 1.7 format.  
 */ 
 
void arm_fir_q7( 
  const arm_fir_instance_q7 * S, 
  q7_t * pSrc, 
  q7_t * pDst, 
  uint32_t blockSize) 
{ 
  uint32_t numTaps = S->numTaps;                 /* Number of taps in the filter */ 
  uint32_t i, blkCnt;                            /* Loop counters */ 
  q7_t *pState = S->pState;                      /* State pointer */ 
  q7_t *pCoeffs = S->pCoeffs;                    /* Coefficient pointer */ 
  q7_t *px, *pb;                                 /* Temporary pointers to state and coeff */ 
  q31_t acc = 0;                                 /* Accumlator */ 
  q31_t input1, input2;                          /* Temporary variables to store input */ 
  q15_t in1, in2;                                /* Temporary variables to store input */ 
  q7_t *pStateCurnt;                             /* Points to the current sample of the state */ 
 
 
  /* S->pState points to state array which contains previous frame (numTaps - 1) samples */ 
  /* pStateCurnt points to the location where the new input data should be written */ 
  pStateCurnt = S->pState + (numTaps - 1u); 
 
  i = blockSize >> 2u; 
 
  /* Copy four new input samples into the state buffer.  
   ** Use 32-bit SIMD to move the four 8-bit data.  Only requires one copy for every four samples. */ 
  while(i > 0u) 
  { 
    *__SIMD32(pStateCurnt)++ = *__SIMD32(pSrc)++; 
    i--; 
  } 
 
  i = blockSize % 0x4u; 
 
  /* Copy remining samples into the state buffer. */ 
  while(i > 0u) 
  { 
    *pStateCurnt++ = *pSrc++; 
    i--; 
  } 
 
  blkCnt = blockSize; 
 
  /* Perform filtering upto BlockSize - BlockSize%4  */ 
  while(blkCnt > 0u) 
  { 
    /* Set accumulator to zero */ 
    acc = 0; 
 
    /* Initialize state pointer of type q7 */ 
    px = pState; 
 
    /* Initialize coeff pointer of type q7 */ 
    pb = pCoeffs; 
 
    i = numTaps >> 2u; 
 
    /* Loop over the number of taps.  Unroll by a factor of 4.  
     ** Repeat until we've computed numTaps-4 coefficients. */ 
    while(i > 0u) 
    { 
      /* Reading two inputs of state buffer and packing */ 
      in1 = (q15_t) * px++; 
      in2 = (q15_t) * px++; 
      input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16); 
 
      /* Reading two inputs of coefficient buffer and packing */ 
      in1 = (q15_t) * pb++; 
      in2 = (q15_t) * pb++; 
      input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16); 
 
      /* Perform Multiply and accumlation of 2 packed inputs and coefficients using SMLALD and store the result in accumlator. */ 
      acc = __SMLAD(input1, input2, acc); 
 
      /* Reading two inputs of state buffer and packing */ 
      in1 = (q15_t) * px++; 
      in2 = (q15_t) * px++; 
      input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16); 
 
      /* Reading two inputs of coefficient buffer and packing */ 
      in1 = (q15_t) * pb++; 
      in2 = (q15_t) * pb++; 
      input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16); 
 
      /* Perform Multiply and accumlation of 2 packed inputs and coefficients using SMLALD and store the result in accumlator. */ 
      acc = __SMLAD(input1, input2, acc); 
 
      /* Decrement the tap loop counter */ 
      i--; 
    } 
 
    i = numTaps % 0x4u; 
 
    /* If the filter length is not a multiple of 4, compute the remaining filter taps */ 
    while(i > 0u) 
    { 
      acc = __SMLAD(*px++, *pb++, acc); 
      i--; 
 
    } 
 
    /* Saturate output */ 
    acc = __SSAT((acc >> 7), 8); 
 
    /*Store filter output */ 
    *pDst++ = (q7_t) (acc); 
 
    /* Advance the state pointer by 1 to process the next sample */ 
    pState = pState + 1; 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Processing is complete.  
   ** Now copy the last numTaps - 1 samples to the satrt of the state buffer.  
   ** This prepares the state buffer for the next function call. */ 
 
  /* Points to the start of the state buffer */ 
  pStateCurnt = S->pState; 
 
  /* Calculation of count for copying integer writes */ 
  i = (numTaps - 1u) >> 2u; 
 
  /* Copy four values using integer pointer */ 
  while(i > 0u) 
  { 
    *__SIMD32(pStateCurnt)++ = *__SIMD32(pState)++; 
 
    i--; 
 
  } 
 
  /* Calculation of count for remaining q7_t data */ 
  i = (numTaps - 1u) % 0x4u; 
 
  /* Copy of remaining q7_t data */ 
  while(i > 0u) 
  { 
    *pStateCurnt++ = *pState++; 
    i--; 
  } 
 
} 
 
/**  
 * @} end of FIR group  
 */ 
