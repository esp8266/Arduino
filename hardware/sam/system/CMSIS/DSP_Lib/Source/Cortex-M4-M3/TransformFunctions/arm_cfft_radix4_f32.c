/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_cfft_radix4_f32.c  
*  
* Description:	Radix-4 Decimation in Frequency CFFT & CIFFT Floating point processing function  
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
* Version 0.0.5  2010/04/26   
* 	 incorporated review comments and updated with latest CMSIS layer  
*  
* Version 0.0.3  2010/03/10   
*    Initial version  
* -------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupTransforms  
 */ 
 
/**  
 * @defgroup CFFT_CIFFT Complex FFT Functions  
 *  
 * \par  
 * Complex Fast Fourier Transform(CFFT) and Complex Inverse Fast Fourier Transform(CIFFT) is an efficient algorithm to compute Discrete Fourier Transform(DFT) and Inverse Discrete Fourier Transform(IDFT).  
 * Computational complexity of CFFT reduces drastically when compared to DFT.  
 * \par  
 * This set of functions implements CFFT/CIFFT  
 * for Q15, Q31, and floating-point data types.  The functions operates on in-place buffer which uses same buffer for input and output.  
 * Complex input is stored in input buffer in an interleaved fashion.  
 *  
 * \par  
 * The functions operate on blocks of input and output data and each call to the function processes  
 * <code>2*fftLen</code> samples through the transform.  <code>pSrc</code>  points to In-place arrays containing <code>2*fftLen</code> values.  
 * \par 
 * The <code>pSrc</code> points to the array of in-place buffer of size <code>2*fftLen</code> and inputs and outputs are stored in an interleaved fashion as shown below.  
 * <pre> {real[0], imag[0], real[1], imag[1],..} </pre>  
 *  
 * \par Lengths supported by the transform: 
 * \par  
 * Internally, the function utilize a radix-4 decimation in frequency(DIF) algorithm  
 * and the size of the FFT supported are of the lengths [16, 64, 256, 1024]. 
 *   
 *  
 * \par Algorithm:  
 *  
 * <b>Complex Fast Fourier Transform:</b>  
 * \par   
 * Input real and imaginary data:  
 * <pre>  
 * x(n) = xa + j * ya  
 * x(n+N/4 ) = xb + j * yb  
 * x(n+N/2 ) = xc + j * yc  
 * x(n+3N 4) = xd + j * yd  
 * </pre>  
 * where N is length of FFT  
 * \par  
 * Output real and imaginary data:  
 * <pre>  
 * X(4r) = xa'+ j * ya'  
 * X(4r+1) = xb'+ j * yb'  
 * X(4r+2) = xc'+ j * yc'  
 * X(4r+3) = xd'+ j * yd'  
 * </pre>  
 * \par  
 * Twiddle factors for radix-4 FFT:  
 * <pre>  
 * Wn = co1 + j * (- si1)  
 * W2n = co2 + j * (- si2)  
 * W3n = co3 + j * (- si3)  
 * </pre>  
 *  
 * \par  
 * \image html CFFT.gif "Radix-4 Decimation-in Frequency Complex Fast Fourier Transform"  
 *  
 * \par  
 * Output from Radix-4 CFFT Results in Digit reversal order. Interchange middle two branches of every butterfly results in Bit reversed output.  
 * \par  
 * <b> Butterfly CFFT equations:</b>  
 * <pre>  
 * xa' = xa + xb + xc + xd  
 * ya' = ya + yb + yc + yd  
 * xc' = (xa+yb-xc-yd)* co1 + (ya-xb-yc+xd)* (si1)  
 * yc' = (ya-xb-yc+xd)* co1 - (xa+yb-xc-yd)* (si1)  
 * xb' = (xa-xb+xc-xd)* co2 + (ya-yb+yc-yd)* (si2)  
 * yb' = (ya-yb+yc-yd)* co2 - (xa-xb+xc-xd)* (si2)  
 * xd' = (xa-yb-xc+yd)* co3 + (ya+xb-yc-xd)* (si3)  
 * yd' = (ya+xb-yc-xd)* co3 - (xa-yb-xc+yd)* (si3)  
 * </pre>  
 *  
 *  
 * <b>Complex Inverse Fast Fourier Transform:</b>  
 * \par  
 * CIFFT uses same twiddle factor table as CFFT with modifications in the design equation as shown below.  
 *  
 * \par  
 * <b> Modified Butterfly CIFFT equations:</b>  
 * <pre>  
 * xa' = xa + xb + xc + xd  
 * ya' = ya + yb + yc + yd  
 * xc' = (xa-yb-xc+yd)* co1 - (ya+xb-yc-xd)* (si1)  
 * yc' = (ya+xb-yc-xd)* co1 + (xa-yb-xc+yd)* (si1)  
 * xb' = (xa-xb+xc-xd)* co2 - (ya-yb+yc-yd)* (si2)  
 * yb' = (ya-yb+yc-yd)* co2 + (xa-xb+xc-xd)* (si2)  
 * xd' = (xa+yb-xc-yd)* co3 - (ya-xb-yc+xd)* (si3)  
 * yd' = (ya-xb-yc+xd)* co3 + (xa+yb-xc-yd)* (si3)  
 * </pre>  
 *  
 * \par Instance Structure  
 * A separate instance structure must be defined for each Instance but the twiddle factors and bit reversal tables can be reused.  
 * There are separate instance structure declarations for each of the 3 supported data types.  
 *  
 * \par Initialization Functions  
 * There is also an associated initialization function for each data type.  
 * The initialization function performs the following operations:  
 * - Sets the values of the internal structure fields.  
 * - Initializes twiddle factor table and bit reversal table pointers  
 * \par  
 * Use of the initialization function is optional.  
 * However, if the initialization function is used, then the instance structure cannot be placed into a const data section.  
 * To place an instance structure into a const data section, the instance structure must be manually initialized.  
 * Manually initialize the instance structure as follows:  
 * <pre>  
 *arm_cfft_radix4_instance_f32 S = {fftLen, ifftFlag, bitReverseFlag, pTwiddle, pBitRevTable, twidCoefModifier, bitRevFactor, onebyfftLen};  
 *arm_cfft_radix4_instance_q31 S = {fftLen, ifftFlag, bitReverseFlag, pTwiddle, pBitRevTable, twidCoefModifier, bitRevFactor};  
 *arm_cfft_radix4_instance_q15 S = {fftLen, ifftFlag, bitReverseFlag, pTwiddle, pBitRevTable, twidCoefModifier, bitRevFactor};  
 * </pre>  
 * \par  
 * where <code>fftLen</code> length of CFFT/CIFFT; <code>ifftFlag</code> Flag for selection of CFFT or CIFFT(Set ifftFlag to calculate CIFFT otherwise calculates CFFT);  
 * <code>bitReverseFlag</code> Flag for selection of output order(Set bitReverseFlag to output in normal order otherwise output in bit reversed order);   
 * <code>pTwiddle</code>points to array of twiddle coefficients; <code>pBitRevTable</code> points to the array of bit reversal table.  
 * <code>twidCoefModifier</code> modifier for twiddle factor table which supports all FFT lengths with same table;   
 * <code>pBitRevTable</code> modifier for bit reversal table which supports all FFT lengths with same table.  
 * <code>onebyfftLen</code> value of 1/fftLen to calculate CIFFT;  
 * 
 * \par Fixed-Point Behavior  
 * Care must be taken when using the fixed-point versions of the CFFT/CIFFT function.  
 * Refer to the function specific documentation below for usage guidelines.  
 */ 
 
 
