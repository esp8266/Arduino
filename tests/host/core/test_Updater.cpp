/*
 test_Updater.cpp - Updater tests
 Copyright © 2019 Earle F. Philhower, III

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 */

#include <catch.hpp>
#include <Updater.h>

// Use a SPIFFS file because we can't instantiate a virtual class like Print
TEST_CASE("Updater fails when writes overflow requested size", "[core][Updater]")
{
    UpdaterClass* u;
    uint8_t       buff[6000];
    memset(buff, 0, sizeof(buff));
    u = new UpdaterClass();
    REQUIRE(u->begin(6000));
    REQUIRE(u->write(buff, 1000));
    REQUIRE(u->write(buff, 1000));
    REQUIRE(u->write(buff, 1000));
    REQUIRE(u->write(buff, 1000));
    REQUIRE(u->write(buff, 1000));
    REQUIRE(u->write(buff, 1000));
    REQUIRE(u->remaining() == 0);
    // All bytes written, should fail next
    REQUIRE(!u->write(buff, 1000));
    delete u;

    // Updater to a 4K aligned size, check nomissing over/underflow
    u = new UpdaterClass();
    REQUIRE(u->begin(8192));
    REQUIRE(u->remaining() == 8192);
    REQUIRE(u->write(buff, 4096));
    REQUIRE(u->write(buff, 4096));
    REQUIRE(!u->write(buff, 1));
    delete u;

    // Issue #4674
    u = new UpdaterClass();
    REQUIRE(u->begin(5000));
    REQUIRE(u->write(buff, 2048));
    REQUIRE(u->write(buff, 2048));
    // Should fail, would write 6KB
    REQUIRE(!u->write(buff, 2048));
    delete u;
}
