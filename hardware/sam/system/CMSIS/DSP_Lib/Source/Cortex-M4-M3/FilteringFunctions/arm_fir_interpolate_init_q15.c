/*-----------------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:        arm_fir_interpolate_init_q15.c  
*  
* Description:  Q15 FIR interpolator initialization function  
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
 * @addtogroup FIR_Interpolate  
 * @{  
 */ 
 
/**  
 * @brief  Initialization function for the Q15 FIR interpolator.  
 * @param[in,out] *S        points to an instance of the Q15 FIR interpolator structure.  
 * @param[in]     L         upsample factor.  
 * @param[in]     numTaps   number of filter coefficients in the filter.  
 * @param[in]     *pCoeffs  points to the filter coefficient buffer.  
 * @param[in]     *pState   points to the state buffer.  
 * @param[in]     blockSize number of input samples to process per call.  
 * @return        The function returns ARM_MATH_SUCCESS if initialization was successful or ARM_MATH_LENGTH_ERROR if  
 * the filter length <code>numTaps</code> is not a multiple of the interpolation factor <code>L</code>.  
 *  
 * <b>Description:</b>  
 * \par  
 * <code>pCoeffs</code> points to the array of filter coefficients stored in time reversed order:  
 * <pre>  
 *    {b[numTaps-1], b[numTaps-2], b[numTaps-2], ..., b[1], b[0]}  
 * </pre>  
 * The length of the filter <code>numTaps</code> must be a multiple of the interpolation factor <code>L</code>.  
 * \par  
 * <code>pState</code> points to the array of state variables.  
 * <code>pState</code> is of length <code>(numTaps/L)+blockSize-1</code> words  
 * where <code>blockSize</code> is the number of input samples processed by each call to <code>arm_fir_interpolate_q15()</code>.  
 */ 
 
arm_status arm_fir_interpolate_init_q15( 
  arm_fir_interpolate_instance_q15 * S, 
  uint8_t L, 
  uint16_t numTaps, 
  q15_t * pCoeffs, 
  q15_t * pState, 
  uint32_t blockSize) 
{ 
  arm_status status; 
 
  /* The filter length must be a multiple of the interpolation factor */ 
  if((numTaps % L) != 0u) 
  { 
    /* Set status as ARM_MATH_LENGTH_ERROR */ 
    status = ARM_MATH_LENGTH_ERROR; 
  } 
  else 
  { 
 
    /* Assign coefficient pointer */ 
    S->pCoeffs = pCoeffs; 
 
    /* Assign Interpolation factor */ 
    S->L = L; 
 
    /* Assign polyPhaseLength */ 
    S->phaseLength = numTaps / L; 
 
    /* Clear state buffer and size of buffer is always phaseLength + blockSize - 1 */ 
    memset(pState, 0, 
           (blockSize + ((uint32_t) S->phaseLength - 1u)) * sizeof(q15_t)); 
 
    /* Assign state pointer */ 
    S->pState = pState; 
 
    status = ARM_MATH_SUCCESS; 
  } 
 
  return (status); 
 
} 
 
 /**  
  * @} end of FIR_Interpolate group  
  */ 
