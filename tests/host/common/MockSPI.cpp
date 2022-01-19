/*
    Arduino emulation - spi
    Copyright (c) 2018 david gauchard. All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal with the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimers.

    - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimers in the
    documentation and/or other materials provided with the distribution.

    - The names of its contributors may not be used to endorse or promote
    products derived from this Software without specific prior written
    permission.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
    THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS WITH THE SOFTWARE.
*/

#include <SPI.h>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPI)
SPIClass SPI;
#endif

SPIClass::SPIClass()
{
}

uint8_t SPIClass::transfer(uint8_t data)
{
    return data;
}

void SPIClass::begin()
{
}

void SPIClass::end()
{
}

void SPIClass::setFrequency(uint32_t freq)
{
    (void)freq;
}

void SPIClass::setHwCs(bool use)
{
    (void)use;
}
