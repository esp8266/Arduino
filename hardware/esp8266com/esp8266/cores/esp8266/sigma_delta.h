/* 
 sigma_delta.h - esp8266 sigma-delta source

 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
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

#ifndef SIGMA_DELTA_H
#define SIGMA_DELTA_H

#include <stdint.h>

void sigma_delta_close(uint32_t gpio);
void set_sigma_target(uint8_t target);
void set_sigma_prescale(uint8_t prescale);
void set_sigma_duty_312KHz(uint8_t duty);

#endif//SIGMA_DELTA_H
