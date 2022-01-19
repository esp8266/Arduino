/*
    test_fs.cpp - host side file system tests
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

#include <catch.hpp>
#include <map>
#include <FS.h>
#include "../common/spiffs_mock.h"
#include "../common/littlefs_mock.h"
#include "../common/sdfs_mock.h"
#include <spiffs/spiffs.h>
#include <LittleFS.h>
#include "../../../libraries/SDFS/src/SDFS.h"
#include "../../../libraries/SD/src/SD.h"


namespace spiffs_test
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#define FSTYPE SPIFFS
#define TESTPRE "SPIFFS - "
#define TESTPAT "[fs]"
#define TOOLONGFILENAME "/2345678901234567890123456789012"
#define FS_MOCK_DECLARE SPIFFS_MOCK_DECLARE
#define FS_MOCK_RESET SPIFFS_MOCK_RESET
#undef FS_HAS_DIRS
#include "test_fs.inc"
#undef FSTYPE
#undef TESTPRE
#undef TESTPAT
#undef TOOLONGFILENAME
#undef FS_MOCK_DECLARE
#undef FS_MOCK_RESET

TEST_CASE("SPIFFS checks the config object passed in", "[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
    FSConfig f;
    SPIFFSConfig s;
    SDFSConfig d;
    LittleFSConfig l;

    REQUIRE_FALSE(SPIFFS.setConfig(f));
    REQUIRE(SPIFFS.setConfig(s));
    REQUIRE_FALSE(SPIFFS.setConfig(d));
    REQUIRE_FALSE(LittleFS.setConfig(l));
}
#pragma GCC diagnostic pop

};


namespace littlefs_test
{
#define FSTYPE LittleFS
#define TESTPRE "LittleFS - "
#define TESTPAT "[lfs]"
// LittleFS routines strip leading slashes before doing anything, so up to 31 char names are allowable
#define TOOLONGFILENAME "/12345678901234567890123456789012"
#define FS_MOCK_DECLARE LITTLEFS_MOCK_DECLARE
#define FS_MOCK_RESET LITTLEFS_MOCK_RESET
#define FS_HAS_DIRS
#include "test_fs.inc"
#undef FSTYPE
#undef TESTPRE
#undef TESTPAT
#undef TOOLONGFILENAME
#undef FS_MOCK_DECLARE
#undef FS_MOCK_RESET

TEST_CASE("LittleFS checks the config object passed in", "[fs]")
{
    LITTLEFS_MOCK_DECLARE(64, 8, 512, "");
    FSConfig f;
    SPIFFSConfig s;
    SDFSConfig d;
    LittleFSConfig l;

    REQUIRE_FALSE(LittleFS.setConfig(f));
    REQUIRE_FALSE(LittleFS.setConfig(s));
    REQUIRE_FALSE(LittleFS.setConfig(d));
    REQUIRE(LittleFS.setConfig(l));
}

};

namespace sdfs_test
{
#define FSTYPE SDFS
#define TESTPRE "SDFS - "
#define TESTPAT "[sdfs]"
// SDFS supports long paths (MAXPATH)
#define TOOLONGFILENAME "/" \
	"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" \
	"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" \
	"12345678901234567890123456789012345678901234567890123456"
#define FS_MOCK_DECLARE SDFS_MOCK_DECLARE
#define FS_MOCK_RESET SDFS_MOCK_RESET
#define FS_HAS_DIRS
#include "test_fs.inc"
#undef FSTYPE
#undef TESTPRE
#undef TESTPAT
#undef TOOLONGFILENAME
#undef FS_MOCK_DECLARE
#undef FS_MOCK_RESET

TEST_CASE("SDFS checks the config object passed in", "[fs]")
{
    SDFS_MOCK_DECLARE(64, 8, 512, "");
    FSConfig f;
    SPIFFSConfig s;
    SDFSConfig d;
    LittleFSConfig l;

    REQUIRE_FALSE(SDFS.setConfig(f));
    REQUIRE_FALSE(SDFS.setConfig(s));
    REQUIRE(SDFS.setConfig(d));
    REQUIRE_FALSE(SDFS.setConfig(l));
}

// Also a SD specific test to check that FILE_OPEN is really an append operation:

TEST_CASE("SD.h FILE_WRITE macro is append", "[fs]")
{
    SDFS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(SDFS.begin());
    REQUIRE(SD.begin(4));

    File f = SD.open("/file.txt", FILE_WRITE);
    f.write('a');
    f.write(65);
    f.write("bbcc");
    f.write("theend", 6);
    char block[3] = {'x', 'y', 'z'};
    f.write(block, 3);
    uint32_t bigone = 0x40404040;
    f.write((const uint8_t*)&bigone, 4);
    f.close();
    REQUIRE(readFile("/file.txt") == "aAbbcctheendxyz@@@@");
    f = SD.open("/file.txt", FILE_WRITE);
    f.write("append", 6);
    f.close();
    REQUIRE(readFile("/file.txt") == "aAbbcctheendxyz@@@@append");
    File g = SD.open("/file2.txt", FILE_WRITE);
    g.write(0);
    g.close();
    g = SD.open("/file2.txt", FILE_READ);
    uint8_t u = 0x66;
    g.read(&u, 1);
    g.close();
    REQUIRE(u == 0);
}

};
