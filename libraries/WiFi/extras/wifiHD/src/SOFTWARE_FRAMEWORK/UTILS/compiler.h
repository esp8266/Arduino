/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Compiler file for AVR32.
 *
 * This file defines commonly used types and macros.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

#if ((defined __GNUC__) && (defined __AVR32__)) || (defined __ICCAVR32__ || defined __AAVR32__)
#  include <avr32/io.h>
#endif
#if (defined __ICCAVR32__)
#  include <intrinsics.h>
#endif
#include "preprocessor.h"

#include "parts.h"


//_____ D E C L A R A T I O N S ____________________________________________

#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.

#include <stddef.h>
#include <stdlib.h>


#if (defined __ICCAVR32__)

/*! \name Compiler Keywords
 *
 * Port of some keywords from GNU GCC for AVR32 to IAR Embedded Workbench for Atmel AVR32.
 */
//! @{
#define __asm__             asm
#define __inline__          inline
#define __volatile__
//! @}

#endif


/*! \name Usual Types
 */
//! @{
typedef unsigned char           Bool; //!< Boolean.
#ifndef __cplusplus
#if !defined(__bool_true_false_are_defined)
typedef unsigned char           bool; //!< Boolean.
#endif
#endif
typedef signed char             S8 ;  //!< 8-bit signed integer.
typedef unsigned char           U8 ;  //!< 8-bit unsigned integer.
typedef signed short int        S16;  //!< 16-bit signed integer.
typedef unsigned short int      U16;  //!< 16-bit unsigned integer.
typedef signed long int         S32;  //!< 32-bit signed integer.
typedef unsigned long int       U32;  //!< 32-bit unsigned integer.
typedef signed long long int    S64;  //!< 64-bit signed integer.
typedef unsigned long long int  U64;  //!< 64-bit unsigned integer.
typedef float                   F32;  //!< 32-bit floating-point number.
typedef double                  F64;  //!< 64-bit floating-point number.
//! @}


/*! \name Status Types
 */
//! @{
typedef Bool                Status_bool_t;  //!< Boolean status.
typedef U8                  Status_t;       //!< 8-bit-coded status.
//! @}


/*! \name Aliasing Aggregate Types
 */
//! @{

//! 16-bit union.
typedef union
{
  S16 s16   ;
  U16 u16   ;
  S8  s8 [2];
  U8  u8 [2];
} Union16;

//! 32-bit union.
typedef union
{
  S32 s32   ;
  U32 u32   ;
  S16 s16[2];
  U16 u16[2];
  S8  s8 [4];
  U8  u8 [4];
} Union32;

//! 64-bit union.
typedef union
{
  S64 s64   ;
  U64 u64   ;
  S32 s32[2];
  U32 u32[2];
  S16 s16[4];
  U16 u16[4];
  S8  s8 [8];
  U8  u8 [8];
} Union64;

//! Union of pointers to 64-, 32-, 16- and 8-bit unsigned integers.
typedef union
{
  S64 *s64ptr;
  U64 *u64ptr;
  S32 *s32ptr;
  U32 *u32ptr;
  S16 *s16ptr;
  U16 *u16ptr;
  S8  *s8ptr ;
  U8  *u8ptr ;
} UnionPtr;

//! Union of pointers to volatile 64-, 32-, 16- and 8-bit unsigned integers.
typedef union
{
  volatile S64 *s64ptr;
  volatile U64 *u64ptr;
  volatile S32 *s32ptr;
  volatile U32 *u32ptr;
  volatile S16 *s16ptr;
  volatile U16 *u16ptr;
  volatile S8  *s8ptr ;
  volatile U8  *u8ptr ;
} UnionVPtr;

//! Union of pointers to constant 64-, 32-, 16- and 8-bit unsigned integers.
typedef union
{
  const S64 *s64ptr;
  const U64 *u64ptr;
  const S32 *s32ptr;
  const U32 *u32ptr;
  const S16 *s16ptr;
  const U16 *u16ptr;
  const S8  *s8ptr ;
  const U8  *u8ptr ;
} UnionCPtr;

//! Union of pointers to constant volatile 64-, 32-, 16- and 8-bit unsigned integers.
typedef union
{
  const volatile S64 *s64ptr;
  const volatile U64 *u64ptr;
  const volatile S32 *s32ptr;
  const volatile U32 *u32ptr;
  const volatile S16 *s16ptr;
  const volatile U16 *u16ptr;
  const volatile S8  *s8ptr ;
  const volatile U8  *u8ptr ;
} UnionCVPtr;

//! Structure of pointers to 64-, 32-, 16- and 8-bit unsigned integers.
typedef struct
{
  S64 *s64ptr;
  U64 *u64ptr;
  S32 *s32ptr;
  U32 *u32ptr;
  S16 *s16ptr;
  U16 *u16ptr;
  S8  *s8ptr ;
  U8  *u8ptr ;
} StructPtr;

//! Structure of pointers to volatile 64-, 32-, 16- and 8-bit unsigned integers.
typedef struct
{
  volatile S64 *s64ptr;
  volatile U64 *u64ptr;
  volatile S32 *s32ptr;
  volatile U32 *u32ptr;
  volatile S16 *s16ptr;
  volatile U16 *u16ptr;
  volatile S8  *s8ptr ;
  volatile U8  *u8ptr ;
} StructVPtr;

