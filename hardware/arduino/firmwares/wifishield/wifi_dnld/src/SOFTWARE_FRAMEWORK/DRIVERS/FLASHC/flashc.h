/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief FLASHC driver for AVR32 UC3.
 *
 * AVR32 Flash Controller driver module.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a FLASHC module can be used.
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

#ifndef _FLASHC_H_
#define _FLASHC_H_

#include <avr32/io.h>
#include <stddef.h>
#include "compiler.h"

//! Number of flash regions defined by the FLASHC.
#define AVR32_FLASHC_REGIONS  (AVR32_FLASHC_FLASH_SIZE /\
                               (AVR32_FLASHC_PAGES_PR_REGION * AVR32_FLASHC_PAGE_SIZE))


/*! \name Flash Properties
 */
//! @{

/*! \brief Gets the size of the whole flash array.
 *
 * \return The size of the whole flash array in bytes.
 */
extern unsigned int flashc_get_flash_size(void);

/*! \brief Gets the total number of pages in the flash array.
 *
 * \return The total number of pages in the flash array.
 */
extern unsigned int flashc_get_page_count(void);

/*! \brief Gets the number of pages in each flash region.
 *
 * \return The number of pages in each flash region.
 */
extern unsigned int flashc_get_page_count_per_region(void);

/*! \brief Gets the region number of a page.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashc_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 *
 * \return The region number of the specified page.
 */
extern unsigned int flashc_get_page_region(int page_number);

/*! \brief Gets the number of the first page of a region.
 *
 * \param region The region number: \c 0 to <tt>(AVR32_FLASHC_REGIONS - 1)</tt>.
 *
 * \return The number of the first page of the specified region.
 */
extern unsigned int flashc_get_region_first_page_number(unsigned int region);

//! @}


/*! \name FLASHC Control
 */
//! @{

/*! \brief Gets the number of wait states of flash read accesses.
 *
 * \return The number of wait states of flash read accesses.
 */
extern unsigned int flashc_get_wait_state(void);

/*! \brief Sets the number of wait states of flash read accesses.
 *
 * \param wait_state The number of wait states of flash read accesses: \c 0 to
 *                   \c 1.
 */
extern void flashc_set_wait_state(unsigned int wait_state);

/*! \brief Tells whether the Flash Ready interrupt is enabled.
 *
 * \return Whether the Flash Ready interrupt is enabled.
 */
extern Bool flashc_is_ready_int_enabled(void);

/*! \brief Enables or disables the Flash Ready interrupt.
 *
 * \param enable Whether to enable the Flash Ready interrupt: \c TRUE or
 *               \c FALSE.
 */
extern void flashc_enable_ready_int(Bool enable);

/*! \brief Tells whether the Lock Error interrupt is enabled.
 *
 * \return Whether the Lock Error interrupt is enabled.
 */
extern Bool flashc_is_lock_error_int_enabled(void);

/*! \brief Enables or disables the Lock Error interrupt.
 *
 * \param enable Whether to enable the Lock Error interrupt: \c TRUE or
 *               \c FALSE.
 */
extern void flashc_enable_lock_error_int(Bool enable);

/*! \brief Tells whether the Programming Error interrupt is enabled.
 *
 * \return Whether the Programming Error interrupt is enabled.
 */
extern Bool flashc_is_prog_error_int_enabled(void);

/*! \brief Enables or disables the Programming Error interrupt.
 *
 * \param enable Whether to enable the Programming Error interrupt: \c TRUE or
 *               \c FALSE.
 */
extern void flashc_enable_prog_error_int(Bool enable);

//! @}


/*! \name FLASHC Status
 */
//! @{

/*! \brief Tells whether the FLASHC is ready to run a new command.
 *
 * \return Whether the FLASHC is ready to run a new command.
 */
extern Bool flashc_is_ready(void);

/*! \brief Waits actively until the FLASHC is ready to run a new command.
 *
 * This is the default function assigned to \ref flashc_wait_until_ready.
 */
extern void flashc_default_wait_until_ready(void);

//! Pointer to the function used by the driver when it needs to wait until the
//! FLASHC is ready to run a new command.
//! The default function is \ref flashc_default_wait_until_ready.
//! The user may change this pointer to use another implementation.
extern void (*volatile flashc_wait_until_ready)(void);

