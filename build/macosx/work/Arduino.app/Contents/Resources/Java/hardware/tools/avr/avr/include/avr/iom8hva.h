/* Copyright (c) 2007, Anatoly Sokolov 
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

/* $Id: iom8hva.h,v 1.2.2.4 2008/08/14 00:08:05 arcanum Exp $ */

/* iom8hva.h - definitions for ATmega8HVA.  */

#ifndef _AVR_IOM8HVA_H_
#define _AVR_IOM8HVA_H_ 1

#include <avr/iomxxhva.h>

/* Constants */
#define SPM_PAGESIZE 128
#define RAMEND      0x2FF
#define XRAMEND     0x2FF
#define E2END       0xFF
#define E2PAGESIZE  4
#define FLASHEND    0x1FFF


/* Fuses */

#define FUSE_MEMORY_SIZE 1

/* Low Fuse Byte */
#define FUSE_SUT0        (unsigned char)~_BV(0)
#define FUSE_SUT1        (unsigned char)~_BV(1)
#define FUSE_SUT2        (unsigned char)~_BV(2)
#define FUSE_SELFPRGEN   (unsigned char)~_BV(3)
#define FUSE_DWEN        (unsigned char)~_BV(4)
#define FUSE_SPIEN       (unsigned char)~_BV(5)
#define FUSE_EESAVE      (unsigned char)~_BV(6)
#define FUSE_WDTON       (unsigned char)~_BV(7)
#define FUSE_DEFAULT (FUSE_SPIEN)


/* Lock Bits */
#define __LOCK_BITS_EXIST


#endif /* _AVR_IOM8HVA_H_ */

