/* ----------------------------------------------------------------------  
* Copyright (C) 2010 ARM Limited. All rights reserved.  
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:		arm_conv_q15.c  
*  
* Description:	Q15 Convolution.  
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
*  
* -------------------------------------------------------------------- */ 
 
#include "arm_math.h" 
 
/**  
 * @ingroup groupFilters  
 */ 
 
/**  
 * @addtogroup Conv  
 * @{  
 */ 
 
/**  
 * @brief Convolution of Q15 sequences.  
 * @param[in] *pSrcA points to the first input sequence.  
 * @param[in] srcALen length of the first input sequence.  
 * @param[in] *pSrcB points to the second input sequence.  
 * @param[in] srcBLen length of the second input sequence.  
 * @param[out] *pDst points to the location where the output result is written.  Length srcALen+srcBLen-1.  
 * @return none.  
 *  
 * @details  
 * <b>Scaling and Overflow Behavior:</b>  
 *  
 * \par  
 * The function is implemented using a 64-bit internal accumulator.  
 * Both inputs are in 1.15 format and multiplications yield a 2.30 result.  
 * The 2.30 intermediate results are accumulated in a 64-bit accumulator in 34.30 format.  
 * This approach provides 33 guard bits and there is no risk of overflow.  
 * The 34.30 result is then truncated to 34.15 format by discarding the low 15 bits and then saturated to 1.15 format.  
 *  
 * \par  
 * Refer to <code>arm_conv_fast_q15()</code> for a faster but less precise version of this function.  
 */ 
 