/*! \brief Tells whether a Lock Error has occurred during the last function
 *         called that issued one or more FLASHC commands.
 *
 * \return Whether a Lock Error has occurred during the last function called
 *         that issued one or more FLASHC commands.
 */
extern Bool flashc_is_lock_error(void);

/*! \brief Tells whether a Programming Error has occurred during the last
 *         function called that issued one or more FLASHC commands.
 *
 * \return Whether a Programming Error has occurred during the last function
 *         called that issued one or more FLASHC commands.
 */
extern Bool flashc_is_programming_error(void);

//! @}


/*! \name FLASHC Command Control
 */
//! @{

/*! \brief Gets the last issued FLASHC command.
 *
 * \return The last issued FLASHC command.
 */
extern unsigned int flashc_get_command(void);

/*! \brief Gets the current FLASHC page number.
 *
 * \return The current FLASHC page number.
 */
extern unsigned int flashc_get_page_number(void);

/*! \brief Issues a FLASHC command.
 *
 * \param command The command: \c AVR32_FLASHC_FCMD_CMD_x.
 * \param page_number The page number to apply the command to:
 *   \arg \c 0 to <tt>(flashc_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: use this to apply the command to the current page number
 *        or if the command does not apply to any page number;
 *   \arg this argument may have other meanings according to the command. See
 *        the FLASHC chapter of the MCU datasheet.
 *
 * \warning A Lock Error is issued if the command violates the protection
 *          mechanism.
 *
 * \warning A Programming Error is issued if the command is invalid.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern void flashc_issue_command(unsigned int command, int page_number);

//! @}


/*! \name FLASHC Global Commands
 */
//! @{

/*! \brief Issues a No Operation command to the FLASHC.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern void flashc_no_operation(void);

/*! \brief Issues an Erase All command to the FLASHC.
 *
 * This command erases all bits in the flash array, the general-purpose fuse
 * bits and the Security bit. The User page is not erased.
 *
 * This command also ensures that all volatile memories, such as register file
 * and RAMs, are erased before the Security bit is erased, i.e. deactivated.
 *
 * \warning A Lock Error is issued if at least one region is locked or the
 *          bootloader protection is active.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 */
extern void flashc_erase_all(void);

//! @}


/*! \name FLASHC Protection Mechanisms
 */
//! @{

/*! \brief Tells whether the Security bit is active.
 *
 * \return Whether the Security bit is active.
 */
extern Bool flashc_is_security_bit_active(void);

/*! \brief Activates the Security bit.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern void flashc_activate_security_bit(void);

/*! \brief Gets the bootloader protected size.
 *
 * \return The bootloader protected size in bytes.
 */
extern unsigned int flashc_get_bootloader_protected_size(void);

/*! \brief Sets the bootloader protected size.
 *
 * \param bootprot_size The wanted bootloader protected size in bytes. If this
 *                      size is not supported, the actual size will be the
 *                      nearest greater available size or the maximal possible
 *                      size if the requested size is too large.
 *
 * \return The actual bootloader protected size in bytes.
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern unsigned int flashc_set_bootloader_protected_size(unsigned int bootprot_size);

/*! \brief Tells whether external privileged fetch is locked.
 *
 * \return Whether external privileged fetch is locked.
 */
extern Bool flashc_is_external_privileged_fetch_locked(void);

/*! \brief Locks or unlocks external privileged fetch.
 *
 * \param lock Whether to lock external privileged fetch: \c TRUE or \c FALSE.
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern void flashc_lock_external_privileged_fetch(Bool lock);

/*! \brief Tells whether the region of a page is locked.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashc_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 *
 * \return Whether the region of the specified page is locked.
 */
extern Bool flashc_is_page_region_locked(int page_number);

/*! \brief Tells whether a region is locked.
 *
 * \param region The region number: \c 0 to <tt>(AVR32_FLASHC_REGIONS - 1)</tt>.
 *
 * \return Whether the specified region is locked.
 */
extern Bool flashc_is_region_locked(unsigned int region);

/*! \brief Locks or unlocks the region of a page.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashc_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 * \param lock Whether to lock the region of the specified page: \c TRUE or
 *             \c FALSE.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern void flashc_lock_page_region(int page_number, Bool lock);

/*! \brief Locks or unlocks a region.
 *
 * \param region The region number: \c 0 to <tt>(AVR32_FLASHC_REGIONS - 1)</tt>.
 * \param lock Whether to lock the specified region: \c TRUE or \c FALSE.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern void flashc_lock_region(unsigned int region, Bool lock);

/*! \brief Locks or unlocks all regions.
 *
 * \param lock Whether to lock the regions: \c TRUE or \c FALSE.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern void flashc_lock_all_regions(Bool lock);

//! @}


/*! \name Access to General-Purpose Fuses
 */
