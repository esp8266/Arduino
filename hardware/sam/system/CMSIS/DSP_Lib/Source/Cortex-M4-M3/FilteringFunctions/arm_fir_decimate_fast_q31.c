/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_fir_decimate_fast_q31.c  
*  
* Description:	Fast Q31 FIR Decimator.  
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
 * @brief Processing function for the Q31 FIR decimator (fast variant).  
 * @param[in] *S points to an instance of the Q31 FIR decimator structure.  
 * @param[in] *pSrc points to the block of input data.  
 * @param[out] *pDst points to the block of output data  
 * @param[in] blockSize number of input samples to process per call.  
 * @return none  
 *  
 * <b>Scaling and Overflow Behavior:</b>  
 *  
 * \par  
 * This function is optimized for speed at the expense of fixed-point precision and overflow protection.  
 * The result of each 1.31 x 1.31 multiplication is truncated to 2.30 format.  
 * These intermediate results are added to a 2.30 accumulator.  
 * Finally, the accumulator is saturated and converted to a 1.31 result.  
 * The fast version has the same overflow behavior as the standard version and provides less precision since it discards the low 32 bits of each multiplication result.  
 * In order to avoid overflows completely the input signal must be scaled down by log2(numTaps) bits (where log2 is read as log to the base 2).  
 *  
 * \par  
 * Refer to the function <code>arm_fir_decimate_q31()</code> for a slower implementation of this function which uses a 64-bit accumulator to provide higher precision.  
 * Both the slow and the fast versions use the same instance structure.  
 * Use the function <code>arm_fir_decimate_init_q31()</code> to initialize the filter structure.  
 */ 
 
void arm_fir_decimate_fast_q31( 
  arm_fir_decimate_instance_q31 * S, 
  q31_t * pSrc, 
  q31_t * pDst, 
  uint32_t blockSize) 
{ 
  q31_t *pState = S->pState;                     /* State pointer */ 
  q31_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */ 
  q31_t *pStateCurnt;                            /* Points to the current sample of the state */ 
  q31_t x0, c0;                                  /* Temporary variables to hold state and coefficient values */ 
  q31_t *px;                                     /* Temporary pointers for state buffer */ 
  q31_t *pb;                                     /* Temporary pointers for coefficient buffer */ 
  q63_t sum0;                                    /* Accumulator */ 
  uint32_t numTaps = S->numTaps;                 /* Number of taps */ 
  uint32_t i, tapCnt, blkCnt, outBlockSize = blockSize / S->M;  /* Loop counters */ 
 
 
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
 
    /* Set accumulator to zero */ 
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
      /* Read the b[numTaps-1] coefficient */ 
      c0 = *(pb++); 
 
      /* Read x[n-numTaps-1] sample */ 
      x0 = *(px++); 
 
      /* Perform the multiply-accumulate */ 
      sum0 = (q31_t) ((((q63_t) x0 * c0) + (sum0 << 32)) >> 32); 
 
      /* Read the b[numTaps-2] coefficient */ 
      c0 = *(pb++); 
 
      /* Read x[n-numTaps-2] sample */ 
      x0 = *(px++); 
 
      /* Perform the multiply-accumulate */ 
      sum0 = (q31_t) ((((q63_t) x0 * c0) + (sum0 << 32)) >> 32); 
 
      /* Read the b[numTaps-3] coefficient */ 
      c0 = *(pb++); 
 
      /* Read x[n-numTaps-3] sample */ 
      x0 = *(px++); 
 
      /* Perform the multiply-accumulate */ 
      sum0 = (q31_t) ((((q63_t) x0 * c0) + (sum0 << 32)) >> 32); 
 
      /* Read the b[numTaps-4] coefficient */ 
      c0 = *(pb++); 
 
      /* Read x[n-numTaps-4] sample */ 
      x0 = *(px++); 
 
      /* Perform the multiply-accumulate */ 
      sum0 = (q31_t) ((((q63_t) x0 * c0) + (sum0 << 32)) >> 32); 
 
      /* Decrement the loop counter */ 
      tapCnt--; 
    } 
 
    /* If the filter length is not a multiple of 4, compute the remaining filter taps */ 
    tapCnt = numTaps % 0x4u; 
 
    while(tapCnt > 0u) 
    { 
      /* Read coefficients */ 
      c0 = *(pb++); 
 
      /* Fetch 1 state variable */ 
      x0 = *(px++); 
 
      /* Perform the multiply-accumulate */ 
      sum0 = (q31_t) ((((q63_t) x0 * c0) + (sum0 << 32)) >> 32); 
 
      /* Decrement the loop counter */ 
      tapCnt--; 
    } 
 
    /* Advance the state pointer by the decimation factor  
     * to process the next group of decimation factor number samples */ 
    pState = pState + S->M; 
 
    /* The result is in the accumulator, store in the destination buffer. */ 
    *pDst++ = (q31_t) (sum0 << 1); 
 
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
    *pStateCurnt++ = *pState++; 
    *pStateCurnt++ = *pState++; 
    *pStateCurnt++ = *pState++; 
    *pStateCurnt++ = *pState++; 
 
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
