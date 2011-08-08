/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_iir_lattice_f32.c   
*   
* Description:	Floating-point IIR Lattice filter processing function.   
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
 * @defgroup IIR_Lattice Infinite Impulse Response (IIR) Lattice Filters   
 *   
 * This set of functions implements lattice filters   
 * for Q15, Q31 and floating-point data types.  Lattice filters are used in a    
 * variety of adaptive filter applications.  The filter structure has feedforward and   
 * feedback components and the net impulse response is infinite length.   
 * The functions operate on blocks   
 * of input and output data and each call to the function processes   
 * <code>blockSize</code> samples through the filter.  <code>pSrc</code> and   
 * <code>pDst</code> point to input and output arrays containing <code>blockSize</code> values.   
   
 * \par Algorithm:   
 * \image html IIRLattice.gif "Infinite Impulse Response Lattice filter"   
 * <pre>   
 *    fN(n)   =  x(n)   
 *    fm-1(n) = fm(n) - km * gm-1(n-1)   for m = N, N-1, ...1   
 *    gm(n)   = km * fm-1(n) + gm-1(n-1) for m = N, N-1, ...1   
 *    y(n)    = vN * gN(n) + vN-1 * gN-1(n) + ...+ v0 * g0(n)   
 * </pre>   
 * \par   
 * <code>pkCoeffs</code> points to array of reflection coefficients of size <code>numStages</code>.    
 * Reflection coefficients are stored in time-reversed order.   
 * \par   
 * <pre>   
 *    {kN, kN-1, ....k1}   
 * </pre>   
 * <code>pvCoeffs</code> points to the array of ladder coefficients of size <code>(numStages+1)</code>.    
 * Ladder coefficients are stored in time-reversed order.   
 * \par   
 * <pre>   
 *    {vN, vN-1, ...v0}   
 * </pre>   
 * <code>pState</code> points to a state array of size <code>numStages + blockSize</code>.   
 * The state variables shown in the figure above (the g values) are stored in the <code>pState</code> array.   
 * The state variables are updated after each block of data is processed; the coefficients are untouched.   
 * \par Instance Structure   
 * The coefficients and state variables for a filter are stored together in an instance data structure.   
 * A separate instance structure must be defined for each filter.   
 * Coefficient arrays may be shared among several instances while state variable arrays cannot be shared.   
 * There are separate instance structure declarations for each of the 3 supported data types.   
  *   
 * \par Initialization Functions   
 * There is also an associated initialization function for each data type.   
 * The initialization function performs the following operations:   
 * - Sets the values of the internal structure fields.   
 * - Zeros out the values in the state buffer.   
 *   
 * \par   
 * Use of the initialization function is optional.   
 * However, if the initialization function is used, then the instance structure cannot be placed into a const data section.   
 * To place an instance structure into a const data section, the instance structure must be manually initialized.   
 * Set the values in the state buffer to zeros and then manually initialize the instance structure as follows:   
 * <pre>   
 *arm_iir_lattice_instance_f32 S = {numStages, pState, pkCoeffs, pvCoeffs};   
 *arm_iir_lattice_instance_q31 S = {numStages, pState, pkCoeffs, pvCoeffs};   
 *arm_iir_lattice_instance_q15 S = {numStages, pState, pkCoeffs, pvCoeffs};   
 * </pre>   
 * \par   
 * where <code>numStages</code> is the number of stages in the filter; <code>pState</code> points to the state buffer array;   
 * <code>pkCoeffs</code> points to array of the reflection coefficients; <code>pvCoeffs</code> points to the array of ladder coefficients.   
 * \par Fixed-Point Behavior   
 * Care must be taken when using the fixed-point versions of the IIR lattice filter functions.   
 * In particular, the overflow and saturation behavior of the accumulator used in each function must be considered.   
 * Refer to the function specific documentation below for usage guidelines.   
 */

/**   
 * @addtogroup IIR_Lattice   
 * @{   
 */

/**   
 * @brief Processing function for the floating-point IIR lattice filter.   
 * @param[in] *S points to an instance of the floating-point IIR lattice structure.   
 * @param[in] *pSrc points to the block of input data.   
 * @param[out] *pDst points to the block of output data.   
 * @param[in] blockSize number of samples to process.   
 * @return none.   
 */

