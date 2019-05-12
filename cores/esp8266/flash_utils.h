/*
 flash_utils.h - Flash access function and data structures
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


#ifndef FLASH_UTILS_H
#define FLASH_UTILS_H


#ifdef __cplusplus
extern "C" {
#endif

/* Definitions are placed in eboot. Include them here rather than duplicate them.
 * Also, prefer to have eboot standalone as much as possible and have the core depend on it
 * rather than have eboot depend on the core.
 */
#include <../../bootloaders/eboot/flash.h>


#ifdef __cplusplus
}
#endif


#endif //FLASH_UTILS_H
