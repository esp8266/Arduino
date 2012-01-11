/*-----------------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:        arm_biquad_cascade_df1_init_f32.c   
*   
* Description:  floating-point Biquad cascade DirectFormI(DF1) filter initialization function.   
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
* ---------------------------------------------------------------------------*/

#include "arm_math.h"

/**   
 * @ingroup groupFilters   
 */

/**   
 * @addtogroup BiquadCascadeDF1   
 * @{   
 */

/**   
 * @details   
 * @brief  Initialization function for the floating-point Biquad cascade filter.   
 * @param[in,out] *S           points to an instance of the floating-point Biquad cascade structure.   
 * @param[in]     numStages    number of 2nd order stages in the filter.   
 * @param[in]     *pCoeffs     points to the filter coefficients array.   
 * @param[in]     *pState      points to the state array.   
 * @return        none   
 *   
 *   
 * <b>Coefficient and State Ordering:</b>   
 *   
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
 * Each Biquad stage has 4 state variables <code>x[n-1], x[n-2], y[n-1],</code> and <code>y[n-2]</code>.   
 * The state variables are arranged in the <code>pState</code> array as:   
 * <pre>   
 *     {x[n-1], x[n-2], y[n-1], y[n-2]}   
 * </pre>   
 * The 4 state variables for stage 1 are first, then the 4 state variables for stage 2, and so on.   
 * The state array has a total length of <code>4*numStages</code> values.   
 * The state variables are updated after each block of data is processed; the coefficients are untouched.   
 *   
 */

void arm_biquad_cascade_df1_init_f32(
  arm_biquad_casd_df1_inst_f32 * S,
  uint8_t numStages,
  float32_t * pCoeffs,
  float32_t * pState)
{
  /* Assign filter stages */
  S->numStages = numStages;

  /* Assign coefficient pointer */
  S->pCoeffs = pCoeffs;

  /* Clear state buffer and size is always 4 * numStages */
  memset(pState, 0, (4u * (uint32_t) numStages) * sizeof(float32_t));

  /* Assign state pointer */
  S->pState = pState;
}

/**   
 * @} end of BiquadCascadeDF1 group   
 */
