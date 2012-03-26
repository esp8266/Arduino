/*-----------------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:        arm_biquad_cascade_df2T_init_f32.c   
*   
* Description:  Initialization function for the floating-point transposed  
*               direct form II Biquad cascade filter.  
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
* ---------------------------------------------------------------------------*/

#include "arm_math.h"

/**   
 * @ingroup groupFilters   
 */

/**   
 * @addtogroup BiquadCascadeDF2T   
 * @{   
 */

/**  
 * @brief  Initialization function for the floating-point transposed direct form II Biquad cascade filter.  
 * @param[in,out] *S           points to an instance of the filter data structure.  
 * @param[in]     numStages    number of 2nd order stages in the filter.  
 * @param[in]     *pCoeffs     points to the filter coefficients.  
 * @param[in]     *pState      points to the state buffer.  
 * @return        none  
 *   
 * <b>Coefficient and State Ordering:</b>   
 * \par   
 * The coefficients are stored in the array <code>pCoeffs</code> in the following order:   
 * <pre>   
 *     {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}   
 * </pre>   
 *   
 * \par   
 * where <code>b1x</code> and <code>a1x</code> are the coefficients for the first stage,   
 * <code>b2x</code> and <code>a2x</code> are the coefficients for the second stage,   
 * and so on.  The <code>pCoeffs</code> array contains a total of <code>5*numStages</code> values.   
 *   
 * \par   
 * The <code>pState</code> is a pointer to state array.   
 * Each Biquad stage has 2 state variables <code>d1,</code> and <code>d2</code>.   
 * The 2 state variables for stage 1 are first, then the 2 state variables for stage 2, and so on.   
 * The state array has a total length of <code>2*numStages</code> values.   
 * The state variables are updated after each block of data is processed; the coefficients are untouched.   
 */

void arm_biquad_cascade_df2T_init_f32(
  arm_biquad_cascade_df2T_instance_f32 * S,
  uint8_t numStages,
  float32_t * pCoeffs,
  float32_t * pState)
{
  /* Assign filter stages */
  S->numStages = numStages;

  /* Assign coefficient pointer */
  S->pCoeffs = pCoeffs;

  /* Clear state buffer and size is always 2 * numStages */
  memset(pState, 0, (2u * (uint32_t) numStages) * sizeof(float32_t));

  /* Assign state pointer */
  S->pState = pState;
}

/**   
 * @} end of BiquadCascadeDF2T group   
 */
