/* Copyright (c) 2005,2006 Joerg Wunsch
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

/* $Id: deprecated.h,v 1.6.2.2 2008/07/30 21:39:21 arcanum Exp $ */

#ifndef _COMPAT_DEPRECATED_H_
#define _COMPAT_DEPRECATED_H_

/** \defgroup deprecated_items <compat/deprecated.h>: Deprecated items

    This header file contains several items that used to be available
    in previous versions of this library, but have eventually been
    deprecated over time.

    \code #include <compat/deprecated.h> \endcode

    These items are supplied within that header file for backward
    compatibility reasons only, so old source code that has been
    written for previous library versions could easily be maintained
    until its end-of-life.  Use of any of these items in new code is
    strongly discouraged.
  */

/** \name Allowing specific system-wide interrupts

    In addition to globally enabling interrupts, each device's particular
    interrupt needs to be enabled separately if interrupts for this device are
    desired.  While some devices maintain their interrupt enable bit inside
    the device's register set, external and timer interrupts have system-wide
    configuration registers. 

    Example:

    \code
    // Enable timer 1 overflow interrupts.
    timer_enable_int(_BV(TOIE1));

    // Do some work...

    // Disable all timer interrupts.
    timer_enable_int(0);
    \endcode

    \note Be careful when you use these functions. If you already have a
    different interrupt enabled, you could inadvertantly disable it by
    enabling another intterupt. */

/*@{*/

/** \ingroup deprecated_items
    \def enable_external_int(mask)
    \deprecated

    This macro gives access to the \c GIMSK register (or \c EIMSK register
    if using an AVR Mega device or \c GICR register for others). Although this
    macro is essentially the same as assigning to the register, it does
    adapt slightly to the type of device being used. This macro is 
    unavailable if none of the registers listed above are defined. */

/* Define common register definition if available. */
#if defined(EIMSK)
#  define __EICR  EIMSK
#elif defined(GIMSK)
#  define __EICR  GIMSK
#elif defined(GICR)
#  define __EICR  GICR
#endif

/* If common register defined, define macro. */
#if defined(__EICR) || defined(__DOXYGEN__)
#define enable_external_int(mask)               (__EICR = mask)
#endif

/** \ingroup deprecated_items
    \deprecated

	This function modifies the \c timsk register.
	The value you pass via \c ints is device specific. */

static __inline__ void timer_enable_int (unsigned char ints)
{
#ifdef TIMSK
    TIMSK = ints;
#endif
}

/** \def INTERRUPT(signame)
    \ingroup deprecated_items
    \deprecated

    Introduces an interrupt handler function that runs with global interrupts
    initially enabled. This allows interrupt handlers to be interrupted.

    As this macro has been used by too many unsuspecting people in the
    past, it has been deprecated, and will be removed in a future
    version of the library.  Users who want to legitimately re-enable
    interrupts in their interrupt handlers as quickly as possible are
    encouraged to explicitly declare their handlers as described
    \ref attr_interrupt "above".
*/

#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 1) || (__GNUC__ > 4)
#  define __INTR_ATTRS used, externally_visible
#else /* GCC < 4.1 */
#  define __INTR_ATTRS used
#endif

#ifdef __cplusplus
#define INTERRUPT(signame)				\
extern "C" void signame(void);				\
void signame (void) __attribute__ ((interrupt,__INTR_ATTRS));	\
void signame (void)
#else
#define INTERRUPT(signame)				\
void signame (void) __attribute__ ((interrupt,__INTR_ATTRS));	\
void signame (void)
#endif

/*@}*/

/**
   \name Obsolete IO macros

   Back in a time when AVR-GCC and avr-libc could not handle IO port
   access in the direct assignment form as they are handled now, all
   IO port access had to be done through specific macros that
   eventually resulted in inline assembly instructions performing the
   desired action.

   These macros became obsolete, as reading and writing IO ports can
   be done by simply using the IO port name in an expression, and all
   bit manipulation (including those on IO ports) can be done using
   generic C bit manipulation operators.

   The macros in this group simulate the historical behaviour.  While
   they are supposed to be applied to IO ports, the emulation actually
   uses standard C methods, so they could be applied to arbitrary
   memory locations as well.
*/

/*@{*/

/**
   \ingroup deprecated_items
   \def inp(port)
   \deprecated

   Read a value from an IO port \c port.
*/
#define inp(port) (port)

/**
   \ingroup deprecated_items
   \def outp(val, port)
   \deprecated

   Write \c val to IO port \c port.
*/
#define outp(val, port) (port) = (val)

/**
   \ingroup deprecated_items
   \def inb(port)
   \deprecated

   Read a value from an IO port \c port.
*/
#define inb(port) (port)

/**
   \ingroup deprecated_items
   \def outb(port, val)
   \deprecated

   Write \c val to IO port \c port.
*/
#define outb(port, val) (port) = (val)

/**
   \ingroup deprecated_items
   \def sbi(port, bit)
   \deprecated

   Set \c bit in IO port \c port.
*/
#define sbi(port, bit) (port) |= (1 << (bit))

/**
   \ingroup deprecated_items
   \def cbi(port, bit)
   \deprecated

   Clear \c bit in IO port \c port.
*/
#define cbi(port, bit) (port) &= ~(1 << (bit))

/*@}*/

#endif /* _COMPAT_DEPRECATED_H_ */
