/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:        arm_fir_fast_q15.c   
*   
* Description:  Q15 Fast FIR filter processing function.   
*   
* Target Processor: Cortex-M4/Cortex-M3
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
* Version 0.0.9  2010/08/16    
*    Initial version   
*   
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
 * @param[in] *S points to an instance of the Q15 FIR filter structure.   
 * @param[in] *pSrc points to the block of input data.   
 * @param[out] *pDst points to the block of output data.   
 * @param[in] blockSize number of samples to process per call.   
 * @return none.   
 *   
 * <b>Scaling and Overflow Behavior:</b>   
 * \par   
 * This fast version uses a 32-bit accumulator with 2.30 format.   
 * The accumulator maintains full precision of the intermediate multiplication results but provides only a single guard bit.   
 * Thus, if the accumulator result overflows it wraps around and distorts the result.   
 * In order to avoid overflows completely the input signal must be scaled down by log2(numTaps) bits.   
 * The 2.30 accumulator is then truncated to 2.15 format and saturated to yield the 1.15 result.   
 *   
 * \par   
 * Refer to the function <code>arm_fir_q15()</code> for a slower implementation of this function which uses 64-bit accumulation to avoid wrap around distortion.  Both the slow and the fast versions use the same instance structure.   
 * Use the function <code>arm_fir_init_q15()</code> to initialize the filter structure.   
 */

