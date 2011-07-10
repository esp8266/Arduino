/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_rfft_q31.c  
*  
* Description:	RFFT & RIFFT Q31 process function  
*  
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
*  
* Version 0.0.7  2010/06/10   
*    Misra-C changes done  
* -------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/*--------------------------------------------------------------------  
*		Internal functions prototypes  
--------------------------------------------------------------------*/ 
 
void arm_split_rfft_q31( 
  q31_t * pSrc, 
  uint32_t fftLen, 
  q31_t * pATable, 
  q31_t * pBTable, 
  q31_t * pDst, 
  uint32_t modifier); 
 
void arm_split_rifft_q31( 
  q31_t * pSrc, 
  uint32_t fftLen, 
  q31_t * pATable, 
  q31_t * pBTable, 
  q31_t * pDst, 
  uint32_t modifier); 
 
/**  
 * @addtogroup RFFT_RIFFT  
 * @{  
 */ 
 
/**  
 * @brief Processing function for the Q31 RFFT/RIFFT. 
 * @param[in]  *S    points to an instance of the Q31 RFFT/RIFFT structure. 
 * @param[in]  *pSrc points to the input buffer. 
 * @param[out] *pDst points to the output buffer. 
 * @return none. 
 *  
 * \par Input an output formats: 
 * \par  
 * Internally input is downscaled by 2 for every stage to avoid saturations inside CFFT/CIFFT process. 
 * Hence the output format is different for different RFFT sizes.  
 * The input and output formats for different RFFT sizes and number of bits to upscale are mentioned in the tables below for RFFT and RIFFT: 
 * \par  
 * \image html RFFTQ31.gif "Input and Output Formats for Q31 RFFT"  
 *  
 * \par  
 * \image html RIFFTQ31.gif "Input and Output Formats for Q31 RIFFT"  
 */ 
 
void arm_rfft_q31( 
  const arm_rfft_instance_q31 * S, 
  q31_t * pSrc, 
  q31_t * pDst) 
{ 
  const arm_cfft_radix4_instance_q31 *S_CFFT = S->pCfft; 
 
  /* Calculation of RIFFT of input */ 
  if(S->ifftFlagR == 1u) 
  { 
    /*  Real IFFT core process */ 
    arm_split_rifft_q31(pSrc, S->fftLenBy2, S->pTwiddleAReal, 
                        S->pTwiddleBReal, pDst, S->twidCoefRModifier); 
 
    /* Complex readix-4 IFFT process */ 
    arm_radix4_butterfly_inverse_q31(pDst, S_CFFT->fftLen, 
                                     S_CFFT->pTwiddle, 
                                     S_CFFT->twidCoefModifier); 
    /* Bit reversal process */ 
    if(S->bitReverseFlagR == 1u) 
    { 
      arm_bitreversal_q31(pDst, S_CFFT->fftLen, 
                          S_CFFT->bitRevFactor, S_CFFT->pBitRevTable); 
    } 
  } 
  else 
  { 
    /* Calculation of RFFT of input */ 
 
    /* Complex readix-4 FFT process */ 
    arm_radix4_butterfly_q31(pSrc, S_CFFT->fftLen, 
                             S_CFFT->pTwiddle, S_CFFT->twidCoefModifier); 
 
    /* Bit reversal process */ 
    if(S->bitReverseFlagR == 1u) 
    { 
      arm_bitreversal_q31(pSrc, S_CFFT->fftLen, 
                          S_CFFT->bitRevFactor, S_CFFT->pBitRevTable); 
    } 
 
    /*  Real FFT core process */ 
    arm_split_rfft_q31(pSrc, S->fftLenBy2, S->pTwiddleAReal, 
                       S->pTwiddleBReal, pDst, S->twidCoefRModifier); 
  } 
 
} 
 
 
  /**  
   * @} end of RFFT_RIFFT group  
   */ 
 
/**  
 * @brief  Core Real FFT process  
 * @param[in]   *pSrc 				points to the input buffer.  
 * @param[in]   fftLen  			length of FFT. 
 * @param[in]   *pATable 			points to the twiddle Coef A buffer.  
 * @param[in]   *pBTable 			points to the twiddle Coef B buffer.  
 * @param[out]  *pDst 				points to the output buffer.  
 * @param[in]   modifier 	        twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. 
 * @return none.  
 */ 
 
