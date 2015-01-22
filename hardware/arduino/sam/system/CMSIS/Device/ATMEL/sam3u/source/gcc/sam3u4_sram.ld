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

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

/* Memory Spaces Definitions */
MEMORY
{
	flash0 (rx) : ORIGIN = 0x00080000, LENGTH = 0x00020000 /* flash0, 128K */
	flash1 (rx) : ORIGIN = 0x00100000, LENGTH = 0x00020000 /* flash1, 128K */
	sram0 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00008000 /* sram0, 32K */
	sram1 (rwx) : ORIGIN = 0x20080000, LENGTH = 0x00004000 /* sram1, 16K */

	unified_flash (rx) : ORIGIN = 0x000E0000, LENGTH = 0x00040000 /* Flash, 256K */
	unified_ram (rwx)  : ORIGIN = 0x20078000, LENGTH = 0x0000C000 /* sram, 48K */

	rom (rx)    : ORIGIN = 0x20000000, LENGTH = 0x00008000 /* sram0, 32K */
	ram (rwx)   : ORIGIN = 0x20080000, LENGTH = 0x00004000 /* sram1, 16K */
}

/* The stack size used by the application. NOTE: you need to adjust  */
STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : 0x900;

INCLUDE sam3u_sram.ld
