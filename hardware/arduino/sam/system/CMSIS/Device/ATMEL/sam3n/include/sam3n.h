/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef _SAM3N_
#define _SAM3N_

#if defined __SAM3N00A__
  #include "sam3n00a.h"
#elif defined __SAM3N0A__
  #include "sam3n0a.h"
#elif defined __SAM3N00B__
  #include "sam3n00b.h"
#elif defined __SAM3N0B__
  #include "sam3n0b.h"
#elif defined __SAM3N0C__
  #include "sam3n0c.h"
#elif defined __SAM3N1A__
  #include "sam3n1a.h"
#elif defined __SAM3N1B__
  #include "sam3n1b.h"
#elif defined __SAM3N1C__
  #include "sam3n1c.h"
#elif defined __SAM3N2A__
  #include "sam3n2a.h"
#elif defined __SAM3N2B__
  #include "sam3n2b.h"
#elif defined __SAM3N2C__
  #include "sam3n2c.h"
#elif defined __SAM3N4A__
  #include "sam3n4a.h"
#elif defined __SAM3N4B__
  #include "sam3n4b.h"
#elif defined __SAM3N4C__
  #include "sam3n4c.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAM3N_ */
