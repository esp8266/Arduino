/**************************************************************************//**
 * @file     core_cm4.c
 * @brief    CMSIS Cortex-M3 Core Peripheral Access Layer Source File
 * @version  V2.01
 * @date     06. December 2010
 *
 * @note
 * Copyright (C) 2010 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


/* ###################  Compiler specific Intrinsics  ########################### */

#if defined ( __CC_ARM   ) /*------------------ RealView Compiler ----------------*/
/* ARM armcc specific functions */




#elif (defined (__ICCARM__)) /*------------------ ICC Compiler -------------------*/
/* IAR iccarm specific functions */



#elif (defined (__GNUC__)) /*------------------ GNU Compiler ---------------------*/
/* GNU gcc specific functions */




#elif (defined (__TASKING__)) /*------------------ TASKING Compiler --------------*/
/* TASKING carm specific functions */

/*
 * The CMSIS functions have been implemented as intrinsics in the compiler.
 * Please use "carm -?i" to get an up to date list of all instrinsics,
 * Including the CMSIS ones.
 */

#endif