//! @{

/*! \brief Reads a general-purpose fuse bit.
 *
 * \param gp_fuse_bit The general-purpose fuse bit: \c 0 to \c 63.
 *
 * \return The value of the specified general-purpose fuse bit.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern Bool flashc_read_gp_fuse_bit(unsigned int gp_fuse_bit);

/*! \brief Reads a general-purpose fuse bit-field.
 *
 * \param pos The bit-position of the general-purpose fuse bit-field: \c 0 to
 *            \c 63.
 * \param width The bit-width of the general-purpose fuse bit-field: \c 0 to
 *              \c 64.
 *
 * \return The value of the specified general-purpose fuse bit-field.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern U64 flashc_read_gp_fuse_bitfield(unsigned int pos, unsigned int width);

/*! \brief Reads a general-purpose fuse byte.
 *
 * \param gp_fuse_byte The general-purpose fuse byte: \c 0 to \c 7.
 *
 * \return The value of the specified general-purpose fuse byte.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern U8 flashc_read_gp_fuse_byte(unsigned int gp_fuse_byte);

/*! \brief Reads all general-purpose fuses.
 *
 * \return The value of all general-purpose fuses as a word.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern U64 flashc_read_all_gp_fuses(void);

/*! \brief Erases a general-purpose fuse bit.
 *
 * \param gp_fuse_bit The general-purpose fuse bit: \c 0 to \c 63.
 * \param check Whether to check erase: \c TRUE or \c FALSE.
 *
 * \return Whether the erase succeeded or always \c TRUE if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued if the Security bit is active and the command
 *          is applied to BOOTPROT or EPFL fuses.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern Bool flashc_erase_gp_fuse_bit(unsigned int gp_fuse_bit, Bool check);

/*! \brief Erases a general-purpose fuse bit-field.
 *
 * \param pos The bit-position of the general-purpose fuse bit-field: \c 0 to
 *            \c 63.
 * \param width The bit-width of the general-purpose fuse bit-field: \c 0 to
 *              \c 64.
 * \param check Whether to check erase: \c TRUE or \c FALSE.
 *
 * \return Whether the erase succeeded or always \c TRUE if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued if the Security bit is active and the command
 *          is applied to BOOTPROT or EPFL fuses.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern Bool flashc_erase_gp_fuse_bitfield(unsigned int pos, unsigned int width, Bool check);

/*! \brief Erases a general-purpose fuse byte.
 *
 * \param gp_fuse_byte The general-purpose fuse byte: \c 0 to \c 7.
 * \param check Whether to check erase: \c TRUE or \c FALSE.
 *
 * \return Whether the erase succeeded or always \c TRUE if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern Bool flashc_erase_gp_fuse_byte(unsigned int gp_fuse_byte, Bool check);

/*! \brief Erases all general-purpose fuses.
 *
 * \param check Whether to check erase: \c TRUE or \c FALSE.
 *
 * \return Whether the erase succeeded or always \c TRUE if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern Bool flashc_erase_all_gp_fuses(Bool check);

/*! \brief Writes a general-purpose fuse bit.
 *
 * \param gp_fuse_bit The general-purpose fuse bit: \c 0 to \c 63.
 * \param value The value of the specified general-purpose fuse bit.
 *
 * \warning A Lock Error is issued if the Security bit is active and the command
 *          is applied to BOOTPROT or EPFL fuses.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note A write operation can only clear bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashc_write_gp_fuse_bit(unsigned int gp_fuse_bit, Bool value);

/*! \brief Writes a general-purpose fuse bit-field.
 *
 * \param pos The bit-position of the general-purpose fuse bit-field: \c 0 to
 *            \c 63.
 * \param width The bit-width of the general-purpose fuse bit-field: \c 0 to
 *              \c 64.
 * \param value The value of the specified general-purpose fuse bit-field.
 *
 * \warning A Lock Error is issued if the Security bit is active and the command
 *          is applied to BOOTPROT or EPFL fuses.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note A write operation can only clear bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashc_write_gp_fuse_bitfield(unsigned int pos, unsigned int width, U64 value);

/*! \brief Writes a general-purpose fuse byte.
 *
 * \param gp_fuse_byte The general-purpose fuse byte: \c 0 to \c 7.
 * \param value The value of the specified general-purpose fuse byte.
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note A write operation can only clear bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashc_write_gp_fuse_byte(unsigned int gp_fuse_byte, U8 value);

/*! \brief Writes all general-purpose fuses.
 *
 * \param value The value of all general-purpose fuses as a word.
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note A write operation can only clear bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashc_write_all_gp_fuses(U64 value);

/*! \brief Sets a general-purpose fuse bit with the appropriate erase and write
 *         operations.
 *
 * \param gp_fuse_bit The general-purpose fuse bit: \c 0 to \c 63.
 * \param value The value of the specified general-purpose fuse bit.
 *
 * \warning A Lock Error is issued if the Security bit is active and the command
 *          is applied to BOOTPROT or EPFL fuses.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashc_set_gp_fuse_bit(unsigned int gp_fuse_bit, Bool value);

/*! \brief Sets a general-purpose fuse bit-field with the appropriate erase and
 *         write operations.
 *
 * \param pos The bit-position of the general-purpose fuse bit-field: \c 0 to
 *            \c 63.
 * \param width The bit-width of the general-purpose fuse bit-field: \c 0 to
 *              \c 64.
 * \param value The value of the specified general-purpose fuse bit-field.
 *
 * \warning A Lock Error is issued if the Security bit is active and the command
 *          is applied to BOOTPROT or EPFL fuses.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashc_set_gp_fuse_bitfield(unsigned int pos, unsigned int width, U64 value);

/*! \brief Sets a general-purpose fuse byte with the appropriate erase and write
 *         operations.
 *
 * \param gp_fuse_byte The general-purpose fuse byte: \c 0 to \c 7.
 * \param value The value of the specified general-purpose fuse byte.
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashc_set_gp_fuse_byte(unsigned int gp_fuse_byte, U8 value);

/*! \brief Sets all general-purpose fuses with the appropriate erase and write
 *         operations.
 *
 * \param value The value of all general-purpose fuses as a word.
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHC_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashc_set_all_gp_fuses(U64 value);

//! @}


/*! \name Access to Flash Pages
 */