//! Structure of pointers to constant 64-, 32-, 16- and 8-bit unsigned integers.
typedef struct
{
  const S64 *s64ptr;
  const U64 *u64ptr;
  const S32 *s32ptr;
  const U32 *u32ptr;
  const S16 *s16ptr;
  const U16 *u16ptr;
  const S8  *s8ptr ;
  const U8  *u8ptr ;
} StructCPtr;

//! Structure of pointers to constant volatile 64-, 32-, 16- and 8-bit unsigned integers.
typedef struct
{
  const volatile S64 *s64ptr;
  const volatile U64 *u64ptr;
  const volatile S32 *s32ptr;
  const volatile U32 *u32ptr;
  const volatile S16 *s16ptr;
  const volatile U16 *u16ptr;
  const volatile S8  *s8ptr ;
  const volatile U8  *u8ptr ;
} StructCVPtr;

//! @}

#endif  // __AVR32_ABI_COMPILER__


//_____ M A C R O S ________________________________________________________

/*! \name Usual Constants
 */
//! @{
#define DISABLE   0
#define ENABLE    1
#define DISABLED  0
#define ENABLED   1
#define OFF       0
#define ON        1
#define FALSE     0
#define TRUE      1
#ifndef __cplusplus
#if !defined(__bool_true_false_are_defined)
#define false     FALSE
#define true      TRUE
#endif
#endif
#define KO        0
#define OK        1
#define PASS      0
#define FAIL      1
#define LOW       0
#define HIGH      1
#define CLR       0
#define SET       1
//! @}


#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.

/*! \name Bit-Field Handling
 */
//! @{

/*! \brief Reads the bits of a value specified by a given bit-mask.
 *
 * \param value Value to read bits from.
 * \param mask  Bit-mask indicating bits to read.
 *
 * \return Read bits.
 */
#define Rd_bits( value, mask)        ((value) & (mask))

/*! \brief Writes the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue to write bits to.
 * \param mask    Bit-mask indicating bits to write.
 * \param bits    Bits to write.
 *
 * \return Resulting value with written bits.
 */
#define Wr_bits(lvalue, mask, bits)  ((lvalue) = ((lvalue) & ~(mask)) |\
                                                 ((bits  ) &  (mask)))

/*! \brief Tests the bits of a value specified by a given bit-mask.
 *
 * \param value Value of which to test bits.
 * \param mask  Bit-mask indicating bits to test.
 *
 * \return \c 1 if at least one of the tested bits is set, else \c 0.
 */
#define Tst_bits( value, mask)  (Rd_bits(value, mask) != 0)

/*! \brief Clears the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue of which to clear bits.
 * \param mask    Bit-mask indicating bits to clear.
 *
 * \return Resulting value with cleared bits.
 */
#define Clr_bits(lvalue, mask)  ((lvalue) &= ~(mask))

/*! \brief Sets the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue of which to set bits.
 * \param mask    Bit-mask indicating bits to set.
 *
 * \return Resulting value with set bits.
 */
#define Set_bits(lvalue, mask)  ((lvalue) |=  (mask))

/*! \brief Toggles the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue of which to toggle bits.
 * \param mask    Bit-mask indicating bits to toggle.
 *
 * \return Resulting value with toggled bits.
 */
#define Tgl_bits(lvalue, mask)  ((lvalue) ^=  (mask))

/*! \brief Reads the bit-field of a value specified by a given bit-mask.
 *
 * \param value Value to read a bit-field from.
 * \param mask  Bit-mask indicating the bit-field to read.
 *
 * \return Read bit-field.
 */
#define Rd_bitfield( value, mask)           (Rd_bits( value, mask) >> ctz(mask))

/*! \brief Writes the bit-field of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue    C lvalue to write a bit-field to.
 * \param mask      Bit-mask indicating the bit-field to write.
 * \param bitfield  Bit-field to write.
 *
 * \return Resulting value with written bit-field.
 */
#define Wr_bitfield(lvalue, mask, bitfield) (Wr_bits(lvalue, mask, (U32)(bitfield) << ctz(mask)))

//! @}


/*! \brief This macro is used to test fatal errors.
 *
 * The macro tests if the expression is FALSE. If it is, a fatal error is
 * detected and the application hangs up.
 *
 * \param expr  Expression to evaluate and supposed to be nonzero.
 */
#ifdef _ASSERT_ENABLE_
  #define Assert(expr) \
  {\
    if (!(expr)) while (TRUE);\
  }
#else
  #define Assert(expr)
#endif


/*! \name Zero-Bit Counting
 *
 * Under AVR32-GCC, __builtin_clz and __builtin_ctz behave like macros when
 * applied to constant expressions (values known at compile time), so they are
 * more optimized than the use of the corresponding assembly instructions and
 * they can be used as constant expressions e.g. to initialize objects having
 * static storage duration, and like the corresponding assembly instructions
 * when applied to non-constant expressions (values unknown at compile time), so
 * they are more optimized than an assembly periphrasis. Hence, clz and ctz
 * ensure a possible and optimized behavior for both constant and non-constant
 * expressions.
 */
//! @{

/*! \brief Counts the leading zero bits of the given value considered as a 32-bit integer.
 *
 * \param u Value of which to count the leading zero bits.
 *
 * \return The count of leading zero bits in \a u.
 */
#if (defined __GNUC__)
  #define clz(u)              __builtin_clz(u)
#elif (defined __ICCAVR32__)
  #define clz(u)              __count_leading_zeros(u)
#endif

