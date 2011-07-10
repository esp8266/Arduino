/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_pid_init_q31.c  
*  
* Description:	Q31 PID Control initialization function   
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
* ------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
 /**  
 * @addtogroup PID  
 * @{  
 */ 
 
/**  
 * @brief  Initialization function for the Q31 PID Control. 
 * @param[in,out] *S points to an instance of the Q31 PID structure. 
 * @param[in]     resetStateFlag  flag to reset the state. 0 = no change in state 1 = reset the state. 
 * @return none.  
 * \par Description: 
 * \par  
 * The <code>resetStateFlag</code> specifies whether to set state to zero or not. \n 
 * The function computes the structure fields: <code>A0</code>, <code>A1</code> <code>A2</code>  
 * using the proportional gain( \c Kp), integral gain( \c Ki) and derivative gain( \c Kd)  
 * also sets the state variables to all zeros.  
 */  
  
void arm_pid_init_q31(  
  arm_pid_instance_q31 * S,  
  int32_t resetStateFlag) 
{  
  /* Derived coefficient A0 */  
  S->A0 = __QADD(__QADD(S->Kp, S->Ki), S->Kd);  
  
  /* Derived coefficient A1 */  
  S->A1 = -__QADD(__QADD(S->Kd, S->Kd), S->Kp);  
  
  /* Derived coefficient A2 */  
  S->A2 = S->Kd;  
  
  /* Check whether state needs reset or not */  
  if(resetStateFlag) 
  {  
    /* Clear the state buffer.  The size will be always 3 samples */  
    memset(S->state, 0, 3u * sizeof(q31_t));  
  }  
  
}  
  
/**  
 * @} end of PID group  
 */ 
