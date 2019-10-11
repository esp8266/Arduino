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
 * and because this is the part that's really specific to SPI0.
 * PRECACHE_* saves having to make the function IRAM_ATTR.
 *
 * Note: if porting to ESP32 mosi/miso bits are set in 2 registers, not 1.
 */
static SpiOpResult PRECACHE_ATTR
_SPI0Command(uint32_t spi0c,uint32_t spi0u,uint32_t spi0u1,uint32_t spi0u2,
             uint32_t *data,uint32_t mosi_bytes,uint32_t miso_bytes)
{
  PRECACHE_START();
  //precache(NULL,200);

  Wait_SPI_Idle(flashchip);
  uint32_t old_spi_usr = SPI0U;
  uint32_t old_spi_usr2= SPI0U2;
  uint32_t old_spi_c   = SPI0C;

  //SPI0S &= ~(SPISE|SPISBE|SPISSE|SPISCD);
  SPI0C = spi0c;
  SPI0U = spi0u;
  SPI0U1= spi0u1;
  SPI0U2= spi0u2;

  if (mosi_bytes>0) {
     // copy the outgoing data to the SPI hardware
     memcpy((void*)&(SPI0W0),data,mosi_bytes);
  }

  // Start the transfer
  SPI0CMD = SPICMDUSR;

  // wait for the command to complete
  uint32_t timeout = 1000;
  while ((SPI0CMD & SPICMDUSR) && timeout--) {}
  
  if ((miso_bytes>0) && (timeout>0)) {
     // copy the response back to the buffer
     memcpy(data,(void *)&(SPI0W0),miso_bytes);
  }

  SPI0U = old_spi_usr;
  SPI0U2= old_spi_usr2;
  SPI0C = old_spi_c;
  
  PRECACHE_END();
  return (timeout>0 ? SPI_RESULT_OK : SPI_RESULT_TIMEOUT);
}


/*  SPI0Command: send a custom SPI command.
 *  This part calculates register values and passes them to _SPI0Command().
 */
SpiOpResult SPI0Command(uint8_t cmd, uint32_t *data, uint32_t mosi_bits, uint32_t miso_bits) {
  if (mosi_bits>(64*8)) return SPI_RESULT_ERR;
  if (miso_bits>(64*8)) return SPI_RESULT_ERR;

  uint32_t mosi_words=mosi_bits/32;
  uint32_t miso_words=miso_bits/32;
  if (mosi_bits % 32 != 0) mosi_words++;
  if (miso_bits % 32 != 0) miso_words++;

  uint32_t spiu=SPIUCOMMAND; //SPI_USR_COMMAND
  uint32_t spiu2 = ((7 & SPIMCOMMAND)<<SPILCOMMAND) | cmd;
  uint32_t spiu1 = 0;
  if (mosi_bits>0) {
     spiu1 |= ((mosi_bits-1) & SPIMMOSI) << SPILMOSI;
     spiu  |= SPIUMOSI;  // SPI_USR_MOSI
  }
  if (miso_bits>0) {
     spiu1 |= ((miso_bits-1) & SPIMMISO) << SPILMISO;
     spiu  |= SPIUMISO;  // SPI_USR_MISO
  }

  uint32_t spic = SPI0C;
  spic &= ~(SPICQIO | SPICDIO | SPICQOUT | SPICDOUT | SPICAHB | SPICFASTRD);
  spic |= (SPICRESANDRES | SPICSHARE | SPICWPR | SPIC2BSE);

  SpiOpResult rc =_SPI0Command(spic,spiu,spiu1,spiu2,data,mosi_words*4,miso_words*4);
  
  if (rc==SPI_RESULT_OK) {
     // clear any bits we did not read in the last word.
     if (miso_bits % 32) {
        data[miso_bits/32] &= ~(0xFFFFFFFF << (miso_bits % 32));
     }
  }
  return rc;
}
