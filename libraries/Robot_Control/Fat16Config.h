/* Arduino FAT16 Library
 * Copyright (C) 2008 by William Greiman
 *
 * This file is part of the Arduino FAT16 Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with the Arduino Fat16 Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 /**
  * \file
  * Configuration file
  */
#ifndef Fat16Config_h
#define Fat16Config_h
/**
 * Allow use of deprecated functions if non-zero
 */
#define ALLOW_DEPRECATED_FUNCTIONS 1
/**
 * SdCard::writeBlock will protect block zero if set non-zero
 */
#define SD_PROTECT_BLOCK_ZERO 1
/**
 * Set non-zero to allow access to Fat16 internals by cardInfo debug sketch
 */
#define FAT16_DEBUG_SUPPORT 1
#endif  // Fat16Config_h