/*! \brief Counts the trailing zero bits of the given value considered as a 32-bit integer.
 *
 * \param u Value of which to count the trailing zero bits.
 *
 * \return The count of trailing zero bits in \a u.
 */
#if (defined __GNUC__)
  #define ctz(u)              __builtin_ctz(u)
#elif (defined __ICCAVR32__)
  #define ctz(u)              __count_trailing_zeros(u)
#endif

//! @}


/*! \name Bit Reversing
 */
//! @{

/*! \brief Reverses the bits of \a u8.
 *
 * \param u8  U8 of which to reverse the bits.
 *
 * \return Value resulting from \a u8 with reversed bits.
 */
#define bit_reverse8(u8)    ((U8)(bit_reverse32((U8)(u8)) >> 24))

/*! \brief Reverses the bits of \a u16.
 *
 * \param u16 U16 of which to reverse the bits.
 *
 * \return Value resulting from \a u16 with reversed bits.
 */
#define bit_reverse16(u16)  ((U16)(bit_reverse32((U16)(u16)) >> 16))

/*! \brief Reverses the bits of \a u32.
 *
 * \param u32 U32 of which to reverse the bits.
 *
 * \return Value resulting from \a u32 with reversed bits.
 */
#if (defined __GNUC__)
  #define bit_reverse32(u32) \
  (\
    {\
      unsigned int __value = (U32)(u32);\
      __asm__ ("brev\t%0" : "+r" (__value) :  : "cc");\
      (U32)__value;\
    }\
  )
#elif (defined __ICCAVR32__)
  #define bit_reverse32(u32)  ((U32)__bit_reverse((U32)(u32)))
#endif

/*! \brief Reverses the bits of \a u64.
 *
 * \param u64 U64 of which to reverse the bits.
 *
 * \return Value resulting from \a u64 with reversed bits.
 */
#define bit_reverse64(u64)  ((U64)(((U64)bit_reverse32((U64)(u64) >> 32)) |\
                                   ((U64)bit_reverse32((U64)(u64)) << 32)))

//! @}


/*! \name Alignment
 */
//! @{

/*! \brief Tests alignment of the number \a val with the \a n boundary.
 *
 * \param val Input value.
 * \param n   Boundary.
 *
 * \return \c 1 if the number \a val is aligned with the \a n boundary, else \c 0.
 */
#define Test_align(val, n     ) (!Tst_bits( val, (n) - 1     )   )

/*! \brief Gets alignment of the number \a val with respect to the \a n boundary.
 *
 * \param val Input value.
 * \param n   Boundary.
 *
 * \return Alignment of the number \a val with respect to the \a n boundary.
 */
#define Get_align( val, n     ) (  Rd_bits( val, (n) - 1     )   )

/*! \brief Sets alignment of the lvalue number \a lval to \a alg with respect to the \a n boundary.
 *
 * \param lval  Input/output lvalue.
 * \param n     Boundary.
 * \param alg   Alignment.
 *
 * \return New value of \a lval resulting from its alignment set to \a alg with respect to the \a n boundary.
 */
#define Set_align(lval, n, alg) (  Wr_bits(lval, (n) - 1, alg)   )

/*! \brief Aligns the number \a val with the upper \a n boundary.
 *
 * \param val Input value.
 * \param n   Boundary.
 *
 * \return Value resulting from the number \a val aligned with the upper \a n boundary.
 */
#define Align_up(  val, n     ) (((val) + ((n) - 1)) & ~((n) - 1))

/*! \brief Aligns the number \a val with the lower \a n boundary.
 *
 * \param val Input value.
 * \param n   Boundary.
 *
 * \return Value resulting from the number \a val aligned with the lower \a n boundary.
 */
#define Align_down(val, n     ) ( (val)              & ~((n) - 1))

//! @}


/*! \name Mathematics
 *
 * The same considerations as for clz and ctz apply here but AVR32-GCC does not
 * provide built-in functions to access the assembly instructions abs, min and
 * max and it does not produce them by itself in most cases, so two sets of
 * macros are defined here:
 *   - Abs, Min and Max to apply to constant expressions (values known at
 *     compile time);
 *   - abs, min and max to apply to non-constant expressions (values unknown at
 *     compile time).
 */
//! @{

/*! \brief Takes the absolute value of \a a.
 *
 * \param a Input value.
 *
 * \return Absolute value of \a a.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Abs(a)              (((a) <  0 ) ? -(a) : (a))

/*! \brief Takes the minimal value of \a a and \a b.
 *
 * \param a Input value.
 * \param b Input value.
 *
 * \return Minimal value of \a a and \a b.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Min(a, b)           (((a) < (b)) ?  (a) : (b))

/*! \brief Takes the maximal value of \a a and \a b.
 *
 * \param a Input value.
 * \param b Input value.
 *
 * \return Maximal value of \a a and \a b.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Max(a, b)           (((a) > (b)) ?  (a) : (b))

/*! \brief Takes the absolute value of \a a.
 *
 * \param a Input value.
 *
 * \return Absolute value of \a a.
 *
 * \note More optimized if only used with values unknown at compile time.
 */
#if (defined __GNUC__)
  #define abs(a) \
  (\
    {\
      int __value = (a);\
      __asm__ ("abs\t%0" : "+r" (__value) :  : "cc");\
      __value;\
    }\
  )
#elif (defined __ICCAVR32__)
  #define abs(a)      Abs(a)
#endif

