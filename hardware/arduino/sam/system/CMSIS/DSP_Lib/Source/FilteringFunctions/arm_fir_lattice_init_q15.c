/*-----------------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:        arm_fir_lattice_init_q15.c   
*   
* Description:  Q15 FIR Lattice filter initialization function.   
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
 * @addtogroup FIR_Lattice   
 * @{   
 */

  /**   
   * @brief Initialization function for the Q15 FIR lattice filter.   
   * @param[in] *S points to an instance of the Q15 FIR lattice structure.   
   * @param[in] numStages  number of filter stages.   
   * @param[in] *pCoeffs points to the coefficient buffer.  The array is of length numStages.    
   * @param[in] *pState points to the state buffer.  The array is of length numStages.    
   * @return none.   
   */

void arm_fir_lattice_init_q15(
  arm_fir_lattice_instance_q15 * S,
  uint16_t numStages,
  q15_t * pCoeffs,
  q15_t * pState)
{
  /* Assign filter taps */
  S->numStages = numStages;

  /* Assign coefficient pointer */
  S->pCoeffs = pCoeffs;

  /* Clear state buffer and size is always numStages */
  memset(pState, 0, (numStages) * sizeof(q15_t));

  /* Assign state pointer */
  S->pState = pState;

}

/**   
 * @} end of FIR_Lattice group   
 */
