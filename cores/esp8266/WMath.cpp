/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 Part of the Wiring project - http://wiring.org.co
 Copyright (c) 2004-06 Hernando Barragan
 Modified 13 August 2006, David A. Mellis for Arduino - http://www.arduino.cc/
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General
 Public License along with this library; if not, write to the
 Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 Boston, MA  02111-1307  USA
 
 $Id$
 */

#include "Arduino.h"

extern "C" {
#include <stdlib.h>
}
#include "esp8266_peri.h"

static bool s_randomSeedCalled = false;

void randomSeed(unsigned long seed) {
    if(seed != 0) {
        srand(seed);
        s_randomSeedCalled = true;
    }
}

long random(long howbig) {
    if(howbig == 0) {
        return 0;
    }
    // if randomSeed was called, fall back to software PRNG
    uint32_t val = (s_randomSeedCalled) ? rand() : RANDOM_REG32;
    return val % howbig;
}

long random(long howsmall, long howbig) {
    if(howsmall >= howbig) {
        return howsmall;
    }
    long diff = howbig - howsmall;
    return random(diff) + howsmall;
}

long secureRandom(long howbig) {
    if(howbig == 0) {
        return 0;
    }
    return RANDOM_REG32 % howbig;
}

long secureRandom(long howsmall, long howbig) {
    if(howsmall >= howbig) {
        return howsmall;
    }
    long diff = howbig - howsmall;
    return secureRandom(diff) + howsmall;
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    long in_length  = in_max - in_min;
    long out_length = out_max - out_min;

    if (in_length == 0 || out_length == 0) { 
        return out_min; 
    }

    long delta = x - in_min;

    if ((out_length < 0) && (in_length < 0)) {
        std::swap(in_min, in_max);
        std::swap(out_min, out_max);
    } else if (out_length < 0) {
        x = in_max - delta;
        std::swap(out_min, out_max);
    } else if (in_length < 0) {
        x = in_max - delta;
        std::swap(in_min, in_max);
    }

    // Update length and delta as in/out values may have changed.
    delta      = x - in_min;
    in_length  = in_max - in_min;
    out_length = out_max - out_min;

    if (out_length == in_length) {
        return out_min + delta;
    }

    // We now know in_min < in_max and out_min < out_max
    // Make sure x is within range of in_min ... in_max
    // Shift the in/out range to contain 'x'
    if ((x < in_min) || (x > in_max)) {
        long shift_factor = 0;

        if (x < in_min) {
            const long before_min = in_min - x;
            shift_factor = -1 - (before_min / in_length);
        } else {
            // x > in_max
            const long passed_max = x - in_max;
            shift_factor = 1 + (passed_max / in_length);
        }

        const long in_shift  = shift_factor * in_length;
        const long out_shift = shift_factor * out_length;
        in_min  += in_shift;
        in_max  += in_shift;
        out_min += out_shift;
        out_max += out_shift;
        delta    = x - in_min;
    }

    if (out_length > in_length) {
        // Map to larger range
        // Do not 'simplify' this calculation
        // as this will result in integer overflow errors
        const long factor    = out_length / in_length;
        const long error_mod = out_length % in_length;
        const long error     = (delta * error_mod) / in_length;
        return (delta * factor) + out_min + error;
    }

    // abs(out_length) < abs(in_length)
    // Map to smaller range
    // Do not 'simplify' this calculation
    // as this will result in integer overflow errors
    const long factor        = (in_length / out_length);
    const long estimate_full = in_length / factor + out_min;
    const long error         = (delta * (out_max - estimate_full)) / in_length;

    return delta / factor + out_min + error;
}

uint16_t makeWord(uint16_t w) {
    return w;
}

uint16_t makeWord(byte h, byte l) {
    return (h << 8) | l;
}
