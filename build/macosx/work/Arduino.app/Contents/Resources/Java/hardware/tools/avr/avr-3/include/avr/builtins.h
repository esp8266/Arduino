/* Copyright (c) 2008 Anatoly Sokolv
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

/* $Id: 31-avr-libc-1.6.4-builtins.patch,v 1.1 2008/12/11 06:30:57 arcanum Exp $ */

/*
   avr/builtins.h - 
 */
 
#ifndef _AVR_BUILTINS_H_
#define _AVR_BUILTINS_H_

/** \file */
/** \defgroup avr_builtins <avr/builtins.h>: GCC builtins
    \code #include <avr/builtins.h> \endcode

    This header file declares avr builtins.  */

/** 
    \ingroup avr_builtins

    Enables interrupts by setting the global interrupt mask.  */
extern void __builtin_avr_sei(void);

/** 
    \ingroup avr_builtins

    Disables all interrupts by clearing the global interrupt mask.  */
extern void __builtin_avr_cli(void);

/** 
    \ingroup avr_builtins

    TODO.  */

extern void __builtin_avr_sleep(void);

/** 
    \ingroup avr_builtins

    TODO.  */
extern void __builtin_avr_wdr(void);

/** 
    \ingroup avr_builtins

    TODO.  */
extern unsigned char __builtin_avr_swap(unsigned char __b);

/** 
    \ingroup avr_builtins

    TODO.  */
extern unsigned int __builtin_avr_fmul(unsigned char __a, unsigned char __b);

/** 
    \ingroup avr_builtins

    TODO.  */
extern int __builtin_avr_fmuls(char __a, char __b);

/** 
    \ingroup avr_builtins

    TODO.  */
extern int __builtin_avr_fmulsu(char __a, unsigned char __b);

/** 
    \ingroup avr_builtins

    TODO.  */
extern void __builtin_avr_delay_cycles(unsigned long __n);

#endif /* _AVR_BUILTINS_H_ */