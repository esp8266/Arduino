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
#include <spiffs/spiffs.h>
#include <LittleFS.h>

namespace spiffs_test {
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
};


namespace littlefs_test {
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
}
