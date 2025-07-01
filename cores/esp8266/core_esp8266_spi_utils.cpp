/*
 core_esp8266_spi_utils.cpp

 Copyright (c) 2019 Mike Nix. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <cstdint>
#include <cstring>
#include <memory>

// register names
#include "esp8266_peri.h"

// for flashchip
#include "spi_flash.h"

// for PRECACHE_*
#include "core_esp8266_features.h"

#include "spi_utils.h"
#include "spi_flash_defs.h"

extern "C" uint32_t Wait_SPI_Idle(SpiFlashChip *fc);

namespace experimental {

/*
 * critical part of SPICommand.
 * Kept in a separate function to aid with precaching
 * PRECACHE_* saves having to make the function IRAM_ATTR.
 *
 * PRELOAD_* allows access to consts and external values
 * through a different name, while also forcing immediate load.
 * (but, note that compiler only knows about DST and SRC as dependencies)
 *
 * Note: if porting to ESP32 mosi/miso bits are set in 2 registers, not 1.
 */

#define PRELOAD_DST_SRC(DST,SRC)\
  __asm__ __volatile__ (\
    "mov %0, %1\n\t"\
    : "=a"(DST)\
    : "r"(SRC)\
    : "memory")

#define PRELOAD_IMMEDIATE(DST,SRC)\
  uint32_t DST;\
  __asm__ __volatile__ (\
    "movi %0, %1\n\t"\
    : "=a"(DST)\
    : "i"(SRC)\
    : "memory")

#define PRELOAD_VAL(DST,SRC)\
  decltype(SRC) DST;\
  PRELOAD_DST_SRC(DST,SRC)

#define PRELOAD_PTR(DST,SRC)\
  decltype(std::addressof(SRC)) DST;\
  PRELOAD_DST_SRC(DST,SRC)