void arm_split_rfft_q31( 
  q31_t * pSrc, 
  uint32_t fftLen, 
  q31_t * pATable, 
  q31_t * pBTable, 
  q31_t * pDst, 
  uint32_t modifier) 
{ 
  uint32_t i;                                    /* Loop Counter */ 
  q31_t outR, outI;                              /* Temporary variables for output */ 
  q31_t *pCoefA, *pCoefB;                        /* Temporary pointers for twiddle factors */ 
  q31_t CoefA1, CoefA2, CoefB1;                  /* Temporary variables for twiddle coefficients */ 
  q31_t *pOut1 = &pDst[2], *pOut2 = &pDst[(4u * fftLen) - 1u]; 
  q31_t *pIn1 = &pSrc[2], *pIn2 = &pSrc[(2u * fftLen) - 1u]; 
 
  pSrc[2u * fftLen] = pSrc[0]; 
  pSrc[(2u * fftLen) + 1u] = pSrc[1]; 
 
  /* Init coefficient pointers */ 
  pCoefA = &pATable[modifier * 2u]; 
  pCoefB = &pBTable[modifier * 2u]; 
 
  i = fftLen - 1u; 
 
  while(i > 0u) 
  { 
    /*  
       outR = (pSrc[2 * i] * pATable[2 * i] - pSrc[2 * i + 1] * pATable[2 * i + 1]  
       + pSrc[2 * n - 2 * i] * pBTable[2 * i] +  
       pSrc[2 * n - 2 * i + 1] * pBTable[2 * i + 1]);  
     */ 
 
    /* outI = (pIn[2 * i + 1] * pATable[2 * i] + pIn[2 * i] * pATable[2 * i + 1] +  
       pIn[2 * n - 2 * i] * pBTable[2 * i + 1] -  
       pIn[2 * n - 2 * i + 1] * pBTable[2 * i]); */ 
 
    CoefA1 = *pCoefA++; 
    CoefA2 = *pCoefA; 
 
    /* outR = (pSrc[2 * i] * pATable[2 * i] */ 
    outR = ((int32_t) (((q63_t) * pIn1 * CoefA1) >> 32)); 
 
    /* outI = pIn[2 * i] * pATable[2 * i + 1] */ 
    outI = ((int32_t) (((q63_t) * pIn1++ * CoefA2) >> 32)); 
 
    /* - pSrc[2 * i + 1] * pATable[2 * i + 1] */ 
    outR = 
      (q31_t) ((((q63_t) outR << 32) + ((q63_t) * pIn1 * (-CoefA2))) >> 32); 
 
    /* (pIn[2 * i + 1] * pATable[2 * i] */ 
    outI = 
      (q31_t) ((((q63_t) outI << 32) + ((q63_t) * pIn1++ * (CoefA1))) >> 32); 
 
    /* pSrc[2 * n - 2 * i] * pBTable[2 * i]  */ 
    outR = 
      (q31_t) ((((q63_t) outR << 32) + ((q63_t) * pIn2 * (-CoefA2))) >> 32); 
    CoefB1 = *pCoefB; 
 
    /* pIn[2 * n - 2 * i] * pBTable[2 * i + 1] */ 
    outI = 
      (q31_t) ((((q63_t) outI << 32) + ((q63_t) * pIn2-- * (-CoefB1))) >> 32); 
 
    /* pSrc[2 * n - 2 * i + 1] * pBTable[2 * i + 1] */ 
    outR = 
      (q31_t) ((((q63_t) outR << 32) + ((q63_t) * pIn2 * (CoefB1))) >> 32); 
 
    /* pIn[2 * n - 2 * i + 1] * pBTable[2 * i] */ 
    outI = 
      (q31_t) ((((q63_t) outI << 32) + ((q63_t) * pIn2-- * (-CoefA2))) >> 32); 
 
    /* write output */ 
    *pOut1++ = (outR << 1u); 
    *pOut1++ = (outI << 1u); 
 
    /* write complex conjugate output */ 
    *pOut2-- = -(outI << 1u); 
    *pOut2-- = (outR << 1u); 
 
    /* update coefficient pointer */ 
    pCoefB = pCoefB + (modifier * 2u); 
    pCoefA = pCoefA + ((modifier * 2u) - 1u); 
 
    i--; 
 
  } 
 
  pDst[2u * fftLen] = pSrc[0] - pSrc[1]; 
  pDst[(2u * fftLen) + 1u] = 0; 
 
  pDst[0] = pSrc[0] + pSrc[1]; 
  pDst[1] = 0; 
 
} 
 
 
/**  
 * @brief  Core Real IFFT process  
 * @param[in]   *pSrc 				points to the input buffer. 
 * @param[in]   fftLen  			length of FFT.  
 * @param[in]   *pATable 			points to the twiddle Coef A buffer. 
 * @param[in]   *pBTable 			points to the twiddle Coef B buffer.  
 * @param[out]  *pDst 				points to the output buffer. 
 * @param[in]   modifier 	        twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. 
 * @return none.  
 */ 
 
