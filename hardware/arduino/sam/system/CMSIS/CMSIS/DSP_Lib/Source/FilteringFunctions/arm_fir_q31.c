/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_fir_q31.c   
*   
* Description:	Q31 FIR filter processing function.   
*   
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
 * @param[in] *S points to an instance of the Q31 FIR filter structure.   
 * @param[in] *pSrc points to the block of input data.   
 * @param[out] *pDst points to the block of output data.   
 * @param[in] blockSize number of samples to process per call.   
 * @return none.   
 *   
 * @details   
 * <b>Scaling and Overflow Behavior:</b>   
 * \par   
 * The function is implemented using an internal 64-bit accumulator.   
 * The accumulator has a 2.62 format and maintains full precision of the intermediate multiplication results but provides only a single guard bit.   
 * Thus, if the accumulator result overflows it wraps around rather than clip.   
 * In order to avoid overflows completely the input signal must be scaled down by log2(numTaps) bits.   
 * After all multiply-accumulates are performed, the 2.62 accumulator is right shifted by 31 bits and saturated to 1.31 format to yield the final result. 
 *   
 * \par   
 * Refer to the function <code>arm_fir_fast_q31()</code> for a faster but less precise implementation of this filter for Cortex-M3 and Cortex-M4.   
 */

void arm_fir_q31(
  const arm_fir_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize)
{
  q31_t *pState = S->pState;                     /* State pointer */
  q31_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */
  q31_t *pStateCurnt;                            /* Points to the current sample of the state */


#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  q31_t x0, x1, x2, x3;                          /* Temporary variables to hold state */
  q31_t c0;                                      /* Temporary variable to hold coefficient value */
  q31_t *px;                                     /* Temporary pointer for state */
  q31_t *pb;                                     /* Temporary pointer for coefficient buffer */
  q63_t acc0, acc1, acc2, acc3;                  /* Accumulators */
  uint32_t numTaps = S->numTaps;                 /* Number of filter coefficients in the filter */
  uint32_t i, tapCnt, blkCnt;                    /* Loop counters */

  /* S->pState points to state array which contains previous frame (numTaps - 1) samples */
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
    /* Copy four new input samples into the state buffer */
    *pStateCurnt++ = *pSrc++;
    *pStateCurnt++ = *pSrc++;
    *pStateCurnt++ = *pSrc++;
    *pStateCurnt++ = *pSrc++;

    /* Set all accumulators to zero */
    acc0 = 0;
    acc1 = 0;
    acc2 = 0;
    acc3 = 0;

    /* Initialize state pointer */
    px = pState;

    /* Initialize coefficient pointer */
    pb = pCoeffs;

    /* Read the first three samples from the state buffer:   
     *  x[n-numTaps], x[n-numTaps-1], x[n-numTaps-2] */
    x0 = *(px++);
    x1 = *(px++);
    x2 = *(px++);

    /* Loop unrolling.  Process 4 taps at a time. */
    tapCnt = numTaps >> 2;
    i = tapCnt;

    while(i > 0u)
    {
      /* Read the b[numTaps] coefficient */
      c0 = *(pb++);

      /* Read x[n-numTaps-3] sample */
      x3 = *(px++);

      /* acc0 +=  b[numTaps] * x[n-numTaps] */
      acc0 += ((q63_t) x0 * c0);

      /* acc1 +=  b[numTaps] * x[n-numTaps-1] */
      acc1 += ((q63_t) x1 * c0);

      /* acc2 +=  b[numTaps] * x[n-numTaps-2] */
      acc2 += ((q63_t) x2 * c0);

      /* acc3 +=  b[numTaps] * x[n-numTaps-3] */
      acc3 += ((q63_t) x3 * c0);

      /* Read the b[numTaps-1] coefficient */
      c0 = *(pb++);

      /* Read x[n-numTaps-4] sample */
      x0 = *(px++);

      /* Perform the multiply-accumulates */
      acc0 += ((q63_t) x1 * c0);
      acc1 += ((q63_t) x2 * c0);
      acc2 += ((q63_t) x3 * c0);
      acc3 += ((q63_t) x0 * c0);

      /* Read the b[numTaps-2] coefficient */
      c0 = *(pb++);

      /* Read x[n-numTaps-5] sample */
      x1 = *(px++);

      /* Perform the multiply-accumulates */
      acc0 += ((q63_t) x2 * c0);
      acc1 += ((q63_t) x3 * c0);
      acc2 += ((q63_t) x0 * c0);
      acc3 += ((q63_t) x1 * c0);
      /* Read the b[numTaps-3] coefficients */
      c0 = *(pb++);

      /* Read x[n-numTaps-6] sample */
      x2 = *(px++);

      /* Perform the multiply-accumulates */
      acc0 += ((q63_t) x3 * c0);
      acc1 += ((q63_t) x0 * c0);
      acc2 += ((q63_t) x1 * c0);
      acc3 += ((q63_t) x2 * c0);
      i--;
    }

    /* If the filter length is not a multiple of 4, compute the remaining filter taps */

    i = numTaps - (tapCnt * 4u);
    while(i > 0u)
    {
      /* Read coefficients */
      c0 = *(pb++);

      /* Fetch 1 state variable */
      x3 = *(px++);

      /* Perform the multiply-accumulates */
      acc0 += ((q63_t) x0 * c0);
      acc1 += ((q63_t) x1 * c0);
      acc2 += ((q63_t) x2 * c0);
      acc3 += ((q63_t) x3 * c0);

      /* Reuse the present sample states for next sample */
      x0 = x1;
      x1 = x2;
      x2 = x3;

      /* Decrement the loop counter */
      i--;
    }

    /* Advance the state pointer by 4 to process the next group of 4 samples */
    pState = pState + 4;

    /* The results in the 4 accumulators are in 2.62 format.  Convert to 1.31   
     ** Then store the 4 outputs in the destination buffer. */
    *pDst++ = (q31_t) (acc0 >> 31u);
    *pDst++ = (q31_t) (acc1 >> 31u);
    *pDst++ = (q31_t) (acc2 >> 31u);
    *pDst++ = (q31_t) (acc3 >> 31u);

    /* Decrement the samples loop counter */
    blkCnt--;
  }


  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = blockSize % 4u;

  while(blkCnt > 0u)
  {
    /* Copy one sample at a time into state buffer */
    *pStateCurnt++ = *pSrc++;

    /* Set the accumulator to zero */
    acc0 = 0;

    /* Initialize state pointer */
    px = pState;

    /* Initialize Coefficient pointer */
    pb = (pCoeffs);

    i = numTaps;

    /* Perform the multiply-accumulates */
    do
    {
      acc0 += (q63_t) * (px++) * (*(pb++));
      i--;
    } while(i > 0u);

    /* The result is in 2.62 format.  Convert to 1.31   
     ** Then store the output in the destination buffer. */
    *pDst++ = (q31_t) (acc0 >> 31u);

    /* Advance state pointer by 1 for the next sample */
    pState = pState + 1;

    /* Decrement the samples loop counter */
    blkCnt--;
  }

  /* Processing is complete.   
   ** Now copy the last numTaps - 1 samples to the satrt of the state buffer.   
   ** This prepares the state buffer for the next function call. */

  /* Points to the start of the state buffer */
  pStateCurnt = S->pState;

  tapCnt = (numTaps - 1u) >> 2u;

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

  /* Calculate remaining number of copies */
  tapCnt = (numTaps - 1u) % 0x4u;

  /* Copy the remaining q31_t data */
  while(tapCnt > 0u)
  {
    *pStateCurnt++ = *pState++;

    /* Decrement the loop counter */
    tapCnt--;
  }

