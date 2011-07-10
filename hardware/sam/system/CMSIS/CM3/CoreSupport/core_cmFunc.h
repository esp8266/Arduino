/**************************************************************************//**
 * @file     core_cmFunc.h
 * @brief    CMSIS Cortex-M Core Function Access Header File
 * @version  V2.01
 * @date     06. December 2010
 *
 * @note
 * Copyright (C) 2009-2010 ARM Limited. All rights reserved.
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

#ifndef __CORE_CMFUNC_H__
#define __CORE_CMFUNC_H__

/* ###########################  Core Function Access  ########################### */
/** \ingroup  CMSIS_Core_FunctionInterface   
    \defgroup CMSIS_Core_RegAccFunctions CMSIS Core Register Access Functions
  @{
 */

#if defined ( __CC_ARM   ) /*------------------ RealView Compiler ----------------*/
/* ARM armcc specific functions */

/* intrinsic void __enable_irq();     */
/* intrinsic void __disable_irq();    */

/** \brief  Get Control Register

    This function returns the content of the Control Register.

    \return               Control Register value
 */
#if       (__ARMCC_VERSION <  400000)
extern          uint32_t __get_CONTROL(void);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE uint32_t __get_CONTROL(void)
{
  register uint32_t __regControl         __ASM("control");
  return(__regControl);
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Set Control Register

    This function writes the given value to the Control Register.

    \param [in]    control  Control Register value to set
 */
#if       (__ARMCC_VERSION <  400000)
extern          void __set_CONTROL(uint32_t control);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE void __set_CONTROL(uint32_t control)
{
  register uint32_t __regControl         __ASM("control");
  __regControl = control;
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Get ISPR Register

    This function returns the content of the ISPR Register.

    \return               ISPR Register value
 */
#if       (__ARMCC_VERSION <  400000)
extern          uint32_t __get_IPSR(void);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE uint32_t __get_IPSR(void)
{
  register uint32_t __regIPSR          __ASM("ipsr");
  return(__regIPSR);
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Get APSR Register

    This function returns the content of the APSR Register.

    \return               APSR Register value
 */
#if       (__ARMCC_VERSION <  400000)
extern          uint32_t __get_APSR(void);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE uint32_t __get_APSR(void)
{
  register uint32_t __regAPSR          __ASM("apsr");
  return(__regAPSR);
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Get xPSR Register

    This function returns the content of the xPSR Register.

    \return               xPSR Register value
 */
#if       (__ARMCC_VERSION <  400000)
extern          uint32_t __get_xPSR(void);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE uint32_t __get_xPSR(void)
{
  register uint32_t __regXPSR          __ASM("xpsr");
  return(__regXPSR);
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Get Process Stack Pointer

    This function returns the current value of the Process Stack Pointer (PSP).

    \return               PSP Register value
 */
#if       (__ARMCC_VERSION <  400000)
extern          uint32_t __get_PSP(void);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE uint32_t __get_PSP(void)
{
  register uint32_t __regProcessStackPointer  __ASM("psp");
  return(__regProcessStackPointer);
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Set Process Stack Pointer

    This function assigns the given value to the Process Stack Pointer (PSP).

    \param [in]    topOfProcStack  Process Stack Pointer value to set
 */
#if       (__ARMCC_VERSION <  400000)
extern          void __set_PSP(uint32_t topOfProcStack);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE void __set_PSP(uint32_t topOfProcStack)
{
  register uint32_t __regProcessStackPointer  __ASM("psp");
  __regProcessStackPointer = topOfProcStack;
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Get Main Stack Pointer

    This function returns the current value of the Main Stack Pointer (MSP).

    \return               MSP Register value
 */
#if       (__ARMCC_VERSION <  400000)
extern          uint32_t __get_MSP(void);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE uint32_t __get_MSP(void)
{
  register uint32_t __regMainStackPointer     __ASM("msp");
  return(__regMainStackPointer);
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Set Main Stack Pointer

    This function assigns the given value to the Main Stack Pointer (MSP).

    \param [in]    topOfMainStack  Main Stack Pointer value to set
 */
#if       (__ARMCC_VERSION <  400000)
extern          void __set_MSP(uint32_t topOfMainStack);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE void __set_MSP(uint32_t topOfMainStack)
{
  register uint32_t __regMainStackPointer     __ASM("msp");
  __regMainStackPointer = topOfMainStack;
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Get Priority Mask

    This function returns the current state of the priority mask bit from the Priority Mask Register.

    \return               Priority Mask value
 */
#if       (__ARMCC_VERSION <  400000)
extern          uint32_t __get_PRIMASK(void);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE uint32_t __get_PRIMASK(void)
{
  register uint32_t __regPriMask         __ASM("primask");
  return(__regPriMask);
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Set Priority Mask

    This function assigns the given value to the Priority Mask Register.

    \param [in]    priMask  Priority Mask
 */
#if       (__ARMCC_VERSION <  400000)
extern          void __set_PRIMASK(uint32_t priMask);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE void __set_PRIMASK(uint32_t priMask)
{
  register uint32_t __regPriMask         __ASM("primask");
  __regPriMask = (priMask);
}
#endif /*  __ARMCC_VERSION  */ 
 

#if       (__CORTEX_M >= 0x03)

/** \brief  Enable FIQ

    This function enables FIQ interrupts by clearing the F-bit in the CPSR.
    Can only be executed in Privileged modes.
 */
#define __enable_fault_irq                __enable_fiq


/** \brief  Disable FIQ

    This function disables FIQ interrupts by setting the F-bit in the CPSR.
    Can only be executed in Privileged modes.
 */
#define __disable_fault_irq               __disable_fiq


/** \brief  Get Base Priority

    This function returns the current value of the Base Priority register.

    \return               Base Priority register value
 */
#if       (__ARMCC_VERSION <  400000)
extern          uint32_t __get_BASEPRI(void);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE uint32_t  __get_BASEPRI(void)
{
  register uint32_t __regBasePri         __ASM("basepri");
  return(__regBasePri);
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Set Base Priority

    This function assigns the given value to the Base Priority register.

    \param [in]    basePri  Base Priority value to set
 */
#if       (__ARMCC_VERSION <  400000)
extern          void __set_BASEPRI(uint32_t basePri);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE void __set_BASEPRI(uint32_t basePri)
{
  register uint32_t __regBasePri         __ASM("basepri");
  __regBasePri = (basePri & 0xff);
}
#endif /*  __ARMCC_VERSION  */ 
 

/** \brief  Get Fault Mask

    This function returns the current value of the Fault Mask register.

    \return               Fault Mask register value
 */
#if       (__ARMCC_VERSION <  400000)
extern          uint32_t __get_FAULTMASK(void);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE uint32_t __get_FAULTMASK(void)
{
  register uint32_t __regFaultMask       __ASM("faultmask");
  return(__regFaultMask);
}
#endif /*  __ARMCC_VERSION  */ 


/** \brief  Set Fault Mask

    This function assigns the given value to the Fault Mask register.

    \param [in]    faultMask  Fault Mask value to set
 */
#if       (__ARMCC_VERSION <  400000)
extern          void __set_FAULTMASK(uint32_t faultMask);
#else  /* (__ARMCC_VERSION >= 400000) */
static __INLINE void __set_FAULTMASK(uint32_t faultMask)
{
  register uint32_t __regFaultMask       __ASM("faultmask");
  __regFaultMask = (faultMask & 1);
}
#endif /*  __ARMCC_VERSION  */ 

#endif /* (__CORTEX_M >= 0x03) */


#if       (__CORTEX_M == 0x04)

/** \brief  Get FPSCR

    This function returns the current value of the Floating Point Status/Control register.

    \return               Floating Point Status/Control register value
 */
static __INLINE uint32_t __get_FPSCR(void)
{
#if (__FPU_PRESENT == 1)
  register uint32_t __regfpscr         __ASM("fpscr");
  return(__regfpscr);
#else
   return(0);
#endif
}


/** \brief  Set FPSCR

    This function assigns the given value to the Floating Point Status/Control register.

    \param [in]    fpscr  Floating Point Status/Control value to set
 */
static __INLINE void __set_FPSCR(uint32_t fpscr)
{
#if (__FPU_PRESENT == 1)
  register uint32_t __regfpscr         __ASM("fpscr");
  __regfpscr = (fpscr);
#endif
}

#endif /* (__CORTEX_M == 0x04) */


 #elif (defined (__ICCARM__)) /*---------------- ICC Compiler ---------------------*/
/* IAR iccarm specific functions */

#if defined (__ICCARM__)
  #include <intrinsics.h>                     /* IAR Intrinsics   */
#endif

#pragma diag_suppress=Pe940

/** \brief  Enable IRQ Interrupts

  This function enables IRQ interrupts by clearing the I-bit in the CPSR.
  Can only be executed in Privileged modes.
 */
#define __enable_irq                              __enable_interrupt


/** \brief  Disable IRQ Interrupts

  This function disables IRQ interrupts by setting the I-bit in the CPSR.
  Can only be executed in Privileged modes.
 */
#define __disable_irq                             __disable_interrupt


/* intrinsic unsigned long __get_CONTROL( void ); (see intrinsic.h) */
/* intrinsic void __set_CONTROL( unsigned long ); (see intrinsic.h) */


/** \brief  Get ISPR Register

    This function returns the content of the ISPR Register.

    \return               ISPR Register value
 */
static uint32_t __get_IPSR(void)
{
  __ASM("mrs r0, ipsr");
}


/** \brief  Get APSR Register

    This function returns the content of the APSR Register.

    \return               APSR Register value
 */
static uint32_t __get_APSR(void)
{
  __ASM("mrs r0, apsr");
}


/** \brief  Get xPSR Register

    This function returns the content of the xPSR Register.

    \return               xPSR Register value
 */
static uint32_t __get_xPSR(void)
{
  __ASM("mrs r0, psr");           // assembler does not know "xpsr"
}


/** \brief  Get Process Stack Pointer

    This function returns the current value of the Process Stack Pointer (PSP).

    \return               PSP Register value
 */
static uint32_t __get_PSP(void)
{
  __ASM("mrs r0, psp");
}
 

/** \brief  Set Process Stack Pointer

    This function assigns the given value to the Process Stack Pointer (PSP).

    \param [in]    topOfProcStack  Process Stack Pointer value to set
 */
static void __set_PSP(uint32_t topOfProcStack)
{
  __ASM("msr psp, r0");
}


/** \brief  Get Main Stack Pointer

    This function returns the current value of the Main Stack Pointer (MSP).

    \return               MSP Register value
 */
static uint32_t __get_MSP(void)
{
  __ASM("mrs r0, msp");
}
 

/** \brief  Set Main Stack Pointer

    This function assigns the given value to the Main Stack Pointer (MSP).

    \param [in]    topOfMainStack  Main Stack Pointer value to set
 */
static void __set_MSP(uint32_t topOfMainStack)
{
  __ASM("msr msp, r0");
}
 

/* intrinsic unsigned long __get_PRIMASK( void ); (see intrinsic.h) */
/* intrinsic void __set_PRIMASK( unsigned long ); (see intrinsic.h) */
 

#if       (__CORTEX_M >= 0x03)

/** \brief  Enable FIQ

    This function enables FIQ interrupts by clearing the F-bit in the CPSR.
    Can only be executed in Privileged modes.
 */
static __INLINE void __enable_fault_irq(void)
{
  __ASM ("cpsie f");
}


/** \brief  Disable FIQ

    This function disables FIQ interrupts by setting the F-bit in the CPSR.
    Can only be executed in Privileged modes.
 */
static __INLINE void __disable_fault_irq(void)
{
  __ASM ("cpsid f");
}


/* intrinsic unsigned long __get_BASEPRI( void );   (see intrinsic.h) */
/* intrinsic void __set_BASEPRI( unsigned long );   (see intrinsic.h) */
/* intrinsic unsigned long __get_FAULTMASK( void ); (see intrinsic.h) */
/* intrinsic void __set_FAULTMASK(unsigned long);   (see intrinsic.h) */

#endif /* (__CORTEX_M >= 0x03) */


#if       (__CORTEX_M == 0x04)

/** \brief  Get FPSCR

    This function returns the current value of the Floating Point Status/Control register.

    \return               Floating Point Status/Control register value
 */
static uint32_t __get_FPSCR(void)
{
#if (__FPU_PRESENT == 1)
  __ASM("vmrs r0, fpscr"); 
#else
  return(0);
#endif
}


/** \brief  Set FPSCR

    This function assigns the given value to the Floating Point Status/Control register.

    \param [in]    fpscr  Floating Point Status/Control value to set
 */
static void __set_FPSCR(uint32_t fpscr)
{
#if (__FPU_PRESENT == 1)
  __ASM("vmsr fpscr, r0");
#endif
}

#endif /* (__CORTEX_M == 0x04) */

#pragma diag_default=Pe940


#elif (defined (__GNUC__)) /*------------------ GNU Compiler ---------------------*/
/* GNU gcc specific functions */

/** \brief  Enable IRQ Interrupts

  This function enables IRQ interrupts by clearing the I-bit in the CPSR.
  Can only be executed in Privileged modes.
 */
__attribute__( ( always_inline ) ) static __INLINE void __enable_irq(void)
{
  __ASM volatile ("cpsie i");
}


/** \brief  Disable IRQ Interrupts

  This function disables IRQ interrupts by setting the I-bit in the CPSR.
  Can only be executed in Privileged modes.
 */
__attribute__( ( always_inline ) ) static __INLINE void __disable_irq(void)
{
  __ASM volatile ("cpsid i");
}


/** \brief  Get Control Register

    This function returns the content of the Control Register.

    \return               Control Register value
 */
__attribute__( ( always_inline ) ) static __INLINE uint32_t __get_CONTROL(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, control" : "=r" (result) );
  return(result);
}


/** \brief  Set Control Register

    This function writes the given value to the Control Register.

    \param [in]    control  Control Register value to set
 */
__attribute__( ( always_inline ) ) static __INLINE void __set_CONTROL(uint32_t control)
{
  __ASM volatile ("MSR control, %0" : : "r" (control) );
}


/** \brief  Get ISPR Register

    This function returns the content of the ISPR Register.

    \return               ISPR Register value
 */
__attribute__( ( always_inline ) ) static __INLINE uint32_t __get_IPSR(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, ipsr" : "=r" (result) );
  return(result);
}


/** \brief  Get APSR Register

    This function returns the content of the APSR Register.

    \return               APSR Register value
 */
__attribute__( ( always_inline ) ) static __INLINE uint32_t __get_APSR(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, apsr" : "=r" (result) );
  return(result);
}


/** \brief  Get xPSR Register

    This function returns the content of the xPSR Register.

    \return               xPSR Register value
 */
__attribute__( ( always_inline ) ) static __INLINE uint32_t __get_xPSR(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, xpsr" : "=r" (result) );
  return(result);
}


/** \brief  Get Process Stack Pointer

    This function returns the current value of the Process Stack Pointer (PSP).

    \return               PSP Register value
 */
__attribute__( ( always_inline ) ) static __INLINE uint32_t __get_PSP(void)
{
  register uint32_t result;

  __ASM volatile ("MRS %0, psp\n"  : "=r" (result) );
  return(result);
}
 

/** \brief  Set Process Stack Pointer

    This function assigns the given value to the Process Stack Pointer (PSP).

    \param [in]    topOfProcStack  Process Stack Pointer value to set
 */
__attribute__( ( always_inline ) ) static __INLINE void __set_PSP(uint32_t topOfProcStack)
{
  __ASM volatile ("MSR psp, %0\n" : : "r" (topOfProcStack) );
}


/** \brief  Get Main Stack Pointer

    This function returns the current value of the Main Stack Pointer (MSP).

    \return               MSP Register value
 */
__attribute__( ( always_inline ) ) static __INLINE uint32_t __get_MSP(void)
{
  register uint32_t result;

  __ASM volatile ("MRS %0, msp\n" : "=r" (result) );
  return(result);
}
 

/** \brief  Set Main Stack Pointer

    This function assigns the given value to the Main Stack Pointer (MSP).

    \param [in]    topOfMainStack  Main Stack Pointer value to set
 */
__attribute__( ( always_inline ) ) static __INLINE void __set_MSP(uint32_t topOfMainStack)
{
  __ASM volatile ("MSR msp, %0\n" : : "r" (topOfMainStack) );
}


/** \brief  Get Priority Mask

    This function returns the current state of the priority mask bit from the Priority Mask Register.

    \return               Priority Mask value
 */
__attribute__( ( always_inline ) ) static __INLINE uint32_t __get_PRIMASK(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, primask" : "=r" (result) );
  return(result);
}


/** \brief  Set Priority Mask

    This function assigns the given value to the Priority Mask Register.

    \param [in]    priMask  Priority Mask
 */
__attribute__( ( always_inline ) ) static __INLINE void __set_PRIMASK(uint32_t priMask)
{
  __ASM volatile ("MSR primask, %0" : : "r" (priMask) );
}
 

#if       (__CORTEX_M >= 0x03)

/** \brief  Enable FIQ

    This function enables FIQ interrupts by clearing the F-bit in the CPSR.
    Can only be executed in Privileged modes.
 */
__attribute__( ( always_inline ) ) static __INLINE void __enable_fault_irq(void)
{
  __ASM volatile ("cpsie f");
}


/** \brief  Disable FIQ

    This function disables FIQ interrupts by setting the F-bit in the CPSR.
    Can only be executed in Privileged modes.
 */
__attribute__( ( always_inline ) ) static __INLINE void __disable_fault_irq(void)
{
  __ASM volatile ("cpsid f");
}


/** \brief  Get Base Priority

    This function returns the current value of the Base Priority register.

    \return               Base Priority register value
 */
__attribute__( ( always_inline ) ) static __INLINE uint32_t __get_BASEPRI(void)
{
  uint32_t result;
  
  __ASM volatile ("MRS %0, basepri_max" : "=r" (result) );
  return(result);
}


/** \brief  Set Base Priority

    This function assigns the given value to the Base Priority register.

    \param [in]    basePri  Base Priority value to set
 */
__attribute__( ( always_inline ) ) static __INLINE void __set_BASEPRI(uint32_t value)
{
  __ASM volatile ("MSR basepri, %0" : : "r" (value) );
}


/** \brief  Get Fault Mask

    This function returns the current value of the Fault Mask register.

    \return               Fault Mask register value
 */
__attribute__( ( always_inline ) ) static __INLINE uint32_t __get_FAULTMASK(void)
{
  uint32_t result;
  
  __ASM volatile ("MRS %0, faultmask" : "=r" (result) );
  return(result);
}


/** \brief  Set Fault Mask

    This function assigns the given value to the Fault Mask register.

    \param [in]    faultMask  Fault Mask value to set
 */
__attribute__( ( always_inline ) ) static __INLINE void __set_FAULTMASK(uint32_t faultMask)
{
  __ASM volatile ("MSR faultmask, %0" : : "r" (faultMask) );
}

#endif /* (__CORTEX_M >= 0x03) */


#if       (__CORTEX_M == 0x04)

/** \brief  Get FPSCR

    This function returns the current value of the Floating Point Status/Control register.

    \return               Floating Point Status/Control register value
 */
__attribute__( ( always_inline ) ) static __INLINE uint32_t __get_FPSCR(void)
{
#if (__FPU_PRESENT == 1)
  uint32_t result;

  __ASM volatile ("MRS %0, fpscr" : "=r" (result) );
  return(result);
#else
   return(0);
#endif
}


/** \brief  Set FPSCR

    This function assigns the given value to the Floating Point Status/Control register.

    \param [in]    fpscr  Floating Point Status/Control value to set
 */
__attribute__( ( always_inline ) ) static __INLINE void __set_FPSCR(uint32_t fpscr)
{
#if (__FPU_PRESENT == 1)
  __ASM volatile ("MSR fpscr, %0" : : "r" (fpscr) );
#endif
}

#endif /* (__CORTEX_M == 0x04) */


#elif (defined (__TASKING__)) /*--------------- TASKING Compiler -----------------*/
/* TASKING carm specific functions */

/*
 * The CMSIS functions have been implemented as intrinsics in the compiler.
 * Please use "carm -?i" to get an up to date list of all instrinsics,
 * Including the CMSIS ones.
 */

#endif

/*@} end of CMSIS_Core_RegAccFunctions */


#endif /* __CORE_CMFUNC_H__ */