/**  
 * @addtogroup CFFT_CIFFT  
 * @{  
 */ 
 
/**  
 * @details  
 * @brief Processing function for the floating-point CFFT/CIFFT. 
 * @param[in]      *S    points to an instance of the floating-point CFFT/CIFFT structure. 
 * @param[in, out] *pSrc points to the complex data buffer of size <code>2*fftLen</code>. Processing occurs in-place. 
 * @return none. 
 */ 
 
void arm_cfft_radix4_f32( 
  const arm_cfft_radix4_instance_f32 * S, 
  float32_t * pSrc) 
{ 
 
  if(S->ifftFlag == 1u) 
  { 
    /*  Complex IFFT radix-4  */ 
    arm_radix4_butterfly_inverse_f32(pSrc, S->fftLen, S->pTwiddle, 
                                     S->twidCoefModifier, S->onebyfftLen); 
  } 
  else 
  { 
    /*  Complex FFT radix-4  */ 
    arm_radix4_butterfly_f32(pSrc, S->fftLen, S->pTwiddle, 
                             S->twidCoefModifier); 
  } 
 
  if(S->bitReverseFlag == 1u) 
  { 
    /*  Bit Reversal */ 
    arm_bitreversal_f32(pSrc, S->fftLen, S->bitRevFactor, S->pBitRevTable); 
  } 
 
} 
 
 
/**  
 * @} end of CFFT_CIFFT group  
 */ 
 
 
 
/* ----------------------------------------------------------------------  
** Internal helper function used by the FFTs  
** ------------------------------------------------------------------- */ 
 
/*  
 * @brief  Core function for the floating-point CFFT butterfly process. 
 * @param[in, out] *pSrc            points to the in-place buffer of floating-point data type. 
 * @param[in]      fftLen           length of the FFT. 
 * @param[in]      *pCoef           points to the twiddle coefficient buffer. 
 * @param[in]      twidCoefModifier twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. 
 * @return none. 
 */ 
 
