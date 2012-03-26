/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_sqrt_q15.c   
*   
* Description:	Q15 square root function.  
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
* -------------------------------------------------------------------- */

#include "arm_math.h"
#include "arm_common_tables.h"


/**   
 * @ingroup groupFastMath   
 */

/**   
 * @addtogroup SQRT   
 * @{   
 */

  /**  
   * @brief  Q15 square root function.  
   * @param[in]   in     input value.  The range of the input value is [0 +1) or 0x0000 to 0x7FFF.  
   * @param[out]  *pOut  square root of input value.  
   * @return The function returns ARM_MATH_SUCCESS if input value is positive value or ARM_MATH_ARGUMENT_ERROR if  
   * <code>in</code> is negative value and returns zero output for negative values.  
   */

arm_status arm_sqrt_q15(
  q15_t in,
  q15_t * pOut)
{
  q31_t prevOut;
  q15_t oneByOut;
  uint32_t sign_bits;

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  q31_t out;

  if(in > 0)
  {
    /* run for ten iterations */

    /* Take initial guess as half of the input and first iteration */
    out = ((q31_t) in >> 1u) + 0x3FFF;

    /* Calculation of reciprocal of out */
    /* oneByOut contains reciprocal of out which is in 2.14 format   
       and oneByOut should be upscaled by signBits */
    sign_bits = arm_recip_q15((q15_t) out, &oneByOut, armRecipTableQ15);

    /* 0.5 * (out) */
    out = out >> 1u;
    /* prevOut = 0.5 * out + (in * (oneByOut << signBits))) */
    prevOut = out + (((q15_t) (((q31_t) in * oneByOut) >> 16)) << sign_bits);

    /* Third iteration */
    sign_bits = arm_recip_q15((q15_t) prevOut, &oneByOut, armRecipTableQ15);
    prevOut = prevOut >> 1u;
    out = prevOut + (((q15_t) (((q31_t) in * oneByOut) >> 16)) << sign_bits);

    sign_bits = arm_recip_q15((q15_t) out, &oneByOut, armRecipTableQ15);
    out = out >> 1u;
    prevOut = out + (((q15_t) (((q31_t) in * oneByOut) >> 16)) << sign_bits);

    /* Fifth iteration */
    sign_bits = arm_recip_q15((q15_t) prevOut, &oneByOut, armRecipTableQ15);
    prevOut = prevOut >> 1u;
    out = prevOut + (((q15_t) (((q31_t) in * oneByOut) >> 16)) << sign_bits);

    sign_bits = arm_recip_q15((q15_t) out, &oneByOut, armRecipTableQ15);
    out = out >> 1u;
    prevOut = out + (((q15_t) (((q31_t) in * oneByOut) >> 16)) << sign_bits);

    /* Seventh iteration */
    sign_bits = arm_recip_q15((q15_t) prevOut, &oneByOut, armRecipTableQ15);
    prevOut = prevOut >> 1u;
    out = prevOut + (((q15_t) (((q31_t) in * oneByOut) >> 16)) << sign_bits);

    sign_bits = arm_recip_q15((q15_t) out, &oneByOut, armRecipTableQ15);
    out = out >> 1u;
    prevOut = out + (((q15_t) (((q31_t) in * oneByOut) >> 16)) << sign_bits);

    sign_bits = arm_recip_q15((q15_t) prevOut, &oneByOut, armRecipTableQ15);
    prevOut = prevOut >> 1u;
    out = prevOut + (((q15_t) (((q31_t) in * oneByOut) >> 16)) << sign_bits);

    /* tenth iteration */
    sign_bits = arm_recip_q15((q15_t) out, &oneByOut, armRecipTableQ15);
    out = out >> 1u;
    *pOut = out + (((q15_t) (((q31_t) in * oneByOut) >> 16)) << sign_bits);

    return (ARM_MATH_SUCCESS);
  }

#else

  /* Run the below code for Cortex-M0 */

  q31_t out, loopVar;                            /* Temporary variable for output, loop variable */
  if(in > 0)
  {
    /* run for ten iterations */

    /* Take initial guess as half of the input and first iteration */
    out = ((q31_t) in >> 1u) + 0x3FFF;

    /* Calculation of reciprocal of out */

    /* oneByOut contains reciprocal of out which is in 2.14 format   
       and oneByOut should be upscaled by sign bits */
    sign_bits = arm_recip_q15((q15_t) out, &oneByOut, armRecipTableQ15);

    /* 0.5 * (out) */
    out = out >> 1u;
    /* prevOut = 0.5 * out + (in * oneByOut) << signbits))) */
    prevOut = out + (((q15_t) (((q31_t) in * oneByOut) >> 16)) << sign_bits);

    /* loop for third iteration to tenth iteration */

    for (loopVar = 1; loopVar <= 8; loopVar++)
    {

      sign_bits = arm_recip_q15((q15_t) prevOut, &oneByOut, armRecipTableQ15);
      /* 0.5 * (prevOut) */
      prevOut = prevOut >> 1u;
      /* prevOut = 0.5 * prevOut+ (in * oneByOut) << signbits))) */
      out =
        prevOut + (((q15_t) (((q31_t) in * oneByOut) >> 16)) << sign_bits);
      /* prevOut      = out */
      prevOut = out;

    }
    /* output is moved to pOut pointer */
    *pOut = prevOut;

    return (ARM_MATH_SUCCESS);
  }

#endif /* #ifndef ARM_MATH_CM0 */

  else
  {

    *pOut = 0;
    return (ARM_MATH_ARGUMENT_ERROR);
  }

}

/**   
 * @} end of SQRT group   
 */
