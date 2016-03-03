//
//  Arduino.cpp
//  esp8266-host-tests
//
//  Created by Ivan Grokhotkov on 02/03/16.
//  Copyright © 2016 esp8266.com. All rights reserved.
//
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <sys/time.h>
#include "Arduino.h"


extern "C" unsigned long millis()
{
    timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}


extern "C" void yield()
{
}


extern "C" void __panic_func(const char* file, int line, const char* func) {
    abort();
}