void arm_radix4_butterfly_f32( 
  float32_t * pSrc, 
  uint16_t fftLen, 
  float32_t * pCoef, 
  uint16_t twidCoefModifier) 
{ 
 
  float32_t co1, co2, co3, si1, si2, si3; 
  float32_t t1, t2, r1, r2, s1, s2; 
  uint32_t ia1, ia2, ia3; 
  uint32_t i0, i1, i2, i3; 
  uint32_t n1, n2, j, k; 
 
  /*  Initializations for the first stage */ 
  n2 = fftLen; 
  n1 = n2; 
 
  /* n2 = fftLen/4 */ 
  n2 >>= 2u; 
  i0 = 0u; 
  ia1 = 0u; 
 
  j = n2; 
 
  /*  Calculation of first stage */ 
  do 
  { 
    /*  index calculation for the input as, */ 
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
    i1 = i0 + n2; 
    i2 = i1 + n2; 
    i3 = i2 + n2; 
 
    /*  Butterfly implementation */ 
 
    /* xa + xc */ 
    r1 = pSrc[(2u * i0)] + pSrc[(2u * i2)]; 
 
    /* xa - xc */ 
    r2 = pSrc[2u * i0] - pSrc[2u * i2]; 
 
    /* ya + yc */ 
    s1 = pSrc[(2u * i0) + 1u] + pSrc[(2u * i2) + 1u]; 
 
    /* ya - yc */ 
    s2 = pSrc[(2u * i0) + 1u] - pSrc[(2u * i2) + 1u]; 
 
    /* xb + xd */ 
    t1 = pSrc[2u * i1] + pSrc[2u * i3]; 
 
    /* xa' = xa + xb + xc + xd */ 
    pSrc[2u * i0] = r1 + t1; 
 
    /* (xa + xc) - (xb + xd) */ 
    r1 = r1 - t1; 
 
    /* yb + yd */ 
    t2 = pSrc[(2u * i1) + 1u] + pSrc[(2u * i3) + 1u]; 
 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[(2u * i0) + 1u] = s1 + t2; 
 
    /* (ya + yc) - (yb + yd) */ 
    s1 = s1 - t2; 
 
    /* yb - yd */ 
    t1 = pSrc[(2u * i1) + 1u] - pSrc[(2u * i3) + 1u]; 
 
    /* xb - xd */ 
    t2 = pSrc[2u * i1] - pSrc[2u * i3]; 
 
    /*  index calculation for the coefficients */ 
    ia2 = ia1 + ia1; 
    co2 = pCoef[ia2 * 2u]; 
    si2 = pCoef[(ia2 * 2u) + 1u]; 
 
    /* xc' = (xa-xb+xc-xd)co2 + (ya-yb+yc-yd)(si2) */ 
    pSrc[2u * i1] = (r1 * co2) + (s1 * si2); 
 
    /* yc' = (ya-yb+yc-yd)co2 - (xa-xb+xc-xd)(si2) */ 
    pSrc[(2u * i1) + 1u] = (s1 * co2) - (r1 * si2); 
 
    /* (xa - xc) + (yb - yd) */ 
    r1 = r2 + t1; 
 
    /* (xa - xc) - (yb - yd) */ 
    r2 = r2 - t1; 
 
    /* (ya - yc) - (xb - xd) */ 
    s1 = s2 - t2; 
 
    /* (ya - yc) + (xb - xd) */ 
    s2 = s2 + t2; 
 
    co1 = pCoef[ia1 * 2u]; 
    si1 = pCoef[(ia1 * 2u) + 1u]; 
 
    /* xb' = (xa+yb-xc-yd)co1 + (ya-xb-yc+xd)(si1) */ 
    pSrc[2u * i2] = (r1 * co1) + (s1 * si1); 
 
    /* yb' = (ya-xb-yc+xd)co1 - (xa+yb-xc-yd)(si1) */ 
    pSrc[(2u * i2) + 1u] = (s1 * co1) - (r1 * si1); 
 
    /*  index calculation for the coefficients */ 
    ia3 = ia2 + ia1; 
    co3 = pCoef[ia3 * 2u]; 
    si3 = pCoef[(ia3 * 2u) + 1u]; 
 
 
    /* xd' = (xa-yb-xc+yd)co3 + (ya+xb-yc-xd)(si3) */ 
    pSrc[2u * i3] = (r2 * co3) + (s2 * si3); 
 
    /* yd' = (ya+xb-yc-xd)co3 - (xa-yb-xc+yd)(si3) */ 
    pSrc[(2u * i3) + 1u] = (s2 * co3) - (r2 * si3); 
 
    /*  Twiddle coefficients index modifier */ 
    ia1 = ia1 + twidCoefModifier; 
 
    /*  Updating input index */ 
    i0 = i0 + 1u; 
 
  } 
  while(--j); 
 
  twidCoefModifier <<= 2u; 
 
  /*  Calculation of second stage to excluding last stage */ 
  for (k = fftLen / 4; k > 4u; k >>= 2u) 
  { 
    /*  Initializations for the first stage */ 
    n1 = n2; 
    n2 >>= 2u; 
    ia1 = 0u; 
 
    /*  Calculation of first stage */ 
    for (j = 0u; j <= (n2 - 1u); j++) 
    { 
      /*  index calculation for the coefficients */ 
      ia2 = ia1 + ia1; 
      ia3 = ia2 + ia1; 
      co1 = pCoef[ia1 * 2u]; 
      si1 = pCoef[(ia1 * 2u) + 1u]; 
      co2 = pCoef[ia2 * 2u]; 
      si2 = pCoef[(ia2 * 2u) + 1u]; 
      co3 = pCoef[ia3 * 2u]; 
      si3 = pCoef[(ia3 * 2u) + 1u]; 
 
      /*  Twiddle coefficients index modifier */ 
      ia1 = ia1 + twidCoefModifier; 
 
      for (i0 = j; i0 < fftLen; i0 += n1) 
      { 
        /*  index calculation for the input as, */ 
        /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
        i1 = i0 + n2; 
        i2 = i1 + n2; 
        i3 = i2 + n2; 
 
        /* xa + xc */ 
        r1 = pSrc[(2u * i0)] + pSrc[(2u * i2)]; 
 
        /* xa - xc */ 
        r2 = pSrc[(2u * i0)] - pSrc[(2u * i2)]; 
 
        /* ya + yc */ 
        s1 = pSrc[(2u * i0) + 1u] + pSrc[(2u * i2) + 1u]; 
 
        /* ya - yc */ 
        s2 = pSrc[(2u * i0) + 1u] - pSrc[(2u * i2) + 1u]; 
 
        /* xb + xd */ 
        t1 = pSrc[2u * i1] + pSrc[2u * i3]; 
 
        /* xa' = xa + xb + xc + xd */ 
        pSrc[2u * i0] = r1 + t1; 
 
        /* xa + xc -(xb + xd) */ 
        r1 = r1 - t1; 
 
        /* yb + yd */ 
        t2 = pSrc[(2u * i1) + 1u] + pSrc[(2u * i3) + 1u]; 
 
        /* ya' = ya + yb + yc + yd */ 
        pSrc[(2u * i0) + 1u] = s1 + t2; 
 
        /* (ya + yc) - (yb + yd) */ 
        s1 = s1 - t2; 
 
        /* (yb - yd) */ 
        t1 = pSrc[(2u * i1) + 1u] - pSrc[(2u * i3) + 1u]; 
 
        /* (xb - xd) */ 
        t2 = pSrc[2u * i1] - pSrc[2u * i3]; 
 
        /* xc' = (xa-xb+xc-xd)co2 + (ya-yb+yc-yd)(si2) */ 
        pSrc[2u * i1] = (r1 * co2) + (s1 * si2); 
 
        /* yc' = (ya-yb+yc-yd)co2 - (xa-xb+xc-xd)(si2) */ 
        pSrc[(2u * i1) + 1u] = (s1 * co2) - (r1 * si2); 
 
        /* (xa - xc) + (yb - yd) */ 
        r1 = r2 + t1; 
 
        /* (xa - xc) - (yb - yd) */ 
        r2 = r2 - t1; 
 
        /* (ya - yc) -  (xb - xd) */ 
        s1 = s2 - t2; 
 
        /* (ya - yc) +  (xb - xd) */ 
        s2 = s2 + t2; 
 
        /* xb' = (xa+yb-xc-yd)co1 + (ya-xb-yc+xd)(si1) */ 
        pSrc[2u * i2] = (r1 * co1) + (s1 * si1); 
 
        /* yb' = (ya-xb-yc+xd)co1 - (xa+yb-xc-yd)(si1) */ 
        pSrc[(2u * i2) + 1u] = (s1 * co1) - (r1 * si1); 
 
        /* xd' = (xa-yb-xc+yd)co3 + (ya+xb-yc-xd)(si3) */ 
        pSrc[2u * i3] = (r2 * co3) + (s2 * si3); 
 
        /* yd' = (ya+xb-yc-xd)co3 - (xa-yb-xc+yd)(si3) */ 
        pSrc[(2u * i3) + 1u] = (s2 * co3) - (r2 * si3); 
      } 
    } 
    twidCoefModifier <<= 2u; 
  } 
 
  /*  Initializations of last stage */ 
  n1 = n2; 
  n2 >>= 2u; 
 
  /*  Calculations of last stage */ 
  for (i0 = 0u; i0 <= (fftLen - n1); i0 += n1) 
  { 
    /*  index calculation for the input as, */ 
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
    i1 = i0 + n2; 
    i2 = i1 + n2; 
    i3 = i2 + n2; 
 
    /*  Butterfly implementation */ 
 
    /* xa + xb */ 
    r1 = pSrc[2u * i0] + pSrc[2u * i2]; 
 
    /* xa - xb */ 
    r2 = pSrc[2u * i0] - pSrc[2u * i2]; 
 
    /* ya + yc */ 
    s1 = pSrc[(2u * i0) + 1u] + pSrc[(2u * i2) + 1u]; 
 
    /* ya - yc */ 
    s2 = pSrc[(2u * i0) + 1u] - pSrc[(2u * i2) + 1u]; 
 
    /* xc + xd */ 
    t1 = pSrc[2u * i1] + pSrc[2u * i3]; 
 
    /* xa' = xa + xb + xc + xd */ 
    pSrc[2u * i0] = r1 + t1; 
 
    /* (xa + xb) - (xc + xd) */ 
    r1 = r1 - t1; 
 
    /* yb + yd */ 
    t2 = pSrc[(2u * i1) + 1u] + pSrc[(2u * i3) + 1u]; 
 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[(2u * i0) + 1u] = s1 + t2; 
 
    /* (ya + yc) - (yb + yd) */ 
    s1 = s1 - t2; 
 
    /* (yb-yd) */ 
    t1 = pSrc[(2u * i1) + 1u] - pSrc[(2u * i3) + 1u]; 
 
    /* (xb-xd) */ 
    t2 = pSrc[2u * i1] - pSrc[2u * i3]; 
 
    /* xc' = (xa-xb+xc-xd)co2 + (ya-yb+yc-yd)(si2) */ 
    pSrc[2u * i1] = r1; 
 
    /* yc' = (ya-yb+yc-yd)co2 - (xa-xb+xc-xd)(si2) */ 
    pSrc[(2u * i1) + 1u] = s1; 
 
    /* (xa+yb-xc-yd) */ 
    r1 = r2 + t1; 
 
    /* (xa-yb-xc+yd) */ 
    r2 = r2 - t1; 
 
    /* (ya-xb-yc+xd) */ 
    s1 = s2 - t2; 
 
    /* (ya+xb-yc-xd) */ 
    s2 = s2 + t2; 
 
    /* xb' = (xa+yb-xc-yd)co1 + (ya-xb-yc+xd)(si1) */ 
    pSrc[2u * i2] = r1; 
 
    /* yb' = (ya-xb-yc+xd)co1 - (xa+yb-xc-yd)(si1) */ 
    pSrc[(2u * i2) + 1u] = s1; 
 
    /* xd' = (xa-yb-xc+yd)co3 + (ya+xb-yc-xd)(si3) */ 
    pSrc[2u * i3] = r2; 
 
    /* yd' = (ya+xb-yc-xd)co3 - (xa-yb-xc+yd)(si3) */ 
    pSrc[(2u * i3) + 1u] = s2; 
  } 
} 
 
