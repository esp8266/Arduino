/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_biquad_cascade_df1_q31.c   
*   
* Description:	Processing function for the   
*				Q31 Biquad cascade filter   
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
 * @addtogroup BiquadCascadeDF1   
 * @{   
 */

/**   
 * @brief Processing function for the Q31 Biquad cascade filter.   
 * @param[in]  *S         points to an instance of the Q31 Biquad cascade structure.   
 * @param[in]  *pSrc      points to the block of input data.   
 * @param[out] *pDst      points to the block of output data.   
 * @param[in]  blockSize  number of samples to process per call.   
 * @return none.   
 *   
 * <b>Scaling and Overflow Behavior:</b>   
 * \par   
 * The function is implemented using an internal 64-bit accumulator.   
 * The accumulator has a 2.62 format and maintains full precision of the intermediate multiplication results but provides only a single guard bit.   
 * Thus, if the accumulator result overflows it wraps around rather than clip.   
 * In order to avoid overflows completely the input signal must be scaled down by 2 bits and lie in the range [-0.25 +0.25).   
 * After all 5 multiply-accumulates are performed, the 2.62 accumulator is shifted by <code>postShift</code> bits and the result truncated to   
 * 1.31 format by discarding the low 32 bits.   
 *   
 * \par   
 * Refer to the function <code>arm_biquad_cascade_df1_fast_q31()</code> for a faster but less precise implementation of this filter for Cortex-M3 and Cortex-M4.   
 */

void arm_biquad_cascade_df1_q31(
  const arm_biquad_casd_df1_inst_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize)
{
  q31_t *pIn = pSrc;                             /*  input pointer initialization  */
  q31_t *pOut = pDst;                            /*  output pointer initialization */
  q31_t *pState = S->pState;                     /*  pState pointer initialization */
  q31_t *pCoeffs = S->pCoeffs;                   /*  coeff pointer initialization  */
  q63_t acc;                                     /*  accumulator                   */
  q31_t Xn1, Xn2, Yn1, Yn2;                      /*  Filter state variables        */
  q31_t b0, b1, b2, a1, a2;                      /*  Filter coefficients           */
  q31_t Xn;                                      /*  temporary input               */
  uint32_t shift = 32u - ((uint32_t) S->postShift + 1u);        /*  Shift to be applied to the output */
  uint32_t sample, stage = S->numStages;         /*  loop counters                     */


#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  do
  {
    /* Reading the coefficients */
    b0 = *pCoeffs++;
    b1 = *pCoeffs++;
    b2 = *pCoeffs++;
    a1 = *pCoeffs++;
    a2 = *pCoeffs++;

    /* Reading the state values */
    Xn1 = pState[0];
    Xn2 = pState[1];
    Yn1 = pState[2];
    Yn2 = pState[3];

    /* Apply loop unrolling and compute 4 output values simultaneously. */
    /*      The variable acc hold output values that are being computed:   
     *   
     *    acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]   
     */

    sample = blockSize >> 2u;

    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
     ** a second loop below computes the remaining 1 to 3 samples. */
    while(sample > 0u)
    {
      /* Read the input */
      Xn = *pIn++;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */

      /* acc =  b0 * x[n] */
      acc = (q63_t) b0 *Xn;
      /* acc +=  b1 * x[n-1] */
      acc += (q63_t) b1 *Xn1;
      /* acc +=  b[2] * x[n-2] */
      acc += (q63_t) b2 *Xn2;
      /* acc +=  a1 * y[n-1] */
      acc += (q63_t) a1 *Yn1;
      /* acc +=  a2 * y[n-2] */
      acc += (q63_t) a2 *Yn2;

      /* The result is converted to 1.31 , Yn2 variable is reused */
      Yn2 = (q31_t) (acc >> shift);

      /* Store the output in the destination buffer. */
      *pOut++ = Yn2;

      /* Read the second input */
      Xn2 = *pIn++;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */

      /* acc =  b0 * x[n] */
      acc = (q63_t) b0 *Xn2;
      /* acc +=  b1 * x[n-1] */
      acc += (q63_t) b1 *Xn;
      /* acc +=  b[2] * x[n-2] */
      acc += (q63_t) b2 *Xn1;
      /* acc +=  a1 * y[n-1] */
      acc += (q63_t) a1 *Yn2;
      /* acc +=  a2 * y[n-2] */
      acc += (q63_t) a2 *Yn1;


      /* The result is converted to 1.31, Yn1 variable is reused  */
      Yn1 = (q31_t) (acc >> shift);

      /* Store the output in the destination buffer. */
      *pOut++ = Yn1;

      /* Read the third input  */
      Xn1 = *pIn++;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */

      /* acc =  b0 * x[n] */
      acc = (q63_t) b0 *Xn1;
      /* acc +=  b1 * x[n-1] */
      acc += (q63_t) b1 *Xn2;
      /* acc +=  b[2] * x[n-2] */
      acc += (q63_t) b2 *Xn;
      /* acc +=  a1 * y[n-1] */
      acc += (q63_t) a1 *Yn1;
      /* acc +=  a2 * y[n-2] */
      acc += (q63_t) a2 *Yn2;

      /* The result is converted to 1.31, Yn2 variable is reused  */
      Yn2 = (q31_t) (acc >> shift);

      /* Store the output in the destination buffer. */
      *pOut++ = Yn2;

      /* Read the forth input */
      Xn = *pIn++;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */

      /* acc =  b0 * x[n] */
      acc = (q63_t) b0 *Xn;
      /* acc +=  b1 * x[n-1] */
      acc += (q63_t) b1 *Xn1;
      /* acc +=  b[2] * x[n-2] */
      acc += (q63_t) b2 *Xn2;
      /* acc +=  a1 * y[n-1] */
      acc += (q63_t) a1 *Yn2;
      /* acc +=  a2 * y[n-2] */
      acc += (q63_t) a2 *Yn1;

      /* The result is converted to 1.31, Yn1 variable is reused  */
      Yn1 = (q31_t) (acc >> shift);

      /* Every time after the output is computed state should be updated. */
      /* The states should be updated as:  */
      /* Xn2 = Xn1    */
      /* Xn1 = Xn     */
      /* Yn2 = Yn1    */
      /* Yn1 = acc    */
      Xn2 = Xn1;
      Xn1 = Xn;

      /* Store the output in the destination buffer. */
      *pOut++ = Yn1;

      /* decrement the loop counter */
      sample--;
    }

    /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
     ** No loop unrolling is used. */
    sample = (blockSize & 0x3u);

    while(sample > 0u)
    {
      /* Read the input */
      Xn = *pIn++;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */

      /* acc =  b0 * x[n] */
      acc = (q63_t) b0 *Xn;
      /* acc +=  b1 * x[n-1] */
      acc += (q63_t) b1 *Xn1;
      /* acc +=  b[2] * x[n-2] */
      acc += (q63_t) b2 *Xn2;
      /* acc +=  a1 * y[n-1] */
      acc += (q63_t) a1 *Yn1;
      /* acc +=  a2 * y[n-2] */
      acc += (q63_t) a2 *Yn2;

      /* The result is converted to 1.31  */
      acc = acc >> shift;

      /* Every time after the output is computed state should be updated. */
      /* The states should be updated as:  */
      /* Xn2 = Xn1    */
      /* Xn1 = Xn     */
      /* Yn2 = Yn1    */
      /* Yn1 = acc    */
      Xn2 = Xn1;
      Xn1 = Xn;
      Yn2 = Yn1;
      Yn1 = (q31_t) acc;

      /* Store the output in the destination buffer. */
      *pOut++ = (q31_t) acc;

      /* decrement the loop counter */
      sample--;
    }

    /*  The first stage goes from the input buffer to the output buffer. */
    /*  Subsequent stages occur in-place in the output buffer */
    pIn = pDst;

    /* Reset to destination pointer */
    pOut = pDst;

    /*  Store the updated state variables back into the pState array */
    *pState++ = Xn1;
    *pState++ = Xn2;
    *pState++ = Yn1;
    *pState++ = Yn2;

  } while(--stage);

#else

  /* Run the below code for Cortex-M0 */

  do
  {
    /* Reading the coefficients */
    b0 = *pCoeffs++;
    b1 = *pCoeffs++;
    b2 = *pCoeffs++;
    a1 = *pCoeffs++;
    a2 = *pCoeffs++;

    /* Reading the state values */
    Xn1 = pState[0];
    Xn2 = pState[1];
    Yn1 = pState[2];
    Yn2 = pState[3];

    /*      The variables acc holds the output value that is computed:        
     *    acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]        
     */

    sample = blockSize;

    while(sample > 0u)
    {
      /* Read the input */
      Xn = *pIn++;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      /* acc =  b0 * x[n] */
      acc = (q63_t) b0 *Xn;

      /* acc +=  b1 * x[n-1] */
      acc += (q63_t) b1 *Xn1;
      /* acc +=  b[2] * x[n-2] */
      acc += (q63_t) b2 *Xn2;
      /* acc +=  a1 * y[n-1] */
      acc += (q63_t) a1 *Yn1;
      /* acc +=  a2 * y[n-2] */
      acc += (q63_t) a2 *Yn2;

      /* The result is converted to 1.31  */
      acc = acc >> shift;

      /* Every time after the output is computed state should be updated. */
      /* The states should be updated as:  */
      /* Xn2 = Xn1    */
      /* Xn1 = Xn     */
      /* Yn2 = Yn1    */
      /* Yn1 = acc    */
      Xn2 = Xn1;
      Xn1 = Xn;
      Yn2 = Yn1;
      Yn1 = (q31_t) acc;

      /* Store the output in the destination buffer. */
      *pOut++ = (q31_t) acc;

      /* decrement the loop counter */
      sample--;
    }

    /*  The first stage goes from the input buffer to the output buffer. */
    /*  Subsequent stages occur in-place in the output buffer */
    pIn = pDst;

    /* Reset to destination pointer */
    pOut = pDst;

    /*  Store the updated state variables back into the pState array */
    *pState++ = Xn1;
    *pState++ = Xn2;
    *pState++ = Yn1;
    *pState++ = Yn2;

  } while(--stage);

#endif /*  #ifndef ARM_MATH_CM0 */
}

/**   
  * @} end of BiquadCascadeDF1 group   
  */