//! @{

/*! \brief Clears the page buffer.
 *
 * This command resets all bits in the page buffer to one. Write accesses to the
 * page buffer can only change page buffer bits from one to zero.
 *
 * \warning The page buffer is not automatically reset after a page write.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern void flashc_clear_page_buffer(void);

/*! \brief Tells whether the page to which the last Quick Page Read or Quick
 *         Page Read User Page command was applied was erased.
 *
 * \return Whether the page to which the last Quick Page Read or Quick Page Read
 *         User Page command was applied was erased.
 */
extern Bool flashc_is_page_erased(void);

/*! \brief Applies the Quick Page Read command to a page.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashc_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 *
 * \return Whether the specified page is erased.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern Bool flashc_quick_page_read(int page_number);

/*! \brief Erases a page.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashc_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 * \param check Whether to check erase: \c TRUE or \c FALSE.
 *
 * \return Whether the erase succeeded or always \c TRUE if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued if the command is applied to a page belonging
 *          to a locked region or to the bootloader protected area.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 */
extern Bool flashc_erase_page(int page_number, Bool check);

/*! \brief Erases all pages within the flash array.
 *
 * \param check Whether to check erase: \c TRUE or \c FALSE.
 *
 * \return Whether the erase succeeded or always \c TRUE if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued if at least one region is locked or the
 *          bootloader protection is active.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 */
extern Bool flashc_erase_all_pages(Bool check);

/*! \brief Writes a page from the page buffer.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashc_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 *
 * \warning A Lock Error is issued if the command is applied to a page belonging
 *          to a locked region or to the bootloader protected area.
 *
 * \warning The page buffer is not automatically reset after a page write.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note A write operation can only clear bits.
 */
extern void flashc_write_page(int page_number);

/*! \brief Issues a Quick Page Read User Page command to the FLASHC.
 *
 * \return Whether the User page is erased.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern Bool flashc_quick_user_page_read(void);

/*! \brief Erases the User page.
 *
 * \param check Whether to check erase: \c TRUE or \c FALSE.
 *
 * \return Whether the erase succeeded or always \c TRUE if erase check was not
 *         requested.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 */