/*  
 * @brief  Core function for the floating-point CIFFT butterfly process. 
 * @param[in, out] *pSrc            points to the in-place buffer of floating-point data type. 
 * @param[in]      fftLen           length of the FFT. 
 * @param[in]      *pCoef           points to twiddle coefficient buffer. 
 * @param[in]      twidCoefModifier twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. 
 * @param[in]      onebyfftLen      value of 1/fftLen. 
 * @return none. 
 */ 
 
void arm_radix4_butterfly_inverse_f32( 
  float32_t * pSrc, 
  uint16_t fftLen, 
  float32_t * pCoef, 
  uint16_t twidCoefModifier, 
  float32_t onebyfftLen) 
{ 
  float32_t co1, co2, co3, si1, si2, si3; 
  float32_t t1, t2, r1, r2, s1, s2; 
  uint32_t ia1, ia2, ia3; 
  uint32_t i0, i1, i2, i3; 
  uint32_t n1, n2, j, k; 
 
  /*  Initializations for the first stage */ 
  n2 = fftLen; 
  n1 = n2; 
 
  /* n2 = fftLen/4 */ 
  n2 >>= 2u; 
  i0 = 0u; 
  ia1 = 0u; 
 
  j = n2; 
 
  /*  Calculation of first stage */ 
  do 
  { 
    /*  index calculation for the input as, */ 
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
    i1 = i0 + n2; 
    i2 = i1 + n2; 
    i3 = i2 + n2; 
 
    /*  Butterfly implementation */ 
    /* xa + xc */ 
    r1 = pSrc[(2u * i0)] + pSrc[(2u * i2)]; 
 
    /* xa - xc */ 
    r2 = pSrc[2u * i0] - pSrc[2u * i2]; 
 
    /* ya + yc */ 
    s1 = pSrc[(2u * i0) + 1u] + pSrc[(2u * i2) + 1u]; 
 
    /* ya - yc */ 
    s2 = pSrc[(2u * i0) + 1u] - pSrc[(2u * i2) + 1u]; 
 
    /* xb + xd */ 
    t1 = pSrc[2u * i1] + pSrc[2u * i3]; 
 
    /* xa' = xa + xb + xc + xd */ 
    pSrc[2u * i0] = r1 + t1; 
 
    /* (xa + xc) - (xb + xd) */ 
    r1 = r1 - t1; 
 
    /* yb + yd */ 
    t2 = pSrc[(2u * i1) + 1u] + pSrc[(2u * i3) + 1u]; 
 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[(2u * i0) + 1u] = s1 + t2; 
 
    /* (ya + yc) - (yb + yd) */ 
    s1 = s1 - t2; 
 
    /* yb - yd */ 
    t1 = pSrc[(2u * i1) + 1u] - pSrc[(2u * i3) + 1u]; 
 
    /* xb - xd */ 
    t2 = pSrc[2u * i1] - pSrc[2u * i3]; 
 
    /*  index calculation for the coefficients */ 
    ia2 = ia1 + ia1; 
    co2 = pCoef[ia2 * 2u]; 
    si2 = pCoef[(ia2 * 2u) + 1u]; 
 
    /* xc' = (xa-xb+xc-xd)co2 - (ya-yb+yc-yd)(si2) */ 
    pSrc[2u * i1] = (r1 * co2) - (s1 * si2); 
 
    /* yc' = (ya-yb+yc-yd)co2 + (xa-xb+xc-xd)(si2) */ 
    pSrc[(2u * i1) + 1u] = (s1 * co2) + (r1 * si2); 
 
    /* (xa - xc) - (yb - yd) */ 
    r1 = r2 - t1; 
 
    /* (xa - xc) + (yb - yd) */ 
    r2 = r2 + t1; 
 
    /* (ya - yc) + (xb - xd) */ 
    s1 = s2 + t2; 
 
    /* (ya - yc) - (xb - xd) */ 
    s2 = s2 - t2; 
 
    co1 = pCoef[ia1 * 2u]; 
    si1 = pCoef[(ia1 * 2u) + 1u]; 
 
    /* xb' = (xa+yb-xc-yd)co1 - (ya-xb-yc+xd)(si1) */ 
    pSrc[2u * i2] = (r1 * co1) - (s1 * si1); 
 
    /* yb' = (ya-xb-yc+xd)co1 + (xa+yb-xc-yd)(si1) */ 
    pSrc[(2u * i2) + 1u] = (s1 * co1) + (r1 * si1); 
 
    /*  index calculation for the coefficients */ 
    ia3 = ia2 + ia1; 
    co3 = pCoef[ia3 * 2u]; 
    si3 = pCoef[(ia3 * 2u) + 1u]; 
 
    /* xd' = (xa-yb-xc+yd)co3 - (ya+xb-yc-xd)(si3) */ 
    pSrc[2u * i3] = (r2 * co3) - (s2 * si3); 
 
    /* yd' = (ya+xb-yc-xd)co3 + (xa-yb-xc+yd)(si3) */ 
    pSrc[(2u * i3) + 1u] = (s2 * co3) + (r2 * si3); 
 
    /*  Twiddle coefficients index modifier */ 
    ia1 = ia1 + twidCoefModifier; 
 
    /*  Updating input index */ 
    i0 = i0 + 1u; 
 
  } 
  while(--j); 
 
  twidCoefModifier <<= 2u; 
 
  /*  Calculation of second stage to excluding last stage */ 
  for (k = fftLen / 4; k > 4u; k >>= 2u) 
  { 
    /*  Initializations for the first stage */ 
    n1 = n2; 
    n2 >>= 2u; 
    ia1 = 0u; 
 
    /*  Calculation of first stage */ 
    for (j = 0u; j <= (n2 - 1u); j++) 
    { 
      /*  index calculation for the coefficients */ 
      ia2 = ia1 + ia1; 
      ia3 = ia2 + ia1; 
      co1 = pCoef[ia1 * 2u]; 
      si1 = pCoef[(ia1 * 2u) + 1u]; 
      co2 = pCoef[ia2 * 2u]; 
      si2 = pCoef[(ia2 * 2u) + 1u]; 
      co3 = pCoef[ia3 * 2u]; 
      si3 = pCoef[(ia3 * 2u) + 1u]; 
 
      /*  Twiddle coefficients index modifier */ 
      ia1 = ia1 + twidCoefModifier; 
 
      for (i0 = j; i0 < fftLen; i0 += n1) 
      { 
        /*  index calculation for the input as, */ 
        /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
        i1 = i0 + n2; 
        i2 = i1 + n2; 
        i3 = i2 + n2; 
 
        /* xa + xc */ 
        r1 = pSrc[(2u * i0)] + pSrc[(2u * i2)]; 
 
        /* xa - xc */ 
        r2 = pSrc[(2u * i0)] - pSrc[(2u * i2)]; 
 
        /* ya + yc */ 
        s1 = pSrc[(2u * i0) + 1u] + pSrc[(2u * i2) + 1u]; 
 
        /* ya - yc */ 
        s2 = pSrc[(2u * i0) + 1u] - pSrc[(2u * i2) + 1u]; 
 
        /* xb + xd */ 
        t1 = pSrc[2u * i1] + pSrc[2u * i3]; 
 
        /* xa' = xa + xb + xc + xd */ 
        pSrc[2u * i0] = r1 + t1; 
 
        /* xa + xc -(xb + xd) */ 
        r1 = r1 - t1; 
 
        /* yb + yd */ 
        t2 = pSrc[(2u * i1) + 1u] + pSrc[(2u * i3) + 1u]; 
 
        /* ya' = ya + yb + yc + yd */ 
        pSrc[(2u * i0) + 1u] = s1 + t2; 
 
        /* (ya + yc) - (yb + yd) */ 
        s1 = s1 - t2; 
 
        /* (yb - yd) */ 
        t1 = pSrc[(2u * i1) + 1u] - pSrc[(2u * i3) + 1u]; 
 
        /* (xb - xd) */ 
        t2 = pSrc[2u * i1] - pSrc[2u * i3]; 
 
        /* xc' = (xa-xb+xc-xd)co2 - (ya-yb+yc-yd)(si2) */ 
        pSrc[2u * i1] = (r1 * co2) - (s1 * si2); 
 
        /* yc' = (ya-yb+yc-yd)co2 + (xa-xb+xc-xd)(si2) */ 
        pSrc[(2u * i1) + 1u] = (s1 * co2) + (r1 * si2); 
 
        /* (xa - xc) - (yb - yd) */ 
        r1 = r2 - t1; 
 
        /* (xa - xc) + (yb - yd) */ 
        r2 = r2 + t1; 
 
        /* (ya - yc) +  (xb - xd) */ 
        s1 = s2 + t2; 
 
        /* (ya - yc) -  (xb - xd) */ 
        s2 = s2 - t2; 
 
        /* xb' = (xa+yb-xc-yd)co1 - (ya-xb-yc+xd)(si1) */ 
        pSrc[2u * i2] = (r1 * co1) - (s1 * si1); 
 
        /* yb' = (ya-xb-yc+xd)co1 + (xa+yb-xc-yd)(si1) */ 
        pSrc[(2u * i2) + 1u] = (s1 * co1) + (r1 * si1); 
 
        /* xd' = (xa-yb-xc+yd)co3 - (ya+xb-yc-xd)(si3) */ 
        pSrc[2u * i3] = (r2 * co3) - (s2 * si3); 
 
        /* yd' = (ya+xb-yc-xd)co3 + (xa-yb-xc+yd)(si3) */ 
        pSrc[(2u * i3) + 1u] = (s2 * co3) + (r2 * si3); 
      } 
    } 
    twidCoefModifier <<= 2u; 
  } 
 
  /*  Initializations of last stage */ 
  n1 = n2; 
  n2 >>= 2u; 
 
  /*  Calculations of last stage */ 
  for (i0 = 0u; i0 <= (fftLen - n1); i0 += n1) 
  { 
    /*  index calculation for the input as, */ 
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
    i1 = i0 + n2; 
    i2 = i1 + n2; 
    i3 = i2 + n2; 
 
    /*  Butterfly implementation */ 
    /* xa + xc */ 
    r1 = pSrc[2u * i0] + pSrc[2u * i2]; 
 
    /* xa - xc */ 
    r2 = pSrc[2u * i0] - pSrc[2u * i2]; 
 
    /* ya + yc */ 
    s1 = pSrc[(2u * i0) + 1u] + pSrc[(2u * i2) + 1u]; 
 
    /* ya - yc */ 
    s2 = pSrc[(2u * i0) + 1u] - pSrc[(2u * i2) + 1u]; 
 
    /* xc + xd */ 
    t1 = pSrc[2u * i1] + pSrc[2u * i3]; 
 
    /* xa' = xa + xb + xc + xd */ 
    pSrc[2u * i0] = (r1 + t1) * onebyfftLen; 
 
    /* (xa + xb) - (xc + xd) */ 
    r1 = r1 - t1; 
 
    /* yb + yd */ 
    t2 = pSrc[(2u * i1) + 1u] + pSrc[(2u * i3) + 1u]; 
 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[(2u * i0) + 1u] = (s1 + t2) * onebyfftLen; 
 
    /* (ya + yc) - (yb + yd) */ 
    s1 = s1 - t2; 
 
    /* (yb-yd) */ 
    t1 = pSrc[(2u * i1) + 1u] - pSrc[(2u * i3) + 1u]; 
 
    /* (xb-xd) */ 
    t2 = pSrc[2u * i1] - pSrc[2u * i3]; 
 
    /* xc' = (xa-xb+xc-xd)co2 - (ya-yb+yc-yd)(si2) */ 
    pSrc[2u * i1] = r1 * onebyfftLen; 
 
    /* yc' = (ya-yb+yc-yd)co2 + (xa-xb+xc-xd)(si2) */ 
    pSrc[(2u * i1) + 1u] = s1 * onebyfftLen; 
 
 
    /* (xa - xc) - (yb-yd) */ 
    r1 = r2 - t1; 
 
    /* (xa - xc) + (yb-yd) */ 
    r2 = r2 + t1; 
 
    /* (ya - yc) + (xb-xd) */ 
    s1 = s2 + t2; 
 
    /* (ya - yc) - (xb-xd) */ 
    s2 = s2 - t2; 
 
    /* xb' = (xa+yb-xc-yd)co1 - (ya-xb-yc+xd)(si1) */ 
    pSrc[2u * i2] = r1 * onebyfftLen; 
 
    /* yb' = (ya-xb-yc+xd)co1 + (xa+yb-xc-yd)(si1) */ 
    pSrc[(2u * i2) + 1u] = s1 * onebyfftLen; 
 
    /* xd' = (xa-yb-xc+yd)co3 - (ya+xb-yc-xd)(si3) */ 
    pSrc[2u * i3] = r2 * onebyfftLen; 
 
    /* yd' = (ya+xb-yc-xd)co3 + (xa-yb-xc+yd)(si3) */ 
    pSrc[(2u * i3) + 1u] = s2 * onebyfftLen; 
  } 
} 
 
