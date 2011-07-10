/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_cfft_radix4_q15.c  
*  
* Description:	This file has function definition of Radix-4 FFT & IFFT function and  
*				In-place bit reversal using bit reversal table  
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
 * @addtogroup CFFT_CIFFT  
 * @{  
 */ 
 
 
/**  
 * @details  
 * @brief Processing function for the Q15 CFFT/CIFFT. 
 * @param[in]      *S    points to an instance of the Q15 CFFT/CIFFT structure. 
 * @param[in, out] *pSrc points to the complex data buffer. Processing occurs in-place. 
 * @return none. 
 *   
 * \par Input and output formats:  
 * \par  
 * Internally input is downscaled by 2 for every stage to avoid saturations inside CFFT/CIFFT process. 
 * Hence the output format is different for different FFT sizes.  
 * The input and output formats for different FFT sizes and number of bits to upscale are mentioned in the tables below for CFFT and CIFFT: 
 * \par 
 * \image html CFFTQ15.gif "Input and Output Formats for Q15 CFFT"  
 * \image html CIFFTQ15.gif "Input and Output Formats for Q15 CIFFT"  
 */ 
 
void arm_cfft_radix4_q15( 
  const arm_cfft_radix4_instance_q15 * S, 
  q15_t * pSrc) 
{ 
  if(S->ifftFlag == 1u) 
  { 
    /*  Complex IFFT radix-4  */ 
    arm_radix4_butterfly_inverse_q15(pSrc, S->fftLen, S->pTwiddle, 
                                     S->twidCoefModifier); 
  } 
  else 
  { 
    /*  Complex FFT radix-4  */ 
    arm_radix4_butterfly_q15(pSrc, S->fftLen, S->pTwiddle, 
                             S->twidCoefModifier); 
  } 
 
  if(S->bitReverseFlag == 1u) 
  { 
    /*  Bit Reversal */ 
    arm_bitreversal_q15(pSrc, S->fftLen, S->bitRevFactor, S->pBitRevTable); 
  } 
 
} 
 
/**  
 * @} end of CFFT_CIFFT group  
 */ 
 
/*  
* Radix-4 FFT algorithm used is :  
*  
* Input real and imaginary data:  
* x(n) = xa + j * ya  
* x(n+N/4 ) = xb + j * yb  
* x(n+N/2 ) = xc + j * yc  
* x(n+3N 4) = xd + j * yd  
*  
*  
* Output real and imaginary data:  
* x(4r) = xa'+ j * ya'  
* x(4r+1) = xb'+ j * yb'  
* x(4r+2) = xc'+ j * yc'  
* x(4r+3) = xd'+ j * yd'  
*  
*  
* Twiddle factors for radix-4 FFT:  
* Wn = co1 + j * (- si1)  
* W2n = co2 + j * (- si2)  
* W3n = co3 + j * (- si3)  
  
* The real and imaginary output values for the radix-4 butterfly are  
* xa' = xa + xb + xc + xd  
* ya' = ya + yb + yc + yd  
* xb' = (xa+yb-xc-yd)* co1 + (ya-xb-yc+xd)* (si1)  
* yb' = (ya-xb-yc+xd)* co1 - (xa+yb-xc-yd)* (si1)  
* xc' = (xa-xb+xc-xd)* co2 + (ya-yb+yc-yd)* (si2)  
* yc' = (ya-yb+yc-yd)* co2 - (xa-xb+xc-xd)* (si2)  
* xd' = (xa-yb-xc+yd)* co3 + (ya+xb-yc-xd)* (si3)  
* yd' = (ya+xb-yc-xd)* co3 - (xa-yb-xc+yd)* (si3)  
*  
*/ 
 
/**  
 * @brief  Core function for the Q15 CFFT butterfly process. 
 * @param[in, out] *pSrc16          points to the in-place buffer of Q15 data type. 
 * @param[in]      fftLen           length of the FFT. 
 * @param[in]      *pCoef16         points to twiddle coefficient buffer. 
 * @param[in]      twidCoefModifier twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. 
 * @return none. 
 */ 
 
void arm_radix4_butterfly_q15( 
  q15_t * pSrc16, 
  uint32_t fftLen, 
  q15_t * pCoef16, 
  uint32_t twidCoefModifier) 
{ 
  q31_t R, S, T, U; 
  q31_t C1, C2, C3, out1, out2; 
  q31_t *pSrc, *pCoeff; 
  uint32_t n1, n2, ic, i0, i1, i2, i3, j, k; 
  q15_t in; 
 
  /* Total process is divided into three stages */ 
 
  /* process first stage, middle stages, & last stage */ 
 
  /*  pointer initializations for SIMD calculations */ 
  pSrc = (q31_t *) pSrc16; 
  pCoeff = (q31_t *) pCoef16; 
 
  /*  Initializations for the first stage */ 
  n2 = fftLen; 
  n1 = n2; 
 
  /* n2 = fftLen/4 */ 
  n2 >>= 2u; 
 
  /* Index for twiddle coefficient */ 
  ic = 0u; 
 
  /* Index for input read and output write */ 
  i0 = 0u; 
  j = n2; 
 
  /* Input is in 1.15(q15) format */ 
 
  /*  start of first stage process */ 
  do 
  { 
    /*  Butterfly implementation */ 
 
    /*  index calculation for the input as, */ 
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
    i1 = i0 + n2; 
    i2 = i1 + n2; 
    i3 = i2 + n2; 
 
    /*  Reading i0, i0+fftLen/2 inputs */ 
    /* Read ya (real), xa(imag) input */ 
    T = pSrc[i0]; 
    in = ((int16_t) (T & 0xFFFF)) >> 2; 
    T = ((T >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
    /* Read yc (real), xc(imag) input */ 
    S = pSrc[i2]; 
    in = ((int16_t) (S & 0xFFFF)) >> 2; 
    S = ((S >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
    /* R = packed((ya + yc), (xa + xc) ) */ 
    R = __QADD16(T, S); 
    /* S = packed((ya - yc), (xa - xc) ) */ 
    S = __QSUB16(T, S); 
 
    /*  Reading i0+fftLen/4 , i0+3fftLen/4 inputs */ 
    /* Read yb (real), xb(imag) input */ 
    T = pSrc[i1]; 
    in = ((int16_t) (T & 0xFFFF)) >> 2; 
    T = ((T >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
    /* Read yd (real), xd(imag) input */ 
    U = pSrc[i3]; 
    in = ((int16_t) (U & 0xFFFF)) >> 2; 
    U = ((U >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
    /* T = packed((yb + yd), (xb + xd) ) */ 
    T = __QADD16(T, U); 
 
    /*  writing the butterfly processed i0 sample */ 
    /* xa' = xa + xb + xc + xd */ 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[i0] = __SHADD16(R, T); 
 
    /* R = packed((ya + yc) - (yb + yd), (xa + xc)- (xb + xd)) */ 
    R = __QSUB16(R, T); 
 
    /* co2 & si2 are read from SIMD Coefficient pointer */ 
    C2 = pCoeff[2u * ic]; 
 
    /* xc' = (xa-xb+xc-xd)* co2 + (ya-yb+yc-yd)* (si2) */ 
    out1 = __SMUAD(C2, R) >> 16u; 
    /* yc' = (ya-yb+yc-yd)* co2 - (xa-xb+xc-xd)* (si2) */ 
    out2 = __SMUSDX(C2, R); 
 
    /*  Reading i0+fftLen/4 */ 
    /* T = packed(yb, xb) */ 
    T = pSrc[i1]; 
    in = ((int16_t) (T & 0xFFFF)) >> 2; 
    T = ((T >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
 
    /* writing the butterfly processed i0 + fftLen/4 sample */ 
    /* writing output(xc', yc') in little endian format */ 
    pSrc[i1] = (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF); 
 
    /*  Butterfly calculations */ 
    /* U = packed(yd, xd) */ 
    U = pSrc[i3]; 
    in = ((int16_t) (U & 0xFFFF)) >> 2; 
    U = ((U >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
    /* T = packed(yb-yd, xb-xd) */ 
    T = __QSUB16(T, U); 
 
    /* R = packed((ya-yc) + (xb- xd) , (xa-xc) - (yb-yd)) */ 
    R = __QASX(S, T); 
    /* S = packed((ya-yc) - (xb- xd),  (xa-xc) + (yb-yd)) */ 
    S = __QSAX(S, T); 
 
    /* co1 & si1 are read from SIMD Coefficient pointer */ 
    C1 = pCoeff[ic]; 
    /*  Butterfly process for the i0+fftLen/2 sample */ 
    /* xb' = (xa+yb-xc-yd)* co1 + (ya-xb-yc+xd)* (si1) */ 
    out1 = __SMUAD(C1, S) >> 16u; 
    /* yb' = (ya-xb-yc+xd)* co1 - (xa+yb-xc-yd)* (si1) */ 
    out2 = __SMUSDX(C1, S); 
    /* writing output(xb', yb') in little endian format */ 
    pSrc[i2] = ((out2) & 0xFFFF0000) | ((out1) & 0x0000FFFF); 
 
 
    /* co3 & si3 are read from SIMD Coefficient pointer */ 
    C3 = pCoeff[3u * ic]; 
    /*  Butterfly process for the i0+3fftLen/4 sample */ 
    /* xd' = (xa-yb-xc+yd)* co3 + (ya+xb-yc-xd)* (si3) */ 
    out1 = __SMUAD(C3, R) >> 16u; 
    /* yd' = (ya+xb-yc-xd)* co3 - (xa-yb-xc+yd)* (si3) */ 
    out2 = __SMUSDX(C3, R); 
    /* writing output(xd', yd') in little endian format */ 
    pSrc[i3] = ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF); 
 
    /*  Twiddle coefficients index modifier */ 
    ic = ic + twidCoefModifier; 
 
    /*  Updating input index */ 
    i0 = i0 + 1u; 
 
  } while(--j); 
  /* data is in 4.11(q11) format */ 
 
  /* end of first stage process */ 
 
 
  /* start of middle stage process */ 
 
  /*  Twiddle coefficients index modifier */ 
  twidCoefModifier <<= 2u; 
 
  /*  Calculation of Middle stage */ 
  for (k = fftLen / 4u; k > 4u; k >>= 2u) 
  { 
    /*  Initializations for the middle stage */ 
    n1 = n2; 
    n2 >>= 2u; 
    ic = 0u; 
 
    for (j = 0u; j <= (n2 - 1u); j++) 
    { 
      /*  index calculation for the coefficients */ 
      C1 = pCoeff[ic]; 
      C2 = pCoeff[2u * ic]; 
      C3 = pCoeff[3u * ic]; 
 
      /*  Twiddle coefficients index modifier */ 
      ic = ic + twidCoefModifier; 
 
      /*  Butterfly implementation */ 
      for (i0 = j; i0 < fftLen; i0 += n1) 
      { 
        /*  index calculation for the input as, */ 
        /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
        i1 = i0 + n2; 
        i2 = i1 + n2; 
        i3 = i2 + n2; 
 
        /*  Reading i0, i0+fftLen/2 inputs */ 
        /* Read ya (real), xa(imag) input */ 
        T = pSrc[i0]; 
 
        /* Read yc (real), xc(imag) input */ 
        S = pSrc[i2]; 
 
        /* R = packed( (ya + yc), (xa + xc)) */ 
        R = __QADD16(T, S); 
 
        /* S = packed((ya - yc), (xa - xc)) */ 
        S = __QSUB16(T, S); 
 
        /*  Reading i0+fftLen/4 , i0+3fftLen/4 inputs */ 
        /* Read yb (real), xb(imag) input */ 
        T = pSrc[i1]; 
 
        /* Read yd (real), xd(imag) input */ 
        U = pSrc[i3]; 
 
 
        /* T = packed( (yb + yd), (xb + xd)) */ 
        T = __QADD16(T, U); 
 
 
        /*  writing the butterfly processed i0 sample */ 
 
        /* xa' = xa + xb + xc + xd */ 
        /* ya' = ya + yb + yc + yd */ 
        out1 = __SHADD16(R, T); 
        in = ((int16_t) (out1 & 0xFFFF)) >> 1; 
        out1 = ((out1 >> 1) & 0xFFFF0000) | (in & 0xFFFF); 
        pSrc[i0] = out1; 
 
        /* R = packed( (ya + yc) - (yb + yd), (xa + xc) - (xb + xd)) */ 
        R = __SHSUB16(R, T); 
 
        /* (ya-yb+yc-yd)* (si2) + (xa-xb+xc-xd)* co2 */ 
        out1 = __SMUAD(C2, R) >> 16u; 
 
        /* (ya-yb+yc-yd)* co2 - (xa-xb+xc-xd)* (si2) */ 
        out2 = __SMUSDX(C2, R); 
 
        /*  Reading i0+3fftLen/4 */ 
        /* Read yb (real), xb(imag) input */ 
        T = pSrc[i1]; 
 
        /*  writing the butterfly processed i0 + fftLen/4 sample */ 
        /* xc' = (xa-xb+xc-xd)* co2 + (ya-yb+yc-yd)* (si2) */ 
        /* yc' = (ya-yb+yc-yd)* co2 - (xa-xb+xc-xd)* (si2) */ 
        pSrc[i1] = ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF); 
 
        /*  Butterfly calculations */ 
 
        /* Read yd (real), xd(imag) input */ 
        U = pSrc[i3]; 
 
        /* T = packed(yb-yd, xb-xd) */ 
        T = __QSUB16(T, U); 
 
        /* R = packed((ya-yc) + (xb- xd) , (xa-xc) - (yb-yd)) */ 
        R = __SHASX(S, T); 
 
        /* S = packed((ya-yc) - (xb- xd),  (xa-xc) + (yb-yd)) */ 
        S = __SHSAX(S, T); 
 
 
        /*  Butterfly process for the i0+fftLen/2 sample */ 
        out1 = __SMUAD(C1, S) >> 16u; 
        out2 = __SMUSDX(C1, S); 
 
        /* xb' = (xa+yb-xc-yd)* co1 + (ya-xb-yc+xd)* (si1) */ 
        /* yb' = (ya-xb-yc+xd)* co1 - (xa+yb-xc-yd)* (si1) */ 
        pSrc[i2] = ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF); 
 
        /*  Butterfly process for the i0+3fftLen/4 sample */ 
        out1 = __SMUAD(C3, R) >> 16u; 
        out2 = __SMUSDX(C3, R); 
        /* xd' = (xa-yb-xc+yd)* co3 + (ya+xb-yc-xd)* (si3) */ 
        /* yd' = (ya+xb-yc-xd)* co3 - (xa-yb-xc+yd)* (si3) */ 
        pSrc[i3] = ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF); 
      } 
    } 
    /*  Twiddle coefficients index modifier */ 
    twidCoefModifier <<= 2u; 
  } 
  /* end of middle stage process */ 
 
 
  /* data is in 10.6(q6) format for the 1024 point */ 
  /* data is in 8.8(q8) format for the 256 point */ 
  /* data is in 6.10(q10) format for the 64 point */ 
  /* data is in 4.12(q12) format for the 16 point */ 
 
  /*  Initializations for the last stage */ 
  n1 = n2; 
  n2 >>= 2u; 
 
  /* start of last stage process */ 
 
  /*  Butterfly implementation */ 
  for (i0 = 0u; i0 <= (fftLen - n1); i0 += n1) 
  { 
    /*  index calculation for the input as, */ 
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
    i1 = i0 + n2; 
    i2 = i1 + n2; 
    i3 = i2 + n2; 
 
    /*  Reading i0, i0+fftLen/2 inputs */ 
    /* Read ya (real), xa(imag) input */ 
    T = pSrc[i0]; 
    /* Read yc (real), xc(imag) input */ 
    S = pSrc[i2]; 
 
    /* R = packed((ya + yc), (xa + xc)) */ 
    R = __QADD16(T, S); 
    /* S = packed((ya - yc), (xa - xc)) */ 
    S = __QSUB16(T, S); 
 
    /*  Reading i0+fftLen/4 , i0+3fftLen/4 inputs */ 
    /* Read yb (real), xb(imag) input */ 
    T = pSrc[i1]; 
    /* Read yd (real), xd(imag) input */ 
    U = pSrc[i3]; 
 
    /* T = packed((yb + yd), (xb + xd)) */ 
    T = __QADD16(T, U); 
 
    /*  writing the butterfly processed i0 sample */ 
    /* xa' = xa + xb + xc + xd */ 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[i0] = __SHADD16(R, T); 
 
    /* R = packed((ya + yc) - (yb + yd), (xa + xc) - (xb + xd)) */ 
    R = __SHSUB16(R, T); 
 
    /* Read yb (real), xb(imag) input */ 
    T = pSrc[i1]; 
 
    /*  writing the butterfly processed i0 + fftLen/4 sample */ 
    /* xc' = (xa-xb+xc-xd) */ 
    /* yc' = (ya-yb+yc-yd) */ 
    pSrc[i1] = R; 
 
    /* Read yd (real), xd(imag) input */ 
    U = pSrc[i3]; 
    /* T = packed( (yb - yd), (xb - xd))  */ 
    T = __QSUB16(T, U); 
 
    /*  writing the butterfly processed i0 + fftLen/2 sample */ 
    /* xb' = (xa+yb-xc-yd) */ 
    /* yb' = (ya-xb-yc+xd) */ 
    pSrc[i2] = __SHSAX(S, T); 
 
    /*  writing the butterfly processed i0 + 3fftLen/4 sample */ 
    /* xd' = (xa-yb-xc+yd) */ 
    /* yd' = (ya+xb-yc-xd) */ 
    pSrc[i3] = __SHASX(S, T); 
 
  } 
 
  /* end of last stage process */ 
 
  /* output is in 11.5(q5) format for the 1024 point */ 
  /* output is in 9.7(q7) format for the 256 point   */ 
  /* output is in 7.9(q9) format for the 64 point  */ 
  /* output is in 5.11(q11) format for the 16 point  */ 
 
} 
 
 
/**  
 * @brief  Core function for the Q15 CIFFT butterfly process. 
 * @param[in, out] *pSrc16          points to the in-place buffer of Q15 data type. 
 * @param[in]      fftLen           length of the FFT. 
 * @param[in]      *pCoef16         points to twiddle coefficient buffer. 
 * @param[in]      twidCoefModifier twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. 
 * @return none. 
 */ 
 
/*  
* Radix-4 IFFT algorithm used is :  
*  
* CIFFT uses same twiddle coefficients as CFFT function  
*  x[k] = x[n] + (j)k * x[n + fftLen/4] + (-1)k * x[n+fftLen/2] + (-j)k * x[n+3*fftLen/4]  
*  
*  
* IFFT is implemented with following changes in equations from FFT  
*  
* Input real and imaginary data:  
* x(n) = xa + j * ya  
* x(n+N/4 ) = xb + j * yb  
* x(n+N/2 ) = xc + j * yc  
* x(n+3N 4) = xd + j * yd  
*  
*  
* Output real and imaginary data:  
* x(4r) = xa'+ j * ya'  
* x(4r+1) = xb'+ j * yb'  
* x(4r+2) = xc'+ j * yc'  
* x(4r+3) = xd'+ j * yd'  
*  
*  
* Twiddle factors for radix-4 IFFT:  
* Wn = co1 + j * (si1)  
* W2n = co2 + j * (si2)  
* W3n = co3 + j * (si3)  
  
* The real and imaginary output values for the radix-4 butterfly are  
* xa' = xa + xb + xc + xd  
* ya' = ya + yb + yc + yd  
* xb' = (xa-yb-xc+yd)* co1 - (ya+xb-yc-xd)* (si1)  
* yb' = (ya+xb-yc-xd)* co1 + (xa-yb-xc+yd)* (si1)  
* xc' = (xa-xb+xc-xd)* co2 - (ya-yb+yc-yd)* (si2)  
* yc' = (ya-yb+yc-yd)* co2 + (xa-xb+xc-xd)* (si2)  
* xd' = (xa+yb-xc-yd)* co3 - (ya-xb-yc+xd)* (si3)  
* yd' = (ya-xb-yc+xd)* co3 + (xa+yb-xc-yd)* (si3)  
*  
*/ 
 
void arm_radix4_butterfly_inverse_q15( 
  q15_t * pSrc16, 
  uint32_t fftLen, 
  q15_t * pCoef16, 
  uint32_t twidCoefModifier) 
{ 
  q31_t R, S, T, U; 
  q31_t C1, C2, C3, out1, out2; 
  q31_t *pSrc, *pCoeff; 
  uint32_t n1, n2, ic, i0, i1, i2, i3, j, k; 
  q15_t in; 
 
  /* Total process is divided into three stages */ 
 
  /* process first stage, middle stages, & last stage */ 
 
  /*  pointer initializations for SIMD calculations */ 
  pSrc = (q31_t *) pSrc16; 
  pCoeff = (q31_t *) pCoef16; 
 
  /*  Initializations for the first stage */ 
  n2 = fftLen; 
  n1 = n2; 
 
  /* n2 = fftLen/4 */ 
  n2 >>= 2u; 
 
  /* Index for twiddle coefficient */ 
  ic = 0u; 
 
  /* Index for input read and output write */ 
  i0 = 0u; 
 
  j = n2; 
 
  /* Input is in 1.15(q15) format */ 
 
  /*  Start of first stage process */ 
  do 
  { 
    /*  Butterfly implementation */ 
 
    /*  index calculation for the input as, */ 
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
    i1 = i0 + n2; 
    i2 = i1 + n2; 
    i3 = i2 + n2; 
 
    /*  Reading i0, i0+fftLen/2 inputs */ 
    /* Read ya (real), xa(imag) input */ 
    T = pSrc[i0]; 
    in = ((int16_t) (T & 0xFFFF)) >> 2; 
    T = ((T >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
    /* Read yc (real), xc(imag) input */ 
    S = pSrc[i2]; 
    in = ((int16_t) (S & 0xFFFF)) >> 2; 
    S = ((S >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
 
    /* R = packed((ya + yc), (xa + xc) ) */ 
    R = __QADD16(T, S); 
    /* S = packed((ya - yc), (xa - xc) ) */ 
    S = __QSUB16(T, S); 
 
    /*  Reading i0+fftLen/4 , i0+3fftLen/4 inputs */ 
    /* Read yb (real), xb(imag) input */ 
    T = pSrc[i1]; 
    in = ((int16_t) (T & 0xFFFF)) >> 2; 
    T = ((T >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
    /* Read yd (real), xd(imag) input */ 
    U = pSrc[i3]; 
    in = ((int16_t) (U & 0xFFFF)) >> 2; 
    U = ((U >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
 
    /* T = packed((yb + yd), (xb + xd) ) */ 
    T = __QADD16(T, U); 
 
    /*  writing the butterfly processed i0 sample */ 
    /* xa' = xa + xb + xc + xd */ 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[i0] = __SHADD16(R, T); 
 
    /* R = packed((ya + yc) - (yb + yd), (xa + xc)- (xb + xd)) */ 
    R = __QSUB16(R, T); 
    /* co2 & si2 are read from SIMD Coefficient pointer */ 
    C2 = pCoeff[2u * ic]; 
    /* xc' = (xa-xb+xc-xd)* co2 - (ya-yb+yc-yd)* (si2) */ 
    out1 = __SMUSD(C2, R) >> 16u; 
    /* yc' = (ya-yb+yc-yd)* co2 + (xa-xb+xc-xd)* (si2) */ 
    out2 = __SMUADX(C2, R); 
 
    /*  Reading i0+fftLen/4 */ 
    /* T = packed(yb, xb) */ 
    T = pSrc[i1]; 
    in = ((int16_t) (T & 0xFFFF)) >> 2; 
    T = ((T >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
 
    /* writing the butterfly processed i0 + fftLen/4 sample */ 
    /* writing output(xc', yc') in little endian format */ 
    pSrc[i1] = (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF); 
 
    /*  Butterfly calculations */ 
    /* U = packed(yd, xd) */ 
    U = pSrc[i3]; 
    in = ((int16_t) (U & 0xFFFF)) >> 2; 
    U = ((U >> 2) & 0xFFFF0000) | (in & 0xFFFF); 
 
    /* T = packed(yb-yd, xb-xd) */ 
    T = __QSUB16(T, U); 
    /* R = packed((ya-yc) - (xb- xd) , (xa-xc) + (yb-yd)) */ 
    R = __QSAX(S, T); 
    /* S = packed((ya-yc) + (xb- xd),  (xa-xc) - (yb-yd)) */ 
    S = __QASX(S, T); 
 
    /* co1 & si1 are read from SIMD Coefficient pointer */ 
    C1 = pCoeff[ic]; 
    /*  Butterfly process for the i0+fftLen/2 sample */ 
    /* xb' = (xa-yb-xc+yd)* co1 - (ya+xb-yc-xd)* (si1) */ 
    out1 = __SMUSD(C1, S) >> 16u; 
    /* yb' = (ya+xb-yc-xd)* co1 + (xa-yb-xc+yd)* (si1) */ 
    out2 = __SMUADX(C1, S); 
    /* writing output(xb', yb') in little endian format */ 
    pSrc[i2] = ((out2) & 0xFFFF0000) | ((out1) & 0x0000FFFF); 
 
    /* co3 & si3 are read from SIMD Coefficient pointer */ 
    C3 = pCoeff[3u * ic]; 
    /*  Butterfly process for the i0+3fftLen/4 sample */ 
    /* xd' = (xa+yb-xc-yd)* co3 - (ya-xb-yc+xd)* (si3) */ 
    out1 = __SMUSD(C3, R) >> 16u; 
    /* yd' = (ya-xb-yc+xd)* co3 + (xa+yb-xc-yd)* (si3) */ 
    out2 = __SMUADX(C3, R); 
    /* writing output(xd', yd') in little endian format */ 
    pSrc[i3] = ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF); 
 
    /*  Twiddle coefficients index modifier */ 
    ic = ic + twidCoefModifier; 
 
    /*  Updating input index */ 
    i0 = i0 + 1u; 
 
  } while(--j); 
 
  /*  End of first stage process */ 
 
  /* data is in 4.11(q11) format */ 
 
 
  /*  Start of Middle stage process */ 
 
  /*  Twiddle coefficients index modifier */ 
  twidCoefModifier <<= 2u; 
 
  /*  Calculation of Middle stage */ 
  for (k = fftLen / 4u; k > 4u; k >>= 2u) 
  { 
    /*  Initializations for the middle stage */ 
    n1 = n2; 
    n2 >>= 2u; 
    ic = 0u; 
 
    for (j = 0u; j <= (n2 - 1u); j++) 
    { 
      /*  index calculation for the coefficients */ 
      C1 = pCoeff[ic]; 
      C2 = pCoeff[2u * ic]; 
      C3 = pCoeff[3u * ic]; 
 
      /*  Twiddle coefficients index modifier */ 
      ic = ic + twidCoefModifier; 
 
      /*  Butterfly implementation */ 
      for (i0 = j; i0 < fftLen; i0 += n1) 
      { 
        /*  index calculation for the input as, */ 
        /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
        i1 = i0 + n2; 
        i2 = i1 + n2; 
        i3 = i2 + n2; 
 
        /*  Reading i0, i0+fftLen/2 inputs */ 
        /* Read ya (real), xa(imag) input */ 
        T = pSrc[i0]; 
 
        /* Read yc (real), xc(imag) input */ 
        S = pSrc[i2]; 
 
 
        /* R = packed( (ya + yc), (xa + xc)) */ 
        R = __QADD16(T, S); 
        /* S = packed((ya - yc), (xa - xc)) */ 
        S = __QSUB16(T, S); 
 
        /*  Reading i0+fftLen/4 , i0+3fftLen/4 inputs */ 
        /* Read yb (real), xb(imag) input */ 
        T = pSrc[i1]; 
 
        /* Read yd (real), xd(imag) input */ 
        U = pSrc[i3]; 
 
 
        /* T = packed( (yb + yd), (xb + xd)) */ 
        T = __QADD16(T, U); 
 
        /*  writing the butterfly processed i0 sample */ 
        /* xa' = xa + xb + xc + xd */ 
        /* ya' = ya + yb + yc + yd */ 
        out1 = __SHADD16(R, T); 
        in = ((int16_t) (out1 & 0xFFFF)) >> 1; 
        out1 = ((out1 >> 1) & 0xFFFF0000) | (in & 0xFFFF); 
        pSrc[i0] = out1; 
 
 
 
        /* R = packed( (ya + yc) - (yb + yd), (xa + xc) - (xb + xd)) */ 
        R = __SHSUB16(R, T); 
 
        /* (ya-yb+yc-yd)* (si2) - (xa-xb+xc-xd)* co2 */ 
        out1 = __SMUSD(C2, R) >> 16u; 
        /* (ya-yb+yc-yd)* co2 + (xa-xb+xc-xd)* (si2) */ 
        out2 = __SMUADX(C2, R); 
 
        /*  Reading i0+3fftLen/4 */ 
        /* Read yb (real), xb(imag) input */ 
        T = pSrc[i1]; 
 
        /*  writing the butterfly processed i0 + fftLen/4 sample */ 
        /* xc' = (xa-xb+xc-xd)* co2 - (ya-yb+yc-yd)* (si2) */ 
        /* yc' = (ya-yb+yc-yd)* co2 + (xa-xb+xc-xd)* (si2) */ 
        pSrc[i1] = ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF); 
 
        /*  Butterfly calculations */ 
        /* Read yd (real), xd(imag) input */ 
        U = pSrc[i3]; 
 
        /* T = packed(yb-yd, xb-xd) */ 
        T = __QSUB16(T, U); 
 
        /* R = packed((ya-yc) - (xb- xd) , (xa-xc) + (yb-yd)) */ 
        R = __SHSAX(S, T); 
 
        /* S = packed((ya-yc) + (xb- xd),  (xa-xc) - (yb-yd)) */ 
        S = __SHASX(S, T); 
 
        /*  Butterfly process for the i0+fftLen/2 sample */ 
        out1 = __SMUSD(C1, S) >> 16u; 
        out2 = __SMUADX(C1, S); 
        /* xb' = (xa-yb-xc+yd)* co1 - (ya+xb-yc-xd)* (si1) */ 
        /* yb' = (ya+xb-yc-xd)* co1 + (xa-yb-xc+yd)* (si1) */ 
        pSrc[i2] = ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF); 
 
        /*  Butterfly process for the i0+3fftLen/4 sample */ 
        out1 = __SMUSD(C3, R) >> 16u; 
        out2 = __SMUADX(C3, R); 
        /* xd' = (xa+yb-xc-yd)* co3 - (ya-xb-yc+xd)* (si3) */ 
        /* yd' = (ya-xb-yc+xd)* co3 + (xa+yb-xc-yd)* (si3) */ 
        pSrc[i3] = ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF); 
 
 
      } 
    } 
    /*  Twiddle coefficients index modifier */ 
    twidCoefModifier <<= 2u; 
  } 
  /*  End of Middle stages process */ 
 
 
  /* data is in 10.6(q6) format for the 1024 point */ 
  /* data is in 8.8(q8) format for the 256 point   */ 
  /* data is in 6.10(q10) format for the 64 point  */ 
  /* data is in 4.12(q12) format for the 16 point  */ 
 
  /* start of last stage process */ 
 
 
  /*  Initializations for the last stage */ 
  n1 = n2; 
  n2 >>= 2u; 
 
  /*  Butterfly implementation */ 
  for (i0 = 0u; i0 <= (fftLen - n1); i0 += n1) 
  { 
    /*  index calculation for the input as, */ 
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2], pSrc[i0 + 3fftLen/4] */ 
    i1 = i0 + n2; 
    i2 = i1 + n2; 
    i3 = i2 + n2; 
 
    /*  Reading i0, i0+fftLen/2 inputs */ 
    /* Read ya (real), xa(imag) input */ 
    T = pSrc[i0]; 
    /* Read yc (real), xc(imag) input */ 
    S = pSrc[i2]; 
 
    /* R = packed((ya + yc), (xa + xc)) */ 
    R = __QADD16(T, S); 
    /* S = packed((ya - yc), (xa - xc)) */ 
    S = __QSUB16(T, S); 
 
    /*  Reading i0+fftLen/4 , i0+3fftLen/4 inputs */ 
    /* Read yb (real), xb(imag) input */ 
    T = pSrc[i1]; 
    /* Read yd (real), xd(imag) input */ 
    U = pSrc[i3]; 
 
    /* T = packed((yb + yd), (xb + xd)) */ 
    T = __QADD16(T, U); 
 
    /*  writing the butterfly processed i0 sample */ 
    /* xa' = xa + xb + xc + xd */ 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[i0] = __SHADD16(R, T); 
 
    /* R = packed((ya + yc) - (yb + yd), (xa + xc) - (xb + xd)) */ 
    R = __SHSUB16(R, T); 
 
    /* Read yb (real), xb(imag) input */ 
    T = pSrc[i1]; 
 
    /*  writing the butterfly processed i0 + fftLen/4 sample */ 
    /* xc' = (xa-xb+xc-xd) */ 
    /* yc' = (ya-yb+yc-yd) */ 
    pSrc[i1] = R; 
 
    /* Read yd (real), xd(imag) input */ 
    U = pSrc[i3]; 
    /* T = packed( (yb - yd), (xb - xd))  */ 
    T = __QSUB16(T, U); 
 
    /*  writing the butterfly processed i0 + fftLen/2 sample */ 
    /* xb' = (xa-yb-xc+yd) */ 
    /* yb' = (ya+xb-yc-xd) */ 
    pSrc[i2] = __SHASX(S, T); 
 
    /*  writing the butterfly processed i0 + 3fftLen/4 sample */ 
    /* xd' = (xa+yb-xc-yd) */ 
    /* yd' = (ya-xb-yc+xd) */ 
    pSrc[i3] = __SHSAX(S, T); 
  } 
  /* end of last stage  process */ 
 
  /* output is in 11.5(q5) format for the 1024 point */ 
  /* output is in 9.7(q7) format for the 256 point   */ 
  /* output is in 7.9(q9) format for the 64 point  */ 
  /* output is in 5.11(q11) format for the 16 point  */ 
} 
 
 
/*  
   * @brief  In-place bit reversal function. 
   * @param[in, out] *pSrc        points to the in-place buffer of Q15 data type. 
   * @param[in]      fftLen       length of the FFT. 
   * @param[in]      bitRevFactor bit reversal modifier that supports different size FFTs with the same bit reversal table 
   * @param[in]      *pBitRevTab  points to bit reversal table. 
   * @return none. 
 */ 
 
void arm_bitreversal_q15( 
  q15_t * pSrc16, 
  uint32_t fftLen, 
  uint16_t bitRevFactor, 
  uint16_t * pBitRevTab) 
{ 
  q31_t *pSrc = (q31_t *) pSrc16; 
  q31_t in; 
  uint32_t fftLenBy2, fftLenBy2p1; 
  uint32_t i, j; 
 
  /*  Initializations */ 
  j = 0u; 
  fftLenBy2 = fftLen / 2u; 
  fftLenBy2p1 = (fftLen / 2u) + 1u; 
 
  /* Bit Reversal Implementation */ 
  for (i = 0u; i <= (fftLenBy2 - 2u); i += 2u) 
  { 
    if(i < j) 
    { 
      /*  pSrc[i] <-> pSrc[j]; */ 
      /*  pSrc[i+1u] <-> pSrc[j+1u] */ 
      in = pSrc[i]; 
      pSrc[i] = pSrc[j]; 
      pSrc[j] = in; 
 
      /*  pSrc[i + fftLenBy2p1] <-> pSrc[j + fftLenBy2p1];  */ 
      /*  pSrc[i + fftLenBy2p1+1u] <-> pSrc[j + fftLenBy2p1+1u] */ 
      in = pSrc[i + fftLenBy2p1]; 
      pSrc[i + fftLenBy2p1] = pSrc[j + fftLenBy2p1]; 
      pSrc[j + fftLenBy2p1] = in; 
    } 
 
    /*  pSrc[i+1u] <-> pSrc[j+fftLenBy2];         */ 
    /*  pSrc[i+2] <-> pSrc[j+fftLenBy2+1u]  */ 
    in = pSrc[i + 1u]; 
    pSrc[i + 1u] = pSrc[j + fftLenBy2]; 
    pSrc[j + fftLenBy2] = in; 
 
    /*  Reading the index for the bit reversal */ 
    j = *pBitRevTab; 
 
    /*  Updating the bit reversal index depending on the fft length  */ 
    pBitRevTab += bitRevFactor; 
  } 
} 