static SpiOpResult PRECACHE_ATTR
_SPICommand(uint32_t spiIfNum,
            uint32_t spic, uint32_t spiu, uint32_t spiu1, uint32_t spiu2,
            uint32_t *data, uint32_t writeWords, uint32_t readWords, uint32_t _pre_cmd)
{
  if (spiIfNum>1)
     return SPI_RESULT_ERR;

  // force SPI register access via base+offset by deconstructing SPI# access macros
  // note that the function below only ever calls this one w/ spiIfNum==0
  // in case it is *really* necessary, preload spiIfNum as well
  #define VOLATILE_PTR(X) reinterpret_cast<volatile uint32_t *>(X)
  #define SPIADDR(X) const_cast<uint32_t *>(std::addressof(X))

  // preload all required constants and functions into variables.
  // when modifying code below, always double-check the asm output

  PRELOAD_IMMEDIATE(spi0cmd_addr, SPIADDR(SPI0CMD));
  PRELOAD_IMMEDIATE(spi1cmd_addr, SPIADDR(SPI1CMD));
  uint32_t *spibase = spiIfNum
      ? reinterpret_cast<uint32_t *>(spi1cmd_addr)
      : reinterpret_cast<uint32_t *>(spi0cmd_addr);
  #define SPIREG(reg) \
      (*VOLATILE_PTR(spibase + (SPIADDR(reg) - SPIADDR(SPI0CMD))))

  PRELOAD_PTR(SPI_write_enablep, SPI_write_enable);
  PRELOAD_PTR(Wait_SPI_Idlep, Wait_SPI_Idle);

  PRELOAD_VAL(fchip, flashchip);

  PRELOAD_IMMEDIATE(spicmdusr, SPICMDUSR);
  PRELOAD_IMMEDIATE(saved_ps, 0);

  // also force 'pre_cmd' & 'spiu' mask constant to be loaded right now
  // (TODO write all of the preamble in asm directly?)
  PRELOAD_VAL(pre_cmd, _pre_cmd);

  PRELOAD_IMMEDIATE(pre_cmd_spiu_mask, ~(SPIUMOSI | SPIUMISO));
  uint32_t _pre_cmd_spiu = spiu & pre_cmd_spiu_mask;
  PRELOAD_VAL(pre_cmd_spiu, _pre_cmd_spiu);

  PRELOAD_IMMEDIATE(pre_cmd_spiu2_mask, ~0xFFFFu);
  uint32_t _pre_cmd_spiu2 = (spiu2 & pre_cmd_spiu2_mask) | pre_cmd;
  PRELOAD_VAL(pre_cmd_spiu2, _pre_cmd_spiu2);

  if (!spiIfNum) {
     // Only need to disable interrupts and precache when using SPI0
     saved_ps = xt_rsil(15);
     PRECACHE_START();
     Wait_SPI_Idlep((SpiFlashChip *)fchip);
  }

  // preserve essential controller state such as incoming/outgoing
  // data lengths and IO mode.
  uint32_t oldSPI0U = SPIREG(SPI0U);
  uint32_t oldSPI0U2= SPIREG(SPI0U2);
  uint32_t oldSPI0C = SPIREG(SPI0C);

  SPIREG(SPI0C) = spic;

  if (SPI_FLASH_CMD_WREN == pre_cmd) {
     // See SPI_write_enable comments in esp8266_undocumented.h
     SPI_write_enablep((SpiFlashChip *)fchip);
  } else if (pre_cmd) {
     // Send prefix cmd w/o data - sends 8 bits. eg. Volatile SR Write Enable, 0x50
     SPIREG(SPI0U)  = pre_cmd_spiu;
     SPIREG(SPI0U1) = 0;
     SPIREG(SPI0U2) = pre_cmd_spiu2;

     SPIREG(SPI0CMD) = spicmdusr;   //Send cmd
     while ((SPIREG(SPI0CMD) & spicmdusr));
  }

  //SPI0S &= ~(SPISE|SPISBE|SPISSE|SPISCD);
  SPIREG(SPI0U) = spiu;
  SPIREG(SPI0U1)= spiu1;
  SPIREG(SPI0U2)= spiu2;

  if (writeWords>0) {
     // copy the outgoing data to the SPI hardware
     uint32_t *src=data;
     volatile uint32_t *dst=&SPIREG(SPI0W0);
     for (uint32_t i=0; i<writeWords; i++)
         *dst++ = *src++;
  }

  // Start the transfer
  SPIREG(SPI0CMD) = spicmdusr;

  // wait for the command to complete (typically only 1-3 iterations)
  uint32_t timeout = 1000;
  while ((SPIREG(SPI0CMD) & spicmdusr) && timeout--);

  if ((readWords>0) && (timeout>0)) {
     // copy the response back to the buffer
     uint32_t *dst=data;
     volatile uint32_t *src=&SPIREG(SPI0W0);
     for (uint32_t i=0; i<readWords; i++)
         *dst++ = *src++;
  }

  // Restore saved registers
  SPIREG(SPI0U) = oldSPI0U;
  SPIREG(SPI0U2)= oldSPI0U2;
  SPIREG(SPI0C) = oldSPI0C;

  if (!spiIfNum) {
      // w/o a call to Wait_SPI_Idlep, 'Exception 0' or other exceptions (saw
      // 28) may occur later after returning to iCache code. This issue was
      // observed with non-volatile status register writes.
      //
      // My guess is: Returning too soon to uncached iCache executable space. An
      // iCache read may not complete properly because the Flash or SPI
      // interface is still busy with the last write operation. In such a case,
      // I expect new reads from iROM to result in zeros. This would explain
      // the Exception 0 for code, and Exception 20, 28, and 29 where a literal
      // was misread as 0 and then used as a pointer.
      Wait_SPI_Idlep((SpiFlashChip *)fchip);
      xt_wsr_ps(saved_ps);
  }
  PRECACHE_END();
  return (timeout>0 ? SPI_RESULT_OK : SPI_RESULT_TIMEOUT);
}


