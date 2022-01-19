/*
    test_pgmspace.cpp - pgmspace tests
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

#include <FS.h>
#include <LittleFS.h>
#include <spiffs/spiffs.h>
#include <string.h>
#include <catch.hpp>
#include "../common/littlefs_mock.h"

// Use a LittleFS file because we can't instantiate a virtual class like Print
TEST_CASE("Print::write overrides all compile properly", "[core][Print]")
{
    LITTLEFS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(LittleFS.begin());
    auto p = LittleFS.open("test.bin", "w");
    REQUIRE(p);
    uint8_t uint8 = 1;
    uint16_t uint16 = 2;
    uint32_t uint32 = 3;
    size_t size = 4;
    int8_t int8 = 1;
    int16_t int16 = 2;
    int32_t int32 = 3;
    char c = 'h';
    int i = 10;
    long l = 11;
    unsigned char uc = 20;
    unsigned int ui = 21;
    unsigned long ul = 22;
    p.write(uint8);
    p.write(uint16);
    p.write(uint32);
    p.write(size);
    p.write(int8);
    p.write(int16);
    p.write(int32);
    p.write(c);
    p.write(i);
    p.write(l);
    p.write(uc);
    p.write(ui);
    p.write(ul);
    p.write(0);
    p.write(1);
    p.close();

    p = LittleFS.open("test.bin", "r");
    REQUIRE(p);
    uint8_t buff[16];
    int len = p.read(buff, 16);
    p.close();
    REQUIRE(len == 15);
    REQUIRE(buff[0] == 1);
    REQUIRE(buff[1] == 2);
    REQUIRE(buff[2] == 3);
    REQUIRE(buff[3] == 4);
    REQUIRE(buff[4] == 1);
    REQUIRE(buff[5] == 2);
    REQUIRE(buff[6] == 3);
    REQUIRE(buff[7] == 'h');
    REQUIRE(buff[8] == 10);
    REQUIRE(buff[9] == 11);
    REQUIRE(buff[10] == 20);
    REQUIRE(buff[11] == 21);
    REQUIRE(buff[12] == 22);
    REQUIRE(buff[13] == 0);
    REQUIRE(buff[14] == 1);
}
