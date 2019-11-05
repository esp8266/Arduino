/*
 spi_utils.h - SPI utility function
 Copyright (c) 2015 Ivan Grokhotkov.  All right reserved.

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


#ifndef SPI_UTILS_H
#define SPI_UTILS_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

namespace experimental {
typedef enum {
    SPI_RESULT_OK,
    SPI_RESULT_ERR,
    SPI_RESULT_TIMEOUT
} SpiOpResult;

SpiOpResult SPI0Command(uint8_t cmd, uint32_t *data, uint32_t mosi_bits, uint32_t miso_bits);
}

#ifdef __cplusplus
}
#endif


#endif //SPI_UTILS_H
