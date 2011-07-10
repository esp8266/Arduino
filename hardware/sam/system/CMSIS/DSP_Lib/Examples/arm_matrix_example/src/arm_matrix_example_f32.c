/* ---------------------------------------------------------------------- 
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_matrix_example_f32.c		  
*  
* Description:	Example code demonstrating least square fit to data  
*				using matrix functions  
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
 * @defgroup MatrixExample Matrix Example    
 * 
 * \par Description: 
 * \par
 * Demonstrates the use of Matrix Transpose, Matrix Muliplication, and Matrix Inverse 
 * functions to apply least squares fitting to input data. Least squares fitting is 
 * the procedure for finding the best-fitting curve that minimizes the sum of the 
 * squares of the offsets (least square error) from a given set of data.
 *
 * \par Algorithm:
 * \par
 * The linear combination of parameters considered is as follows: 
 * \par 
 * <code>A * X = B</code>, where \c X is the unknown value and can be estimated 
 * from \c A & \c B.
 * \par
 * The least squares estimate \c X is given by the following equation:
 * \par
 * <code>X = Inverse(A<sup>T</sup> * A) *  A<sup>T</sup> * B</code>
 *
 * \par Block Diagram:
 * \par
 * \image html matrixExample.gif
 *
 * \par Variables Description:
 * \par
 * \li \c A_f32 input matrix in the linear combination equation
 * \li \c B_f32 output matrix in the linear combination equation
 * \li \c X_f32 unknown matrix estimated using \c A_f32 & \c B_f32 matrices
 *
 * \par CMSIS DSP Software Library Functions Used:
 * \par
 * - arm_mat_init_f32()
 * - arm_mat_trans_f32()
 * - arm_mat_mult_f32()
 * - arm_mat_inverse_f32() 
 * 
 * <b> Refer  </b> 
 * \link arm_matrix_example_f32.c \endlink
 * 
 */ 
 
 
/** \example arm_matrix_example_f32.c 
  */  
     
#include "arm_math.h" 
#include "math_helper.h" 
 
#define SNR_THRESHOLD 	90 
 
/* -------------------------------------------------------------------------------- 
* Test input data(Cycles) taken from FIR Q15 module for differant cases of blockSize  
* and tapSize 
* --------------------------------------------------------------------------------- */ 
 
const float32_t B_f32[4] =  
{    
	782.0, 7577.0, 470.0, 4505.0 
}; 
 
/* -------------------------------------------------------------------------------- 
* Formula to fit is  C1 + C2 * numTaps + C3 * blockSize + C4 * numTaps * blockSize 
* -------------------------------------------------------------------------------- */ 
 
const float32_t A_f32[16] =  
{ 
	/* Const, 	numTaps, 	blockSize, 	numTaps*blockSize */    
	1.0, 		32.0, 		4.0, 		128.0,  
	1.0, 		32.0, 		64.0, 		2048.0, 
	1.0, 		16.0, 		4.0, 		64.0, 
	1.0, 		16.0, 		64.0, 		1024.0, 
};  
 
 
/* ---------------------------------------------------------------------- 
* Temporary buffers  for storing intermediate values 
* ------------------------------------------------------------------- */ 
/* Transpose of A Buffer */ 
float32_t AT_f32[16]; 
/* (Transpose of A * A) Buffer */ 
float32_t ATMA_f32[16]; 
/* Inverse(Transpose of A * A)  Buffer */ 
float32_t ATMAI_f32[16]; 
/* Test Output Buffer */ 
float32_t X_f32[4]; 
 
/* ---------------------------------------------------------------------- 
* Reference ouput buffer C1, C2, C3 and C4 taken from MATLAB  
* ------------------------------------------------------------------- */ 
const float32_t xRef_f32[4] = {73.0, 8.0, 21.25, 2.875}; 
 
float32_t snr; 
 
 
/* ---------------------------------------------------------------------- 
* Max magnitude FFT Bin test 
* ------------------------------------------------------------------- */ 
 
int32_t main(void) 
{ 
 
	arm_matrix_instance_f32 A;		/* Matrix A Instance */ 
	arm_matrix_instance_f32 AT;		/* Matrix AT(A transpose) instance */ 
	arm_matrix_instance_f32 ATMA;	/* Matrix ATMA( AT multiply with A) instance */ 
	arm_matrix_instance_f32 ATMAI;	/* Matrix ATMAI(Inverse of ATMA) instance */ 
	arm_matrix_instance_f32 B;		/* Matrix B instance */ 
	arm_matrix_instance_f32 X;		/* Matrix X(Unknown Matrix) instance */ 
 
	uint32_t srcRows, srcColumns;	/* Temporary variables */
	arm_status status; 
 
	/* Initialise A Matrix Instance with numRows, numCols and data array(A_f32) */ 
	srcRows = 4; 
    srcColumns = 4; 
	arm_mat_init_f32(&A, srcRows, srcColumns, (float32_t *)A_f32); 
 
	/* Initialise Matrix Instance AT with numRows, numCols and data array(AT_f32) */ 
	srcRows = 4; 
    srcColumns = 4; 
	arm_mat_init_f32(&AT, srcRows, srcColumns, AT_f32); 
 
	/* calculation of A transpose */ 
	status = arm_mat_trans_f32(&A, &AT); 
	 
 
	/* Initialise ATMA Matrix Instance with numRows, numCols and data array(ATMA_f32) */ 
	srcRows = 4; 
    srcColumns = 4; 
	arm_mat_init_f32(&ATMA, srcRows, srcColumns, ATMA_f32); 
 
	/* calculation of AT Multiply with A */ 
	status = arm_mat_mult_f32(&AT, &A, &ATMA); 
 
	/* Initialise ATMAI Matrix Instance with numRows, numCols and data array(ATMAI_f32) */ 
	srcRows = 4; 
    srcColumns = 4; 
	arm_mat_init_f32(&ATMAI, srcRows, srcColumns, ATMAI_f32); 
 
	/* calculation of Inverse((Transpose(A) * A) */ 
	status = arm_mat_inverse_f32(&ATMA, &ATMAI); 
 
	/* calculation of (Inverse((Transpose(A) * A)) *  Transpose(A)) */ 
	status = arm_mat_mult_f32(&ATMAI, &AT, &ATMA); 
 
	/* Initialise B Matrix Instance with numRows, numCols and data array(B_f32) */ 
	srcRows = 4; 
    srcColumns = 1; 
	arm_mat_init_f32(&B, srcRows, srcColumns, (float32_t *)B_f32);  
 
	/* Initialise X Matrix Instance with numRows, numCols and data array(X_f32) */ 
	srcRows = 4; 
    srcColumns = 1; 
	arm_mat_init_f32(&X, srcRows, srcColumns, X_f32); 
 
	/* calculation ((Inverse((Transpose(A) * A)) *  Transpose(A)) * B) */ 
	status = arm_mat_mult_f32(&ATMA, &B, &X); 
	 
	/* Comparison of reference with test output */	   
	snr = arm_snr_f32((float32_t *)xRef_f32, X_f32, 4); 
 
	/*------------------------------------------------------------------------------ 
	*  					Initialise status depending on SNR calculations 
	*------------------------------------------------------------------------------*/  
	if( snr > SNR_THRESHOLD) 
	{ 
		status = ARM_MATH_SUCCESS; 
	} 
	else 
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
} 
 
 /** \endlink */ 
 
