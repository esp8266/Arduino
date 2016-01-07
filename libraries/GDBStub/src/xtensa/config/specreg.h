/*
 * Xtensa Special Register symbolic names
 */

/* $Id: //depot/rel/Boreal/Xtensa/SWConfig/hal/specreg.h.tpp#2 $ */

/* Customer ID=7011; Build=0x2b6f6; Copyright (c) 1998-2002 Tensilica Inc.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#ifndef XTENSA_SPECREG_H
#define XTENSA_SPECREG_H

/*  Include these special register bitfield definitions, for historical reasons:  */
#include <xtensa/corebits.h>


/*  Special registers:  */
#define SAR		3
#define LITBASE		5
#define IBREAKENABLE	96
#define DDR		104
#define IBREAKA_0	128
#define DBREAKA_0	144
#define DBREAKC_0	160
#define EPC_1		177
#define EPC_2		178
#define EPC_3		179
#define DEPC		192
#define EPS_2		194
#define EPS_3		195
#define EXCSAVE_1	209
#define EXCSAVE_2	210
#define EXCSAVE_3	211
#define INTERRUPT	226
#define INTENABLE	228
#define PS		230
#define VECBASE		231
#define EXCCAUSE	232
#define DEBUGCAUSE	233
#define CCOUNT		234
#define PRID		235
#define ICOUNT		236
#define ICOUNTLEVEL	237
#define EXCVADDR	238
#define CCOMPARE_0	240

/*  Special cases (bases of special register series):  */
#define IBREAKA		128
#define DBREAKA		144
#define DBREAKC		160
#define EPC		176
#define EPS		192
#define EXCSAVE		208
#define CCOMPARE	240

/*  Special names for read-only and write-only interrupt registers:  */
#define INTREAD		226
#define INTSET		226
#define INTCLEAR	227

#endif /* XTENSA_SPECREG_H */

