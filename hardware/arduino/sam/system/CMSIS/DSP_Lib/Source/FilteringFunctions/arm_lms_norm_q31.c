/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_lms_norm_q31.c   
*   
* Description:	Processing function for the Q31 NLMS filter.   
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
 * @addtogroup LMS_NORM   
 * @{   
 */

/**   
* @brief Processing function for Q31 normalized LMS filter.   
* @param[in] *S points to an instance of the Q31 normalized LMS filter structure.   
* @param[in] *pSrc points to the block of input data.   
* @param[in] *pRef points to the block of reference data.   
* @param[out] *pOut points to the block of output data.   
* @param[out] *pErr points to the block of error data.   
* @param[in] blockSize number of samples to process.   
* @return none.   
*   
* <b>Scaling and Overflow Behavior:</b>    
* \par    
* The function is implemented using an internal 64-bit accumulator.    
* The accumulator has a 2.62 format and maintains full precision of the intermediate  
* multiplication results but provides only a single guard bit.    
* Thus, if the accumulator result overflows it wraps around rather than clip.    
* In order to avoid overflows completely the input signal must be scaled down by   
* log2(numTaps) bits. The reference signal should not be scaled down.    
* After all multiply-accumulates are performed, the 2.62 accumulator is shifted   
* and saturated to 1.31 format to yield the final result.    
* The output signal and error signal are in 1.31 format.    
*   
* \par   
* 	In this filter, filter coefficients are updated for each sample and the   
* updation of filter cofficients are saturted.   
*    
*/

void arm_lms_norm_q31(
  arm_lms_norm_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pRef,
  q31_t * pOut,
  q31_t * pErr,
  uint32_t blockSize)
{
  q31_t *pState = S->pState;                     /* State pointer */
  q31_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */
  q31_t *pStateCurnt;                            /* Points to the current sample of the state */
  q31_t *px, *pb;                                /* Temporary pointers for state and coefficient buffers */
  q31_t mu = S->mu;                              /* Adaptive factor */
  uint32_t numTaps = S->numTaps;                 /* Number of filter coefficients in the filter */
  uint32_t tapCnt, blkCnt;                       /* Loop counters */
  q63_t energy;                                  /* Energy of the input */
  q63_t acc;                                     /* Accumulator */
  q31_t e = 0, d = 0;                            /* error, reference data sample */
  q31_t w = 0, in;                               /* weight factor and state */
  q31_t x0;                                      /* temporary variable to hold input sample */
  uint32_t shift = 32u - ((uint32_t) S->postShift + 1u);        /* Shift to be applied to the output */
  q31_t errorXmu, oneByEnergy;                   /* Temporary variables to store error and mu product and reciprocal of energy */
  q31_t postShift;                               /* Post shift to be applied to weight after reciprocal calculation */
  q31_t coef;                                    /* Temporary variable for coef */

  energy = S->energy;
  x0 = S->x0;

  /* S->pState points to buffer which contains previous frame (numTaps - 1) samples */
  /* pStateCurnt points to the location where the new input data should be written */
  pStateCurnt = &(S->pState[(numTaps - 1u)]);

  /* Loop over blockSize number of values */
  blkCnt = blockSize;


#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  while(blkCnt > 0u)
  {

    /* Copy the new input sample into the state buffer */
    *pStateCurnt++ = *pSrc;

    /* Initialize pState pointer */
    px = pState;

    /* Initialize coeff pointer */
    pb = (pCoeffs);

    /* Read the sample from input buffer */
    in = *pSrc++;

    /* Update the energy calculation */
    energy = (q31_t) ((((q63_t) energy << 32) -
                       (((q63_t) x0 * x0) << 1)) >> 32);
    energy = (q31_t) (((((q63_t) in * in) << 1) + (energy << 32)) >> 32);

    /* Set the accumulator to zero */
    acc = 0;

    /* Loop unrolling.  Process 4 taps at a time. */
    tapCnt = numTaps >> 2;

    while(tapCnt > 0u)
    {
      /* Perform the multiply-accumulate */
      acc += ((q63_t) (*px++)) * (*pb++);
      acc += ((q63_t) (*px++)) * (*pb++);
      acc += ((q63_t) (*px++)) * (*pb++);
      acc += ((q63_t) (*px++)) * (*pb++);

      /* Decrement the loop counter */
      tapCnt--;
    }

    /* If the filter length is not a multiple of 4, compute the remaining filter taps */
    tapCnt = numTaps % 0x4u;

    while(tapCnt > 0u)
    {
      /* Perform the multiply-accumulate */
      acc += ((q63_t) (*px++)) * (*pb++);

      /* Decrement the loop counter */
      tapCnt--;
    }

    /* Converting the result to 1.31 format */
    acc = (q31_t) (acc >> shift);

    /* Store the result from accumulator into the destination buffer. */
    *pOut++ = (q31_t) acc;

    /* Compute and store error */
    d = *pRef++;
    e = d - (q31_t) acc;
    *pErr++ = e;

    /* Calculates the reciprocal of energy */
    postShift = arm_recip_q31(energy + DELTA_Q31,
                              &oneByEnergy, &S->recipTable[0]);

    /* Calculation of product of (e * mu) */
    errorXmu = (q31_t) (((q63_t) e * mu) >> 31);

    /* Weighting factor for the normalized version */
    w = clip_q63_to_q31(((q63_t) errorXmu * oneByEnergy) >> (31 - postShift));

    /* Initialize pState pointer */
    px = pState;

    /* Initialize coeff pointer */
    pb = (pCoeffs);

    /* Loop unrolling.  Process 4 taps at a time. */
    tapCnt = numTaps >> 2;

    /* Update filter coefficients */
    while(tapCnt > 0u)
    {
      /* Perform the multiply-accumulate */

      /* coef is in 2.30 format */
      coef = (q31_t) (((q63_t) w * (*px++)) >> (32));
      /* get coef in 1.31 format by left shifting */
      *pb = clip_q63_to_q31((q63_t) * pb + (coef << 1u));
      /* update coefficient buffer to next coefficient */
      pb++;

      coef = (q31_t) (((q63_t) w * (*px++)) >> (32));
      *pb = clip_q63_to_q31((q63_t) * pb + (coef << 1u));
      pb++;

      coef = (q31_t) (((q63_t) w * (*px++)) >> (32));
      *pb = clip_q63_to_q31((q63_t) * pb + (coef << 1u));
      pb++;

      coef = (q31_t) (((q63_t) w * (*px++)) >> (32));
      *pb = clip_q63_to_q31((q63_t) * pb + (coef << 1u));
      pb++;

      /* Decrement the loop counter */
      tapCnt--;
    }

    /* If the filter length is not a multiple of 4, compute the remaining filter taps */
    tapCnt = numTaps % 0x4u;

    while(tapCnt > 0u)
    {
      /* Perform the multiply-accumulate */
      coef = (q31_t) (((q63_t) w * (*px++)) >> (32));
      *pb = clip_q63_to_q31((q63_t) * pb + (coef << 1u));
      pb++;

      /* Decrement the loop counter */
      tapCnt--;
    }

    /* Read the sample from state buffer */
    x0 = *pState;

    /* Advance state pointer by 1 for the next sample */
    pState = pState + 1;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Save energy and x0 values for the next frame */
  S->energy = (q31_t) energy;
  S->x0 = x0;

  /* Processing is complete. Now copy the last numTaps - 1 samples to the   
     satrt of the state buffer. This prepares the state buffer for the   
     next function call. */

  /* Points to the start of the pState buffer */
  pStateCurnt = S->pState;

  /* Loop unrolling for (numTaps - 1u) samples copy */
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

  while(blkCnt > 0u)
  {

    /* Copy the new input sample into the state buffer */
    *pStateCurnt++ = *pSrc;

    /* Initialize pState pointer */
    px = pState;

    /* Initialize pCoeffs pointer */
    pb = pCoeffs;

    /* Read the sample from input buffer */
    in = *pSrc++;

    /* Update the energy calculation */
    energy =
      (q31_t) ((((q63_t) energy << 32) - (((q63_t) x0 * x0) << 1)) >> 32);
    energy = (q31_t) (((((q63_t) in * in) << 1) + (energy << 32)) >> 32);

    /* Set the accumulator to zero */
    acc = 0;

    /* Loop over numTaps number of values */
    tapCnt = numTaps;

    while(tapCnt > 0u)
    {
      /* Perform the multiply-accumulate */
      acc += ((q63_t) (*px++)) * (*pb++);

      /* Decrement the loop counter */
      tapCnt--;
    }

    /* Converting the result to 1.31 format */
    acc = (q31_t) (acc >> shift);

    /* Store the result from accumulator into the destination buffer. */
    *pOut++ = (q31_t) acc;

    /* Compute and store error */
    d = *pRef++;
    e = d - (q31_t) acc;
    *pErr++ = e;

    /* Calculates the reciprocal of energy */
    postShift =
      arm_recip_q31(energy + DELTA_Q31, &oneByEnergy, &S->recipTable[0]);

    /* Calculation of product of (e * mu) */
    errorXmu = (q31_t) (((q63_t) e * mu) >> 31);

    /* Weighting factor for the normalized version */
    w = clip_q63_to_q31(((q63_t) errorXmu * oneByEnergy) >> (31 - postShift));

    /* Initialize pState pointer */
    px = pState;

    /* Initialize coeff pointer */
    pb = (pCoeffs);

    /* Loop over numTaps number of values */
    tapCnt = numTaps;

    while(tapCnt > 0u)
    {
      /* Perform the multiply-accumulate */
      /* coef is in 2.30 format */
      coef = (q31_t) (((q63_t) w * (*px++)) >> (32));
      /* get coef in 1.31 format by left shifting */
      *pb = clip_q63_to_q31((q63_t) * pb + (coef << 1u));
      /* update coefficient buffer to next coefficient */
      pb++;

      /* Decrement the loop counter */
      tapCnt--;
    }

    /* Read the sample from state buffer */
    x0 = *pState;

    /* Advance state pointer by 1 for the next sample */
    pState = pState + 1;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Save energy and x0 values for the next frame */
  S->energy = (q31_t) energy;
  S->x0 = x0;

  /* Processing is complete. Now copy the last numTaps - 1 samples to the    
     start of the state buffer. This prepares the state buffer for the       
     next function call. */

  /* Points to the start of the pState buffer */
  pStateCurnt = S->pState;

  /* Loop for (numTaps - 1u) samples copy */
  tapCnt = (numTaps - 1u);

  /* Copy the remaining q31_t data */
  while(tapCnt > 0u)
  {
    *pStateCurnt++ = *pState++;

    /* Decrement the loop counter */
    tapCnt--;
  }

#endif /*   #ifndef ARM_MATH_CM0 */

}

/**   
 * @} end of LMS_NORM group   
 */
