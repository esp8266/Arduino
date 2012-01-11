/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_pid_reset_q31.c   
*   
* Description:	Q31 PID Control reset function  
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
* ------------------------------------------------------------------- */

#include "arm_math.h"

 /**   
 * @addtogroup PID   
 * @{   
 */

/**   
* @brief  Reset function for the Q31 PID Control.  
* @param[in] *S	Instance pointer of PID control data structure.  
* @return none.   
* \par Description:  
* The function resets the state buffer to zeros.   
*/
void arm_pid_reset_q31(
  arm_pid_instance_q31 * S)
{

  /* Clear the state buffer.  The size will be always 3 samples */
  memset(S->state, 0, 3u * sizeof(q31_t));
}

/**   
 * @} end of PID group   
 */
