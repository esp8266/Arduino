/* ---------------------------------------------------------------------- 
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_fft_bin_example_f32.c		  
*  
* Description:	Example code demonstrating calculation of Max energy bin of  
*				frequency domain of input signal. 
* 
* Target Processor: Cortex-M4/Cortex-M3  
*
*
* Version 1.0.3 2010/11/29 
*    Re-organized the CMSIS folders and updated documentation. 
* 
* Version 1.0.1 2010/10/05 KK 
*    Production release and review comments incorporated.  
*
* Version 1.0.0 2010/09/20 KK
*    Production release and review comments incorporated.
* ------------------------------------------------------------------- */ 
 
/** 
 * @ingroup groupExamples 
 */ 
 
/**    
 * @defgroup FrequencyBin Frequency Bin Example    
 * 
 * \par Description
 * \par
 * Demonstrates the calculation of the maximum energy bin in the frequency 
 * domain of the input signal with the use of Complex FFT, Complex 
 * Magnitude, and Maximum functions. 
 * 
 * \par Algorithm:
 * \par
 * The input test signal contains a 10 kHz signal with uniformly distributed white noise.  
 * Calculating the FFT of the input signal will give us the maximum energy of the 
 * bin corresponding to the input frequency of 10 kHz.  
 * 
 * \par Block Diagram:
 * \image html FFTBin.gif "Block Diagram"
 * \par
 * The figure below shows the time domain signal of 10 kHz signal with 
 * uniformly distributed white noise, and the next figure shows the input
 * in the frequency domain. The bin with maximum energy corresponds to 10 kHz signal. 
 * \par
 * \image html FFTBinInput.gif "Input signal in Time domain" 
 * \image html FFTBinOutput.gif "Input signal in Frequency domain"
 *
 * \par Variables Description:
 * \par
 * \li \c testInput_f32_10khz points to the input data
 * \li \c testOutput points to the output data
 * \li \c fftSize length of FFT
 * \li \c ifftFlag flag for the selection of CFFT/CIFFT
 * \li \c doBitReverse Flag for selection of normal order or bit reversed order
 * \li \c refIndex reference index value at which maximum energy of bin ocuurs
 * \li \c testIndex calculated index value at which maximum energy of bin ocuurs
 * 
 * \par CMSIS DSP Software Library Functions Used:
 * \par
 * - arm_cfft_radix4_init_f32()
 * - arm_cfft_radix4_f32()
 * - arm_cmplx_mag_f32()
 * - arm_max_f32()
 *
 * <b> Refer  </b> 
 * \link arm_fft_bin_example_f32.c \endlink
 * 
 */ 
 
 
/** \example arm_fft_bin_example_f32.c 
  */  

     
#include "arm_math.h" 
 
#define TEST_LENGTH_SAMPLES 2048 
 
/* ------------------------------------------------------------------- 
* External Input and Output buffer Declarations for FFT Bin Example 
* ------------------------------------------------------------------- */ 
extern float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES]; 
static float32_t testOutput[TEST_LENGTH_SAMPLES/2]; 
 
/* ------------------------------------------------------------------ 
* Global variables for FFT Bin Example 
* ------------------------------------------------------------------- */ 
uint32_t fftSize = 1024; 
uint32_t ifftFlag = 0; 
uint32_t doBitReverse = 1; 
 
/* Reference index at which max energy of bin ocuurs */ 
uint32_t refIndex = 213, testIndex = 0; 
 
/* ---------------------------------------------------------------------- 
* Max magnitude FFT Bin test 
* ------------------------------------------------------------------- */ 
 
int32_t main(void) 
{ 
   
	arm_status status; 
	arm_cfft_radix4_instance_f32 S; 
	float32_t maxValue; 
	 
	status = ARM_MATH_SUCCESS; 
	 
	/* Initialize the CFFT/CIFFT module */  
	status = arm_cfft_radix4_init_f32(&S, fftSize,  
	  								ifftFlag, doBitReverse); 
	 
	/* Process the data through the CFFT/CIFFT module */ 
	arm_cfft_radix4_f32(&S, testInput_f32_10khz); 
	 
	 
	/* Process the data through the Complex Magnitude Module for  
	calculating the magnitude at each bin */ 
	arm_cmplx_mag_f32(testInput_f32_10khz, testOutput,  
	  				fftSize);  
	 
	/* Calculates maxValue and returns corresponding BIN value */ 
	arm_max_f32(testOutput, fftSize, &maxValue, &testIndex); 
	 
	if(testIndex !=  refIndex) 
	{ 
		status = ARM_MATH_TEST_FAILURE; 
	} 
	 
	/* ---------------------------------------------------------------------- 
	** Loop here if the signals fail the PASS check. 
	** This denotes a test failure 
	** ------------------------------------------------------------------- */ 
	 
	if( status != ARM_MATH_SUCCESS) 
	{ 
		while(1); 
	} 

    while(1);                             /* main function does not return */
} 
 
 /** \endlink */ 
 
 
 
