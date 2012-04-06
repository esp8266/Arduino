/* Copyright (c) 2007, Dmitry Xmelkov
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

/* $Id: alloca.h,v 1.2 2007/12/18 13:34:15 dmix Exp $	*/

#ifndef	_ALLOCA_H
#define _ALLOCA_H  1

#include <stddef.h>

/** \defgroup alloca <alloca.h>: Allocate space in the stack	*/

/** \ingroup alloca
    \brief Allocate \a __size bytes of space in the stack frame of the caller.

    This temporary space is automatically freed when the function that
    called alloca() returns to its caller. Avr-libc defines the alloca() as
    a macro, which is translated into the inlined \c __builtin_alloca()
    function. The fact that the code is inlined, means that it is impossible
    to take the address of this function, or to change its behaviour by
    linking with a different library.

    \return alloca() returns a pointer to the beginning of the allocated
    space. If the allocation causes stack overflow, program behaviour is
    undefined.
    
    \warning Avoid use alloca() inside the list of arguments of a function
    call.
 */
extern void *alloca (size_t __size);

#define alloca(size)	__builtin_alloca (size)

#endif	/* alloca.h */
