/* Copyright (c) 2002, 2003, 2004  Marek Michalkiewicz
   Copyright (c) 2005, 2007 Joerg Wunsch
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

/* $Id: crc16.h,v 1.4 2007/01/23 15:32:48 joerg_wunsch Exp $ */

#ifndef _UTIL_CRC16_H_
#define _UTIL_CRC16_H_

#include <stdint.h>

/** \file */
/** \defgroup util_crc <util/crc16.h>: CRC Computations
    \code#include <util/crc16.h>\endcode

    This header file provides a optimized inline functions for calculating
    cyclic redundancy checks (CRC) using common polynomials.

    \par References:

    \par

    See the Dallas Semiconductor app note 27 for 8051 assembler example and
    general CRC optimization suggestions. The table on the last page of the
    app note is the key to understanding these implementations.

    \par

    Jack Crenshaw's "Implementing CRCs" article in the January 1992 isue of \e
    Embedded \e Systems \e Programming. This may be difficult to find, but it
    explains CRC's in very clear and concise terms. Well worth the effort to
    obtain a copy.

    A typical application would look like:

    \code
    // Dallas iButton test vector.
    uint8_t serno[] = { 0x02, 0x1c, 0xb8, 0x01, 0, 0, 0, 0xa2 };

    int
    checkcrc(void)
    {
	uint8_t crc = 0, i;

	for (i = 0; i < sizeof serno / sizeof serno[0]; i++)
	    crc = _crc_ibutton_update(crc, serno[i]);

	return crc; // must be 0
    }
    \endcode
*/

/** \ingroup util_crc
    Optimized CRC-16 calculation.

    Polynomial: x^16 + x^15 + x^2 + 1 (0xa001)<br>
    Initial value: 0xffff

    This CRC is normally used in disk-drive controllers.

    The following is the equivalent functionality written in C.

    \code
    uint16_t
    crc16_update(uint16_t crc, uint8_t a)
    {
	int i;

	crc ^= a;
	for (i = 0; i < 8; ++i)
	{
	    if (crc & 1)
		crc = (crc >> 1) ^ 0xA001;
	    else
		crc = (crc >> 1);
	}

	return crc;
    }

    \endcode */

static __inline__ uint16_t
_crc16_update(uint16_t __crc, uint8_t __data)
{
	uint8_t __tmp;
	uint16_t __ret;

	__asm__ __volatile__ (
		"eor %A0,%2" "\n\t"
		"mov %1,%A0" "\n\t"
		"swap %1" "\n\t"
		"eor %1,%A0" "\n\t"
		"mov __tmp_reg__,%1" "\n\t"
		"lsr %1" "\n\t"
		"lsr %1" "\n\t"
		"eor %1,__tmp_reg__" "\n\t"
		"mov __tmp_reg__,%1" "\n\t"
		"lsr %1" "\n\t"
		"eor %1,__tmp_reg__" "\n\t"
		"andi %1,0x07" "\n\t"
		"mov __tmp_reg__,%A0" "\n\t"
		"mov %A0,%B0" "\n\t"
		"lsr %1" "\n\t"
		"ror __tmp_reg__" "\n\t"
		"ror %1" "\n\t"
		"mov %B0,__tmp_reg__" "\n\t"
		"eor %A0,%1" "\n\t"
		"lsr __tmp_reg__" "\n\t"
		"ror %1" "\n\t"
		"eor %B0,__tmp_reg__" "\n\t"
		"eor %A0,%1"
		: "=r" (__ret), "=d" (__tmp)
		: "r" (__data), "0" (__crc)
		: "r0"
	);
	return __ret;
}

/** \ingroup util_crc
    Optimized CRC-XMODEM calculation.

    Polynomial: x^16 + x^12 + x^5 + 1 (0x1021)<br>
    Initial value: 0x0

    This is the CRC used by the Xmodem-CRC protocol.

    The following is the equivalent functionality written in C.

    \code
    uint16_t
    crc_xmodem_update (uint16_t crc, uint8_t data)
    {
        int i;

        crc = crc ^ ((uint16_t)data << 8);
        for (i=0; i<8; i++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }

        return crc;
    }
    \endcode */

