/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_pid_reset_f32.c  
*  
* Description:	Floating-point PID Control reset function 
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
* @brief  Reset function for the floating-point PID Control. 
* @param[in] *S	Instance pointer of PID control data structure. 
* @return none.  
* \par Description: 
* The function resets the state buffer to zeros.  
*/  
void arm_pid_reset_f32(  
  arm_pid_instance_f32 * S)  
{  
  
  /* Clear the state buffer.  The size will be always 3 samples */  
  memset(S->state, 0, 3u * sizeof(float32_t));  
}  
  
/**  
 * @} end of PID group  
 */ 
