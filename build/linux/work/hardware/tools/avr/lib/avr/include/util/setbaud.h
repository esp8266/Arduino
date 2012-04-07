/* Copyright (c) 2007  Cliff Lawson
   Copyright (c) 2007  Carlos Lamas
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

/* $Id: setbaud.h,v 1.1 2007/10/28 23:01:09 joerg_wunsch Exp $ */

/**
   \file
*/

/**
   \defgroup util_setbaud <util/setbaud.h>: Helper macros for baud rate calculations
   \code
   #define F_CPU 11059200
   #define BAUD 38400
   #include <util/setbaud.h>
   \endcode

   This header file requires that on entry values are already defined
   for F_CPU and BAUD.  In addition, the macro BAUD_TOL will define
   the baud rate tolerance (in percent) that is acceptable during
   the calculations.  The value of BAUD_TOL will default to 2 %.

   This header file defines macros suitable to setup the UART baud
   rate prescaler registers of an AVR.  All calculations are done
   using the C preprocessor.  Including this header file causes no
   other side effects so it is possible to include this file more than
   once (supposedly, with different values for the BAUD parameter),
   possibly even within the same function.

   Assuming that the requested BAUD is valid for the given F_CPU then
   the macro UBRR_VALUE is set to the required prescaler value.  Two
   additional macros are provided for the low and high bytes of the
   prescaler, respectively: UBRRL_VALUE is set to the lower byte of
   the UBRR_VALUE and UBRRH_VALUE is set to the upper byte.  An
   additional macro USE_2X will be defined.  Its value is set to 1 if
   the desired BAUD rate within the given tolerance could only be
   achieved by setting the U2X bit in the UART configuration.  It will
   be defined to 0 if U2X is not needed.

   Example usage:

   \code
   #include <avr/io.h>

   #define F_CPU 4000000

   static void
   uart_9600(void)
   {
   #define BAUD 9600
   #include <util/setbaud.h>
   UBRRH = UBRRH_VALUE;
   UBRRL = UBRRL_VALUE;
   #if USE_2X
   UCSRA |= (1 << U2X);
   #else
   UCSRA &= ~(1 << U2X);
   #endif
   }

   static void
   uart_38400(void)
   {
   #undef BAUD  // avoid compiler warning
   #define BAUD 38400
   #include <util/setbaud.h>
   UBRRH = UBRRH_VALUE;
   UBRRL = UBRRL_VALUE;
   #if USE_2X
   UCSRA |= (1 << U2X);
   #else
   UCSRA &= ~(1 << U2X);
   #endif
   }
   \endcode

   In this example, two functions are defined to setup the UART
   to run at 9600 Bd, and 38400 Bd, respectively.  Using a CPU
   clock of 4 MHz, 9600 Bd can be achieved with an acceptable
   tolerance without setting U2X (prescaler 25), while 38400 Bd
   require U2X to be set (prescaler 12).
*/

#ifndef F_CPU
#  error "setbaud.h requires F_CPU to be defined"
#endif

#ifndef BAUD
#  error "setbaud.h requires BAUD to be defined"
#endif

#if !(F_CPU)
#  error "F_CPU must be a constant value"
#endif

#if !(BAUD)
#  error "BAUD must be a constant value"
#endif

#if defined(__DOXYGEN__)
/**
   \def BAUD_TOL
   \ingroup util_setbaud

   Input and output macro for <util/setbaud.h>

   Define the acceptable baud rate tolerance in percent.  If not set
   on entry, it will be set to its default value of 2.
*/
#define BAUD_TOL 2

/**
   \def UBRR_VALUE
   \ingroup util_setbaud

   Output macro from <util/setbaud.h>

   Contains the calculated baud rate prescaler value for the UBRR
   register.
*/
#define UBRR_VALUE

/**
   \def UBRRL_VALUE
   \ingroup util_setbaud

   Output macro from <util/setbaud.h>

   Contains the lower byte of the calculated prescaler value
   (UBRR_VALUE).
*/
#define UBRRL_VALUE

/**
   \def UBRRH_VALUE
   \ingroup util_setbaud

   Output macro from <util/setbaud.h>

   Contains the upper byte of the calculated prescaler value
   (UBRR_VALUE).
*/
#define UBRRH_VALUE

/**
   \def USE_2X
   \ingroup util_setbaud

   Output bacro from <util/setbaud.h>

   Contains the value 1 if the desired baud rate tolerance could only
   be achieved by setting the U2X bit in the UART configuration.
   Contains 0 otherwise.
*/
#define USE_2X 0

#else /* !__DOXYGEN__ */

#undef USE_2X

/* Baud rate tolerance is 2 % unless previously defined */
#ifndef BAUD_TOL
#  define BAUD_TOL 2
#endif

#define UBRR_VALUE (((F_CPU) + 8UL * (BAUD)) / (16UL * (BAUD)) -1UL)

#if 100 * (F_CPU) > \
  (16 * ((UBRR_VALUE) + 1)) * (100 * (BAUD) + (BAUD) * (BAUD_TOL))
#  define USE_2X 1
#elif 100 * (F_CPU) < \
  (16 * ((UBRR_VALUE) + 1)) * (100 * (BAUD) - (BAUD) * (BAUD_TOL))
#  define USE_2X 1
#else
#  define USE_2X 0
#endif

#if USE_2X
/* U2X required, recalculate */
#undef UBRR_VALUE
#define UBRR_VALUE (((F_CPU) + 4UL * (BAUD)) / (8UL * (BAUD)) -1UL)

#if 100 * (F_CPU) > \
  (8 * ((UBRR_VALUE) + 1)) * (100 * (BAUD) + (BAUD) * (BAUD_TOL))
#  warning "Baud rate achieved is higher than allowed"
#endif

#if 100 * (F_CPU) < \
  (8 * ((UBRR_VALUE) + 1)) * (100 * (BAUD) - (BAUD) * (BAUD_TOL))
#  warning "Baud rate achieved is lower than allowed"
#endif

#endif /* USE_U2X */

#ifdef UBRR_VALUE
#  define UBRRL_VALUE (UBRR_VALUE & 0xff)
#  define UBRRH_VALUE (UBRR_VALUE >> 8)
#endif

#endif /* __DOXYGEN__ */
/* end of util/setbaud.h */