void arm_fir_fast_q15(
  const arm_fir_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize)
{
  q15_t *pState = S->pState;                     /* State pointer */
  q15_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */
  q15_t *pStateCurnt;                            /* Points to the current sample of the state */
  q15_t *px1;                                    /* Temporary q15 pointer for state buffer */
  q31_t *pb;                                     /* Temporary pointer for coefficient buffer */
  q31_t *px2;                                    /* Temporary q31 pointer for SIMD state buffer accesses */
  q31_t x0, x1, x2, x3, c0;                      /* Temporary variables to hold SIMD state and coefficient values */
  q31_t acc0, acc1, acc2, acc3;                  /* Accumulators */
  uint32_t numTaps = S->numTaps;                 /* Number of taps in the filter */
  uint32_t tapCnt, blkCnt;                       /* Loop counters */

  /* S->pState points to buffer which contains previous frame (numTaps - 1) samples */
  /* pStateCurnt points to the location where the new input data should be written */
  pStateCurnt = &(S->pState[(numTaps - 1u)]);

  /* Apply loop unrolling and compute 4 output values simultaneously.   
   * The variables acc0 ... acc3 hold output values that are being computed:   
   *   
   *    acc0 =  b[numTaps-1] * x[n-numTaps-1] + b[numTaps-2] * x[n-numTaps-2] + b[numTaps-3] * x[n-numTaps-3] +...+ b[0] * x[0]   
   *    acc1 =  b[numTaps-1] * x[n-numTaps] +   b[numTaps-2] * x[n-numTaps-1] + b[numTaps-3] * x[n-numTaps-2] +...+ b[0] * x[1]   
   *    acc2 =  b[numTaps-1] * x[n-numTaps+1] + b[numTaps-2] * x[n-numTaps] +   b[numTaps-3] * x[n-numTaps-1] +...+ b[0] * x[2]   
   *    acc3 =  b[numTaps-1] * x[n-numTaps+2] + b[numTaps-2] * x[n-numTaps+1] + b[numTaps-3] * x[n-numTaps]   +...+ b[0] * x[3]   
   */
  blkCnt = blockSize >> 2;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* Copy four new input samples into the state buffer.   
     ** Use 32-bit SIMD to move the 16-bit data.  Only requires two copies. */
    *__SIMD32(pStateCurnt)++ = *__SIMD32(pSrc)++;
    *__SIMD32(pStateCurnt)++ = *__SIMD32(pSrc)++;

    /* Set all accumulators to zero */
    acc0 = 0;
    acc1 = 0;
    acc2 = 0;
    acc3 = 0;

    /* Initialize state pointer of type q15 */
    px1 = pState;

    /* Initialize coeff pointer of type q31 */
    pb = (q31_t *) (pCoeffs);

    /* Read the first two samples from the state buffer:  x[n-N], x[n-N-1] */
    x0 = *(q31_t *) (px1++);

    /* Read the third and forth samples from the state buffer: x[n-N-1], x[n-N-2] */
    x1 = *(q31_t *) (px1++);

    /* Loop over the number of taps.  Unroll by a factor of 4.   
     ** Repeat until we've computed numTaps-4 coefficients. */
    tapCnt = numTaps >> 2;
    do
    {
      /* Read the first two coefficients using SIMD:  b[N] and b[N-1] coefficients */
      c0 = *(pb++);

      /* acc0 +=  b[N] * x[n-N] + b[N-1] * x[n-N-1] */
      acc0 = __SMLAD(x0, c0, acc0);

      /* acc1 +=  b[N] * x[n-N-1] + b[N-1] * x[n-N-2] */
      acc1 = __SMLAD(x1, c0, acc1);

      /* Read state x[n-N-2], x[n-N-3] */
      x2 = *(q31_t *) (px1++);

      /* Read state x[n-N-3], x[n-N-4] */
      x3 = *(q31_t *) (px1++);

      /* acc2 +=  b[N] * x[n-N-2] + b[N-1] * x[n-N-3] */
      acc2 = __SMLAD(x2, c0, acc2);

      /* acc3 +=  b[N] * x[n-N-3] + b[N-1] * x[n-N-4] */
      acc3 = __SMLAD(x3, c0, acc3);

      /* Read coefficients b[N-2], b[N-3] */
      c0 = *(pb++);

      /* acc0 +=  b[N-2] * x[n-N-2] + b[N-3] * x[n-N-3] */
      acc0 = __SMLAD(x2, c0, acc0);

      /* acc1 +=  b[N-2] * x[n-N-3] + b[N-3] * x[n-N-4] */
      acc1 = __SMLAD(x3, c0, acc1);

      /* Read state x[n-N-4], x[n-N-5] */
      x0 = *(q31_t *) (px1++);

      /* Read state x[n-N-5], x[n-N-6] */
      x1 = *(q31_t *) (px1++);

      /* acc2 +=  b[N-2] * x[n-N-4] + b[N-3] * x[n-N-5] */
      acc2 = __SMLAD(x0, c0, acc2);

      /* acc3 +=  b[N-2] * x[n-N-5] + b[N-3] * x[n-N-6] */
      acc3 = __SMLAD(x1, c0, acc3);
      tapCnt--;

    }
    while(tapCnt > 0u);

    /* If the filter length is not a multiple of 4, compute the remaining filter taps.   
     ** This is always 2 taps since the filter length is always even. */
    if((numTaps & 0x3u) != 0u)
    {
      /* Read 2 coefficients */
      c0 = *(pb++);
      /* Fetch 4 state variables */
      x2 = *(q31_t *) (px1++);
      x3 = *(q31_t *) (px1++);

      /* Perform the multiply-accumulates */
      acc0 = __SMLAD(x0, c0, acc0);
      acc1 = __SMLAD(x1, c0, acc1);
      acc2 = __SMLAD(x2, c0, acc2);
      acc3 = __SMLAD(x3, c0, acc3);
    }

    /* The results in the 4 accumulators are in 2.30 format.  Convert to 1.15 with saturation.   
     ** Then store the 4 outputs in the destination buffer. */

#ifndef ARM_MATH_BIG_ENDIAN

    *__SIMD32(pDst)++ = __PKHBT((acc0 >> 15), (acc1 >> 15), 16u);
    *__SIMD32(pDst)++ = __PKHBT((acc2 >> 15), (acc3 >> 15), 16u);

#else

    *__SIMD32(pDst)++ = __PKHBT((acc1 >> 15), (acc0 >> 15), 16u);
    *__SIMD32(pDst)++ = __PKHBT((acc3 >> 15), (acc2 >> 15), 16u);

#endif /*      #ifndef ARM_MATH_BIG_ENDIAN     */

    /* Advance the state pointer by 4 to process the next group of 4 samples */
    pState = pState + 4;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;
  while(blkCnt > 0u)
  {
    /* Copy two samples into state buffer */
    *pStateCurnt++ = *pSrc++;

    /* Set the accumulator to zero */
    acc0 = 0;

    /* Use SIMD to hold states and coefficients */
    px2 = (q31_t *) pState;
    pb = (q31_t *) (pCoeffs);
    tapCnt = numTaps >> 1;

    do
    {
      acc0 = __SMLAD(*px2++, *(pb++), acc0);
      tapCnt--;
    }
    while(tapCnt > 0u);

    /* The result is in 2.30 format.  Convert to 1.15 with saturation.   
     ** Then store the output in the destination buffer. */
    *pDst++ = (q15_t) ((acc0 >> 15));

    /* Advance state pointer by 1 for the next sample */
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
  tapCnt = (numTaps - 1u) >> 2;

  while(tapCnt > 0u)
  {
    *__SIMD32(pStateCurnt)++ = *__SIMD32(pState)++;
    *__SIMD32(pStateCurnt)++ = *__SIMD32(pState)++;

    tapCnt--;
  }

  /* Calculation of count for remaining q15_t data */
  tapCnt = (numTaps - 1u) % 0x4u;

  /* copy remaining data */
  while(tapCnt > 0u)
  {
    *pStateCurnt++ = *pState++;

    /* Decrement the loop counter */
    tapCnt--;
  }
}

/**   
 * @} end of FIR group   
 */
