/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_cfft_radix4_q31.c  
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
 * @brief Processing function for the Q31 CFFT/CIFFT.  
 * @param[in]      *S    points to an instance of the Q31 CFFT/CIFFT structure. 
 * @param[in, out] *pSrc points to the complex data buffer of size <code>2*fftLen</code>. Processing occurs in-place. 
 * @return none.  
 *   
 * \par Input and output formats:  
 * \par  
 * Internally input is downscaled by 2 for every stage to avoid saturations inside CFFT/CIFFT process. 
 * Hence the output format is different for different FFT sizes.  
 * The input and output formats for different FFT sizes and number of bits to upscale are mentioned in the tables below for CFFT and CIFFT: 
 * \par 
 * \image html CFFTQ31.gif "Input and Output Formats for Q31 CFFT"  
 * \image html CIFFTQ31.gif "Input and Output Formats for Q31 CIFFT"  
 *  
 */ 
 
void arm_cfft_radix4_q31( 
  const arm_cfft_radix4_instance_q31 * S, 
  q31_t * pSrc) 
{ 
  if(S->ifftFlag == 1u) 
  { 
    /* Complex IFFT radix-4 */ 
    arm_radix4_butterfly_inverse_q31(pSrc, S->fftLen, S->pTwiddle, 
                                     S->twidCoefModifier); 
  } 
  else 
  { 
    /* Complex FFT radix-4 */ 
    arm_radix4_butterfly_q31(pSrc, S->fftLen, S->pTwiddle, 
                             S->twidCoefModifier); 
  } 
 
 
  if(S->bitReverseFlag == 1u) 
  { 
    /*  Bit Reversal */ 
    arm_bitreversal_q31(pSrc, S->fftLen, S->bitRevFactor, S->pBitRevTable); 
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
*  
*  Butterfly implementation:  
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
 * @brief  Core function for the Q31 CFFT butterfly process. 
 * @param[in, out] *pSrc            points to the in-place buffer of Q31 data type. 
 * @param[in]      fftLen           length of the FFT. 
 * @param[in]      *pCoef           points to twiddle coefficient buffer. 
 * @param[in]      twidCoefModifier twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. 
 * @return none. 
 */ 
 
void arm_radix4_butterfly_q31( 
  q31_t * pSrc, 
  uint32_t fftLen, 
  q31_t * pCoef, 
  uint32_t twidCoefModifier) 
{ 
  uint32_t n1, n2, ia1, ia2, ia3, i0, i1, i2, i3, j, k; 
  q31_t t1, t2, r1, r2, s1, s2, co1, co2, co3, si1, si2, si3; 
 
 
  /* Total process is divided into three stages */ 
 
  /* process first stage, middle stages, & last stage */ 
 
 
  /* start of first stage process */ 
 
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
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2u], pSrc[i0 + 3fftLen/4] */ 
    i1 = i0 + n2; 
    i2 = i1 + n2; 
    i3 = i2 + n2; 
 
    /* input is in 1.31(q31) format and provide 4 guard bits for the input */ 
 
    /*  Butterfly implementation */ 
    /* xa + xc */ 
    r1 = (pSrc[(2u * i0)] >> 4u) + (pSrc[(2u * i2)] >> 4u); 
    /* xa - xc */ 
    r2 = (pSrc[2u * i0] >> 4u) - (pSrc[2u * i2] >> 4u); 
 
    /* ya + yc */ 
    s1 = (pSrc[(2u * i0) + 1u] >> 4u) + (pSrc[(2u * i2) + 1u] >> 4u); 
    /* ya - yc */ 
    s2 = (pSrc[(2u * i0) + 1u] >> 4u) - (pSrc[(2u * i2) + 1u] >> 4u); 
 
    /* xb + xd */ 
    t1 = (pSrc[2u * i1] >> 4u) + (pSrc[2u * i3] >> 4u); 
 
    /* xa' = xa + xb + xc + xd */ 
    pSrc[2u * i0] = (r1 + t1); 
    /* (xa + xc) - (xb + xd) */ 
    r1 = r1 - t1; 
    /* yb + yd */ 
    t2 = (pSrc[(2u * i1) + 1u] >> 4u) + (pSrc[(2u * i3) + 1u] >> 4u); 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[(2u * i0) + 1u] = (s1 + t2); 
 
    /* (ya + yc) - (yb + yd) */ 
    s1 = s1 - t2; 
 
    /* yb - yd */ 
    t1 = (pSrc[(2u * i1) + 1u] >> 4u) - (pSrc[(2u * i3) + 1u] >> 4u); 
    /* xb - xd */ 
    t2 = (pSrc[2u * i1] >> 4u) - (pSrc[2u * i3] >> 4u); 
 
    /*  index calculation for the coefficients */ 
    ia2 = 2u * ia1; 
    co2 = pCoef[ia2 * 2u]; 
    si2 = pCoef[(ia2 * 2u) + 1u]; 
 
    /* xc' = (xa-xb+xc-xd)co2 + (ya-yb+yc-yd)(si2) */ 
    pSrc[2u * i1] = (((int32_t) (((q63_t) r1 * co2) >> 32)) + 
                     ((int32_t) (((q63_t) s1 * si2) >> 32))) << 1u; 
 
    /* yc' = (ya-yb+yc-yd)co2 - (xa-xb+xc-xd)(si2) */ 
    pSrc[(2u * i1) + 1u] = (((int32_t) (((q63_t) s1 * co2) >> 32)) - 
                            ((int32_t) (((q63_t) r1 * si2) >> 32))) << 1u; 
 
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
    pSrc[2u * i2] = (((int32_t) (((q63_t) r1 * co1) >> 32)) + 
                     ((int32_t) (((q63_t) s1 * si1) >> 32))) << 1u; 
 
    /* yb' = (ya-xb-yc+xd)co1 - (xa+yb-xc-yd)(si1) */ 
    pSrc[(2u * i2) + 1u] = (((int32_t) (((q63_t) s1 * co1) >> 32)) - 
                            ((int32_t) (((q63_t) r1 * si1) >> 32))) << 1u; 
 
    /*  index calculation for the coefficients */ 
    ia3 = 3u * ia1; 
    co3 = pCoef[ia3 * 2u]; 
    si3 = pCoef[(ia3 * 2u) + 1u]; 
 
    /* xd' = (xa-yb-xc+yd)co3 + (ya+xb-yc-xd)(si3) */ 
    pSrc[2u * i3] = (((int32_t) (((q63_t) r2 * co3) >> 32)) + 
                     ((int32_t) (((q63_t) s2 * si3) >> 32))) << 1u; 
 
    /* yd' = (ya+xb-yc-xd)co3 - (xa-yb-xc+yd)(si3) */ 
    pSrc[(2u * i3) + 1u] = (((int32_t) (((q63_t) s2 * co3) >> 32)) - 
                            ((int32_t) (((q63_t) r2 * si3) >> 32))) << 1u; 
 
    /*  Twiddle coefficients index modifier */ 
    ia1 = ia1 + twidCoefModifier; 
 
    /*  Updating input index */ 
    i0 = i0 + 1u; 
 
  } while(--j); 
 
  /* end of first stage process */ 
 
  /* data is in 5.27(q27) format */ 
 
 
  /* start of Middle stages process */ 
 
 
  /* each stage in middle stages provides two down scaling of the input */ 
 
  twidCoefModifier <<= 2u; 
 
 
  for (k = fftLen / 4u; k > 4u; k >>= 2u) 
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
        /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2u], pSrc[i0 + 3fftLen/4] */ 
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
 
        /* xb + xd */ 
        t1 = pSrc[2u * i1] + pSrc[2u * i3]; 
 
        /* xa' = xa + xb + xc + xd */ 
        pSrc[2u * i0] = (r1 + t1) >> 2u; 
        /* xa + xc -(xb + xd) */ 
        r1 = r1 - t1; 
 
        /* yb + yd */ 
        t2 = pSrc[(2u * i1) + 1u] + pSrc[(2u * i3) + 1u]; 
        /* ya' = ya + yb + yc + yd */ 
        pSrc[(2u * i0) + 1u] = (s1 + t2) >> 2u; 
 
        /* (ya + yc) - (yb + yd) */ 
        s1 = s1 - t2; 
 
        /* (yb - yd) */ 
        t1 = pSrc[(2u * i1) + 1u] - pSrc[(2u * i3) + 1u]; 
        /* (xb - xd) */ 
        t2 = pSrc[2u * i1] - pSrc[2u * i3]; 
 
        /* xc' = (xa-xb+xc-xd)co2 + (ya-yb+yc-yd)(si2) */ 
        pSrc[2u * i1] = (((int32_t) (((q63_t) r1 * co2) >> 32)) + 
                         ((int32_t) (((q63_t) s1 * si2) >> 32))) >> 1u; 
 
        /* yc' = (ya-yb+yc-yd)co2 - (xa-xb+xc-xd)(si2) */ 
        pSrc[(2u * i1) + 1u] = (((int32_t) (((q63_t) s1 * co2) >> 32)) - 
                                ((int32_t) (((q63_t) r1 * si2) >> 32))) >> 1u; 
 
        /* (xa - xc) + (yb - yd) */ 
        r1 = r2 + t1; 
        /* (xa - xc) - (yb - yd) */ 
        r2 = r2 - t1; 
 
        /* (ya - yc) -  (xb - xd) */ 
        s1 = s2 - t2; 
        /* (ya - yc) +  (xb - xd) */ 
        s2 = s2 + t2; 
 
        /* xb' = (xa+yb-xc-yd)co1 + (ya-xb-yc+xd)(si1) */ 
        pSrc[2u * i2] = (((int32_t) (((q63_t) r1 * co1) >> 32)) + 
                         ((int32_t) (((q63_t) s1 * si1) >> 32))) >> 1u; 
 
        /* yb' = (ya-xb-yc+xd)co1 - (xa+yb-xc-yd)(si1) */ 
        pSrc[(2u * i2) + 1u] = (((int32_t) (((q63_t) s1 * co1) >> 32)) - 
                                ((int32_t) (((q63_t) r1 * si1) >> 32))) >> 1u; 
 
        /* xd' = (xa-yb-xc+yd)co3 + (ya+xb-yc-xd)(si3) */ 
        pSrc[2u * i3] = (((int32_t) (((q63_t) r2 * co3) >> 32)) + 
                         ((int32_t) (((q63_t) s2 * si3) >> 32))) >> 1u; 
 
        /* yd' = (ya+xb-yc-xd)co3 - (xa-yb-xc+yd)(si3) */ 
        pSrc[(2u * i3) + 1u] = (((int32_t) (((q63_t) s2 * co3) >> 32)) - 
                                ((int32_t) (((q63_t) r2 * si3) >> 32))) >> 1u; 
      } 
    } 
    twidCoefModifier <<= 2u; 
  } 
 
  /* End of Middle stages process */ 
 
  /* data is in 11.21(q21) format for the 1024 point as there are 3 middle stages */ 
  /* data is in 9.23(q23) format for the 256 point as there are 2 middle stages */ 
  /* data is in 7.25(q25) format for the 64 point as there are 1 middle stage */ 
  /* data is in 5.27(q27) format for the 16 point as there are no middle stages */ 
 
 
  /* start of Last stage process */ 
 
  /*  Initializations of last stage */ 
  n1 = n2; 
  n2 >>= 2u; 
 
  /*  Calculations of last stage */ 
  for (i0 = 0u; i0 <= (fftLen - n1); i0 += n1) 
  { 
    /*  index calculation for the input as, */ 
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2u], pSrc[i0 + 3fftLen/4] */ 
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
    pSrc[2u * i0] = (r1 + t1); 
    /* (xa + xb) - (xc + xd) */ 
    r1 = r1 - t1; 
 
    /* yb + yd */ 
    t2 = pSrc[(2u * i1) + 1u] + pSrc[(2u * i3) + 1u]; 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[(2u * i0) + 1u] = (s1 + t2); 
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
 
  /* output is in 11.21(q21) format for the 1024 point */ 
  /* output is in 9.23(q23) format for the 256 point */ 
  /* output is in 7.25(q25) format for the 64 point */ 
  /* output is in 5.27(q27) format for the 16 point */ 
 
  /* End of last stage process */ 
 
} 
 
 
/**  
 * @brief  Core function for the Q31 CIFFT butterfly process. 
 * @param[in, out] *pSrc            points to the in-place buffer of Q31 data type. 
 * @param[in]      fftLen           length of the FFT. 
 * @param[in]      *pCoef           points to twiddle coefficient buffer. 
 * @param[in]      twidCoefModifier twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. 
 * @return none. 
 */ 
 
 
