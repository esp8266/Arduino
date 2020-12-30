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

static struct timeval gtod0 = { 0, 0 };

extern "C" unsigned long millis()
{
    timeval time;
    gettimeofday(&time, NULL);
    if (gtod0.tv_sec == 0)
        memcpy(&gtod0, &time, sizeof gtod0);
    return ((time.tv_sec - gtod0.tv_sec) * 1000) + ((time.tv_usec - gtod0.tv_usec) / 1000);
}

extern "C" unsigned long micros()
{
    timeval time;
    gettimeofday(&time, NULL);
    if (gtod0.tv_sec == 0)
        memcpy(&gtod0, &time, sizeof gtod0);
    return ((time.tv_sec - gtod0.tv_sec) * 1000000) + time.tv_usec - gtod0.tv_usec;
}


extern "C" void yield()
{
}

extern "C" bool can_yield()
{
    return true;
}

extern "C" void optimistic_yield (uint32_t interval_us)
{
    (void)interval_us;
}

extern "C" void esp_yield()
{
}

extern "C" void esp_delay (unsigned long ms)
{
    usleep(ms * 1000);
}

inline void esp_yield(const IsBlockedCB& blocked) {
    (void)blocked;
}

void esp_delay(const uint32_t timeout_ms, const IsBlockedCB& blocked, const uint32_t intvl_ms)
{
    (void)blocked;
    (void)intvl_ms;
    usleep(timeout_ms * 1000);
}

inline void esp_delay(const uint32_t timeout_ms, const IsBlockedCB& blocked)
{
    (void)blocked;
    usleep(timeout_ms * 1000);
}

extern "C" void __panic_func(const char* file, int line, const char* func) {
    (void)file;
    (void)line;
    (void)func;
    abort();
}

extern "C" void delay(unsigned long ms)
{
    esp_delay(ms);
}

extern "C" void delayMicroseconds(unsigned int us)
{
    usleep(us);
}

#include "cont.h"
cont_t* g_pcont = NULL;
extern "C" void cont_yield(cont_t*)
{
}
