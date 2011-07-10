/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_sqrt_q31.c  
*  
* Description:	Q31 square root function. 
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
#include "arm_common_tables.h" 
 
/**  
 * @ingroup groupFastMath  
 */ 
 
/**  
 * @addtogroup SQRT  
 * @{  
 */ 
 
/** 
 * @brief Q31 square root function. 
 * @param[in]   in    input value.  The range of the input value is [0 +1) or 0x00000000 to 0x7FFFFFFF. 
 * @param[out]  *pOut square root of input value. 
 * @return The function returns ARM_MATH_SUCCESS if input value is positive value or ARM_MATH_ARGUMENT_ERROR if 
 * <code>in</code> is negative value and returns zero output for negative values. 
 */ 
 
arm_status arm_sqrt_q31( 
  q31_t in, 
  q31_t * pOut) 
{ 
  q63_t out; 
  q63_t prevOut; 
  q31_t oneByOut; 
  uint32_t signBits; 
 
 
  if(in > 0) 
  { 
 
    /* run for ten iterations */ 
 
    /* Take initial guess as half of the input and first iteration */ 
    out = (in >> 1) + 0x3FFFFFFF; 
 
    /* Calculation of reciprocal of out */ 
    /* oneByOut contains reciprocal of out which is in 2.30 format  
       and oneByOut should be upscaled by signBits */ 
    signBits = arm_recip_q31((q31_t) out, &oneByOut, armRecipTableQ31); 
 
    /* 0.5 * (out) */ 
    out = out >> 1u; 
 
    /* prevOut = 0.5 * out + (in * (oneByOut << signBits))) */ 
    prevOut = out + (((q31_t) (((q63_t) in * oneByOut) >> 32)) << signBits); 
 
    /* Third iteration */ 
    signBits = arm_recip_q31((q31_t) prevOut, &oneByOut, armRecipTableQ31); 
    prevOut = prevOut >> 1u; 
    out = prevOut + (((q31_t) (((q63_t) in * oneByOut) >> 32)) << signBits); 
 
    signBits = arm_recip_q31((q31_t) out, &oneByOut, armRecipTableQ31); 
    out = out >> 1u; 
    prevOut = out + (((q31_t) (((q63_t) in * oneByOut) >> 32)) << signBits); 
 
    /* Fifth iteration */ 
    signBits = arm_recip_q31((q31_t) prevOut, &oneByOut, armRecipTableQ31); 
    prevOut = prevOut >> 1u; 
    out = prevOut + (((q31_t) (((q63_t) in * oneByOut) >> 32)) << signBits); 
 
    signBits = arm_recip_q31((q31_t) out, &oneByOut, armRecipTableQ31); 
    out = out >> 1u; 
    prevOut = out + (((q31_t) (((q63_t) in * oneByOut) >> 32)) << signBits); 
 
    /* Seventh iteration */ 
    signBits = arm_recip_q31((q31_t) prevOut, &oneByOut, armRecipTableQ31); 
    prevOut = prevOut >> 1u; 
    out = prevOut + (((q31_t) (((q63_t) in * oneByOut) >> 32)) << signBits); 
 
    signBits = arm_recip_q31((q31_t) out, &oneByOut, armRecipTableQ31); 
    out = out >> 1u; 
    prevOut = out + (((q31_t) (((q63_t) in * oneByOut) >> 32)) << signBits); 
 
    signBits = arm_recip_q31((q31_t) prevOut, &oneByOut, armRecipTableQ31); 
    prevOut = prevOut >> 1u; 
    out = prevOut + (((q31_t) (((q63_t) in * oneByOut) >> 32)) << signBits); 
 
    /* tenth iteration */ 
    signBits = arm_recip_q31((q31_t) out, &oneByOut, armRecipTableQ31); 
    out = out >> 1u; 
    *pOut = out + (((q31_t) (((q63_t) in * oneByOut) >> 32)) << signBits); 
 
    return (ARM_MATH_SUCCESS); 
  } 
  else 
  { 
    *pOut = 0; 
    return (ARM_MATH_ARGUMENT_ERROR); 
  } 
 
} 
 
/**  
 * @} end of SQRT group  
 */ 