/*  SPI0Command: send a custom SPI command.
 *  This part calculates register values and passes them to _SPI0Command().
 *  Parameters:
 *	cmd	The command byte (first 8 bits) to send to the SPI device
 *	*data	The buffer containing the outgoing data for the SPI bus.
 *		The data is expected to be mosi_bits long, and the buffer
 *		is overwritten by the incoming bus data, which will be
 *		miso_bits long.
 *	mosi_bits
 *		Number of bits to be sent after the command byte.
 *	miso_bits
 *		Number of bits to read from the SPI bus after the outgoing
 *		data has been sent.
 *  pre_cmd
 *    A few SPI Flash commands require enable commands to immediately preceed
 *    them. Since two calls to SPI0Command from ICACHE memory most likely would
 *    be separated by SPI Flash read request for iCache, use this option to
 *    supply a prefix command, 8-bits w/o read or write data.
 *
 *    Case in point from the GD25Q32E datasheet: "The Write Enable for Volatile
 *    Status Register command must be issued prior to a Write Status Register
 *    command and any other commands can’t be inserted between them."
 *
 *  Note: This code has only been tested with SPI bus 0, but should work
 *        equally well with other buses. The ESP8266 has bus 0 and 1,
 *        newer chips may have more one day.
 *
 *  Supplemental Notes:
 *
 *  SPI Bus wire view: Think of *data as an array of bytes, byte[0] goes out
 *  first with the most significant bit shifted out first and so on. When
 *  thinking of the data as an array of 32bit-words, the least significant byte
 *  of the first 32bit-word goes out first on the SPI bus with the most
 *  significant bit of that byte shifted out first onto the wire.
 *
 *  When presenting a 3 or 4-byte address, the byte order will need to be
 *  reversed. Don't overthink it. For a 3-byte address, view *data as a byte
 *  array and set the first 3-bytes to the address. eg. byteData[0] MSB,
 *  byteData[1] middle, and byteData[2] LSB.
 *
 *  When sending a fractional byte, fill in the most significant bit positions
 *  of the byte first.
 */
SpiOpResult SPI0Command(uint8_t cmd, uint32_t *data, uint32_t mosi_bits, uint32_t miso_bits, uint32_t pre_cmd) {
  if (mosi_bits>(64*8))
     return SPI_RESULT_ERR;
  if (miso_bits>(64*8))
     return SPI_RESULT_ERR;

  // Calculate the number of data words (aka registers) that need to be copied
  // to/from the SPI controller.
  uint32_t mosi_words=mosi_bits/32;
  uint32_t miso_words=miso_bits/32;
  if (mosi_bits % 32 != 0)
     mosi_words++;
  if (miso_bits % 32 != 0)
     miso_words++;

  // Use SPI_CS_SETUP to add time for #CS to settle (ringing) before SPI CLK
  // begins. The BootROM does not do this; however, RTOS SDK and NONOS SDK do
  // as part of flash init/configuration.
  //
  // One SPI bus clock cycle time inserted between #CS active and the 1st SPI
  // bus clock cycle. The number of clock cycles is in SPI_CNTRL2
  // SPI_SETUP_TIME, which defaults to 1.
  //
  // Select user defined command mode in the controller
  uint32_t spiu=SPIUCOMMAND | SPIUCSSETUP; //SPI_USR_COMMAND | SPI_CS_SETUP

  // Set the command byte to send
  uint32_t spiu2 = ((7 & SPIMCOMMAND)<<SPILCOMMAND) | cmd;

  uint32_t spiu1 = 0;
  if (mosi_bits>0) {
     // set the number of outgoing data bits to send
     spiu1 |= ((mosi_bits-1) & SPIMMOSI) << SPILMOSI;
     spiu  |= SPIUMOSI;  // SPI_USR_MOSI
  }
  if (miso_bits>0) {
     // set the number of incoming bits to read
     spiu1 |= ((miso_bits-1) & SPIMMISO) << SPILMISO;
     spiu  |= SPIUMISO;  // SPI_USR_MISO
  }

  uint32_t spic = SPI0C;
  // Select the most basic IO mode for maximum compatibility
  // Some flash commands are only available in this mode.
  spic &= ~(SPICQIO | SPICDIO | SPICQOUT | SPICDOUT | SPICAHB | SPICFASTRD);
  spic |= (SPICRESANDRES | SPICSHARE | SPICWPR | SPIC2BSE);

  SpiOpResult rc =_SPICommand(0,spic,spiu,spiu1,spiu2,data,mosi_words,miso_words,pre_cmd);

  if (rc==SPI_RESULT_OK) {
     // Clear any bits we did not read in the last word. Bits in a fractional
     // bytes will be stored in the most significant part of the byte first.
     if (miso_bits % 32u) {
        uint32_t whole_byte_bits = (miso_bits % 32u) & ~7u;
        uint32_t mask = ~(0xFFFFFFFFu << whole_byte_bits);
        if (miso_bits % 8u) {
           // Select fractional byte bits.
           mask |= (~(0xFFu >> (miso_bits % 8u)) & 0xFFu) << whole_byte_bits;
        }
        data[miso_bits/32u] &= mask;
     }
  }
  return rc;
}

} // namespace experimental
