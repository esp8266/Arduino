/* Copyright (c) 2004, Joerg Wunsch
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
  POSSIBILITY OF SUCH DAMAGE.
*/

/* $Id: stdlib_private.h 1657 2008-03-24 17:11:08Z arcanum $ */

#include <inttypes.h>
#include <stdlib.h>
#include <avr/io.h>

#if !defined(__DOXYGEN__)

struct __freelist {
	size_t sz;
	struct __freelist *nx;
};

#endif

extern char *__brkval;		/* first location not yet allocated */
extern struct __freelist *__flp; /* freelist pointer (head of freelist) */
extern size_t __malloc_margin;	/* user-changeable before the first malloc() */
extern char *__malloc_heap_start;
extern char *__malloc_heap_end;

extern char __heap_start;
extern char __heap_end;

/* Needed for definition of AVR_STACK_POINTER_REG. */
#include <avr/io.h>

#define STACK_POINTER() ((char *)AVR_STACK_POINTER_REG)

