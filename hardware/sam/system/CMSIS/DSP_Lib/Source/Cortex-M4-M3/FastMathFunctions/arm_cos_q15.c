/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_cos_q15.c  
*  
* Description:	Fast cosine calculation for Q15 values. 
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
* -------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupFastMath  
 */ 
 
 /**  
 * @addtogroup cos  
 * @{  
 */ 
 
/** 
* \par  
* Table Values are in Q15(1.15 Fixed point format) and generation is done in three steps  
* \par  
* First Generate cos values in floating point:  
* tableSize = 256;   
* <pre>for(n = -1; n < (tableSize + 1); n++)  
* {  
*	cosTable[n+1]= cos(2*pi*n/tableSize);  
* }</pre> 
* where pi value is  3.14159265358979  
* \par  
* Secondly Convert Floating point to Q15(Fixed point):  
*	round(cosTable[i] * pow(2, 15))  
* \par  
* Finally Rounding to nearest integer is done  
* 	cosTable[i] += (cosTable[i] > 0 ? 0.5 :-0.5);  
*/ 
 
static const q15_t cosTableQ15[259] = { 
  0x7ff6, 0x7fff, 0x7ff6, 0x7fd9, 0x7fa7, 0x7f62, 0x7f0a, 0x7e9d, 
  0x7e1e, 0x7d8a, 0x7ce4, 0x7c2a, 0x7b5d, 0x7a7d, 0x798a, 0x7885, 
  0x776c, 0x7642, 0x7505, 0x73b6, 0x7255, 0x70e3, 0x6f5f, 0x6dca, 
  0x6c24, 0x6a6e, 0x68a7, 0x66d0, 0x64e9, 0x62f2, 0x60ec, 0x5ed7, 
  0x5cb4, 0x5a82, 0x5843, 0x55f6, 0x539b, 0x5134, 0x4ec0, 0x4c40, 
  0x49b4, 0x471d, 0x447b, 0x41ce, 0x3f17, 0x3c57, 0x398d, 0x36ba, 
  0x33df, 0x30fc, 0x2e11, 0x2b1f, 0x2827, 0x2528, 0x2224, 0x1f1a, 
  0x1c0c, 0x18f9, 0x15e2, 0x12c8, 0xfab, 0xc8c, 0x96b, 0x648, 
  0x324, 0x0, 0xfcdc, 0xf9b8, 0xf695, 0xf374, 0xf055, 0xed38, 
  0xea1e, 0xe707, 0xe3f4, 0xe0e6, 0xdddc, 0xdad8, 0xd7d9, 0xd4e1, 
  0xd1ef, 0xcf04, 0xcc21, 0xc946, 0xc673, 0xc3a9, 0xc0e9, 0xbe32, 
  0xbb85, 0xb8e3, 0xb64c, 0xb3c0, 0xb140, 0xaecc, 0xac65, 0xaa0a, 
  0xa7bd, 0xa57e, 0xa34c, 0xa129, 0x9f14, 0x9d0e, 0x9b17, 0x9930, 
  0x9759, 0x9592, 0x93dc, 0x9236, 0x90a1, 0x8f1d, 0x8dab, 0x8c4a, 
  0x8afb, 0x89be, 0x8894, 0x877b, 0x8676, 0x8583, 0x84a3, 0x83d6, 
  0x831c, 0x8276, 0x81e2, 0x8163, 0x80f6, 0x809e, 0x8059, 0x8027, 
  0x800a, 0x8000, 0x800a, 0x8027, 0x8059, 0x809e, 0x80f6, 0x8163, 
  0x81e2, 0x8276, 0x831c, 0x83d6, 0x84a3, 0x8583, 0x8676, 0x877b, 
  0x8894, 0x89be, 0x8afb, 0x8c4a, 0x8dab, 0x8f1d, 0x90a1, 0x9236, 
  0x93dc, 0x9592, 0x9759, 0x9930, 0x9b17, 0x9d0e, 0x9f14, 0xa129, 
  0xa34c, 0xa57e, 0xa7bd, 0xaa0a, 0xac65, 0xaecc, 0xb140, 0xb3c0, 
  0xb64c, 0xb8e3, 0xbb85, 0xbe32, 0xc0e9, 0xc3a9, 0xc673, 0xc946, 
  0xcc21, 0xcf04, 0xd1ef, 0xd4e1, 0xd7d9, 0xdad8, 0xdddc, 0xe0e6, 
  0xe3f4, 0xe707, 0xea1e, 0xed38, 0xf055, 0xf374, 0xf695, 0xf9b8, 
  0xfcdc, 0x0, 0x324, 0x648, 0x96b, 0xc8c, 0xfab, 0x12c8, 
  0x15e2, 0x18f9, 0x1c0c, 0x1f1a, 0x2224, 0x2528, 0x2827, 0x2b1f, 
  0x2e11, 0x30fc, 0x33df, 0x36ba, 0x398d, 0x3c57, 0x3f17, 0x41ce, 
  0x447b, 0x471d, 0x49b4, 0x4c40, 0x4ec0, 0x5134, 0x539b, 0x55f6, 
  0x5843, 0x5a82, 0x5cb4, 0x5ed7, 0x60ec, 0x62f2, 0x64e9, 0x66d0, 
  0x68a7, 0x6a6e, 0x6c24, 0x6dca, 0x6f5f, 0x70e3, 0x7255, 0x73b6, 
  0x7505, 0x7642, 0x776c, 0x7885, 0x798a, 0x7a7d, 0x7b5d, 0x7c2a, 
  0x7ce4, 0x7d8a, 0x7e1e, 0x7e9d, 0x7f0a, 0x7f62, 0x7fa7, 0x7fd9, 
  0x7ff6, 0x7fff, 0x7ff6 
}; 
 
 
/** 
 * @brief Fast approximation to the trigonometric cosine function for Q15 data. 
 * @param[in] x Scaled input value in radians. 
 * @return  cos(x). 
 * 
 * The Q15 input value is in the range [0 +1) and is mapped to a radian value in the range [0 2*pi). 
 */ 
 
