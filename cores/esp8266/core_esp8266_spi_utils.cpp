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

#include <stdint.h>
#include <string.h>

// register names
#include "esp8266_peri.h"

// for flashchip
#include "spi_flash.h"

// for PRECACHE_*
#include "core_esp8266_features.h"

#include "spi_utils.h"

extern "C" uint32_t Wait_SPI_Idle(SpiFlashChip *fc);

/*
 * critical part of SPICommand.
 * Kept in a separate function to aid with precaching
 * PRECACHE_* saves having to make the function IRAM_ATTR.
 *
 * spiIfNum needs to be volatile to keep the optimiser from
 * deciding it can be treated as a constant (due to this being a
 * static function only called with spiIfNum set to 0)
 *
 * Note: if porting to ESP32 mosi/miso bits are set in 2 registers, not 1.
 */
static SpiOpResult PRECACHE_ATTR
_SPICommand(volatile uint32_t spiIfNum,
            uint32_t spic,uint32_t spiu,uint32_t spiu1,uint32_t spiu2,
            uint32_t *data,uint32_t writeWords,uint32_t readWords)
{ 
  if (spiIfNum>1)
     return SPI_RESULT_ERR;

  // force SPI register access via base+offest. 
  // Prevents loading individual address constants from flash.
  uint32_t *spibase = (uint32_t*)(spiIfNum ? &(SPI1CMD) : &(SPI0CMD));
  #define SPIREG(reg) (*((volatile uint32_t *)(spibase+(&(reg) - &(SPI0CMD)))))

  // preload any constants and functions we need into variables
  // Everything defined here must be volatile or the optimizer can
  // treat them as constants, resulting in the flash reads we're
  // trying to avoid
  void    *(* volatile memcpyp)(void *,const void *, size_t) = memcpy;
  uint32_t (* volatile Wait_SPI_Idlep)(SpiFlashChip *) = Wait_SPI_Idle;
  volatile SpiFlashChip *fchip=flashchip;
  volatile uint32_t spicmdusr=SPICMDUSR;

  if (!spiIfNum) {
     // Only need to precache when using SPI0
     PRECACHE_START();
     Wait_SPI_Idlep((SpiFlashChip *)fchip);
  }
  
  // preserve essential controller state such as incoming/outgoing
  // data lengths and IO mode.
  uint32_t oldSPI0U = SPIREG(SPI0U);
  uint32_t oldSPI0U2= SPIREG(SPI0U2);
  uint32_t oldSPI0C = SPIREG(SPI0C);

  //SPI0S &= ~(SPISE|SPISBE|SPISSE|SPISCD);
  SPIREG(SPI0C) = spic;
  SPIREG(SPI0U) = spiu;
  SPIREG(SPI0U1)= spiu1;
  SPIREG(SPI0U2)= spiu2;

  if (writeWords>0) {
     // copy the outgoing data to the SPI hardware
     memcpyp((void*)&(SPIREG(SPI0W0)),data,writeWords*4);
  }

  // Start the transfer
  SPIREG(SPI0CMD) = spicmdusr;

  // wait for the command to complete (typically only 1-3 iterations)
  uint32_t timeout = 1000;
  while ((SPIREG(SPI0CMD) & spicmdusr) && timeout--);

  if ((readWords>0) && (timeout>0)) {
     // copy the response back to the buffer
     memcpyp(data,(void *)&(SPIREG(SPI0W0)),readWords*4);
  }

  // Restore saved registers
  SPIREG(SPI0U) = oldSPI0U;
  SPIREG(SPI0U2)= oldSPI0U2;
  SPIREG(SPI0C) = oldSPI0C;
  
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
 */
SpiOpResult SPI0Command(uint8_t cmd, uint32_t *data, uint32_t mosi_bits, uint32_t miso_bits) {
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

  // Select user defined command mode in the controller
  uint32_t spiu=SPIUCOMMAND; //SPI_USR_COMMAND

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

  SpiOpResult rc =_SPICommand(0,spic,spiu,spiu1,spiu2,data,mosi_words,miso_words);

  if (rc==SPI_RESULT_OK) {
     // clear any bits we did not read in the last word.
     if (miso_bits % 32) {
        data[miso_bits/32] &= ~(0xFFFFFFFF << (miso_bits % 32));
     }
  }
  return rc;
}
