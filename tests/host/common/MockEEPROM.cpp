/*
    Arduino emulation - EEPROM
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

#ifndef __EEPROM_H
#define __EEPROM_H

#include <EEPROM.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define EEPROM_FILE_NAME "eeprom"

EEPROMClass::EEPROMClass()
{
}

EEPROMClass::~EEPROMClass()
{
    if (_fd >= 0)
    {
        close(_fd);
    }
}

void EEPROMClass::begin(size_t size)
{
    _size = size;
    if ((_fd = open(EEPROM_FILE_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1 || ftruncate(_fd, size) == -1)
    {
        fprintf(stderr, MOCK "EEPROM: cannot open/create '%s' for r/w: %s\n\r", EEPROM_FILE_NAME, strerror(errno));
        _fd = -1;
    }
}

void EEPROMClass::end()
{
    if (_fd != -1)
    {
        close(_fd);
    }
}

bool EEPROMClass::commit()
{
    return true;
}

uint8_t EEPROMClass::read(int x)
{
    char c = 0;
    if (pread(_fd, &c, 1, x) != 1)
    {
        fprintf(stderr, MOCK "eeprom: %s\n\r", strerror(errno));
    }
    return c;
}

void EEPROMClass::write(int x, uint8_t c)
{
    if (x > (int)_size)
    {
        fprintf(stderr, MOCK "### eeprom beyond\r\n");
    }
    else if (pwrite(_fd, &c, 1, x) != 1)
    {
        fprintf(stderr, MOCK "eeprom: %s\n\r", strerror(errno));
    }
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
EEPROMClass EEPROM;
#endif

#endif