q15_t arm_cos_q15( 
  q15_t x) 
{ 
  q31_t cosVal;                                  /* Temporary variables output */ 
  q15_t *tablePtr;                               /* Pointer to table */ 
  q15_t fract, in, in2;                          /* Temporary variables for input, output */ 
  q31_t wa, wb, wc, wd;                          /* Cubic interpolation coefficients */ 
  q15_t a, b, c, d;                              /* Four nearest output values */ 
  q15_t fractCube, fractSquare;                  /* Temporary values for fractional value */ 
  q15_t oneBy6 = 0x1555;                         /* Fixed point value of 1/6 */ 
  q15_t tableSpacing = TABLE_SPACING_Q15;        /* Table spacing */ 
  int32_t index;                                 /* Index variable */ 
 
  in = x; 
 
  /* Calculate the nearest index */ 
  index = (int32_t) in / tableSpacing; 
 
  /* Calculate the nearest value of input */ 
  in2 = (q15_t) index *tableSpacing; 
 
  /* Calculation of fractional value */ 
  fract = (in - in2) << 8; 
 
  /* fractSquare = fract * fract */ 
  fractSquare = (q15_t) ((fract * fract) >> 15); 
 
  /* fractCube = fract * fract * fract */ 
  fractCube = (q15_t) ((fractSquare * fract) >> 15); 
 
  /* Initialise table pointer */ 
  tablePtr = (q15_t *) & cosTableQ15[index]; 
 
  /* Cubic interpolation process */ 
  /* Calculation of wa */ 
  /* wa = -(oneBy6)*fractCube + (fractSquare >> 1u) - (0x2AAA)*fract; */ 
  wa = (q31_t) oneBy6 *fractCube; 
  wa += (q31_t) 0x2AAA * fract; 
  wa = -(wa >> 15); 
  wa += (fractSquare >> 1u); 
 
  /* Read first nearest value of output from the cos table */ 
  a = *tablePtr++; 
 
  /* cosVal = a * wa */ 
  cosVal = a * wa; 
 
  /* Calculation of wb */ 
  wb = (((fractCube >> 1u) - fractSquare) - (fract >> 1u)) + 0x7FFF; 
 
  /* Read second nearest value of output from the cos table */ 
  b = *tablePtr++; 
 
  /*      cosVal += b*wb */ 
  cosVal += b * wb; 
 
  /* Calculation of wc */ 
  wc = -(q31_t) fractCube + fractSquare; 
  wc = (wc >> 1u) + fract; 
 
  /* Read third nearest value of output from the cos table */ 
  c = *tablePtr++; 
 
  /*      cosVal += c*wc */ 
  cosVal += c * wc; 
 
  /* Calculation of wd */ 
  /* wd = (oneBy6)*fractCube - (oneBy6)*fract; */ 
  fractCube = fractCube - fract; 
  wd = ((q15_t) (((q31_t) oneBy6 * fractCube) >> 15)); 
 
  /* Read fourth nearest value of output from the cos table */ 
  d = *tablePtr++; 
 
  /* cosVal += d*wd; */ 
  cosVal += d * wd; 
 
  /* Return the output value in 1.15(q15) format */ 
  return ((q15_t) (cosVal >> 15u)); 
 
} 
 
/**  
 * @} end of cos group  
 */ 
