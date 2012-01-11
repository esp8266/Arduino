/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_pid_init_q15.c   
*   
* Description:	Q15 PID Control initialization function   
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

 /**   
 * @addtogroup PID   
 * @{   
 */

/**   
 * @details   
 * @param[in,out] *S points to an instance of the Q15 PID structure.   
 * @param[in]     resetStateFlag  flag to reset the state. 0 = no change in state 1 = reset the state.   
 * @return none.   
 * \par Description:  
 * \par   
 * The <code>resetStateFlag</code> specifies whether to set state to zero or not. \n  
 * The function computes the structure fields: <code>A0</code>, <code>A1</code> <code>A2</code>   
 * using the proportional gain( \c Kp), integral gain( \c Ki) and derivative gain( \c Kd)   
 * also sets the state variables to all zeros.   
 */

void arm_pid_init_q15(
  arm_pid_instance_q15 * S,
  int32_t resetStateFlag)
{

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  /* Derived coefficient A0 */
  S->A0 = __QADD16(__QADD16(S->Kp, S->Ki), S->Kd);

  /* Derived coefficients and pack into A1 */

#ifndef  ARM_MATH_BIG_ENDIAN

  S->A1 = __PKHBT(-__QADD16(__QADD16(S->Kd, S->Kd), S->Kp), S->Kd, 16);

#else

  S->A1 = __PKHBT(S->Kd, -__QADD16(__QADD16(S->Kd, S->Kd), S->Kp), 16);

#endif /*      #ifndef  ARM_MATH_BIG_ENDIAN    */

  /* Check whether state needs reset or not */
  if(resetStateFlag)
  {
    /* Clear the state buffer.  The size will be always 3 samples */
    memset(S->state, 0, 3u * sizeof(q15_t));
  }

#else

  /* Run the below code for Cortex-M0 */

  q31_t temp;                                    /*to store the sum */

  /* Derived coefficient A0 */
  temp = S->Kp + S->Ki + S->Kd;
  S->A0 = (q15_t) __SSAT(temp, 16);

  /* Derived coefficients and pack into A1 */
  temp = -(S->Kd + S->Kd + S->Kp);
  S->A1 = (q15_t) __SSAT(temp, 16);
  S->A2 = S->Kd;



  /* Check whether state needs reset or not */
  if(resetStateFlag)
  {
    /* Clear the state buffer.  The size will be always 3 samples */
    memset(S->state, 0, 3u * sizeof(q15_t));
  }

#endif /* #ifndef ARM_MATH_CM0 */

}

/**   
 * @} end of PID group   
 */