/*  
 * @brief  In-place bit reversal function. 
 * @param[in, out] *pSrc        points to the in-place buffer of floating-point data type. 
 * @param[in]      fftSize      length of the FFT. 
 * @param[in]      bitRevFactor bit reversal modifier that supports different size FFTs with the same bit reversal table. 
 * @param[in]      *pBitRevTab  points to the bit reversal table. 
 * @return none. 
 */ 
 
void arm_bitreversal_f32( 
  float32_t * pSrc, 
  uint16_t fftSize, 
  uint16_t bitRevFactor, 
  uint16_t * pBitRevTab) 
{ 
  uint16_t fftLenBy2, fftLenBy2p1; 
  uint16_t i, j; 
  float32_t in; 
 
  /*  Initializations */ 
  j = 0u; 
  fftLenBy2 = fftSize >> 1u; 
  fftLenBy2p1 = (fftSize >> 1u) + 1u; 
 
  /* Bit Reversal Implementation */ 
  for (i = 0u; i <= (fftLenBy2 - 2u); i += 2u) 
  { 
    if(i < j) 
    { 
      /*  pSrc[i] <-> pSrc[j]; */ 
      in = pSrc[2u * i]; 
      pSrc[2u * i] = pSrc[2u * j]; 
      pSrc[2u * j] = in; 
 
      /*  pSrc[i+1u] <-> pSrc[j+1u] */ 
      in = pSrc[(2u * i) + 1u]; 
      pSrc[(2u * i) + 1u] = pSrc[(2u * j) + 1u]; 
      pSrc[(2u * j) + 1u] = in; 
 
      /*  pSrc[i+fftLenBy2p1] <-> pSrc[j+fftLenBy2p1] */ 
      in = pSrc[2u * (i + fftLenBy2p1)]; 
      pSrc[2u * (i + fftLenBy2p1)] = pSrc[2u * (j + fftLenBy2p1)]; 
      pSrc[2u * (j + fftLenBy2p1)] = in; 
 
      /*  pSrc[i+fftLenBy2p1+1u] <-> pSrc[j+fftLenBy2p1+1u] */ 
      in = pSrc[(2u * (i + fftLenBy2p1)) + 1u]; 
      pSrc[(2u * (i + fftLenBy2p1)) + 1u] = 
        pSrc[(2u * (j + fftLenBy2p1)) + 1u]; 
      pSrc[(2u * (j + fftLenBy2p1)) + 1u] = in; 
 
    } 
 
    /*  pSrc[i+1u] <-> pSrc[j+1u] */ 
    in = pSrc[2u * (i + 1u)]; 
    pSrc[2u * (i + 1u)] = pSrc[2u * (j + fftLenBy2)]; 
    pSrc[2u * (j + fftLenBy2)] = in; 
 
    /*  pSrc[i+2u] <-> pSrc[j+2u] */ 
    in = pSrc[(2u * (i + 1u)) + 1u]; 
    pSrc[(2u * (i + 1u)) + 1u] = pSrc[(2u * (j + fftLenBy2)) + 1u]; 
    pSrc[(2u * (j + fftLenBy2)) + 1u] = in; 
 
    /*  Reading the index for the bit reversal */ 
    j = *pBitRevTab; 
 
    /*  Updating the bit reversal index depending on the fft length  */ 
    pBitRevTab += bitRevFactor; 
  } 
} 
