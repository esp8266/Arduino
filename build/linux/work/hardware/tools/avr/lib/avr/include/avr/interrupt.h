/* Copyright (c) 2002,2005,2007 Marek Michalkiewicz
   Copyright (c) 2007, Dean Camera

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

/* $Id: interrupt.h,v 1.25.2.1 2008/01/05 06:33:11 dmix Exp $ */

#ifndef _AVR_INTERRUPT_H_
#define _AVR_INTERRUPT_H_

#include <avr/io.h>

#if !defined(__DOXYGEN__) && !defined(__STRINGIFY)
/* Auxiliary macro for ISR_ALIAS(). */
#define __STRINGIFY(x) #x
#endif /* !defined(__DOXYGEN__) */

/** 
\file 
\@{ 
*/


/** \name Global manipulation of the interrupt flag

    The global interrupt flag is maintained in the I bit of the status
    register (SREG). 
*/

#if defined(__DOXYGEN__)
/** \def sei()
    \ingroup avr_interrupts

    \code #include <avr/interrupt.h> \endcode

    Enables interrupts by setting the global interrupt mask. This function
    actually compiles into a single line of assembly, so there is no function
    call overhead. */
#define sei()
#else  /* !DOXYGEN */
# define sei()  __asm__ __volatile__ ("sei" ::)
#endif /* DOXYGEN */

#if defined(__DOXYGEN__)
/** \def cli()
    \ingroup avr_interrupts

    \code #include <avr/interrupt.h> \endcode

    Disables all interrupts by clearing the global interrupt mask. This function
    actually compiles into a single line of assembly, so there is no function
    call overhead. */
#define cli()
#else  /* !DOXYGEN */
# define cli()  __asm__ __volatile__ ("cli" ::)
#endif /* DOXYGEN */


/** \name Macros for writing interrupt handler functions */


#if defined(__DOXYGEN__)
/** \def ISR(vector [, attributes])
    \ingroup avr_interrupts

    \code #include <avr/interrupt.h> \endcode

    Introduces an interrupt handler function (interrupt service
    routine) that runs with global interrupts initially disabled
    by default with no attributes specified.

    The attributes are optional and alter the behaviour and resultant
    generated code of the interrupt routine. Multiple attributes may
    be used for a single function, with a space seperating each
    attribute.

    Valid attributes are ISR_BLOCK, ISR_NOBLOCK, ISR_NAKED and
    ISR_ALIASOF(vect).

    \c vector must be one of the interrupt vector names that are
    valid for the particular MCU type.
*/
#  define ISR(vector, [attributes])
#else  /* real code */

#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 1) || (__GNUC__ > 4)
#  define __INTR_ATTRS used, externally_visible
#else /* GCC < 4.1 */
#  define __INTR_ATTRS used
#endif

#ifdef __cplusplus
#  define ISR(vector, ...)            \
    extern "C" void vector (void) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__; \
    void vector (void)
#else
#  define ISR(vector, ...)            \
    void vector (void) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__; \
    void vector (void)
#endif

#endif /* DOXYGEN */

#if defined(__DOXYGEN__)
/** \def SIGNAL(vector)
    \ingroup avr_interrupts

    \code #include <avr/interrupt.h> \endcode

    Introduces an interrupt handler function that runs with global interrupts
    initially disabled.

    This is the same as the ISR macro without optional attributes.
    \deprecated Do not use SIGNAL() in new code. Use ISR() instead.
*/
#  define SIGNAL(vector)
#else  /* real code */

#ifdef __cplusplus
#  define SIGNAL(vector)					\
    extern "C" void vector(void) __attribute__ ((signal, __INTR_ATTRS));	\
    void vector (void)
#else
#  define SIGNAL(vector)					\
    void vector (void) __attribute__ ((signal, __INTR_ATTRS));		\
    void vector (void)
#endif

#endif /* DOXYGEN */

#if defined(__DOXYGEN__)
/** \def EMPTY_INTERRUPT(vector)
    \ingroup avr_interrupts

    \code #include <avr/interrupt.h> \endcode

    Defines an empty interrupt handler function. This will not generate
    any prolog or epilog code and will only return from the ISR. Do not
    define a function body as this will define it for you.
    Example:
    \code EMPTY_INTERRUPT(ADC_vect);\endcode */
#  define EMPTY_INTERRUPT(vector)
#else  /* real code */

#ifdef __cplusplus
#  define EMPTY_INTERRUPT(vector)                \
    extern "C" void vector(void) __attribute__ ((signal,naked,__INTR_ATTRS));    \
    void vector (void) {  __asm__ __volatile__ ("reti" ::); }
#else
#  define EMPTY_INTERRUPT(vector)                \
    void vector (void) __attribute__ ((signal,naked,__INTR_ATTRS));    \
    void vector (void) { __asm__ __volatile__ ("reti" ::); }
#endif

#endif /* DOXYGEN */

