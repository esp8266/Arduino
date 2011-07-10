/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_fir_decimate_q15.c  
*  
* Description:	Q15 FIR Decimator.  
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
* -------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupFilters  
 */ 
 
/**  
 * @addtogroup FIR_decimate  
 * @{  
 */ 
 
/**  
 * @brief Processing function for the Q15 FIR decimator.  
 * @param[in] *S points to an instance of the Q15 FIR decimator structure.  
 * @param[in] *pSrc points to the block of input data.  
 * @param[out] *pDst points to the location where the output result is written.  
 * @param[in] blockSize number of input samples to process per call.  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function is implemented using a 64-bit internal accumulator.  
 * Both coefficients and state variables are represented in 1.15 format and multiplications yield a 2.30 result.  
 * The 2.30 intermediate results are accumulated in a 64-bit accumulator in 34.30 format.  
 * There is no risk of internal overflow with this approach and the full precision of intermediate multiplications is preserved.  
 * After all additions have been performed, the accumulator is truncated to 34.15 format by discarding low 15 bits.  
 * Lastly, the accumulator is saturated to yield a result in 1.15 format.  
 *  
 * \par  
 * Refer to the function <code>arm_fir_decimate_fast_q15()</code> for a faster but less precise implementation of this function.  
 */ 
 
void arm_fir_decimate_q15( 
  const arm_fir_decimate_instance_q15 * S, 
  q15_t * pSrc, 
  q15_t * pDst, 
  uint32_t blockSize) 
{ 
  q15_t *pState = S->pState;                     /* State pointer */ 
  q15_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */ 
  q15_t *pStateCurnt;                            /* Points to the current sample of the state */ 
  q15_t *px;                                     /* Temporary pointer for state buffer */ 
  q15_t *pb;                                     /* Temporary pointer coefficient buffer */ 
  q31_t x0, c0;                                  /* Temporary variables to hold state and coefficient values */ 
  q63_t sum0;                                    /* Accumulators */ 
  uint32_t numTaps = S->numTaps;                 /* Number of taps */ 
  uint32_t i, blkCnt, tapCnt, outBlockSize = blockSize / S->M;  /* Loop counters */ 
 
 
  /* S->pState buffer contains previous frame (numTaps - 1) samples */ 
  /* pStateCurnt points to the location where the new input data should be written */ 
  pStateCurnt = S->pState + (numTaps - 1u); 
 
  /* Total number of output samples to be computed */ 
  blkCnt = outBlockSize; 
 
  while(blkCnt > 0u) 
  { 
    /* Copy decimation factor number of new input samples into the state buffer */ 
    i = S->M; 
 
    do 
    { 
      *pStateCurnt++ = *pSrc++; 
 
    } while(--i); 
 
    /*Set sum to zero */ 
    sum0 = 0; 
 
    /* Initialize state pointer */ 
    px = pState; 
 
    /* Initialize coeff pointer */ 
    pb = pCoeffs; 
 
    /* Loop unrolling.  Process 4 taps at a time. */ 
    tapCnt = numTaps >> 2; 
 
    /* Loop over the number of taps.  Unroll by a factor of 4.  
     ** Repeat until we've computed numTaps-4 coefficients. */ 
    while(tapCnt > 0u) 
    { 
      /* Read the Read b[numTaps-1] and b[numTaps-2]  coefficients */ 
      c0 = *__SIMD32(pb)++; 
 
      /* Read x[n-numTaps-1] and x[n-numTaps-2]sample */ 
      x0 = *__SIMD32(px)++; 
 
      /* Perform the multiply-accumulate */ 
      sum0 = __SMLALD(x0, c0, sum0); 
 
      /* Read the b[numTaps-3] and b[numTaps-4] coefficient */ 
      c0 = *__SIMD32(pb)++; 
 
      /* Read x[n-numTaps-2] and x[n-numTaps-3] sample */ 
      x0 = *__SIMD32(px)++; 
 
      /* Perform the multiply-accumulate */ 
      sum0 = __SMLALD(x0, c0, sum0); 
 
      /* Decrement the loop counter */ 
      tapCnt--; 
    } 
 
    /* If the filter length is not a multiple of 4, compute the remaining filter taps */ 
    tapCnt = numTaps % 0x4u; 
 
    while(tapCnt > 0u) 
    { 
      /* Read coefficients */ 
      c0 = *pb++; 
 
      /* Fetch 1 state variable */ 
      x0 = *px++; 
 
      /* Perform the multiply-accumulate */ 
      sum0 = __SMLALD(x0, c0, sum0); 
 
      /* Decrement the loop counter */ 
      tapCnt--; 
    } 
 
    /* Advance the state pointer by the decimation factor  
     * to process the next group of decimation factor number samples */ 
    pState = pState + S->M; 
 
    /* Store filter output, smlad returns the values in 2.14 format */ 
    /* so downsacle by 15 to get output in 1.15 */ 
    *pDst++ = (q15_t) (__SSAT((sum0 >> 15), 16)); 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Processing is complete.  
   ** Now copy the last numTaps - 1 samples to the satrt of the state buffer.  
   ** This prepares the state buffer for the next function call. */ 
 
  /* Points to the start of the state buffer */ 
  pStateCurnt = S->pState; 
 
  i = (numTaps - 1u) >> 2u; 
 
  /* copy data */ 
  while(i > 0u) 
  { 
    *__SIMD32(pStateCurnt)++ = *__SIMD32(pState)++; 
    *__SIMD32(pStateCurnt)++ = *__SIMD32(pState)++; 
 
    /* Decrement the loop counter */ 
    i--; 
  } 
 
  i = (numTaps - 1u) % 0x04u; 
 
  /* copy data */ 
  while(i > 0u) 
  { 
    *pStateCurnt++ = *pState++; 
 
    /* Decrement the loop counter */ 
    i--; 
  } 
} 
 
/**  
 * @} end of FIR_decimate group  
 */ 