/*! \brief Takes the minimal value of \a a and \a b.
 *
 * \param a Input value.
 * \param b Input value.
 *
 * \return Minimal value of \a a and \a b.
 *
 * \note More optimized if only used with values unknown at compile time.
 */
#if (defined __GNUC__)
  #define min(a, b) \
  (\
    {\
      int __value, __arg_a = (a), __arg_b = (b);\
      __asm__ ("min\t%0, %1, %2" : "=r" (__value) : "r" (__arg_a), "r" (__arg_b));\
      __value;\
    }\
  )
#elif (defined __ICCAVR32__)
  #define min(a, b)   __min(a, b)
#endif

/*! \brief Takes the maximal value of \a a and \a b.
 *
 * \param a Input value.
 * \param b Input value.
 *
 * \return Maximal value of \a a and \a b.
 *
 * \note More optimized if only used with values unknown at compile time.
 */
#if (defined __GNUC__)
  #define max(a, b) \
  (\
    {\
      int __value, __arg_a = (a), __arg_b = (b);\
      __asm__ ("max\t%0, %1, %2" : "=r" (__value) : "r" (__arg_a), "r" (__arg_b));\
      __value;\
    }\
  )
#elif (defined __ICCAVR32__)
  #define max(a, b)   __max(a, b)
#endif

//! @}


/*! \brief Calls the routine at address \a addr.
 *
 * It generates a long call opcode.
 *
 * For example, `Long_call(0x80000000)' generates a software reset on a UC3 if
 * it is invoked from the CPU supervisor mode.
 *
 * \param addr  Address of the routine to call.
 *
 * \note It may be used as a long jump opcode in some special cases.
 */
#define Long_call(addr)                   ((*(void (*)(void))(addr))())

/*! \brief Resets the CPU by software.
 *
 * \warning It shall not be called from the CPU application mode.
 */
#if (defined __GNUC__)
  #define Reset_CPU() \
  (\
    {\
      __asm__ __volatile__ (\
        "lddpc   r9, 3f\n\t"\
        "mfsr    r8, %[SR]\n\t"\
        "bfextu  r8, r8, %[SR_M_OFFSET], %[SR_M_SIZE]\n\t"\
        "cp.w    r8, 0b001\n\t"\
        "breq    0f\n\t"\
        "sub     r8, pc, $ - 1f\n\t"\
        "pushm   r8-r9\n\t"\
        "rete\n"\
        "0:\n\t"\
        "mtsr    %[SR], r9\n"\
        "1:\n\t"\
        "mov     r0, 0\n\t"\
        "mov     r1, 0\n\t"\
        "mov     r2, 0\n\t"\
        "mov     r3, 0\n\t"\
        "mov     r4, 0\n\t"\
        "mov     r5, 0\n\t"\
        "mov     r6, 0\n\t"\
        "mov     r7, 0\n\t"\
        "mov     r8, 0\n\t"\
        "mov     r9, 0\n\t"\
        "mov     r10, 0\n\t"\
        "mov     r11, 0\n\t"\
        "mov     r12, 0\n\t"\
        "mov     sp, 0\n\t"\
        "stdsp   sp[0], sp\n\t"\
        "ldmts   sp, sp\n\t"\
        "mov     lr, 0\n\t"\
        "lddpc   pc, 2f\n\t"\
        ".balign 4\n"\
        "2:\n\t"\
        ".word   _start\n"\
        "3:\n\t"\
        ".word   %[RESET_SR]"\
        :\
        : [SR] "i" (AVR32_SR),\
          [SR_M_OFFSET] "i" (AVR32_SR_M_OFFSET),\
          [SR_M_SIZE] "i" (AVR32_SR_M_SIZE),\
          [RESET_SR] "i" (AVR32_SR_GM_MASK | AVR32_SR_EM_MASK | (AVR32_SR_M_SUP << AVR32_SR_M_OFFSET))\
      );\
    }\
  )
#elif (defined __ICCAVR32__)
  #define Reset_CPU() \
  {\
    extern void *volatile __program_start;\
    __asm__ __volatile__ (\
      "mov     r7, LWRD(__program_start)\n\t"\
      "orh     r7, HWRD(__program_start)\n\t"\
      "mov     r9, LWRD("ASTRINGZ(AVR32_SR_GM_MASK | AVR32_SR_EM_MASK | (AVR32_SR_M_SUP << AVR32_SR_M_OFFSET))")\n\t"\
      "orh     r9, HWRD("ASTRINGZ(AVR32_SR_GM_MASK | AVR32_SR_EM_MASK | (AVR32_SR_M_SUP << AVR32_SR_M_OFFSET))")\n\t"\
      "mfsr    r8, "ASTRINGZ(AVR32_SR)"\n\t"\
      "bfextu  r8, r8, "ASTRINGZ(AVR32_SR_M_OFFSET)", "ASTRINGZ(AVR32_SR_M_SIZE)"\n\t"\
      "cp.w    r8, 001b\n\t"\
      "breq    $ + 10\n\t"\
      "sub     r8, pc, -12\n\t"\
      "pushm   r8-r9\n\t"\
      "rete\n\t"\
      "mtsr    "ASTRINGZ(AVR32_SR)", r9\n\t"\
      "mov     r0, 0\n\t"\
      "mov     r1, 0\n\t"\
      "mov     r2, 0\n\t"\
      "mov     r3, 0\n\t"\
      "mov     r4, 0\n\t"\
      "mov     r5, 0\n\t"\
      "mov     r6, 0\n\t"\
      "st.w    r0[4], r7\n\t"\
      "mov     r7, 0\n\t"\
      "mov     r8, 0\n\t"\
      "mov     r9, 0\n\t"\
      "mov     r10, 0\n\t"\
      "mov     r11, 0\n\t"\
      "mov     r12, 0\n\t"\
      "mov     sp, 0\n\t"\
      "stdsp   sp[0], sp\n\t"\
      "ldmts   sp, sp\n\t"\
      "mov     lr, 0\n\t"\
      "ld.w    pc, lr[4]"\
    );\
    __program_start;\
  }
