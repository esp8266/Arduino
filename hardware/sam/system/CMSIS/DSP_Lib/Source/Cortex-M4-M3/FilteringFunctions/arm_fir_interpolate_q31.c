/*-----------------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_fir_interpolate_q31.c  
*  
* Description:	Q31 FIR interpolation.  
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
 * @brief Processing function for the Q31 FIR interpolator.  
 * @param[in] *S        points to an instance of the Q31 FIR interpolator structure.  
 * @param[in] *pSrc     points to the block of input data.  
 * @param[out] *pDst    points to the block of output data.  
 * @param[in] blockSize number of input samples to process per call.  
 * @return none.  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 * \par  
 * The function is implemented using an internal 64-bit accumulator.  
 * The accumulator has a 2.62 format and maintains full precision of the intermediate multiplication results but provides only a single guard bit.  
 * Thus, if the accumulator result overflows it wraps around rather than clip.  
 * In order to avoid overflows completely the input signal must be scaled down by <code>1/(numTaps/L)</code>.  
 * since <code>numTaps/L</code> additions occur per output sample.  
 * After all multiply-accumulates are performed, the 2.62 accumulator is truncated to 1.32 format and then saturated to 1.31 format.  
 */ 
 
 
void arm_fir_interpolate_q31( 
  const arm_fir_interpolate_instance_q31 * S, 
  q31_t * pSrc, 
  q31_t * pDst, 
  uint32_t blockSize) 
{ 
  q31_t *pState = S->pState;                     /* State pointer */ 
  q31_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */ 
  q31_t *pStateCurnt;                            /* Points to the current sample of the state */ 
  q31_t *ptr1, *ptr2;                            /* Temporary pointers for state and coefficient buffers */ 
  q63_t sum0;                                    /* Accumulators */ 
  q31_t x0, c0;                                  /* Temporary variables to hold state and coefficient values */ 
  uint32_t i, blkCnt, j;                         /* Loop counters */ 
  uint16_t phaseLen = S->phaseLength, tapCnt;    /* Length of each polyphase filter component */ 
 
 
  /* S->pState buffer contains previous frame (phaseLen - 1) samples */ 
  /* pStateCurnt points to the location where the new input data should be written */ 
  pStateCurnt = S->pState + ((q31_t) phaseLen - 1); 
 
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
      tapCnt = phaseLen >> 2; 
      while(tapCnt > 0u) 
      { 
 
        /* Read the coefficient */ 
        c0 = *(ptr2); 
 
        /* Upsampling is done by stuffing L-1 zeros between each sample.  
         * So instead of multiplying zeros with coefficients,  
         * Increment the coefficient pointer by interpolation factor times. */ 
        ptr2 += S->L; 
 
        /* Read the input sample */ 
        x0 = *(ptr1++); 
 
        /* Perform the multiply-accumulate */ 
        sum0 += (q63_t) x0 *c0; 
 
        /* Read the coefficient */ 
        c0 = *(ptr2); 
 
        /* Increment the coefficient pointer by interpolation factor times. */ 
        ptr2 += S->L; 
 
        /* Read the input sample */ 
        x0 = *(ptr1++); 
 
        /* Perform the multiply-accumulate */ 
        sum0 += (q63_t) x0 *c0; 
 
        /* Read the coefficient */ 
        c0 = *(ptr2); 
 
        /* Increment the coefficient pointer by interpolation factor times. */ 
        ptr2 += S->L; 
 
        /* Read the input sample */ 
        x0 = *(ptr1++); 
 
        /* Perform the multiply-accumulate */ 
        sum0 += (q63_t) x0 *c0; 
 
        /* Read the coefficient */ 
        c0 = *(ptr2); 
 
        /* Increment the coefficient pointer by interpolation factor times. */ 
        ptr2 += S->L; 
 
        /* Read the input sample */ 
        x0 = *(ptr1++); 
 
        /* Perform the multiply-accumulate */ 
        sum0 += (q63_t) x0 *c0; 
 
        /* Decrement the loop counter */ 
        tapCnt--; 
      } 
 
      /* If the polyPhase length is not a multiple of 4, compute the remaining filter taps */ 
      tapCnt = phaseLen & 0x3u; 
 
      while(tapCnt > 0u) 
      { 
        /* Read the coefficient */ 
        c0 = *(ptr2); 
 
        /* Increment the coefficient pointer by interpolation factor times. */ 
        ptr2 += S->L; 
 
        /* Read the input sample */ 
        x0 = *(ptr1++); 
 
        /* Perform the multiply-accumulate */ 
        sum0 += (q63_t) x0 *c0; 
 
        /* Decrement the loop counter */ 
        tapCnt--; 
      } 
 
      /* The result is in the accumulator, store in the destination buffer. */ 
      *pDst++ = (q31_t) (sum0 >> 31); 
 
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
 
  tapCnt = (phaseLen - 1u) >> 2u; 
 
  /* copy data */ 
  while(tapCnt > 0u) 
  { 
    *pStateCurnt++ = *pState++; 
    *pStateCurnt++ = *pState++; 
    *pStateCurnt++ = *pState++; 
    *pStateCurnt++ = *pState++; 
 
    /* Decrement the loop counter */ 
    tapCnt--; 
  } 
 
  tapCnt = (phaseLen - 1u) % 0x04u; 
 
  /* copy data */ 
  while(tapCnt > 0u) 
  { 
    *pStateCurnt++ = *pState++; 
 
    /* Decrement the loop counter */ 
    tapCnt--; 
  } 
} 
 
 /**  
  * @} end of FIR_Interpolate group  
  */ 