extern Bool flashc_erase_user_page(Bool check);

/*! \brief Writes the User page from the page buffer.
 *
 * \warning The page buffer is not automatically reset after a page write.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 *
 * \note A write operation can only clear bits.
 */
extern void flashc_write_user_page(void);

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the repeated \a src source byte.
 *
 * The destination areas that are not within the flash array or the User page
 * are ignored.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Source byte.
 * \param nbytes Number of bytes to set.
 * \param erase Whether to erase before writing: \c TRUE or \c FALSE.
 *
 * \return The value of \a dst.
 *
 * \warning This function may be called with \a erase set to \c FALSE only if
 *          the destination consists only of erased words, i.e. this function
 *          can not be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern volatile void *flashc_memset8(volatile void *dst, U8 src, size_t nbytes, Bool erase);

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the repeated \a src big-endian source half-word.
 *
 * The destination areas that are not within the flash array or the User page
 * are ignored.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Source half-word.
 * \param nbytes Number of bytes to set.
 * \param erase Whether to erase before writing: \c TRUE or \c FALSE.
 *
 * \return The value of \a dst.
 *
 * \warning This function may be called with \a erase set to \c FALSE only if
 *          the destination consists only of erased words, i.e. this function
 *          can not be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern volatile void *flashc_memset16(volatile void *dst, U16 src, size_t nbytes, Bool erase);

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the repeated \a src big-endian source word.
 *
 * The destination areas that are not within the flash array or the User page
 * are ignored.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Source word.
 * \param nbytes Number of bytes to set.
 * \param erase Whether to erase before writing: \c TRUE or \c FALSE.
 *
 * \return The value of \a dst.
 *
 * \warning This function may be called with \a erase set to \c FALSE only if
 *          the destination consists only of erased words, i.e. this function
 *          can not be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern volatile void *flashc_memset32(volatile void *dst, U32 src, size_t nbytes, Bool erase);

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the repeated \a src big-endian source double-word.
 *
 * The destination areas that are not within the flash array or the User page
 * are ignored.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Source double-word.
 * \param nbytes Number of bytes to set.
 * \param erase Whether to erase before writing: \c TRUE or \c FALSE.
 *
 * \return The value of \a dst.
 *
 * \warning This function may be called with \a erase set to \c FALSE only if
 *          the destination consists only of erased words, i.e. this function
 *          can not be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern volatile void *flashc_memset64(volatile void *dst, U64 src, size_t nbytes, Bool erase);

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the repeated \a src big-endian source pattern.
 *
 * The destination areas that are not within the flash array or the User page
 * are ignored.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Source double-word.
 * \param src_width \a src width in bits: 8, 16, 32 or 64.
 * \param nbytes Number of bytes to set.
 * \param erase Whether to erase before writing: \c TRUE or \c FALSE.
 *
 * \return The value of \a dst.
 *
 * \warning This function may be called with \a erase set to \c FALSE only if
 *          the destination consists only of erased words, i.e. this function
 *          can not be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
#define flashc_memset(dst, src, src_width, nbytes, erase) \
          TPASTE2(flashc_memset, src_width)((dst), (src), (nbytes), (erase))

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the source pointed to by \a src.
 *
 * The destination areas that are not within the flash array or the User page
 * are ignored.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Pointer to source data.
 * \param nbytes Number of bytes to copy.
 * \param erase Whether to erase before writing: \c TRUE or \c FALSE.
 *
 * \return The value of \a dst.
 *
 * \warning If copying takes place between areas that overlap, the behavior is
 *          undefined.
 *
 * \warning This function may be called with \a erase set to \c FALSE only if
 *          the destination consists only of erased words, i.e. this function
 *          can not be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area.
 *
 * \note The FLASHC error status returned by \ref flashc_is_lock_error and
 *       \ref flashc_is_programming_error is updated.
 */
extern volatile void *flashc_memcpy(volatile void *dst, const void *src, size_t nbytes, Bool erase);

#if UC3C

/*! \brief Depednding to the CPU frequency, set the wait states of flash read
 *         accesses and enable or disable the High speed read mode.
 *
 * \param cpu_f_hz The CPU frequency
 */
void flashc_set_flash_waitstate_and_readmode(unsigned long cpu_f_hz);
#endif // UC3C device-specific implementation

//! @}


#endif  // _FLASHC_H_