#else

/* Run the below code for Cortex-M0 */

  q31_t *px;                                     /* Temporary pointer for state */
  q31_t *pb;                                     /* Temporary pointer for coefficient buffer */
  q63_t acc;                                     /* Accumulator */
  uint32_t numTaps = S->numTaps;                 /* Length of the filter */
  uint32_t i, tapCnt, blkCnt;                    /* Loop counters */

  /* S->pState buffer contains previous frame (numTaps - 1) samples */
  /* pStateCurnt points to the location where the new input data should be written */
  pStateCurnt = &(S->pState[(numTaps - 1u)]);

  /* Initialize blkCnt with blockSize */
  blkCnt = blockSize;

  while(blkCnt > 0u)
  {
    /* Copy one sample at a time into state buffer */
    *pStateCurnt++ = *pSrc++;

    /* Set the accumulator to zero */
    acc = 0;

    /* Initialize state pointer */
    px = pState;

    /* Initialize Coefficient pointer */
    pb = pCoeffs;

    i = numTaps;

    /* Perform the multiply-accumulates */
    do
    {
      /* acc =  b[numTaps-1] * x[n-numTaps-1] + b[numTaps-2] * x[n-numTaps-2] + b[numTaps-3] * x[n-numTaps-3] +...+ b[0] * x[0] */
      acc += (q63_t) * px++ * *pb++;
      i--;
    } while(i > 0u);

    /* The result is in 2.62 format.  Convert to 1.31        
     ** Then store the output in the destination buffer. */
    *pDst++ = (q31_t) (acc >> 31u);

    /* Advance state pointer by 1 for the next sample */
    pState = pState + 1;

    /* Decrement the samples loop counter */
    blkCnt--;
  }

  /* Processing is complete.        
   ** Now copy the last numTaps - 1 samples to the starting of the state buffer.      
   ** This prepares the state buffer for the next function call. */

  /* Points to the start of the state buffer */
  pStateCurnt = S->pState;

  /* Copy numTaps number of values */
  tapCnt = numTaps - 1u;

  /* Copy the data */
  while(tapCnt > 0u)
  {
    *pStateCurnt++ = *pState++;

    /* Decrement the loop counter */
    tapCnt--;
  }


#endif /*  #ifndef ARM_MATH_CM0 */

}

/**   
 * @} end of FIR group   
 */