static __inline__ uint16_t
_crc_xmodem_update(uint16_t __crc, uint8_t __data)
{
    uint16_t __ret;             /* %B0:%A0 (alias for __crc) */
    uint8_t __tmp1;             /* %1 */
    uint8_t __tmp2;             /* %2 */
                                /* %3  __data */

    __asm__ __volatile__ (
        "eor    %B0,%3"          "\n\t" /* crc.hi ^ data */
        "mov    __tmp_reg__,%B0" "\n\t"
        "swap   __tmp_reg__"     "\n\t" /* swap(crc.hi ^ data) */

        /* Calculate the ret.lo of the CRC. */
        "mov    %1,__tmp_reg__"  "\n\t"
        "andi   %1,0x0f"         "\n\t"
        "eor    %1,%B0"          "\n\t"
        "mov    %2,%B0"          "\n\t"
        "eor    %2,__tmp_reg__"  "\n\t"
        "lsl    %2"              "\n\t"
        "andi   %2,0xe0"         "\n\t"
        "eor    %1,%2"           "\n\t" /* __tmp1 is now ret.lo. */

        /* Calculate the ret.hi of the CRC. */
        "mov    %2,__tmp_reg__"  "\n\t"
        "eor    %2,%B0"          "\n\t"
        "andi   %2,0xf0"         "\n\t"
        "lsr    %2"              "\n\t"
        "mov    __tmp_reg__,%B0" "\n\t"
        "lsl    __tmp_reg__"     "\n\t"
        "rol    %2"              "\n\t"
        "lsr    %B0"             "\n\t"
        "lsr    %B0"             "\n\t"
        "lsr    %B0"             "\n\t"
        "andi   %B0,0x1f"        "\n\t"
        "eor    %B0,%2"          "\n\t"
        "eor    %B0,%A0"         "\n\t" /* ret.hi is now ready. */
        "mov    %A0,%1"          "\n\t" /* ret.lo is now ready. */
        : "=d" (__ret), "=d" (__tmp1), "=d" (__tmp2)
        : "r" (__data), "0" (__crc)
        : "r0"
    );
    return __ret;
}

/** \ingroup util_crc
    Optimized CRC-CCITT calculation.

    Polynomial: x^16 + x^12 + x^5 + 1 (0x8408)<br>
    Initial value: 0xffff

    This is the CRC used by PPP and IrDA.

    See RFC1171 (PPP protocol) and IrDA IrLAP 1.1

    \note Although the CCITT polynomial is the same as that used by the Xmodem
    protocol, they are quite different. The difference is in how the bits are
    shifted through the alorgithm. Xmodem shifts the MSB of the CRC and the
    input first, while CCITT shifts the LSB of the CRC and the input first.

    The following is the equivalent functionality written in C.

    \code
    uint16_t
    crc_ccitt_update (uint16_t crc, uint8_t data)
    {
        data ^= lo8 (crc);
        data ^= data << 4;

        return ((((uint16_t)data << 8) | hi8 (crc)) ^ (uint8_t)(data >> 4) 
                ^ ((uint16_t)data << 3));
    }
    \endcode */

static __inline__ uint16_t
_crc_ccitt_update (uint16_t __crc, uint8_t __data)
{
    uint16_t __ret;

    __asm__ __volatile__ (
        "eor    %A0,%1"          "\n\t"

        "mov    __tmp_reg__,%A0" "\n\t"
        "swap   %A0"             "\n\t"
        "andi   %A0,0xf0"        "\n\t"
        "eor    %A0,__tmp_reg__" "\n\t"

        "mov    __tmp_reg__,%B0" "\n\t"

        "mov    %B0,%A0"         "\n\t"

        "swap   %A0"             "\n\t"
        "andi   %A0,0x0f"        "\n\t"
        "eor    __tmp_reg__,%A0" "\n\t"

        "lsr    %A0"             "\n\t"
        "eor    %B0,%A0"         "\n\t"

        "eor    %A0,%B0"         "\n\t"
        "lsl    %A0"             "\n\t"
        "lsl    %A0"             "\n\t"
        "lsl    %A0"             "\n\t"
        "eor    %A0,__tmp_reg__"

        : "=d" (__ret)
        : "r" (__data), "0" (__crc)
        : "r0"
    );
    return __ret;
}

/** \ingroup util_crc
    Optimized Dallas (now Maxim) iButton 8-bit CRC calculation.

    Polynomial: x^8 + x^5 + x^4 + 1 (0x8C)<br>
    Initial value: 0x0

    See http://www.maxim-ic.com/appnotes.cfm/appnote_number/27

    The following is the equivalent functionality written in C.

    \code
    uint8_t
    _crc_ibutton_update(uint8_t crc, uint8_t data)
    {
	uint8_t i;

	crc = crc ^ data;
	for (i = 0; i < 8; i++)
	{
	    if (crc & 0x01)
	        crc = (crc >> 1) ^ 0x8C;
	    else
	        crc >>= 1;
	}

	return crc;
    }
    \endcode
*/

static __inline__ uint8_t
_crc_ibutton_update(uint8_t __crc, uint8_t __data)
{
	uint8_t __i, __pattern;
	__asm__ __volatile__ (
		"	eor	%0, %4" "\n\t"
		"	ldi	%1, 8" "\n\t"
		"	ldi	%2, 0x8C" "\n\t"
		"1:	bst	%0, 0" "\n\t"
		"	lsr	%0" "\n\t"
		"	brtc	2f" "\n\t"
		"	eor	%0, %2" "\n\t"
		"2:	dec	%1" "\n\t"
		"	brne	1b" "\n\t"
		: "=r" (__crc), "=d" (__i), "=d" (__pattern)
		: "0" (__crc), "r" (__data));
	return __crc;
}

#endif /* _UTIL_CRC16_H_ */
