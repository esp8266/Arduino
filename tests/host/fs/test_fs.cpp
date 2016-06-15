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
#include <spiffs/spiffs.h>

static void createFile (const char* name, const char* content)
{
    auto f = SPIFFS.open(name, "w");
    REQUIRE(f);
    if (content) {
        f.print(content);
    }
}

static String readFile (const char* name)
{
    auto f = SPIFFS.open(name, "r");
    if (f) {
        return f.readString();
    }
    return String();
}

static std::set<String> listDir (const char* path)
{
    std::set<String> result;
    Dir dir = SPIFFS.openDir(path);
    while (dir.next()) {
        REQUIRE(result.find(dir.fileName()) == std::end(result));
        result.insert(dir.fileName());
    }
    return result;
}

TEST_CASE("FS can begin","[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512);
    REQUIRE(SPIFFS.begin());
}

TEST_CASE("FS can't begin with zero size","[fs]")
{
    SPIFFS_MOCK_DECLARE(0, 8, 512);
    REQUIRE_FALSE(SPIFFS.begin());
}

TEST_CASE("Before begin is called, open will fail","[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512);
    REQUIRE_FALSE(SPIFFS.open("/foo", "w"));
}

TEST_CASE("FS can create file","[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512);
    REQUIRE(SPIFFS.begin());
    createFile("/test", "");
    REQUIRE(SPIFFS.exists("/test"));
}

TEST_CASE("Files can be written and appended to","[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512);
    REQUIRE(SPIFFS.begin());
    {
        File f = SPIFFS.open("config1.txt", "w");
        REQUIRE(f);
        f.println("file 1");
    }
    {
        File f = SPIFFS.open("config1.txt", "a");
        REQUIRE(f);
        f.println("file 1 again");
    }
    {
        File f = SPIFFS.open("config1.txt", "r");
        REQUIRE(f);
        char buf[128];
        size_t len = f.read((uint8_t*)buf, sizeof(buf));
        buf[len] = 0;
        REQUIRE(strcmp(buf, "file 1\r\nfile 1 again\r\n") == 0);
    }
}

TEST_CASE("Files persist after reset", "[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512);
    REQUIRE(SPIFFS.begin());
    createFile("config1.txt", "file 1");

    SPIFFS_MOCK_RESET();
    REQUIRE(SPIFFS.begin());
    REQUIRE(readFile("config1.txt") == "file 1");
}


TEST_CASE("Filesystem is empty after format", "[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512);
    REQUIRE(SPIFFS.format());
    REQUIRE(SPIFFS.begin());
    createFile("/1", "first");
    createFile("/2", "second");
    REQUIRE(SPIFFS.format());
    Dir root = SPIFFS.openDir("/");
    size_t count = 0;
    while (root.next()) {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("Dir lists all files", "[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512);
    REQUIRE(SPIFFS.begin());
    createFile("/empty", "");
    createFile("/not_empty", "some text");
    createFile("/another", "more text");
    createFile("/subdir/empty", "");
    createFile("/subdir/not_empty", "text again");
    auto files = listDir("/");
    REQUIRE(files.size() == 5);
    REQUIRE(files.find("/empty") != std::end(files));
    REQUIRE(files.find("/not_empty") != std::end(files));
    REQUIRE(files.find("/another") != std::end(files));
    REQUIRE(files.find("/subdir/empty") != std::end(files));
    REQUIRE(files.find("/subdir/not_empty") != std::end(files));
}

TEST_CASE("File names which are too long are rejected", "[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512);
    REQUIRE(SPIFFS.begin());
    const char* emptyName = "";
    const char* longName_31 = "/234567890123456789012345678901";
    const char* longName_32 = "/2345678901234567890123456789012";
    REQUIRE_FALSE(SPIFFS.open(emptyName, "w"));
    REQUIRE_FALSE(SPIFFS.open(emptyName, "r"));
    REQUIRE_FALSE(SPIFFS.exists(emptyName));
    REQUIRE_FALSE(SPIFFS.open(longName_32, "w"));
    REQUIRE_FALSE(SPIFFS.open(longName_32, "r"));
    REQUIRE_FALSE(SPIFFS.exists(longName_32));
    REQUIRE(SPIFFS.open(longName_31, "w"));
    REQUIRE(SPIFFS.open(longName_31, "r"));
    REQUIRE(SPIFFS.exists(longName_31));
}

TEST_CASE("#1685 Duplicate files", "[fs][bugreport]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512);
    REQUIRE(SPIFFS.begin());
    createFile("/config", "some text");
    createFile("/data", "");
    readFile("/config");
    createFile("/data", "more text");
    listDir("/");
}

TEST_CASE("#1819 Can list all files with openDir(\"\")", "[fs][bugreport]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512);
    REQUIRE(SPIFFS.begin());
    createFile("/file1", "some text");
    createFile("/file2", "other text");
    createFile("file3", "more text");
    createFile("sorta-dir/file4", "\n");
    auto files = listDir("");
    REQUIRE(files.size() == 4);
}