#endif


/*! \name System Register Access
 */
//! @{

/*! \brief Gets the value of the \a sysreg system register.
 *
 * \param sysreg  Address of the system register of which to get the value.
 *
 * \return Value of the \a sysreg system register.
 */
#if (defined __GNUC__)
  #define Get_system_register(sysreg)         __builtin_mfsr(sysreg)
#elif (defined __ICCAVR32__)
  #define Get_system_register(sysreg)         __get_system_register(sysreg)
#endif

/*! \brief Sets the value of the \a sysreg system register to \a value.
 *
 * \param sysreg  Address of the system register of which to set the value.
 * \param value   Value to set the \a sysreg system register to.
 */
#if (defined __GNUC__)
  #define Set_system_register(sysreg, value)  __builtin_mtsr(sysreg, value)
#elif (defined __ICCAVR32__)
  #define Set_system_register(sysreg, value)  __set_system_register(sysreg, value)
#endif

//! @}


/*! \name CPU Status Register Access
 */
//! @{

/*! \brief Tells whether exceptions are globally enabled.
 *
 * \return \c 1 if exceptions are globally enabled, else \c 0.
 */
#define Is_global_exception_enabled()         (!Tst_bits(Get_system_register(AVR32_SR), AVR32_SR_EM_MASK))

/*! \brief Disables exceptions globally.
 */
#if (defined __GNUC__)
  #define Disable_global_exception()          ({__asm__ __volatile__ ("ssrf\t%0" :  : "i" (AVR32_SR_EM_OFFSET));})
#elif (defined __ICCAVR32__)
  #define Disable_global_exception()          (__set_status_flag(AVR32_SR_EM_OFFSET))
#endif

/*! \brief Enables exceptions globally.
 */
#if (defined __GNUC__)
  #define Enable_global_exception()           ({__asm__ __volatile__ ("csrf\t%0" :  : "i" (AVR32_SR_EM_OFFSET));})
#elif (defined __ICCAVR32__)
  #define Enable_global_exception()           (__clear_status_flag(AVR32_SR_EM_OFFSET))
#endif

/*! \brief Tells whether interrupts are globally enabled.
 *
 * \return \c 1 if interrupts are globally enabled, else \c 0.
 */
#define Is_global_interrupt_enabled()         (!Tst_bits(Get_system_register(AVR32_SR), AVR32_SR_GM_MASK))

/*! \brief Disables interrupts globally.
 */
#if (defined __GNUC__)
  #define Disable_global_interrupt()          ({__asm__ __volatile__ ("ssrf\t%0" :  : "i" (AVR32_SR_GM_OFFSET));})
#elif (defined __ICCAVR32__)
  #define Disable_global_interrupt()          (__disable_interrupt())
#endif

/*! \brief Enables interrupts globally.
 */
#if (defined __GNUC__)
  #define Enable_global_interrupt()           ({__asm__ __volatile__ ("csrf\t%0" :  : "i" (AVR32_SR_GM_OFFSET));})
#elif (defined __ICCAVR32__)
  #define Enable_global_interrupt()           (__enable_interrupt())
#endif

/*! \brief Tells whether interrupt level \a int_level is enabled.
 *
 * \param int_level Interrupt level (0 to 3).
 *
 * \return \c 1 if interrupt level \a int_level is enabled, else \c 0.
 */
#define Is_interrupt_level_enabled(int_level) (!Tst_bits(Get_system_register(AVR32_SR), TPASTE3(AVR32_SR_I, int_level, M_MASK)))

/*! \brief Disables interrupt level \a int_level.
 *
 * \param int_level Interrupt level to disable (0 to 3).
 */
#if (defined __GNUC__)
  #define Disable_interrupt_level(int_level)  ({__asm__ __volatile__ ("ssrf\t%0" :  : "i" (TPASTE3(AVR32_SR_I, int_level, M_OFFSET)));})
#elif (defined __ICCAVR32__)
  #define Disable_interrupt_level(int_level)  (__set_status_flag(TPASTE3(AVR32_SR_I, int_level, M_OFFSET)))
#endif

/*! \brief Enables interrupt level \a int_level.
 *
 * \param int_level Interrupt level to enable (0 to 3).
 */
#if (defined __GNUC__)
  #define Enable_interrupt_level(int_level)   ({__asm__ __volatile__ ("csrf\t%0" :  : "i" (TPASTE3(AVR32_SR_I, int_level, M_OFFSET)));})
#elif (defined __ICCAVR32__)
  #define Enable_interrupt_level(int_level)   (__clear_status_flag(TPASTE3(AVR32_SR_I, int_level, M_OFFSET)))
#endif

/*! \brief Protects subsequent code from interrupts.
 */
