/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011-2012, Atmel Corporation
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

#ifndef USB_DEVICE_H_INCLUDED
#define USB_DEVICE_H_INCLUDED

#include <stdint.h>

extern void UDD_WaitIN(void);
extern void UDD_WaitOUT(void);
extern void UDD_ClearIN(void);
extern void UDD_ClearOUT(void);
extern uint32_t UDD_WaitForINOrOUT(void);
extern void UDD_ClearRxFlag(unsigned char bEndpoint);
extern uint32_t UDD_ReceivedSetupInt(void);
extern void UDD_ClearSetupInt(void);
extern uint32_t UDD_ReadWriteAllowed(uint32_t ep);
extern uint32_t UDD_FifoByteCount(uint32_t ep);
extern uint8_t UDD_FifoFree(void);
extern void UDD_ReleaseRX(uint32_t ep);
extern void UDD_ReleaseTX(uint32_t ep);
extern uint8_t UDD_FrameNumber(void);
extern uint8_t UDD_GetConfiguration(void);

extern uint32_t UDD_Send(uint32_t ep, const void* data, uint32_t len);
extern void UDD_Send8(uint32_t ep,  uint8_t data );
extern uint8_t UDD_Recv8(uint32_t ep);
extern void UDD_Recv(uint32_t ep, uint8_t* data, uint32_t len);

extern void UDD_InitEndpoints(const uint32_t* eps_table, const uint32_t ul_eps_table_size);
extern void UDD_InitControl(int end);
extern uint32_t UDD_Init(void);
extern void UDD_InitEP( uint32_t ul_ep, uint32_t ul_ep_cfg );

extern void UDD_Attach(void);
extern void UDD_Detach(void);

extern void UDD_SetStack(void (*pf_isr)(void));
extern void UDD_SetAddress(uint32_t addr);
extern void UDD_Stall(void);
extern uint32_t UDD_GetFrameNumber(void);

/*! \name Usual Types
 */

//! @{

typedef unsigned char           Bool; //!< Boolean.
#ifndef __cplusplus
#if !defined(__bool_true_false_are_defined)
typedef unsigned char           bool; //!< Boolean.
#endif
#endif
typedef int8_t                  S8;  //!< 8-bit signed integer.
typedef uint8_t                 U8;  //!< 8-bit unsigned integer.
typedef int16_t                 S16;  //!< 16-bit signed integer.
typedef uint16_t                U16;  //!< 16-bit unsigned integer.
typedef uint16_t                le16_t;
typedef uint16_t                be16_t;
typedef int32_t                 S32;  //!< 32-bit signed integer.
typedef uint32_t                U32;  //!< 32-bit unsigned integer.
typedef uint32_t                le32_t;
typedef uint32_t                be32_t;
typedef int64_t                 S64;  //!< 64-bit signed integer.
typedef uint64_t                U64;  //!< 64-bit unsigned integer.
typedef float                   F32;  //!< 32-bit floating-point number.
typedef double                  F64;  //!< 64-bit floating-point number.
typedef uint32_t                iram_size_t;

//! @}

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

/*! \name Token Paste
 *
 * Paste N preprocessing tokens together, these tokens being allowed to be \#defined.
 *
 * May be used only within macros with the tokens passed as arguments if the tokens are \#defined.
 *
 * For example, writing TPASTE2(U, WIDTH) within a macro \#defined by
 * UTYPE(WIDTH) and invoked as UTYPE(UL_WIDTH) with UL_WIDTH \#defined as 32 is
 * equivalent to writing U32.
 */

//! @{

#define TPASTE2( a, b)                            a##b
#define TPASTE3( a, b, c)                         a##b##c
#define TPASTE4( a, b, c, d)                      a##b##c##d
#define TPASTE5( a, b, c, d, e)                   a##b##c##d##e
#define TPASTE6( a, b, c, d, e, f)                a##b##c##d##e##f
#define TPASTE7( a, b, c, d, e, f, g)             a##b##c##d##e##f##g
#define TPASTE8( a, b, c, d, e, f, g, h)          a##b##c##d##e##f##g##h
#define TPASTE9( a, b, c, d, e, f, g, h, i)       a##b##c##d##e##f##g##h##i
#define TPASTE10(a, b, c, d, e, f, g, h, i, j)    a##b##c##d##e##f##g##h##i##j

//! @}

/*! \name Absolute Token Paste
 *
 * Paste N preprocessing tokens together, these tokens being allowed to be \#defined.
 *
 * No restriction of use if the tokens are \#defined.
 *
 * For example, writing ATPASTE2(U, UL_WIDTH) anywhere with UL_WIDTH \#defined
 * as 32 is equivalent to writing U32.
 */

//! @{

#define ATPASTE2( a, b)                           TPASTE2( a, b)
#define ATPASTE3( a, b, c)                        TPASTE3( a, b, c)
#define ATPASTE4( a, b, c, d)                     TPASTE4( a, b, c, d)
#define ATPASTE5( a, b, c, d, e)                  TPASTE5( a, b, c, d, e)
#define ATPASTE6( a, b, c, d, e, f)               TPASTE6( a, b, c, d, e, f)
#define ATPASTE7( a, b, c, d, e, f, g)            TPASTE7( a, b, c, d, e, f, g)
#define ATPASTE8( a, b, c, d, e, f, g, h)         TPASTE8( a, b, c, d, e, f, g, h)
#define ATPASTE9( a, b, c, d, e, f, g, h, i)      TPASTE9( a, b, c, d, e, f, g, h, i)
#define ATPASTE10(a, b, c, d, e, f, g, h, i, j)   TPASTE10(a, b, c, d, e, f, g, h, i, j)