void arm_conv_q15( 
  q15_t * pSrcA, 
  uint32_t srcALen, 
  q15_t * pSrcB, 
  uint32_t srcBLen, 
  q15_t * pDst) 
{ 
  q15_t *pIn1;                                   /* inputA pointer */ 
  q15_t *pIn2;                                   /* inputB pointer */ 
  q15_t *pOut = pDst;                            /* output pointer */ 
  q63_t sum, acc0, acc1, acc2, acc3;             /* Accumulator */ 
  q15_t *px;                                     /* Intermediate inputA pointer  */ 
  q15_t *py;                                     /* Intermediate inputB pointer  */ 
  q15_t *pSrc1, *pSrc2;                          /* Intermediate pointers */ 
  q31_t x0, x1, x2, x3, c0;                      /* Temporary variables to hold state and coefficient values */ 
  uint32_t blockSize1, blockSize2, blockSize3, j, k, count, blkCnt;     /* loop counter */ 
  q31_t *pb;                                     /* 32 bit pointer for inputB buffer */ 
 
 
  /* The algorithm implementation is based on the lengths of the inputs. */ 
  /* srcB is always made to slide across srcA. */ 
  /* So srcBLen is always considered as shorter or equal to srcALen */ 
  if(srcALen >= srcBLen) 
  { 
    /* Initialization of inputA pointer */ 
    pIn1 = pSrcA; 
 
    /* Initialization of inputB pointer */ 
    pIn2 = pSrcB; 
  } 
  else 
  { 
    /* Initialization of inputA pointer */ 
    pIn1 = pSrcB; 
 
    /* Initialization of inputB pointer */ 
    pIn2 = pSrcA; 
 
    /* srcBLen is always considered as shorter or equal to srcALen */ 
    j = srcBLen; 
    srcBLen = srcALen; 
    srcALen = j; 
  } 
 
  /* conv(x,y) at n = x[n] * y[0] + x[n-1] * y[1] + x[n-2] * y[2] + ...+ x[n-N+1] * y[N -1] */ 
  /* The function is internally  
   * divided into three stages according to the number of multiplications that has to be  
   * taken place between inputA samples and inputB samples. In the first stage of the  
   * algorithm, the multiplications increase by one for every iteration.  
   * In the second stage of the algorithm, srcBLen number of multiplications are done.  
   * In the third stage of the algorithm, the multiplications decrease by one  
   * for every iteration. */ 
 
  /* The algorithm is implemented in three stages.  
     The loop counters of each stage is initiated here. */ 
  blockSize1 = srcBLen - 1u; 
  blockSize2 = srcALen - (srcBLen - 1u); 
 
  /* --------------------------  
   * Initializations of stage1  
   * -------------------------*/ 
 
  /* sum = x[0] * y[0]  
   * sum = x[0] * y[1] + x[1] * y[0]  
   * ....  
   * sum = x[0] * y[srcBlen - 1] + x[1] * y[srcBlen - 2] +...+ x[srcBLen - 1] * y[0]  
   */ 
 
  /* In this stage the MAC operations are increased by 1 for every iteration.  
     The count variable holds the number of MAC operations performed */ 
  count = 1u; 
 
  /* Working pointer of inputA */ 
  px = pIn1; 
 
  /* Working pointer of inputB */ 
  py = pIn2; 
 
 
  /* ------------------------  
   * Stage1 process  
   * ----------------------*/ 
 
  /* For loop unrolling by 4, this stage is divided into two. */ 
  /* First part of this stage computes the MAC operations less than 4 */ 
  /* Second part of this stage computes the MAC operations greater than or equal to 4 */ 
 
  /* The first part of the stage starts here */ 
  while((count < 4u) && (blockSize1 > 0u)) 
  { 
    /* Accumulator is made zero for every iteration */ 
    sum = 0; 
 
    /* Loop over number of MAC operations between  
     * inputA samples and inputB samples */ 
    k = count; 
 
    while(k > 0u) 
    { 
      /* Perform the multiply-accumulates */ 
      sum = __SMLALD(*px++, *py--, sum); 
 
      /* Decrement the loop counter */ 
      k--; 
    } 
 
    /* Store the result in the accumulator in the destination buffer. */ 
    *pOut++ = (q15_t) (__SSAT((sum >> 15), 16)); 
 
    /* Update the inputA and inputB pointers for next MAC calculation */ 
    py = pIn2 + count; 
    px = pIn1; 
 
    /* Increment the MAC count */ 
    count++; 
 
    /* Decrement the loop counter */ 
    blockSize1--; 
  } 
 
  /* The second part of the stage starts here */ 
  /* The internal loop, over count, is unrolled by 4 */ 
  /* To, read the last two inputB samples using SIMD:  
   * y[srcBLen] and y[srcBLen-1] coefficients, py is decremented by 1 */ 
  py = py - 1; 
 
  while(blockSize1 > 0u) 
  { 
    /* Accumulator is made zero for every iteration */ 
    sum = 0; 
 
    /* Apply loop unrolling and compute 4 MACs simultaneously. */ 
    k = count >> 2u; 
 
    /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.  
     ** a second loop below computes MACs for the remaining 1 to 3 samples. */ 
    while(k > 0u) 
    { 
      /* Perform the multiply-accumulates */ 
      /* x[0], x[1] are multiplied with y[srcBLen - 1], y[srcBLen - 2] respectively */ 
      sum = __SMLALDX(*__SIMD32(px)++, *__SIMD32(py)--, sum); 
      /* x[2], x[3] are multiplied with y[srcBLen - 3], y[srcBLen - 4] respectively */ 
      sum = __SMLALDX(*__SIMD32(px)++, *__SIMD32(py)--, sum); 
 
      /* Decrement the loop counter */ 
      k--; 
    } 
 
    /* For the next MAC operations, the pointer py is used without SIMD  
     * So, py is incremented by 1 */ 
    py = py + 1u; 
 
    /* If the count is not a multiple of 4, compute any remaining MACs here.  
     ** No loop unrolling is used. */ 
    k = count % 0x4u; 
 
    while(k > 0u) 
    { 
      /* Perform the multiply-accumulates */ 
      sum = __SMLALD(*px++, *py--, sum); 
 
      /* Decrement the loop counter */ 
      k--; 
    } 
 
    /* Store the result in the accumulator in the destination buffer. */ 
    *pOut++ = (q15_t) (__SSAT((sum >> 15), 16)); 
 
    /* Update the inputA and inputB pointers for next MAC calculation */ 
    py = pIn2 + (count - 1u); 
    px = pIn1; 
 
    /* Increment the MAC count */ 
    count++; 
 
    /* Decrement the loop counter */ 
    blockSize1--; 
  } 
 
  /* --------------------------  
   * Initializations of stage2  
   * ------------------------*/ 
 
  /* sum = x[0] * y[srcBLen-1] + x[1] * y[srcBLen-2] +...+ x[srcBLen-1] * y[0]  
   * sum = x[1] * y[srcBLen-1] + x[2] * y[srcBLen-2] +...+ x[srcBLen] * y[0]  
   * ....  
   * sum = x[srcALen-srcBLen-2] * y[srcBLen-1] + x[srcALen] * y[srcBLen-2] +...+ x[srcALen-1] * y[0]  
   */ 
 
  /* Working pointer of inputA */ 
  px = pIn1; 
 
  /* Working pointer of inputB */ 
  pSrc2 = pIn2 + (srcBLen - 1u); 
  py = pSrc2; 
 
  /* Initialize inputB pointer of type q31 */ 
  pb = (q31_t *) (py - 1u); 
 
  /* count is the index by which the pointer pIn1 to be incremented */ 
  count = 1u; 
 
 
  /* --------------------  
   * Stage2 process  
   * -------------------*/ 
 
  /* Stage2 depends on srcBLen as in this stage srcBLen number of MACS are performed.  
   * So, to loop unroll over blockSize2,  
   * srcBLen should be greater than or equal to 4 */ 
  if(srcBLen >= 4u) 
  { 
    /* Loop unroll over blockSize2, by 4 */ 
    blkCnt = blockSize2 >> 2u; 
 
    while(blkCnt > 0u) 
    { 
      /* Set all accumulators to zero */ 
      acc0 = 0; 
      acc1 = 0; 
      acc2 = 0; 
      acc3 = 0; 
 
 
      /* read x[0], x[1] samples */ 
      x0 = *(q31_t *) (px++); 
      /* read x[1], x[2] samples */ 
      x1 = *(q31_t *) (px++); 
 
 
      /* Apply loop unrolling and compute 4 MACs simultaneously. */ 
      k = srcBLen >> 2u; 
 
      /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.  
       ** a second loop below computes MACs for the remaining 1 to 3 samples. */ 
      do 
      { 
        /* Read the last two inputB samples using SIMD:  
         * y[srcBLen - 1] and y[srcBLen - 2] */ 
        c0 = *(pb--); 
 
        /* acc0 +=  x[0] * y[srcBLen - 1] + x[1] * y[srcBLen - 2] */ 
        acc0 = __SMLALDX(x0, c0, acc0); 
 
        /* acc1 +=  x[1] * y[srcBLen - 1] + x[2] * y[srcBLen - 2] */ 
        acc1 = __SMLALDX(x1, c0, acc1); 
 
        /* Read x[2], x[3] */ 
        x2 = *(q31_t *) (px++); 
 
        /* Read x[3], x[4] */ 
        x3 = *(q31_t *) (px++); 
 
        /* acc2 +=  x[2] * y[srcBLen - 1] + x[3] * y[srcBLen - 2] */ 
        acc2 = __SMLALDX(x2, c0, acc2); 
 
        /* acc3 +=  x[3] * y[srcBLen - 1] + x[4] * y[srcBLen - 2] */ 
        acc3 = __SMLALDX(x3, c0, acc3); 
 
        /* Read y[srcBLen - 3] and y[srcBLen - 4] */ 
        c0 = *(pb--); 
 
        /* acc0 +=  x[2] * y[srcBLen - 3] + x[3] * y[srcBLen - 4] */ 
        acc0 = __SMLALDX(x2, c0, acc0); 
 
        /* acc1 +=  x[3] * y[srcBLen - 3] + x[4] * y[srcBLen - 4] */ 
        acc1 = __SMLALDX(x3, c0, acc1); 
 
        /* Read x[4], x[5] */ 
        x0 = *(q31_t *) (px++); 
 
        /* Read x[5], x[6] */ 
        x1 = *(q31_t *) (px++); 
 
        /* acc2 +=  x[4] * y[srcBLen - 3] + x[5] * y[srcBLen - 4] */ 
        acc2 = __SMLALDX(x0, c0, acc2); 
 
        /* acc3 +=  x[5] * y[srcBLen - 3] + x[6] * y[srcBLen - 4] */ 
        acc3 = __SMLALDX(x1, c0, acc3); 
 
      } while(--k); 
 
      /* For the next MAC operations, SIMD is not used  
       * So, the 16 bit pointer if inputB, py is updated */ 
      py = (q15_t *) pb; 
      py = py + 1; 
 
      /* If the srcBLen is not a multiple of 4, compute any remaining MACs here.  
       ** No loop unrolling is used. */ 
      k = srcBLen % 0x4u; 
 
      if(k == 1u) 
      { 
        /* Read y[srcBLen - 5] */ 
        c0 = *(py); 
 
        /* Read x[7] */ 
        x3 = *(q31_t *) px++; 
 
        /* Perform the multiply-accumulates */ 
        acc0 = __SMLALD(x0, c0, acc0); 
        acc1 = __SMLALD(x1, c0, acc1); 
        acc2 = __SMLALDX(x1, c0, acc2); 
        acc3 = __SMLALDX(x3, c0, acc3); 
      } 
 
      if(k == 2u) 
      { 
        /* Read y[srcBLen - 5], y[srcBLen - 6] */ 
        c0 = *(pb); 
 
        /* Read x[7], x[8] */ 
        x3 = *(q31_t *) px++; 
 
        /* Read x[9] */ 
        x2 = *(q31_t *) px++; 
 
        /* Perform the multiply-accumulates */ 
        acc0 = __SMLALDX(x0, c0, acc0); 
        acc1 = __SMLALDX(x1, c0, acc1); 
        acc2 = __SMLALDX(x3, c0, acc2); 
        acc3 = __SMLALDX(x2, c0, acc3); 
      } 
 
      if(k == 3u) 
      { 
        /* Read y[srcBLen - 5], y[srcBLen - 6] */ 
        c0 = *pb--; 
 
        /* Read x[7], x[8] */ 
        x3 = *(q31_t *) px++; 
 
        /* Read x[9] */ 
        x2 = *(q31_t *) px++; 
 
        /* Perform the multiply-accumulates */ 
        acc0 = __SMLALDX(x0, c0, acc0); 
        acc1 = __SMLALDX(x1, c0, acc1); 
        acc2 = __SMLALDX(x3, c0, acc2); 
        acc3 = __SMLALDX(x2, c0, acc3); 
 
        /* Read y[srcBLen - 7] */ 
        c0 = (q15_t) (*pb >> 16); 
 
        /* Read x[10] */ 
        x3 = *(q31_t *) px++; 
 
        /* Perform the multiply-accumulates */ 
        acc0 = __SMLALDX(x1, c0, acc0); 
        acc1 = __SMLALD(x2, c0, acc1); 
        acc2 = __SMLALDX(x2, c0, acc2); 
        acc3 = __SMLALDX(x3, c0, acc3); 
      } 
 
      /* Store the results in the accumulators in the destination buffer. */ 
      *__SIMD32(pOut)++ = 
        __PKHBT(__SSAT((acc0 >> 15), 16), __SSAT((acc1 >> 15), 16), 16); 
      *__SIMD32(pOut)++ = 
        __PKHBT(__SSAT((acc2 >> 15), 16), __SSAT((acc3 >> 15), 16), 16); 
 
      /* Update the inputA and inputB pointers for next MAC calculation */ 
      px = pIn1 + (count * 4u); 
      py = pSrc2; 
      pb = (q31_t *) (py - 1); 
 
      /* Increment the pointer pIn1 index, count by 1 */ 
      count++; 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
 
    /* If the blockSize2 is not a multiple of 4, compute any remaining output samples here.  
     ** No loop unrolling is used. */ 
    blkCnt = blockSize2 % 0x4u; 
 
    while(blkCnt > 0u) 
    { 
      /* Accumulator is made zero for every iteration */ 
      sum = 0; 
 
      /* Apply loop unrolling and compute 4 MACs simultaneously. */ 
      k = srcBLen >> 2u; 
 
      /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.  
       ** a second loop below computes MACs for the remaining 1 to 3 samples. */ 
      while(k > 0u) 
      { 
        /* Perform the multiply-accumulates */ 
        sum += (q63_t) ((q31_t) * px++ * *py--); 
        sum += (q63_t) ((q31_t) * px++ * *py--); 
        sum += (q63_t) ((q31_t) * px++ * *py--); 
        sum += (q63_t) ((q31_t) * px++ * *py--); 
 
        /* Decrement the loop counter */ 
        k--; 
      } 
 
      /* If the srcBLen is not a multiple of 4, compute any remaining MACs here.  
       ** No loop unrolling is used. */ 
      k = srcBLen % 0x4u; 
 
      while(k > 0u) 
      { 
        /* Perform the multiply-accumulates */ 
        sum += (q63_t) ((q31_t) * px++ * *py--); 
 
        /* Decrement the loop counter */ 
        k--; 
      } 
 
      /* Store the result in the accumulator in the destination buffer. */ 
      *pOut++ = (q15_t) (__SSAT(sum >> 15, 16)); 
 
      /* Update the inputA and inputB pointers for next MAC calculation */ 
      px = pIn1 + count; 
      py = pSrc2; 
 
      /* Increment the pointer pIn1 index, count by 1 */ 
      count++; 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
  } 
  else 
  { 
    /* If the srcBLen is not a multiple of 4,  
     * the blockSize2 loop cannot be unrolled by 4 */ 
    blkCnt = blockSize2; 
 
    while(blkCnt > 0u) 
    { 
      /* Accumulator is made zero for every iteration */ 
      sum = 0; 
 
      /* srcBLen number of MACS should be performed */ 
      k = srcBLen; 
 
      while(k > 0u) 
      { 
        /* Perform the multiply-accumulate */ 
        sum += (q63_t) ((q31_t) * px++ * *py--); 
 
        /* Decrement the loop counter */ 
        k--; 
      } 
 
      /* Store the result in the accumulator in the destination buffer. */ 
      *pOut++ = (q15_t) (__SSAT(sum >> 15, 16)); 
 
      /* Update the inputA and inputB pointers for next MAC calculation */ 
      px = pIn1 + count; 
      py = pSrc2; 
 
      /* Increment the MAC count */ 
      count++; 
 
      /* Decrement the loop counter */ 
      blkCnt--; 
    } 
  } 
 
 
  /* --------------------------  
   * Initializations of stage3  
   * -------------------------*/ 
 
  /* sum += x[srcALen-srcBLen+1] * y[srcBLen-1] + x[srcALen-srcBLen+2] * y[srcBLen-2] +...+ x[srcALen-1] * y[1]  
   * sum += x[srcALen-srcBLen+2] * y[srcBLen-1] + x[srcALen-srcBLen+3] * y[srcBLen-2] +...+ x[srcALen-1] * y[2]  
   * ....  
   * sum +=  x[srcALen-2] * y[srcBLen-1] + x[srcALen-1] * y[srcBLen-2]  
   * sum +=  x[srcALen-1] * y[srcBLen-1]  
   */ 
 
  /* In this stage the MAC operations are decreased by 1 for every iteration.  
     The blockSize3 variable holds the number of MAC operations performed */ 
 
  blockSize3 = srcBLen - 1u; 
 
  /* Working pointer of inputA */ 
  pSrc1 = (pIn1 + srcALen) - (srcBLen - 1u); 
  px = pSrc1; 
 
  /* Working pointer of inputB */ 
  pSrc2 = pIn2 + (srcBLen - 1u); 
  pIn2 = pSrc2 - 1u; 
  py = pIn2; 
 
  /* -------------------  
   * Stage3 process  
   * ------------------*/ 
 
  /* For loop unrolling by 4, this stage is divided into two. */ 
  /* First part of this stage computes the MAC operations greater than 4 */ 
  /* Second part of this stage computes the MAC operations less than or equal to 4 */ 
 
  /* The first part of the stage starts here */ 
  j = blockSize3 >> 2u; 
 
  while((j > 0u) && (blockSize3 > 0u)) 
  { 
    /* Accumulator is made zero for every iteration */ 
    sum = 0; 
 
    /* Apply loop unrolling and compute 4 MACs simultaneously. */ 
    k = blockSize3 >> 2u; 
 
    /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.  
     ** a second loop below computes MACs for the remaining 1 to 3 samples. */ 
    while(k > 0u) 
    { 
      /* x[srcALen - srcBLen + 1], x[srcALen - srcBLen + 2] are multiplied  
       * with y[srcBLen - 1], y[srcBLen - 2] respectively */ 
      sum = __SMLALDX(*__SIMD32(px)++, *__SIMD32(py)--, sum); 
      /* x[srcALen - srcBLen + 3], x[srcALen - srcBLen + 4] are multiplied  
       * with y[srcBLen - 3], y[srcBLen - 4] respectively */ 
      sum = __SMLALDX(*__SIMD32(px)++, *__SIMD32(py)--, sum); 
 
      /* Decrement the loop counter */ 
      k--; 
    } 
 
    /* For the next MAC operations, the pointer py is used without SIMD  
     * So, py is incremented by 1 */ 
    py = py + 1u; 
 
    /* If the blockSize3 is not a multiple of 4, compute any remaining MACs here.  
     ** No loop unrolling is used. */ 
    k = blockSize3 % 0x4u; 
 
    while(k > 0u) 
    { 
      /* sum += x[srcALen - srcBLen + 5] * y[srcBLen - 5] */ 
      sum = __SMLALD(*px++, *py--, sum); 
 
      /* Decrement the loop counter */ 
      k--; 
    } 
 
    /* Store the result in the accumulator in the destination buffer. */ 
    *pOut++ = (q15_t) (__SSAT((sum >> 15), 16)); 
 
    /* Update the inputA and inputB pointers for next MAC calculation */ 
    px = ++pSrc1; 
    py = pIn2; 
 
    /* Decrement the loop counter */ 
    blockSize3--; 
 
    j--; 
  } 
 
  /* The second part of the stage starts here */ 
  /* SIMD is not used for the next MAC operations,  
   * so pointer py is updated to read only one sample at a time */ 
  py = py + 1u; 
 
  while(blockSize3 > 0u) 
  { 
    /* Accumulator is made zero for every iteration */ 
    sum = 0; 
 
    /* Apply loop unrolling and compute 4 MACs simultaneously. */ 
    k = blockSize3; 
 
    while(k > 0u) 
    { 
      /* Perform the multiply-accumulates */ 
      /* sum +=  x[srcALen-1] * y[srcBLen-1] */ 
      sum = __SMLALD(*px++, *py--, sum); 
 
      /* Decrement the loop counter */ 
      k--; 
    } 
 
    /* Store the result in the accumulator in the destination buffer. */ 
    *pOut++ = (q15_t) (__SSAT((sum >> 15), 16)); 
 
    /* Update the inputA and inputB pointers for next MAC calculation */ 
    px = ++pSrc1; 
    py = pSrc2; 
 
    /* Decrement the loop counter */ 
    blockSize3--; 
  } 
 
} 
 
/**  
 * @} end of Conv group  
 */ 