#define AVR32_ENTER_CRITICAL_REGION( ) \
  { \
  Bool global_interrupt_enabled = Is_global_interrupt_enabled(); \
  Disable_global_interrupt(); // Disable the appropriate interrupts.

/*! \brief This macro must always be used in conjunction with AVR32_ENTER_CRITICAL_REGION
 *         so that interrupts are enabled again.
 */
#define AVR32_LEAVE_CRITICAL_REGION( ) \
  if (global_interrupt_enabled) Enable_global_interrupt(); \
  }

//! @}


/*! \name Debug Register Access
 */
//! @{

/*! \brief Gets the value of the \a dbgreg debug register.
 *
 * \param dbgreg  Address of the debug register of which to get the value.
 *
 * \return Value of the \a dbgreg debug register.
 */
#if (defined __GNUC__)
  #define Get_debug_register(dbgreg)          __builtin_mfdr(dbgreg)
#elif (defined __ICCAVR32__)
  #define Get_debug_register(dbgreg)          __get_debug_register(dbgreg)
#endif

/*! \brief Sets the value of the \a dbgreg debug register to \a value.
 *
 * \param dbgreg  Address of the debug register of which to set the value.
 * \param value   Value to set the \a dbgreg debug register to.
 */
#if (defined __GNUC__)
  #define Set_debug_register(dbgreg, value)   __builtin_mtdr(dbgreg, value)
#elif (defined __ICCAVR32__)
  #define Set_debug_register(dbgreg, value)   __set_debug_register(dbgreg, value)
#endif

//! @}

#endif  // __AVR32_ABI_COMPILER__


//! Boolean evaluating MCU little endianism.
#if ((defined __GNUC__) && (defined __AVR32__)) || ((defined __ICCAVR32__) || (defined __AAVR32__))
  #define LITTLE_ENDIAN_MCU     FALSE
#else
  #error If you are here, you should check what is exactly the processor you are using...
  #define LITTLE_ENDIAN_MCU     FALSE
#endif

// Check that MCU endianism is correctly defined.
#ifndef LITTLE_ENDIAN_MCU
  #error YOU MUST define the MCU endianism with LITTLE_ENDIAN_MCU: either FALSE or TRUE
#endif

//! Boolean evaluating MCU big endianism.
#define BIG_ENDIAN_MCU        (!LITTLE_ENDIAN_MCU)


#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.

/*! \name MCU Endianism Handling
 */
//! @{

#if (LITTLE_ENDIAN_MCU==TRUE)
  #define LSB(u16)        (((U8  *)&(u16))[0])  //!< Least significant byte of \a u16.
  #define MSB(u16)        (((U8  *)&(u16))[1])  //!< Most significant byte of \a u16.

  #define LSH(u32)        (((U16 *)&(u32))[0])  //!< Least significant half-word of \a u32.
  #define MSH(u32)        (((U16 *)&(u32))[1])  //!< Most significant half-word of \a u32.
  #define LSB0W(u32)      (((U8  *)&(u32))[0])  //!< Least significant byte of 1st rank of \a u32.
  #define LSB1W(u32)      (((U8  *)&(u32))[1])  //!< Least significant byte of 2nd rank of \a u32.
  #define LSB2W(u32)      (((U8  *)&(u32))[2])  //!< Least significant byte of 3rd rank of \a u32.
  #define LSB3W(u32)      (((U8  *)&(u32))[3])  //!< Least significant byte of 4th rank of \a u32.
  #define MSB3W(u32)      LSB0W(u32)            //!< Most significant byte of 4th rank of \a u32.
  #define MSB2W(u32)      LSB1W(u32)            //!< Most significant byte of 3rd rank of \a u32.
  #define MSB1W(u32)      LSB2W(u32)            //!< Most significant byte of 2nd rank of \a u32.
  #define MSB0W(u32)      LSB3W(u32)            //!< Most significant byte of 1st rank of \a u32.

  #define LSW(u64)        (((U32 *)&(u64))[0])  //!< Least significant word of \a u64.
  #define MSW(u64)        (((U32 *)&(u64))[1])  //!< Most significant word of \a u64.
  #define LSH0(u64)       (((U16 *)&(u64))[0])  //!< Least significant half-word of 1st rank of \a u64.
  #define LSH1(u64)       (((U16 *)&(u64))[1])  //!< Least significant half-word of 2nd rank of \a u64.
  #define LSH2(u64)       (((U16 *)&(u64))[2])  //!< Least significant half-word of 3rd rank of \a u64.
  #define LSH3(u64)       (((U16 *)&(u64))[3])  //!< Least significant half-word of 4th rank of \a u64.
  #define MSH3(u64)       LSH0(u64)             //!< Most significant half-word of 4th rank of \a u64.
  #define MSH2(u64)       LSH1(u64)             //!< Most significant half-word of 3rd rank of \a u64.
  #define MSH1(u64)       LSH2(u64)             //!< Most significant half-word of 2nd rank of \a u64.
  #define MSH0(u64)       LSH3(u64)             //!< Most significant half-word of 1st rank of \a u64.
  #define LSB0D(u64)      (((U8  *)&(u64))[0])  //!< Least significant byte of 1st rank of \a u64.
  #define LSB1D(u64)      (((U8  *)&(u64))[1])  //!< Least significant byte of 2nd rank of \a u64.
  #define LSB2D(u64)      (((U8  *)&(u64))[2])  //!< Least significant byte of 3rd rank of \a u64.
  #define LSB3D(u64)      (((U8  *)&(u64))[3])  //!< Least significant byte of 4th rank of \a u64.
  #define LSB4D(u64)      (((U8  *)&(u64))[4])  //!< Least significant byte of 5th rank of \a u64.
  #define LSB5D(u64)      (((U8  *)&(u64))[5])  //!< Least significant byte of 6th rank of \a u64.
  #define LSB6D(u64)      (((U8  *)&(u64))[6])  //!< Least significant byte of 7th rank of \a u64.
  #define LSB7D(u64)      (((U8  *)&(u64))[7])  //!< Least significant byte of 8th rank of \a u64.
  #define MSB7D(u64)      LSB0D(u64)            //!< Most significant byte of 8th rank of \a u64.
  #define MSB6D(u64)      LSB1D(u64)            //!< Most significant byte of 7th rank of \a u64.
  #define MSB5D(u64)      LSB2D(u64)            //!< Most significant byte of 6th rank of \a u64.
  #define MSB4D(u64)      LSB3D(u64)            //!< Most significant byte of 5th rank of \a u64.
  #define MSB3D(u64)      LSB4D(u64)            //!< Most significant byte of 4th rank of \a u64.
  #define MSB2D(u64)      LSB5D(u64)            //!< Most significant byte of 3rd rank of \a u64.
  #define MSB1D(u64)      LSB6D(u64)            //!< Most significant byte of 2nd rank of \a u64.
  #define MSB0D(u64)      LSB7D(u64)            //!< Most significant byte of 1st rank of \a u64.