#if defined(__DOXYGEN__)
/** \def ISR_ALIAS(vector, target_vector)
    \ingroup avr_interrupts

    \code #include <avr/interrupt.h> \endcode

    Aliases a given vector to another one in the same manner as the
    ISR_ALIASOF attribute for the ISR() macro. Unlike the ISR_ALIASOF
    attribute macro however, this is compatible for all versions of
    GCC rather than just GCC version 4.2 onwards.

    \note This macro creates a trampoline function for the aliased
    macro.  This will result in a two cycle penalty for the aliased
    vector compared to the ISR the vector is aliased to, due to the
    JMP/RJMP opcode used.

    \deprecated
    For new code, the use of ISR(..., ISR_ALIASOF(...))  is
    recommended.

    Example:
    \code
    ISR(INT0_vect)
    {
        PORTB = 42;
    }

    ISR_ALIAS(INT1_vect, INT0_vect);
    \endcode 
*/
#  define ISR_ALIAS(vector, target_vector)
#else /* real code */

#ifdef __cplusplus
#  if defined(__AVR_MEGA__) && __AVR_MEGA__
#    define ISR_ALIAS(vector, tgt) extern "C" void vector (void) \
	__attribute__((signal, naked, __INTR_ATTRS)); \
	void vector (void) { asm volatile ("jmp " __STRINGIFY(tgt) ::); }
#  else /* !__AVR_MEGA */
#    define ISR_ALIAS(vector, tgt) extern "C" void vector (void) \
	__attribute__((signal, naked, __INTR_ATTRS)); \
	void vector (void) { asm volatile ("rjmp " __STRINGIFY(tgt) ::); }
#  endif  /* __AVR_MEGA__ */
#else	  /* !__cplusplus */
#  if defined(__AVR_MEGA__) && __AVR_MEGA__
#  define ISR_ALIAS(vector, tgt) void vector (void) \
	__attribute__((signal, naked, __INTR_ATTRS)); \
	void vector (void) { asm volatile ("jmp " __STRINGIFY(tgt) ::); }
#  else /* !__AVR_MEGA */
#  define ISR_ALIAS(vector, tgt) void vector (void) \
	__attribute__((signal, naked, __INTR_ATTRS)); \
	void vector (void) { asm volatile ("rjmp " __STRINGIFY(tgt) ::); }
#  endif  /* __AVR_MEGA__ */
#endif	/* __cplusplus */

#endif /* DOXYGEN */

#if defined(__DOXYGEN__)
/** \def reti()
    \ingroup avr_interrupts

    \code #include <avr/interrupt.h> \endcode

    Returns from an interrupt routine, enabling global interrupts. This should
    be the last command executed before leaving an ISR defined with the ISR_NAKED
    attribute.

    This macro actually compiles into a single line of assembly, so there is
    no function call overhead.
*/
#  define reti()
#else  /* !DOXYGEN */
#  define reti()  __asm__ __volatile__ ("reti" ::)
#endif /* DOXYGEN */

#if defined(__DOXYGEN__)
/** \def BADISR_vect
    \ingroup avr_interrupts

    \code #include <avr/interrupt.h> \endcode

    This is a vector which is aliased to __vector_default, the vector
    executed when an ISR fires with no accompanying ISR handler. This
    may be used along with the ISR() macro to create a catch-all for
    undefined but used ISRs for debugging purposes.
*/
#  define BADISR_vect
#else  /* !DOXYGEN */
#  define BADISR_vect __vector_default
#endif /* DOXYGEN */

/** \name ISR attributes */

#if defined(__DOXYGEN__)
/** \def ISR_BLOCK
    \ingroup avr_interrupts

    \code# include <avr/interrupt.h> \endcode

    Identical to an ISR with no attributes specified. Global
    interrupts are initially disabled by the AVR hardware when
    entering the ISR, without the compiler modifying this state.

    Use this attribute in the attributes parameter of the ISR macro.
*/
#  define ISR_BLOCK

/** \def ISR_NOBLOCK
    \ingroup avr_interrupts

    \code# include <avr/interrupt.h> \endcode

    ISR runs with global interrupts initially enabled.  The interrupt
    enable flag is activated by the compiler as early as possible
    within the ISR to ensure minimal processing delay for nested
    interrupts.

    This may be used to create nested ISRs, however care should be
    taken to avoid stack overflows, or to avoid infinitely entering
    the ISR for those cases where the AVR hardware does not clear the
    respective interrupt flag before entering the ISR.

    Use this attribute in the attributes parameter of the ISR macro.
*/
#  define ISR_NOBLOCK

/** \def ISR_NAKED
    \ingroup avr_interrupts

    \code# include <avr/interrupt.h> \endcode

    ISR is created with no prologue or epilogue code. The user code is
    responsible for preservation of the machine state including the
    SREG register, as well as placing a reti() at the end of the
    interrupt routine.

    Use this attribute in the attributes parameter of the ISR macro.
*/
#  define ISR_NAKED

/** \def ISR_ALIASOF(target_vector)
    \ingroup avr_interrupts

    \code#include <avr/interrupt.h>\endcode

    The ISR is linked to another ISR, specified by the vect parameter.
    This is compatible with GCC 4.2 and greater only.

    Use this attribute in the attributes parameter of the ISR macro.
*/
#  define ISR_ALIASOF(target_vector)
#else  /* !DOXYGEN */
#  define ISR_BLOCK
#  define ISR_NOBLOCK    __attribute__((interrupt))
#  define ISR_NAKED      __attribute__((naked))
#  define ISR_ALIASOF(v) __attribute__((alias(__STRINGIFY(v))))
#endif /* DOXYGEN */

/* \@} */

#endif
