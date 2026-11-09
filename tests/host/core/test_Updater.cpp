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
#include <cstdlib>
#include <new>

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

// Count and, on request, refuse array allocations of given sizes, so a test
// can see which buffer size UpdaterClass::begin() asks for.
// UpdaterClass::begin() uses new (std::nothrow) uint8_t[], so replacing
// this one operator is enough. Everything else is passed to malloc().
namespace
{
    constexpr size_t maxLog = 16;
    size_t           allocLog[maxLog];
    size_t           allocCount = 0;
    size_t           refuseA    = 0;  // 0: refuse nothing
    size_t           refuseB    = 0;
    bool             logging    = false;
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept
{
    if (logging && allocCount < maxLog)
    {
        allocLog[allocCount++] = size;
    }
    if (size == refuseA || size == refuseB)
    {
        return nullptr;
    }
    return malloc(size);
}

extern uint32_t mockFreeHeap;

namespace
{
    // Run begin() with logging on. Sizes refuseA/refuseB are refused,
    // heap is the mocked free heap.
    bool beginWith(UpdaterClass& u, size_t a, size_t b, uint32_t heap)
    {
        allocCount   = 0;
        refuseA      = a;
        refuseB      = b;
        mockFreeHeap = heap;
        logging      = true;
        const bool ok = u.begin(6000);
        logging      = false;
        refuseA      = 0;
        refuseB      = 0;
        mockFreeHeap = 30000;
        return ok;
    }

    // Under valgrind, operator new is replaced by valgrind's own, whatever the
    // test binary defines, so the hook above is never called. Check that
    // once, and let the tests that need the hook skip themselves in that case.
    bool allocHookWorks()
    {
        static const bool works = []() {
            allocCount = 0;
            logging    = true;
            // volatile: the compiler may not drop the new/delete pair
            volatile uint8_t* probe = new (std::nothrow) uint8_t[1];
            logging                 = false;
            if (probe)
            {
                *probe = 1;
            }
            delete[] probe;
            return allocCount == 1;
        }();
        if (!works)
        {
            WARN("operator new[] cannot be replaced here (valgrind?), test skipped");
        }
        return works;
    }
}

TEST_CASE("Updater uses a sector sized buffer when there is room", "[core][Updater]")
{
    if (!allocHookWorks())
    {
        return;
    }
    UpdaterClass u;
    REQUIRE(beginWith(u, 0, 0, 30000));
    REQUIRE(allocCount == 1);
    REQUIRE(allocLog[0] == FLASH_SECTOR_SIZE);
    u.end(true);  // no data written: fails, and frees the buffer
}

TEST_CASE("Updater falls back to a small buffer when the big one cannot be allocated", "[core][Updater]")
{
    if (!allocHookWorks())
    {
        return;
    }
    UpdaterClass u;
    uint8_t      buff[6000];
    memset(buff, 0, sizeof(buff));
    // Plenty of free heap in total, but no block of 4 KB (fragmentation).
    REQUIRE(beginWith(u, FLASH_SECTOR_SIZE, 0, 30000));
    REQUIRE(allocCount == 2);
    REQUIRE(allocLog[0] == FLASH_SECTOR_SIZE);
    REQUIRE(allocLog[1] == 256);
    // And the update still works with the small buffer.
    REQUIRE(u.write(buff, 6000));
    REQUIRE(u.remaining() == 0);
    REQUIRE(!u.hasError());
    u.end(true);  // frees the buffer
}

TEST_CASE("Updater starts with the small buffer when the heap is low", "[core][Updater]")
{
    if (!allocHookWorks())
    {
        return;
    }
    UpdaterClass u;
    REQUIRE(beginWith(u, 0, 0, 2 * FLASH_SECTOR_SIZE));
    REQUIRE(allocCount == 1);
    REQUIRE(allocLog[0] == 256);
    u.end(true);
}

TEST_CASE("Updater reports OOM when no buffer can be allocated", "[core][Updater]")
{
    if (!allocHookWorks())
    {
        return;
    }
    UpdaterClass u;
    // Low heap: only the small buffer is tried.
    REQUIRE(!beginWith(u, 256, 0, 2 * FLASH_SECTOR_SIZE));
    REQUIRE(allocCount == 1);
    REQUIRE(u.getError() == UPDATE_ERROR_OOM);
    u.clearError();
    // Enough heap: both sizes are tried, both fail.
    REQUIRE(!beginWith(u, FLASH_SECTOR_SIZE, 256, 30000));
    REQUIRE(allocCount == 2);
    REQUIRE(u.getError() == UPDATE_ERROR_OOM);
    // A failed begin() leaves the updater usable.
    u.clearError();
    REQUIRE(beginWith(u, 0, 0, 30000));
    u.end(true);
}