#elif (BIG_ENDIAN_MCU==TRUE) 
  #define MSB(u16)        (((U8  *)&(u16))[0])  //!< Most significant byte of \a u16.
  #define LSB(u16)        (((U8  *)&(u16))[1])  //!< Least significant byte of \a u16.

  #define MSH(u32)        (((U16 *)&(u32))[0])  //!< Most significant half-word of \a u32.
  #define LSH(u32)        (((U16 *)&(u32))[1])  //!< Least significant half-word of \a u32.
  #define MSB0W(u32)      (((U8  *)&(u32))[0])  //!< Most significant byte of 1st rank of \a u32.
  #define MSB1W(u32)      (((U8  *)&(u32))[1])  //!< Most significant byte of 2nd rank of \a u32.
  #define MSB2W(u32)      (((U8  *)&(u32))[2])  //!< Most significant byte of 3rd rank of \a u32.
  #define MSB3W(u32)      (((U8  *)&(u32))[3])  //!< Most significant byte of 4th rank of \a u32.
  #define LSB3W(u32)      MSB0W(u32)            //!< Least significant byte of 4th rank of \a u32.
  #define LSB2W(u32)      MSB1W(u32)            //!< Least significant byte of 3rd rank of \a u32.
  #define LSB1W(u32)      MSB2W(u32)            //!< Least significant byte of 2nd rank of \a u32.
  #define LSB0W(u32)      MSB3W(u32)            //!< Least significant byte of 1st rank of \a u32.

  #define MSW(u64)        (((U32 *)&(u64))[0])  //!< Most significant word of \a u64.
  #define LSW(u64)        (((U32 *)&(u64))[1])  //!< Least significant word of \a u64.
  #define MSH0(u64)       (((U16 *)&(u64))[0])  //!< Most significant half-word of 1st rank of \a u64.
  #define MSH1(u64)       (((U16 *)&(u64))[1])  //!< Most significant half-word of 2nd rank of \a u64.
  #define MSH2(u64)       (((U16 *)&(u64))[2])  //!< Most significant half-word of 3rd rank of \a u64.
  #define MSH3(u64)       (((U16 *)&(u64))[3])  //!< Most significant half-word of 4th rank of \a u64.
  #define LSH3(u64)       MSH0(u64)             //!< Least significant half-word of 4th rank of \a u64.
  #define LSH2(u64)       MSH1(u64)             //!< Least significant half-word of 3rd rank of \a u64.
  #define LSH1(u64)       MSH2(u64)             //!< Least significant half-word of 2nd rank of \a u64.
  #define LSH0(u64)       MSH3(u64)             //!< Least significant half-word of 1st rank of \a u64.
  #define MSB0D(u64)      (((U8  *)&(u64))[0])  //!< Most significant byte of 1st rank of \a u64.
  #define MSB1D(u64)      (((U8  *)&(u64))[1])  //!< Most significant byte of 2nd rank of \a u64.
  #define MSB2D(u64)      (((U8  *)&(u64))[2])  //!< Most significant byte of 3rd rank of \a u64.
  #define MSB3D(u64)      (((U8  *)&(u64))[3])  //!< Most significant byte of 4th rank of \a u64.
  #define MSB4D(u64)      (((U8  *)&(u64))[4])  //!< Most significant byte of 5th rank of \a u64.
  #define MSB5D(u64)      (((U8  *)&(u64))[5])  //!< Most significant byte of 6th rank of \a u64.
  #define MSB6D(u64)      (((U8  *)&(u64))[6])  //!< Most significant byte of 7th rank of \a u64.
  #define MSB7D(u64)      (((U8  *)&(u64))[7])  //!< Most significant byte of 8th rank of \a u64.
  #define LSB7D(u64)      MSB0D(u64)            //!< Least significant byte of 8th rank of \a u64.
  #define LSB6D(u64)      MSB1D(u64)            //!< Least significant byte of 7th rank of \a u64.
  #define LSB5D(u64)      MSB2D(u64)            //!< Least significant byte of 6th rank of \a u64.
  #define LSB4D(u64)      MSB3D(u64)            //!< Least significant byte of 5th rank of \a u64.
  #define LSB3D(u64)      MSB4D(u64)            //!< Least significant byte of 4th rank of \a u64.
  #define LSB2D(u64)      MSB5D(u64)            //!< Least significant byte of 3rd rank of \a u64.
  #define LSB1D(u64)      MSB6D(u64)            //!< Least significant byte of 2nd rank of \a u64.
  #define LSB0D(u64)      MSB7D(u64)            //!< Least significant byte of 1st rank of \a u64.

