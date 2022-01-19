/*
    sdfs_mock.cpp - SDFS HAL mock for host side testing
    Copyright (c) 2019 Earle F. Philhower, III

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
*/

#include "sdfs_mock.h"
#include "../../../libraries/SDFS/src/SDFS.h"

#define SDSIZE 16LL
uint64_t _sdCardSizeB = 0;
uint8_t *_sdCard = nullptr;
