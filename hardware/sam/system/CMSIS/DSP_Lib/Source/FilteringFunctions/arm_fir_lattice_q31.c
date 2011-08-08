/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_fir_lattice_q31.c   
*   
* Description:	Q31 FIR lattice filter processing function.   
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
 * @addtogroup FIR_Lattice   
 * @{   
 */


/**   
 * @brief Processing function for the Q31 FIR lattice filter.   
 * @param[in]  *S        points to an instance of the Q31 FIR lattice structure.   
 * @param[in]  *pSrc     points to the block of input data.   
 * @param[out] *pDst     points to the block of output data   
 * @param[in]  blockSize number of samples to process.   
 * @return none.   
 *   
 * @details   
 * <b>Scaling and Overflow Behavior:</b>   
 * In order to avoid overflows the input signal must be scaled down by 2*log2(numStages) bits.   
 */

void arm_fir_lattice_q31(
  const arm_fir_lattice_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize)
{
  q31_t *pState;                                 /* State pointer */
  q31_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */
  q31_t *px;                                     /* temporary state pointer */
  q31_t *pk;                                     /* temporary coefficient pointer */


#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  q31_t fcurr1, fnext1, gcurr1 = 0, gnext1;      /* temporary variables for first sample in loop unrolling */
  q63_t fcurr2, fnext2, gnext2;                  /* temporary variables for second sample in loop unrolling */
  q63_t fcurr3, fnext3, gnext3;                  /* temporary variables for third sample in loop unrolling */
  q63_t fcurr4, fnext4, gnext4;                  /* temporary variables for fourth sample in loop unrolling */
  uint32_t numStages = S->numStages;             /* Length of the filter */
  uint32_t blkCnt, stageCnt;                     /* temporary variables for counts */

  pState = &S->pState[0];

  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {

    /* Read two samples from input buffer */
    /* f0(n) = x(n) */
    fcurr1 = *pSrc++;
    /* f0(n) = x(n) */
    fcurr2 = *pSrc++;

    /* Initialize coeff pointer */
    pk = (pCoeffs);

    /* Initialize state pointer */
    px = pState;

    /* Read g0(n-1) from state */
    gcurr1 = *px;

    /* Process first sample for first tap */
    /* f1(n) = f0(n) +  K1 * g0(n-1) */
    fnext1 = (q31_t) (((q63_t) gcurr1 * (*pk)) >> 31) + fcurr1;
    /* g1(n) = f0(n) * K1  +  g0(n-1) */
    gnext1 = (q31_t) (((q63_t) fcurr1 * (*pk)) >> 31) + gcurr1;

    /* Process second sample for first tap */
    /* for sample 2 processing */
    fnext2 = (q31_t) (((q63_t) fcurr1 * (*pk)) >> 31) + fcurr2;
    gnext2 = (q31_t) (((q63_t) fcurr2 * (*pk)) >> 31) + fcurr1;


    /* Read next two samples from input buffer */
    /* f0(n+2) = x(n+2) */
    fcurr3 = *pSrc++;
    fcurr4 = *pSrc++;

    /* Copy only last input samples into the state buffer   
       which will be used for next four samples processing */
    *px++ = (q31_t) fcurr4;

    /* Process third sample for first tap */
    fnext3 = (q31_t) (((q63_t) fcurr2 * (*pk)) >> 31) + fcurr3;
    gnext3 = (q31_t) (((q63_t) fcurr3 * (*pk)) >> 31) + fcurr2;

    /* Process fourth sample for first tap */
    fnext4 = (q31_t) (((q63_t) fcurr3 * (*pk)) >> 31) + fcurr4;
    gnext4 = (q31_t) (((q63_t) fcurr4 * (*pk++)) >> 31) + fcurr3;

    /* save g1(n) in state buffer for next sample processing */
    /* *px++ = gnext4;       */

    /* Update of f values for next coefficient set processing */
    fcurr1 = fnext1;
    fcurr2 = fnext2;
    fcurr3 = fnext3;
    fcurr4 = fnext4;


    /* Loop unrolling.  Process 4 taps at a time . */
    stageCnt = (numStages - 1u) >> 2u;


    /* Loop over the number of taps.  Unroll by a factor of 4.   
     ** Repeat until we've computed numStages-3 coefficients. */

    /* Process 2nd, 3rd, 4th and 5th taps ... here */
    while(stageCnt > 0u)
    {
      /* Read g1(n-1), g3(n-1) .... from state */
      gcurr1 = *px;

      /* save g1(n) in state buffer */
      *px++ = (q31_t) gnext4;

      /* Process first sample for 2nd, 6th .. tap */
      /* Sample processing for K2, K6.... */
      /* f2(n) = f1(n) +  K2 * g1(n-1) */
      fnext1 = (q31_t) (((q63_t) gcurr1 * (*pk)) >> 31) + fcurr1;
      /* Process second sample for 2nd, 6th .. tap */
      /* for sample 2 processing */
      fnext2 = (q31_t) (((q63_t) gnext1 * (*pk)) >> 31) + fcurr2;
      /* Process third sample for 2nd, 6th .. tap */
      fnext3 = (q31_t) (((q63_t) gnext2 * (*pk)) >> 31) + fcurr3;
      /* Process fourth sample for 2nd, 6th .. tap */
      fnext4 = (q31_t) (((q63_t) gnext3 * (*pk)) >> 31) + fcurr4;

      /* g2(n) = f1(n) * K2  +  g1(n-1) */
      /* Calculation of state values for next stage */
      gnext4 = (q31_t) (((q63_t) fcurr4 * (*pk)) >> 31) + gnext3;
      gnext3 = (q31_t) (((q63_t) fcurr3 * (*pk)) >> 31) + gnext2;
      gnext2 = (q31_t) (((q63_t) fcurr2 * (*pk)) >> 31) + gnext1;
      gnext1 = (q31_t) (((q63_t) fcurr1 * (*pk++)) >> 31) + gcurr1;


      /* Read g2(n-1), g4(n-1) .... from state */
      gcurr1 = *px;

      /* save g2(n) in state buffer */
      *px++ = (q31_t) gnext4;

      /* Sample processing for K3, K7.... */
      /* Process first sample for 3rd, 7th .. tap */
      /* f3(n) = f2(n) +  K3 * g2(n-1) */
      fcurr1 = (q31_t) (((q63_t) gcurr1 * (*pk)) >> 31) + fnext1;
      /* Process second sample for 3rd, 7th .. tap */
      fcurr2 = (q31_t) (((q63_t) gnext1 * (*pk)) >> 31) + fnext2;
      /* Process third sample for 3rd, 7th .. tap */
      fcurr3 = (q31_t) (((q63_t) gnext2 * (*pk)) >> 31) + fnext3;
      /* Process fourth sample for 3rd, 7th .. tap */
      fcurr4 = (q31_t) (((q63_t) gnext3 * (*pk)) >> 31) + fnext4;

      /* Calculation of state values for next stage */
      /*  gnext4 = fnext4 * (*pk) +  gnext3; */
      gnext4 = (q31_t) (((q63_t) fnext4 * (*pk)) >> 31) + gnext3;
      gnext3 = (q31_t) (((q63_t) fnext3 * (*pk)) >> 31) + gnext2;
      /*  gnext2 = fnext2 * (*pk) +  gnext1; */
      gnext2 = (q31_t) (((q63_t) fnext2 * (*pk)) >> 31) + gnext1;

      /* g1(n) = f0(n) * K1  +  g0(n-1) */
      /*  gnext1 = fnext1 * (*pk++) +  gcurr1; */
      gnext1 = (q31_t) (((q63_t) fnext1 * (*pk++)) >> 31) + gcurr1;

      /* Read g1(n-1), g3(n-1) .... from state */
      gcurr1 = *px;

      /* save g1(n) in state buffer */
      *px++ = (q31_t) gnext4;

      /* Sample processing for K4, K8.... */
      /* Process first sample for 4th, 8th .. tap */
      /* f4(n) = f3(n) +  K4 * g3(n-1) */
      fnext1 = (q31_t) (((q63_t) gcurr1 * (*pk)) >> 31) + fcurr1;
      /* Process second sample for 4th, 8th .. tap */
      /* for sample 2 processing */
      fnext2 = (q31_t) (((q63_t) gnext1 * (*pk)) >> 31) + fcurr2;
      /* Process third sample for 4th, 8th .. tap */
      fnext3 = (q31_t) (((q63_t) gnext2 * (*pk)) >> 31) + fcurr3;
      /* Process fourth sample for 4th, 8th .. tap */
      fnext4 = (q31_t) (((q63_t) gnext3 * (*pk)) >> 31) + fcurr4;

      /* g4(n) = f3(n) * K4  +  g3(n-1) */
      /* Calculation of state values for next stage */
      gnext4 = (q31_t) (((q63_t) fcurr4 * (*pk)) >> 31) + gnext3;
      gnext3 = (q31_t) (((q63_t) fcurr3 * (*pk)) >> 31) + gnext2;
      gnext2 = (q31_t) (((q63_t) fcurr2 * (*pk)) >> 31) + gnext1;
      gnext1 = (q31_t) (((q63_t) fcurr1 * (*pk++)) >> 31) + gcurr1;

      /* Read g2(n-1), g4(n-1) .... from state */
      gcurr1 = *px;

      /* save g4(n) in state buffer */
      *px++ = (q31_t) gnext4;

      /* Sample processing for K5, K9.... */
      /* Process first sample for 5th, 9th .. tap */
      /* f5(n) = f4(n) +  K5 * g4(n-1) */
      fcurr1 = (q31_t) (((q63_t) gcurr1 * (*pk)) >> 31) + fnext1;
      /* Process second sample for 5th, 9th .. tap */
      fcurr2 = (q31_t) (((q63_t) gnext1 * (*pk)) >> 31) + fnext2;
      /* Process third sample for 5th, 9th .. tap */
      fcurr3 = (q31_t) (((q63_t) gnext2 * (*pk)) >> 31) + fnext3;
      /* Process fourth sample for 5th, 9th .. tap */
      fcurr4 = (q31_t) (((q63_t) gnext3 * (*pk)) >> 31) + fnext4;

      /* Calculation of state values for next stage */
      /* g5(n) = f4(n) * K5  +  g4(n-1) */
      gnext4 = (q31_t) (((q63_t) fnext4 * (*pk)) >> 31) + gnext3;
      gnext3 = (q31_t) (((q63_t) fnext3 * (*pk)) >> 31) + gnext2;
      gnext2 = (q31_t) (((q63_t) fnext2 * (*pk)) >> 31) + gnext1;
      gnext1 = (q31_t) (((q63_t) fnext1 * (*pk++)) >> 31) + gcurr1;

      stageCnt--;
    }

    /* If the (filter length -1) is not a multiple of 4, compute the remaining filter taps */
    stageCnt = (numStages - 1u) % 0x4u;

    while(stageCnt > 0u)
    {
      gcurr1 = *px;

      /* save g value in state buffer */
      *px++ = (q31_t) gnext4;

      /* Process four samples for last three taps here */
      fnext1 = (q31_t) (((q63_t) gcurr1 * (*pk)) >> 31) + fcurr1;
      fnext2 = (q31_t) (((q63_t) gnext1 * (*pk)) >> 31) + fcurr2;
      fnext3 = (q31_t) (((q63_t) gnext2 * (*pk)) >> 31) + fcurr3;
      fnext4 = (q31_t) (((q63_t) gnext3 * (*pk)) >> 31) + fcurr4;

      /* g1(n) = f0(n) * K1  +  g0(n-1) */
      gnext4 = (q31_t) (((q63_t) fcurr4 * (*pk)) >> 31) + gnext3;
      gnext3 = (q31_t) (((q63_t) fcurr3 * (*pk)) >> 31) + gnext2;
      gnext2 = (q31_t) (((q63_t) fcurr2 * (*pk)) >> 31) + gnext1;
      gnext1 = (q31_t) (((q63_t) fcurr1 * (*pk++)) >> 31) + gcurr1;

      /* Update of f values for next coefficient set processing */
      fcurr1 = fnext1;
      fcurr2 = fnext2;
      fcurr3 = fnext3;
      fcurr4 = fnext4;

      stageCnt--;

    }

    /* The results in the 4 accumulators, store in the destination buffer. */
    /* y(n) = fN(n) */
    *pDst++ = fcurr1;
    *pDst++ = (q31_t) fcurr2;
    *pDst++ = (q31_t) fcurr3;
    *pDst++ = (q31_t) fcurr4;

    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

  while(blkCnt > 0u)
  {
    /* f0(n) = x(n) */
    fcurr1 = *pSrc++;

    /* Initialize coeff pointer */
    pk = (pCoeffs);

    /* Initialize state pointer */
    px = pState;

    /* read g2(n) from state buffer */
    gcurr1 = *px;

    /* for sample 1 processing */
    /* f1(n) = f0(n) +  K1 * g0(n-1) */
    fnext1 = (q31_t) (((q63_t) gcurr1 * (*pk)) >> 31) + fcurr1;
    /* g1(n) = f0(n) * K1  +  g0(n-1) */
    gnext1 = (q31_t) (((q63_t) fcurr1 * (*pk++)) >> 31) + gcurr1;
    /* save g1(n) in state buffer */
    *px++ = fcurr1;

    /* f1(n) is saved in fcurr1   
       for next stage processing */
    fcurr1 = fnext1;

    stageCnt = (numStages - 1u);

    /* stage loop */
    while(stageCnt > 0u)
    {
      /* read g2(n) from state buffer */
      gcurr1 = *px;

      /* save g1(n) in state buffer */
      *px++ = gnext1;

      /* Sample processing for K2, K3.... */
      /* f2(n) = f1(n) +  K2 * g1(n-1) */
      fnext1 = (q31_t) (((q63_t) gcurr1 * (*pk)) >> 31) + fcurr1;
      /* g2(n) = f1(n) * K2  +  g1(n-1) */
      gnext1 = (q31_t) (((q63_t) fcurr1 * (*pk++)) >> 31) + gcurr1;

      /* f1(n) is saved in fcurr1   
         for next stage processing */
      fcurr1 = fnext1;

      stageCnt--;

    }

    /* y(n) = fN(n) */
    *pDst++ = fcurr1;

    blkCnt--;

  }

#else

/* Run the below code for Cortex-M0 */

  q31_t fcurr, fnext, gcurr, gnext;              /* temporary variables */
  uint32_t numStages = S->numStages;             /* Length of the filter */
  uint32_t blkCnt, stageCnt;                     /* temporary variables for counts */

  pState = &S->pState[0];

  blkCnt = blockSize;

  while(blkCnt > 0u)
  {
    /* f0(n) = x(n) */
    fcurr = *pSrc++;

    /* Initialize coeff pointer */
    pk = (pCoeffs);

    /* Initialize state pointer */
    px = pState;

    /* read g0(n-1) from state buffer */
    gcurr = *px;

    /* for sample 1 processing */
    /* f1(n) = f0(n) +  K1 * g0(n-1) */
    fnext = (q31_t) (((q63_t) gcurr * (*pk)) >> 31) + fcurr;
    /* g1(n) = f0(n) * K1  +  g0(n-1) */
    gnext = (q31_t) (((q63_t) fcurr * (*pk++)) >> 31) + gcurr;
    /* save g1(n) in state buffer */
    *px++ = fcurr;

    /* f1(n) is saved in fcurr1           
       for next stage processing */
    fcurr = fnext;

    stageCnt = (numStages - 1u);

    /* stage loop */
    while(stageCnt > 0u)
    {
      /* read g2(n) from state buffer */
      gcurr = *px;

      /* save g1(n) in state buffer */
      *px++ = gnext;

      /* Sample processing for K2, K3.... */
      /* f2(n) = f1(n) +  K2 * g1(n-1) */
      fnext = (q31_t) (((q63_t) gcurr * (*pk)) >> 31) + fcurr;
      /* g2(n) = f1(n) * K2  +  g1(n-1) */
      gnext = (q31_t) (((q63_t) fcurr * (*pk++)) >> 31) + gcurr;

      /* f1(n) is saved in fcurr1           
         for next stage processing */
      fcurr = fnext;

      stageCnt--;

    }

    /* y(n) = fN(n) */
    *pDst++ = fcurr;

    blkCnt--;

  }

#endif /*   #ifndef ARM_MATH_CM0 */

}

/**   
 * @} end of FIR_Lattice group   
 */