#else
  #error  Unknown endianism.
#endif

//! @}


/*! \name Endianism Conversion
 *
 * The same considerations as for clz and ctz apply here but AVR32-GCC's
 * __builtin_bswap_16 and __builtin_bswap_32 do not behave like macros when
 * applied to constant expressions, so two sets of macros are defined here:
 *   - Swap16, Swap32 and Swap64 to apply to constant expressions (values known
 *     at compile time);
 *   - swap16, swap32 and swap64 to apply to non-constant expressions (values
 *     unknown at compile time).
 */
//! @{

/*! \brief Toggles the endianism of \a u16 (by swapping its bytes).
 *
 * \param u16 U16 of which to toggle the endianism.
 *
 * \return Value resulting from \a u16 with toggled endianism.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Swap16(u16) ((U16)(((U16)(u16) >> 8) |\
                           ((U16)(u16) << 8)))

/*! \brief Toggles the endianism of \a u32 (by swapping its bytes).
 *
 * \param u32 U32 of which to toggle the endianism.
 *
 * \return Value resulting from \a u32 with toggled endianism.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Swap32(u32) ((U32)(((U32)Swap16((U32)(u32) >> 16)) |\
                           ((U32)Swap16((U32)(u32)) << 16)))

/*! \brief Toggles the endianism of \a u64 (by swapping its bytes).
 *
 * \param u64 U64 of which to toggle the endianism.
 *
 * \return Value resulting from \a u64 with toggled endianism.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Swap64(u64) ((U64)(((U64)Swap32((U64)(u64) >> 32)) |\
                           ((U64)Swap32((U64)(u64)) << 32)))

/*! \brief Toggles the endianism of \a u16 (by swapping its bytes).
 *
 * \param u16 U16 of which to toggle the endianism.
 *
 * \return Value resulting from \a u16 with toggled endianism.
 *
 * \note More optimized if only used with values unknown at compile time.
 */
#if (defined __GNUC__)
  #define swap16(u16) ((U16)__builtin_bswap_16((U16)(u16)))
#elif (defined __ICCAVR32__)
  #define swap16(u16) ((U16)__swap_bytes_in_halfwords((U16)(u16)))
#endif

/*! \brief Toggles the endianism of \a u32 (by swapping its bytes).
 *
 * \param u32 U32 of which to toggle the endianism.
 *
 * \return Value resulting from \a u32 with toggled endianism.
 *
 * \note More optimized if only used with values unknown at compile time.
 */
#if (defined __GNUC__)
  #define swap32(u32) ((U32)__builtin_bswap_32((U32)(u32)))
#elif (defined __ICCAVR32__)
  #define swap32(u32) ((U32)__swap_bytes((U32)(u32)))
#endif

/*! \brief Toggles the endianism of \a u64 (by swapping its bytes).
 *
 * \param u64 U64 of which to toggle the endianism.
 *
 * \return Value resulting from \a u64 with toggled endianism.
 *
 * \note More optimized if only used with values unknown at compile time.
 */
#define swap64(u64) ((U64)(((U64)swap32((U64)(u64) >> 32)) |\
                           ((U64)swap32((U64)(u64)) << 32)))

//! @}


/*! \name Target Abstraction
 */
//! @{

#define _GLOBEXT_           extern      //!< extern storage-class specifier.
#define _CONST_TYPE_        const       //!< const type qualifier.
#define _MEM_TYPE_SLOW_                 //!< Slow memory type.
#define _MEM_TYPE_MEDFAST_              //!< Fairly fast memory type.
#define _MEM_TYPE_FAST_                 //!< Fast memory type.

typedef U8                  Byte;       //!< 8-bit unsigned integer.

#define memcmp_ram2ram      memcmp      //!< Target-specific memcmp of RAM to RAM.
#define memcmp_code2ram     memcmp      //!< Target-specific memcmp of RAM to NVRAM.
#define memcpy_ram2ram      memcpy      //!< Target-specific memcpy from RAM to RAM.
#define memcpy_code2ram     memcpy      //!< Target-specific memcpy from NVRAM to RAM.

#define LSB0(u32)           LSB0W(u32)  //!< Least significant byte of 1st rank of \a u32.
#define LSB1(u32)           LSB1W(u32)  //!< Least significant byte of 2nd rank of \a u32.
#define LSB2(u32)           LSB2W(u32)  //!< Least significant byte of 3rd rank of \a u32.
#define LSB3(u32)           LSB3W(u32)  //!< Least significant byte of 4th rank of \a u32.
#define MSB3(u32)           MSB3W(u32)  //!< Most significant byte of 4th rank of \a u32.
#define MSB2(u32)           MSB2W(u32)  //!< Most significant byte of 3rd rank of \a u32.
#define MSB1(u32)           MSB1W(u32)  //!< Most significant byte of 2nd rank of \a u32.
#define MSB0(u32)           MSB0W(u32)  //!< Most significant byte of 1st rank of \a u32.

//! @}

#endif  // __AVR32_ABI_COMPILER__


#endif  // _COMPILER_H_