void arm_iir_lattice_f32(
  const arm_iir_lattice_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize)
{
  float32_t fcurr, fnext = 0, gcurr, gnext;      /* Temporary variables for lattice stages */
  float32_t acc;                                 /* Accumlator */
  uint32_t blkCnt, tapCnt;                       /* temporary variables for counts */
  float32_t *px1, *px2, *pk, *pv;                /* temporary pointers for state and coef */
  uint32_t numStages = S->numStages;             /* number of stages */
  float32_t *pState;                             /* State pointer */
  float32_t *pStateCurnt;                        /* State current pointer */


#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  gcurr = 0.0f;
  blkCnt = blockSize;

  pState = &S->pState[0];

  /* Sample processing */
  while(blkCnt > 0u)
  {
    /* Read Sample from input buffer */
    /* fN(n) = x(n) */
    fcurr = *pSrc++;

    /* Initialize state read pointer */
    px1 = pState;
    /* Initialize state write pointer */
    px2 = pState;
    /* Set accumulator to zero */
    acc = 0.0f;
    /* Initialize Ladder coeff pointer */
    pv = &S->pvCoeffs[0];
    /* Initialize Reflection coeff pointer */
    pk = &S->pkCoeffs[0];


    /* Process sample for first tap */
    gcurr = *px1++;
    /* fN-1(n) = fN(n) - kN * gN-1(n-1) */
    fnext = fcurr - ((*pk) * gcurr);
    /* gN(n) = kN * fN-1(n) + gN-1(n-1) */
    gnext = (fnext * (*pk++)) + gcurr;
    /* write gN(n) into state for next sample processing */
    *px2++ = gnext;
    /* y(n) += gN(n) * vN  */
    acc += (gnext * (*pv++));

    /* Update f values for next coefficient processing */
    fcurr = fnext;

    /* Loop unrolling.  Process 4 taps at a time. */
    tapCnt = (numStages - 1u) >> 2;

    while(tapCnt > 0u)
    {
      /* Process sample for 2nd, 6th ...taps */
      /* Read gN-2(n-1) from state buffer */
      gcurr = *px1++;
      /* Process sample for 2nd, 6th .. taps */
      /* fN-2(n) = fN-1(n) - kN-1 * gN-2(n-1) */
      fnext = fcurr - ((*pk) * gcurr);
      /* gN-1(n) = kN-1 * fN-2(n) + gN-2(n-1) */
      gnext = (fnext * (*pk++)) + gcurr;
      /* y(n) += gN-1(n) * vN-1  */
      /* process for gN-5(n) * vN-5, gN-9(n) * vN-9 ... */
      acc += (gnext * (*pv++));
      /* write gN-1(n) into state for next sample processing */
      *px2++ = gnext;


      /* Process sample for 3nd, 7th ...taps */
      /* Read gN-3(n-1) from state buffer */
      gcurr = *px1++;
      /* Process sample for 3rd, 7th .. taps */
      /* fN-3(n) = fN-2(n) - kN-2 * gN-3(n-1) */
      fcurr = fnext - ((*pk) * gcurr);
      /* gN-2(n) = kN-2 * fN-3(n) + gN-3(n-1) */
      gnext = (fcurr * (*pk++)) + gcurr;
      /* y(n) += gN-2(n) * vN-2  */
      /* process for gN-6(n) * vN-6, gN-10(n) * vN-10 ... */
      acc += (gnext * (*pv++));
      /* write gN-2(n) into state for next sample processing */
      *px2++ = gnext;


      /* Process sample for 4th, 8th ...taps */
      /* Read gN-4(n-1) from state buffer */
      gcurr = *px1++;
      /* Process sample for 4th, 8th .. taps */
      /* fN-4(n) = fN-3(n) - kN-3 * gN-4(n-1) */
      fnext = fcurr - ((*pk) * gcurr);
      /* gN-3(n) = kN-3 * fN-4(n) + gN-4(n-1) */
      gnext = (fnext * (*pk++)) + gcurr;
      /* y(n) += gN-3(n) * vN-3  */
      /* process for gN-7(n) * vN-7, gN-11(n) * vN-11 ... */
      acc += (gnext * (*pv++));
      /* write gN-3(n) into state for next sample processing */
      *px2++ = gnext;


      /* Process sample for 5th, 9th ...taps */
      /* Read gN-5(n-1) from state buffer */
      gcurr = *px1++;
      /* Process sample for 5th, 9th .. taps */
      /* fN-5(n) = fN-4(n) - kN-4 * gN-1(n-1) */
      fcurr = fnext - ((*pk) * gcurr);
      /* gN-4(n) = kN-4 * fN-5(n) + gN-5(n-1) */
      gnext = (fcurr * (*pk++)) + gcurr;
      /* y(n) += gN-4(n) * vN-4  */
      /* process for gN-8(n) * vN-8, gN-12(n) * vN-12 ... */
      acc += (gnext * (*pv++));
      /* write gN-4(n) into state for next sample processing */
      *px2++ = gnext;

      tapCnt--;

    }

    fnext = fcurr;

    /* If the filter length is not a multiple of 4, compute the remaining filter taps */
    tapCnt = (numStages - 1u) % 0x4u;

    while(tapCnt > 0u)
    {
      gcurr = *px1++;
      /* Process sample for last taps */
      fnext = fcurr - ((*pk) * gcurr);
      gnext = (fnext * (*pk++)) + gcurr;
      /* Output samples for last taps */
      acc += (gnext * (*pv++));
      *px2++ = gnext;
      fcurr = fnext;

      tapCnt--;

    }


    /* y(n) += g0(n) * v0 */
    acc += (fnext * (*pv));

    *px2++ = fnext;

    /* write out into pDst */
    *pDst++ = acc;

    /* Advance the state pointer by 4 to process the next group of 4 samples */
    pState = pState + 1u;
    blkCnt--;

  }

  /* Processing is complete. Now copy last S->numStages samples to start of the buffer   
     for the preperation of next frame process */

  /* Points to the start of the state buffer */
  pStateCurnt = &S->pState[0];
  pState = &S->pState[blockSize];

  tapCnt = numStages >> 2u;

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
  tapCnt = (numStages) % 0x4u;

  /* Copy the remaining q31_t data */
  while(tapCnt > 0u)
  {
    *pStateCurnt++ = *pState++;

    /* Decrement the loop counter */
    tapCnt--;
  }

#else

  /* Run the below code for Cortex-M0 */

  blkCnt = blockSize;

  pState = &S->pState[0];

  /* Sample processing */
  while(blkCnt > 0u)
  {
    /* Read Sample from input buffer */
    /* fN(n) = x(n) */
    fcurr = *pSrc++;

    /* Initialize state read pointer */
    px1 = pState;
    /* Initialize state write pointer */
    px2 = pState;
    /* Set accumulator to zero */
    acc = 0.0f;
    /* Initialize Ladder coeff pointer */
    pv = &S->pvCoeffs[0];
    /* Initialize Reflection coeff pointer */
    pk = &S->pkCoeffs[0];


    /* Process sample for numStages */
    tapCnt = numStages;

    while(tapCnt > 0u)
    {
      gcurr = *px1++;
      /* Process sample for last taps */
      fnext = fcurr - ((*pk) * gcurr);
      gnext = (fnext * (*pk++)) + gcurr;

      /* Output samples for last taps */
      acc += (gnext * (*pv++));
      *px2++ = gnext;
      fcurr = fnext;

      /* Decrementing loop counter */
      tapCnt--;

    }

    /* y(n) += g0(n) * v0 */
    acc += (fnext * (*pv));

    *px2++ = fnext;

    /* write out into pDst */
    *pDst++ = acc;

    /* Advance the state pointer by 1 to process the next group of samples */
    pState = pState + 1u;
    blkCnt--;

  }

  /* Processing is complete. Now copy last S->numStages samples to start of the buffer          
     for the preperation of next frame process */

  /* Points to the start of the state buffer */
  pStateCurnt = &S->pState[0];
  pState = &S->pState[blockSize];

  tapCnt = numStages;

  /* Copy the data */
  while(tapCnt > 0u)
  {
    *pStateCurnt++ = *pState++;

    /* Decrement the loop counter */
    tapCnt--;
  }

#endif /*   #ifndef ARM_MATH_CM0 */

}




/**   
 * @} end of IIR_Lattice group   
 */