void arm_split_rifft_q31( 
  q31_t * pSrc, 
  uint32_t fftLen, 
  q31_t * pATable, 
  q31_t * pBTable, 
  q31_t * pDst, 
  uint32_t modifier) 
{ 
  q31_t outR, outI;                              /* Temporary variables for output */ 
  q31_t *pCoefA, *pCoefB;                        /* Temporary pointers for twiddle factors */ 
  q31_t CoefA1, CoefA2, CoefB1;                  /* Temporary variables for twiddle coefficients */ 
  q31_t *pIn1 = &pSrc[0], *pIn2 = &pSrc[(2u * fftLen) + 1u]; 
 
  pCoefA = &pATable[0]; 
  pCoefB = &pBTable[0]; 
 
  while(fftLen > 0u) 
  { 
    /*  
       outR = (pIn[2 * i] * pATable[2 * i] + pIn[2 * i + 1] * pATable[2 * i + 1] +  
       pIn[2 * n - 2 * i] * pBTable[2 * i] -  
       pIn[2 * n - 2 * i + 1] * pBTable[2 * i + 1]);  
 
       outI = (pIn[2 * i + 1] * pATable[2 * i] - pIn[2 * i] * pATable[2 * i + 1] -  
       pIn[2 * n - 2 * i] * pBTable[2 * i + 1] -  
       pIn[2 * n - 2 * i + 1] * pBTable[2 * i]);  
 
     */ 
    CoefA1 = *pCoefA++; 
    CoefA2 = *pCoefA; 
 
    /* outR = (pIn[2 * i] * pATable[2 * i] */ 
    outR = ((int32_t) (((q63_t) * pIn1 * CoefA1) >> 32)); 
 
    /* - pIn[2 * i] * pATable[2 * i + 1] */ 
    outI = -((int32_t) (((q63_t) * pIn1++ * CoefA2) >> 32)); 
 
    /* pIn[2 * i + 1] * pATable[2 * i + 1] */ 
    outR = 
      (q31_t) ((((q63_t) outR << 32) + ((q63_t) * pIn1 * (CoefA2))) >> 32); 
 
    /* pIn[2 * i + 1] * pATable[2 * i] */ 
    outI = 
      (q31_t) ((((q63_t) outI << 32) + ((q63_t) * pIn1++ * (CoefA1))) >> 32); 
 
    /* pIn[2 * n - 2 * i] * pBTable[2 * i] */ 
    outR = 
      (q31_t) ((((q63_t) outR << 32) + ((q63_t) * pIn2 * (CoefA2))) >> 32); 
 
    CoefB1 = *pCoefB; 
 
    /* pIn[2 * n - 2 * i] * pBTable[2 * i + 1] */ 
    outI = 
      (q31_t) ((((q63_t) outI << 32) - ((q63_t) * pIn2-- * (CoefB1))) >> 32); 
 
    /* pIn[2 * n - 2 * i + 1] * pBTable[2 * i + 1] */ 
    outR = 
      (q31_t) ((((q63_t) outR << 32) + ((q63_t) * pIn2 * (CoefB1))) >> 32); 
 
    /* pIn[2 * n - 2 * i + 1] * pBTable[2 * i] */ 
    outI = 
      (q31_t) ((((q63_t) outI << 32) + ((q63_t) * pIn2-- * (CoefA2))) >> 32); 
 
    /* write output */ 
    *pDst++ = (outR << 1u); 
    *pDst++ = (outI << 1u); 
 
    /* update coefficient pointer */ 
    pCoefB = pCoefB + (modifier * 2u); 
    pCoefA = pCoefA + ((modifier * 2u) - 1u); 
 
    /* Decrement loop count */ 
    fftLen--; 
 
  } 
 
 
} 
