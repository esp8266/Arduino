/*-----------------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:        arm_fir_lattice_init_q31.c  
*  
* Description:  Q31 FIR lattice filter initialization function.  
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
 * @addtogroup FIR_Lattice  
 * @{  
 */ 
 
  /**  
   * @brief Initialization function for the Q31 FIR lattice filter.  
   * @param[in] *S points to an instance of the Q31 FIR lattice structure.  
   * @param[in] numStages  number of filter stages.  
   * @param[in] *pCoeffs points to the coefficient buffer.  The array is of length numStages.  
   * @param[in] *pState points to the state buffer.   The array is of length numStages.  
   * @return none.  
   */ 
 
void arm_fir_lattice_init_q31( 
  arm_fir_lattice_instance_q31 * S, 
  uint16_t numStages, 
  q31_t * pCoeffs, 
  q31_t * pState) 
{ 
  /* Assign filter taps */ 
  S->numStages = numStages; 
 
  /* Assign coefficient pointer */ 
  S->pCoeffs = pCoeffs; 
 
  /* Clear state buffer and size is always numStages */ 
  memset(pState, 0, (numStages) * sizeof(q31_t)); 
 
  /* Assign state pointer */ 
  S->pState = pState; 
 
} 
 
/**  
 * @} end of FIR_Lattice group  
 */ 