//! @}

/*! \brief Counts the trailing zero bits of the given value considered as a 32-bit integer.
 *
 * \param u Value of which to count the trailing zero bits.
 *
 * \return The count of trailing zero bits in \a u.
 */
#if (defined __GNUC__) || (defined __CC_ARM)
#   define ctz(u)              __builtin_ctz(u)
#else
#   define ctz(u)              ((u) & (1ul <<  0) ?  0 : \
                                (u) & (1ul <<  1) ?  1 : \
                                (u) & (1ul <<  2) ?  2 : \
                                (u) & (1ul <<  3) ?  3 : \
                                (u) & (1ul <<  4) ?  4 : \
                                (u) & (1ul <<  5) ?  5 : \
                                (u) & (1ul <<  6) ?  6 : \
                                (u) & (1ul <<  7) ?  7 : \
                                (u) & (1ul <<  8) ?  8 : \
                                (u) & (1ul <<  9) ?  9 : \
                                (u) & (1ul << 10) ? 10 : \
                                (u) & (1ul << 11) ? 11 : \
                                (u) & (1ul << 12) ? 12 : \
                                (u) & (1ul << 13) ? 13 : \
                                (u) & (1ul << 14) ? 14 : \
                                (u) & (1ul << 15) ? 15 : \
                                (u) & (1ul << 16) ? 16 : \
                                (u) & (1ul << 17) ? 17 : \
                                (u) & (1ul << 18) ? 18 : \
                                (u) & (1ul << 19) ? 19 : \
                                (u) & (1ul << 20) ? 20 : \
                                (u) & (1ul << 21) ? 21 : \
                                (u) & (1ul << 22) ? 22 : \
                                (u) & (1ul << 23) ? 23 : \
                                (u) & (1ul << 24) ? 24 : \
                                (u) & (1ul << 25) ? 25 : \
                                (u) & (1ul << 26) ? 26 : \
                                (u) & (1ul << 27) ? 27 : \
                                (u) & (1ul << 28) ? 28 : \
                                (u) & (1ul << 29) ? 29 : \
                                (u) & (1ul << 30) ? 30 : \
                                (u) & (1ul << 31) ? 31 : \
                                32)
#endif

/*! \name Zero-Bit Counting
 *
 * Under GCC, __builtin_clz and __builtin_ctz behave like macros when
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
#if (defined __GNUC__) || (defined __CC_ARM)
#   define clz(u)              __builtin_clz(u)
#elif (defined __ICCARM__)
#   define clz(u)              __CLZ(u)
#else
#   define clz(u)              (((u) == 0)          ? 32 : \
                                ((u) & (1ul << 31)) ?  0 : \
                                ((u) & (1ul << 30)) ?  1 : \
                                ((u) & (1ul << 29)) ?  2 : \
                                ((u) & (1ul << 28)) ?  3 : \
                                ((u) & (1ul << 27)) ?  4 : \
                                ((u) & (1ul << 26)) ?  5 : \
                                ((u) & (1ul << 25)) ?  6 : \
                                ((u) & (1ul << 24)) ?  7 : \
                                ((u) & (1ul << 23)) ?  8 : \
                                ((u) & (1ul << 22)) ?  9 : \
                                ((u) & (1ul << 21)) ? 10 : \
                                ((u) & (1ul << 20)) ? 11 : \
                                ((u) & (1ul << 19)) ? 12 : \
                                ((u) & (1ul << 18)) ? 13 : \
                                ((u) & (1ul << 17)) ? 14 : \
                                ((u) & (1ul << 16)) ? 15 : \
                                ((u) & (1ul << 15)) ? 16 : \
                                ((u) & (1ul << 14)) ? 17 : \
                                ((u) & (1ul << 13)) ? 18 : \
                                ((u) & (1ul << 12)) ? 19 : \
                                ((u) & (1ul << 11)) ? 20 : \
                                ((u) & (1ul << 10)) ? 21 : \
                                ((u) & (1ul <<  9)) ? 22 : \
                                ((u) & (1ul <<  8)) ? 23 : \
                                ((u) & (1ul <<  7)) ? 24 : \
                                ((u) & (1ul <<  6)) ? 25 : \
                                ((u) & (1ul <<  5)) ? 26 : \
                                ((u) & (1ul <<  4)) ? 27 : \
                                ((u) & (1ul <<  3)) ? 28 : \
                                ((u) & (1ul <<  2)) ? 29 : \
                                ((u) & (1ul <<  1)) ? 30 : \
                                31)
#endif

/*! \name Mathematics
 *
 * The same considerations as for clz and ctz apply here but GCC does not
 * provide built-in functions to access the assembly instructions abs, min and
 * max and it does not produce them by itself in most cases, so two sets of
 * macros are defined here:
 *   - Abs, Min and Max to apply to constant expressions (values known at
 *     compile time);
 *   - abs, min and max to apply to non-constant expressions (values unknown at
 *     compile time), abs is found in stdlib.h.
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

// abs() is already defined by stdlib.h

/*! \brief Takes the minimal value of \a a and \a b.
 *
 * \param a Input value.
 * \param b Input value.
 *
 * \return Minimal value of \a a and \a b.
 *
 * \note More optimized if only used with values unknown at compile time.
 */
#define min(a, b)   Min(a, b)

/*! \brief Takes the maximal value of \a a and \a b.
 *
 * \param a Input value.
 * \param b Input value.
 *
 * \return Maximal value of \a a and \a b.
 *
 * \note More optimized if only used with values unknown at compile time.
 */
#define max(a, b)   Max(a, b)

//! @}

#endif /* USB_DEVICE_H_INCLUDED */
