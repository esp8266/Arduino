/*
 Arduino.cpp - Mocks for common Arduino APIs
 Copyright © 2016 Ivan Grokhotkov
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
*/

#include <sys/time.h>
#include "Arduino.h"

#include <unistd.h>

extern "C" unsigned long millis()
{
    timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

extern "C" unsigned long micros()
{
    timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000000) + time.tv_usec;
}


extern "C" void yield()
{
}

extern "C" void optimistic_yield (uint32_t interval_us)
{
    usleep(interval_us);
}

extern "C" void esp_yield()
{
}


extern "C" void __panic_func(const char* file, int line, const char* func) {
    (void)file;
    (void)line;
    (void)func;
    abort();
}

extern "C" void delay(unsigned long ms)
{
    usleep(ms * 1000);
}

extern "C" void delayMicroseconds(unsigned int us)
{
    usleep(us);
}
