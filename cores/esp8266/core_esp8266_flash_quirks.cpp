/*
 flash_quirks.cpp - Chip specific flash init
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

#include <c_types.h>
#include "spi_flash.h"

#include "spi_utils.h"
#include "flash_quirks.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace experimental {

static int get_flash_mhz() {
    // FIXME: copied from Esp.cpp - we really should define the magic values
    uint32_t data;
    uint8_t * bytes = (uint8_t *) &data;
    // read first 4 byte (magic byte + flash config)
    if(spi_flash_read(0x0000, &data, 4) == SPI_FLASH_RESULT_OK) {
        switch (bytes[3] & 0x0F) {
        case 0x0: // 40 MHz
            return 40;
        case 0x1: // 26 MHz
            return 26;
        case 0x2: // 20 MHz
            return 20;
        case 0xf: // 80 MHz
            return 80;
        default: // fail?
            return 0;
        }
    }
    return 0;
}

/* initFlashQuirks()
 * Do any chip-specific initialization to improve performance and reliability.
 */
void initFlashQuirks() {
  using namespace experimental;
  uint32_t vendor = spi_flash_get_id() & 0x000000ff;

  switch (vendor) {
    case SPI_FLASH_VENDOR_XMC:
         uint32_t SR3, newSR3;
         if (SPI0Command(SPI_FLASH_CMD_RSR3, &SR3, 0, 8)==SPI_RESULT_OK) { // read SR3
            newSR3=SR3;
            if (get_flash_mhz()>26) { // >26Mhz?
               // Set the output drive to 100%
               // These definitions are for the XM25QH32B part. On a XM25QH32C
               // part, the XM25QH32B's 100% is C's 25% driver strength.
               newSR3 &= ~(SPI_FLASH_SR3_XMC_DRV_MASK << SPI_FLASH_SR3_XMC_DRV_S);
               newSR3 |= (SPI_FLASH_SR3_XMC_DRV_100 << SPI_FLASH_SR3_XMC_DRV_S);
            }
            if (newSR3 != SR3) { // only write if changed
               SPI0Command(SPI_FLASH_CMD_WSR3,&newSR3,8,0,SPI_FLASH_CMD_WEVSR);  // write to SR3, use write enable volatile prefix
               SPI0Command(SPI_FLASH_CMD_WRDI,NULL,0,0);        // write disable - probably not needed
            }
         }
  }
}

} // namespace experimental

#ifdef __cplusplus
}
#endif
