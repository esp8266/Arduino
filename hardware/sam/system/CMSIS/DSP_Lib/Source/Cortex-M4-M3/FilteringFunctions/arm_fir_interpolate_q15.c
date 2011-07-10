/*-----------------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_fir_interpolate_q15.c  
*  
* Description:	Q15 FIR interpolation.  
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
* ---------------------------------------------------------------------------*/ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupFilters  
 */ 
 
/**  
 * @addtogroup FIR_Interpolate  
 * @{  
 */ 
 
/**  
 * @brief Processing function for the Q15 FIR interpolator.  
 * @param[in] *S        points to an instance of the Q15 FIR interpolator structure.  
 * @param[in] *pSrc     points to the block of input data.  
 * @param[out] *pDst    points to the block of output data.  
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
 */ 
 
void arm_fir_interpolate_q15( 
  const arm_fir_interpolate_instance_q15 * S, 
  q15_t * pSrc, 
  q15_t * pDst, 
  uint32_t blockSize) 
{ 
  q15_t *pState = S->pState;                     /* State pointer                                            */ 
  q15_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer                                      */ 
  q15_t *pStateCurnt;                            /* Points to the current sample of the state                */ 
  q15_t *ptr1, *ptr2;                            /* Temporary pointers for state and coefficient buffers     */ 
  q63_t sum0;                                    /* Accumulators                                             */ 
  q15_t x0, c0, c1;                              /* Temporary variables to hold state and coefficient values */ 
  q31_t c, x; 
  uint32_t i, blkCnt, j, tapCnt;                 /* Loop counters                                            */ 
  uint16_t phaseLen = S->phaseLength;            /* Length of each polyphase filter component */ 
 
 
  /* S->pState buffer contains previous frame (phaseLen - 1) samples */ 
  /* pStateCurnt points to the location where the new input data should be written */ 
  pStateCurnt = S->pState + (phaseLen - 1u); 
 
  /* Total number of intput samples */ 
  blkCnt = blockSize; 
 
  /* Loop over the blockSize. */ 
  while(blkCnt > 0u) 
  { 
    /* Copy new input sample into the state buffer */ 
    *pStateCurnt++ = *pSrc++; 
 
    /* Address modifier index of coefficient buffer */ 
    j = 1u; 
 
    /* Loop over the Interpolation factor. */ 
    i = S->L; 
    while(i > 0u) 
    { 
      /* Set accumulator to zero */ 
      sum0 = 0; 
 
      /* Initialize state pointer */ 
      ptr1 = pState; 
 
      /* Initialize coefficient pointer */ 
      ptr2 = pCoeffs + (S->L - j); 
 
      /* Loop over the polyPhase length. Unroll by a factor of 4.  
       ** Repeat until we've computed numTaps-(4*S->L) coefficients. */ 
      tapCnt = (uint32_t) phaseLen >> 2u; 
      while(tapCnt > 0u) 
      { 
        /* Read the coefficient */ 
        c0 = *(ptr2); 
 
        /* Upsampling is done by stuffing L-1 zeros between each sample.  
         * So instead of multiplying zeros with coefficients,  
         * Increment the coefficient pointer by interpolation factor times. */ 
        ptr2 += S->L; 
 
        /* Read the coefficient */ 
        c1 = *(ptr2); 
 
        /* Increment the coefficient pointer by interpolation factor times. */ 
        ptr2 += S->L; 
 
        /* Pack the coefficients */ 
        c = __PKHBT(c0, c1, 16); 
 
        /* Read twp consecutive input samples */ 
        x = *__SIMD32(ptr1)++; 
 
        /* Perform the multiply-accumulate */ 
        sum0 = __SMLALD(x, c, sum0); 
 
        /* Read the coefficient */ 
        c0 = *(ptr2); 
 
        /* Upsampling is done by stuffing L-1 zeros between each sample.  
         * So insted of multiplying zeros with coefficients,  
         * Increment the coefficient pointer by interpolation factor times. */ 
        ptr2 += S->L; 
 
        /* Read the coefficient */ 
        c1 = *(ptr2); 
 
        /* Increment the coefficient pointer by interpolation factor times. */ 
        ptr2 += S->L; 
 
        /* Pack the coefficients */ 
        c = __PKHBT(c0, c1, 16); 
 
        /* Read twp consecutive input samples */ 
        x = *__SIMD32(ptr1)++; 
 
        /* Perform the multiply-accumulate */ 
        sum0 = __SMLALD(x, c, sum0); 
 
        /* Decrement the loop counter */ 
        tapCnt--; 
      } 
 
      /* If the polyPhase length is not a multiple of 4, compute the remaining filter taps */ 
      tapCnt = (uint32_t) phaseLen & 0x3u; 
 
      while(tapCnt > 0u) 
      { 
        /* Read the coefficient */ 
        c0 = *(ptr2); 
 
        /* Increment the coefficient pointer by interpolation factor times. */ 
        ptr2 += S->L; 
 
        /* Read the input sample */ 
        x0 = *(ptr1++); 
 
        /* Perform the multiply-accumulate */ 
        sum0 = __SMLALD(x0, c0, sum0); 
 
        /* Decrement the loop counter */ 
        tapCnt--; 
      } 
 
      /* The result is in the accumulator, store in the destination buffer. */ 
      *pDst++ = (q15_t) (__SSAT((sum0 >> 15), 16)); 
 
      /* Increment the address modifier index of coefficient buffer */ 
      j++; 
 
      /* Decrement the loop counter */ 
      i--; 
    } 
 
    /* Advance the state pointer by 1  
     * to process the next group of interpolation factor number samples */ 
    pState = pState + 1; 
 
    /* Decrement the loop counter */ 
    blkCnt--; 
  } 
 
  /* Processing is complete.  
   ** Now copy the last phaseLen - 1 samples to the satrt of the state buffer.  
   ** This prepares the state buffer for the next function call. */ 
 
  /* Points to the start of the state buffer */ 
  pStateCurnt = S->pState; 
 
  i = ((uint32_t) phaseLen - 1u) >> 2u; 
 
  /* copy data */ 
  while(i > 0u) 
  { 
    *__SIMD32(pStateCurnt)++ = *__SIMD32(pState)++; 
    *__SIMD32(pStateCurnt)++ = *__SIMD32(pState)++; 
 
    /* Decrement the loop counter */ 
    i--; 
  } 
 
  i = ((uint32_t) phaseLen - 1u) % 0x04u; 
 
  while(i > 0u) 
  { 
    *pStateCurnt++ = *pState++; 
 
    /* Decrement the loop counter */ 
    i--; 
  } 
 
} 
 
 /**  
  * @} end of FIR_Interpolate group  
  */ 