/*  
* Radix-4 IFFT algorithm used is :  
*  
* CIFFT uses same twiddle coefficients as CFFT Function  
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
 
void arm_radix4_butterfly_inverse_q31( 
  q31_t * pSrc, 
  uint32_t fftLen, 
  q31_t * pCoef, 
  uint32_t twidCoefModifier) 
{ 
  uint32_t n1, n2, ia1, ia2, ia3, i0, i1, i2, i3, j, k; 
  q31_t t1, t2, r1, r2, s1, s2, co1, co2, co3, si1, si2, si3; 
 
  /* input is be 1.31(q31) format for all FFT sizes */ 
  /* Total process is divided into three stages */ 
  /* process first stage, middle stages, & last stage */ 
 
  /* Start of first stage process */ 
 
  /* Initializations for the first stage */ 
  n2 = fftLen; 
  n1 = n2; 
  /* n2 = fftLen/4 */ 
  n2 >>= 2u; 
  i0 = 0u; 
  ia1 = 0u; 
 
  j = n2; 
 
  do 
  { 
 
    /* input is in 1.31(q31) format and provide 4 guard bits for the input */ 
 
    /*  index calculation for the input as, */ 
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2u], pSrc[i0 + 3fftLen/4] */ 
    i1 = i0 + n2; 
    i2 = i1 + n2; 
    i3 = i2 + n2; 
 
    /*  Butterfly implementation */ 
    /* xa + xc */ 
    r1 = (pSrc[2u * i0] >> 4u) + (pSrc[2u * i2] >> 4u); 
    /* xa - xc */ 
    r2 = (pSrc[2u * i0] >> 4u) - (pSrc[2u * i2] >> 4u); 
 
    /* ya + yc */ 
    s1 = (pSrc[(2u * i0) + 1u] >> 4u) + (pSrc[(2u * i2) + 1u] >> 4u); 
    /* ya - yc */ 
    s2 = (pSrc[(2u * i0) + 1u] >> 4u) - (pSrc[(2u * i2) + 1u] >> 4u); 
 
    /* xb + xd */ 
    t1 = (pSrc[2u * i1] >> 4u) + (pSrc[2u * i3] >> 4u); 
 
    /* xa' = xa + xb + xc + xd */ 
    pSrc[2u * i0] = (r1 + t1); 
    /* (xa + xc) - (xb + xd) */ 
    r1 = r1 - t1; 
    /* yb + yd */ 
    t2 = (pSrc[(2u * i1) + 1u] >> 4u) + (pSrc[(2u * i3) + 1u] >> 4u); 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[(2u * i0) + 1u] = (s1 + t2); 
 
    /* (ya + yc) - (yb + yd) */ 
    s1 = s1 - t2; 
 
    /* yb - yd */ 
    t1 = (pSrc[(2u * i1) + 1u] >> 4u) - (pSrc[(2u * i3) + 1u] >> 4u); 
    /* xb - xd */ 
    t2 = (pSrc[2u * i1] >> 4u) - (pSrc[2u * i3] >> 4u); 
 
    /*  index calculation for the coefficients */ 
    ia2 = 2u * ia1; 
    co2 = pCoef[ia2 * 2u]; 
    si2 = pCoef[(ia2 * 2u) + 1u]; 
 
    /* xc' = (xa-xb+xc-xd)co2 - (ya-yb+yc-yd)(si2) */ 
    pSrc[2u * i1] = (((int32_t) (((q63_t) r1 * co2) >> 32)) - 
                     ((int32_t) (((q63_t) s1 * si2) >> 32))) << 1u; 
 
    /* yc' = (ya-yb+yc-yd)co2 + (xa-xb+xc-xd)(si2) */ 
    pSrc[2u * i1 + 1u] = (((int32_t) (((q63_t) s1 * co2) >> 32)) + 
                          ((int32_t) (((q63_t) r1 * si2) >> 32))) << 1u; 
 
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
    pSrc[2u * i2] = (((int32_t) (((q63_t) r1 * co1) >> 32)) - 
                     ((int32_t) (((q63_t) s1 * si1) >> 32))) << 1u; 
 
    /* yb' = (ya-xb-yc+xd)co1 + (xa+yb-xc-yd)(si1) */ 
    pSrc[(2u * i2) + 1u] = (((int32_t) (((q63_t) s1 * co1) >> 32)) + 
                            ((int32_t) (((q63_t) r1 * si1) >> 32))) << 1u; 
 
    /*  index calculation for the coefficients */ 
    ia3 = 3u * ia1; 
    co3 = pCoef[ia3 * 2u]; 
    si3 = pCoef[(ia3 * 2u) + 1u]; 
 
    /* xd' = (xa-yb-xc+yd)co3 - (ya+xb-yc-xd)(si3) */ 
    pSrc[2u * i3] = (((int32_t) (((q63_t) r2 * co3) >> 32)) - 
                     ((int32_t) (((q63_t) s2 * si3) >> 32))) << 1u; 
 
    /* yd' = (ya+xb-yc-xd)co3 + (xa-yb-xc+yd)(si3) */ 
    pSrc[(2u * i3) + 1u] = (((int32_t) (((q63_t) s2 * co3) >> 32)) + 
                            ((int32_t) (((q63_t) r2 * si3) >> 32))) << 1u; 
 
    /*  Twiddle coefficients index modifier */ 
    ia1 = ia1 + twidCoefModifier; 
 
    /*  Updating input index */ 
    i0 = i0 + 1u; 
 
  } while(--j); 
 
  /* data is in 5.27(q27) format */ 
  /* each stage provides two down scaling of the input */ 
 
 
  /* Start of Middle stages process */ 
 
  twidCoefModifier <<= 2u; 
 
  /*  Calculation of second stage to excluding last stage */ 
  for (k = fftLen / 4u; k > 4u; k >>= 2u) 
  { 
    /*  Initializations for the first stage */ 
    n1 = n2; 
    n2 >>= 2u; 
    ia1 = 0u; 
 
    for (j = 0; j <= (n2 - 1u); j++) 
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
        /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2u], pSrc[i0 + 3fftLen/4] */ 
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
 
        /* xb + xd */ 
        t1 = pSrc[2u * i1] + pSrc[2u * i3]; 
 
        /* xa' = xa + xb + xc + xd */ 
        pSrc[2u * i0] = (r1 + t1) >> 2u; 
        /* xa + xc -(xb + xd) */ 
        r1 = r1 - t1; 
        /* yb + yd */ 
        t2 = pSrc[(2u * i1) + 1u] + pSrc[(2u * i3) + 1u]; 
        /* ya' = ya + yb + yc + yd */ 
        pSrc[(2u * i0) + 1u] = (s1 + t2) >> 2u; 
 
        /* (ya + yc) - (yb + yd) */ 
        s1 = s1 - t2; 
 
        /* (yb - yd) */ 
        t1 = pSrc[(2u * i1) + 1u] - pSrc[(2u * i3) + 1u]; 
        /* (xb - xd) */ 
        t2 = pSrc[2u * i1] - pSrc[2u * i3]; 
 
        /* xc' = (xa-xb+xc-xd)co2 - (ya-yb+yc-yd)(si2) */ 
        pSrc[2u * i1] = (((int32_t) (((q63_t) r1 * co2) >> 32u)) - 
                         ((int32_t) (((q63_t) s1 * si2) >> 32u))) >> 1u; 
 
        /* yc' = (ya-yb+yc-yd)co2 + (xa-xb+xc-xd)(si2) */ 
        pSrc[(2u * i1) + 1u] = 
          (((int32_t) (((q63_t) s1 * co2) >> 32u)) + 
           ((int32_t) (((q63_t) r1 * si2) >> 32u))) >> 1u; 
 
        /* (xa - xc) - (yb - yd) */ 
        r1 = r2 - t1; 
        /* (xa - xc) + (yb - yd) */ 
        r2 = r2 + t1; 
 
        /* (ya - yc) +  (xb - xd) */ 
        s1 = s2 + t2; 
        /* (ya - yc) -  (xb - xd) */ 
        s2 = s2 - t2; 
 
        /* xb' = (xa+yb-xc-yd)co1 - (ya-xb-yc+xd)(si1) */ 
        pSrc[2u * i2] = (((int32_t) (((q63_t) r1 * co1) >> 32)) - 
                         ((int32_t) (((q63_t) s1 * si1) >> 32))) >> 1u; 
 
        /* yb' = (ya-xb-yc+xd)co1 + (xa+yb-xc-yd)(si1) */ 
        pSrc[(2u * i2) + 1u] = (((int32_t) (((q63_t) s1 * co1) >> 32)) + 
                                ((int32_t) (((q63_t) r1 * si1) >> 32))) >> 1u; 
 
        /* xd' = (xa-yb-xc+yd)co3 - (ya+xb-yc-xd)(si3) */ 
        pSrc[(2u * i3)] = (((int32_t) (((q63_t) r2 * co3) >> 32)) - 
                           ((int32_t) (((q63_t) s2 * si3) >> 32))) >> 1u; 
 
        /* yd' = (ya+xb-yc-xd)co3 + (xa-yb-xc+yd)(si3) */ 
        pSrc[(2u * i3) + 1u] = (((int32_t) (((q63_t) s2 * co3) >> 32)) + 
                                ((int32_t) (((q63_t) r2 * si3) >> 32))) >> 1u; 
      } 
    } 
    twidCoefModifier <<= 2u; 
  } 
 
  /* End of Middle stages process */ 
 
  /* data is in 11.21(q21) format for the 1024 point as there are 3 middle stages */ 
  /* data is in 9.23(q23) format for the 256 point as there are 2 middle stages */ 
  /* data is in 7.25(q25) format for the 64 point as there are 1 middle stage */ 
  /* data is in 5.27(q27) format for the 16 point as there are no middle stages */ 
 
 
  /* Start of last stage process */ 
 
 
  /*  Initializations of last stage */ 
  n1 = n2; 
  n2 >>= 2u; 
 
  /*  Calculations of last stage */ 
  for (i0 = 0u; i0 <= (fftLen - n1); i0 += n1) 
  { 
    /*  index calculation for the input as, */ 
    /*  pSrc[i0 + 0], pSrc[i0 + fftLen/4], pSrc[i0 + fftLen/2u], pSrc[i0 + 3fftLen/4] */ 
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
    pSrc[2u * i0] = (r1 + t1); 
    /* (xa + xb) - (xc + xd) */ 
    r1 = r1 - t1; 
 
    /* yb + yd */ 
    t2 = pSrc[(2u * i1) + 1u] + pSrc[(2u * i3) + 1u]; 
    /* ya' = ya + yb + yc + yd */ 
    pSrc[(2u * i0) + 1u] = (s1 + t2); 
    /* (ya + yc) - (yb + yd) */ 
    s1 = s1 - t2; 
 
    /* (yb-yd) */ 
    t1 = pSrc[(2u * i1) + 1u] - pSrc[(2u * i3) + 1u]; 
    /* (xb-xd) */ 
    t2 = pSrc[2u * i1] - pSrc[2u * i3]; 
 
    /* xc' = (xa-xb+xc-xd)co2 - (ya-yb+yc-yd)(si2) */ 
    pSrc[2u * i1] = r1; 
    /* yc' = (ya-yb+yc-yd)co2 + (xa-xb+xc-xd)(si2) */ 
    pSrc[(2u * i1) + 1u] = s1; 
 
    /* (xa - xc) - (yb-yd) */ 
    r1 = r2 - t1; 
 
    /* (xa - xc) + (yb-yd) */ 
    r2 = r2 + t1; 
 
    /* (ya - yc) + (xb-xd) */ 
    s1 = s2 + t2; 
 
    /* (ya - yc) - (xb-xd) */ 
    s2 = s2 - t2; 
 
    /* xb' = (xa+yb-xc-yd)co1 - (ya-xb-yc+xd)(si1) */ 
    pSrc[2u * i2] = r1; 
    /* yb' = (ya-xb-yc+xd)co1 + (xa+yb-xc-yd)(si1) */ 
    pSrc[(2u * i2) + 1u] = s1; 
 
    /* xd' = (xa-yb-xc+yd)co3 - (ya+xb-yc-xd)(si3) */ 
    pSrc[2u * i3] = r2; 
    /* yd' = (ya+xb-yc-xd)co3 + (xa-yb-xc+yd)(si3) */ 
    pSrc[(2u * i3) + 1u] = s2; 
 
  } 
 
  /* output is in 11.21(q21) format for the 1024 point */ 
  /* output is in 9.23(q23) format for the 256 point */ 
  /* output is in 7.25(q25) format for the 64 point */ 
  /* output is in 5.27(q27) format for the 16 point */ 
 
  /* End of last stage process */ 
} 
 
 
/*  
 * @brief  In-place bit reversal function. 
 * @param[in, out] *pSrc        points to the in-place buffer of Q31 data type. 
 * @param[in]      fftLen       length of the FFT. 
 * @param[in]      bitRevFactor bit reversal modifier that supports different size FFTs with the same bit reversal table 
 * @param[in]      *pBitRevTab  points to bit reversal table. 
 * @return none. 
 */ 
 
void arm_bitreversal_q31( 
  q31_t * pSrc, 
  uint32_t fftLen, 
  uint16_t bitRevFactor, 
  uint16_t * pBitRevTable) 
{ 
  uint32_t fftLenBy2, fftLenBy2p1, i, j; 
  q31_t in; 
 
  /*  Initializations      */ 
  j = 0u; 
  fftLenBy2 = fftLen / 2u; 
  fftLenBy2p1 = (fftLen / 2u) + 1u; 
 
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
    j = *pBitRevTable; 
 
    /*  Updating the bit reversal index depending on the fft length */ 
    pBitRevTable += bitRevFactor; 
  } 
} 
