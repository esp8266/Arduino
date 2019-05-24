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
#include "../common/sdfs_mock.h"
#include <spiffs/spiffs.h>
#include "../../../libraries/SDFS/src/SDFS.h"
#include "../../../libraries/SD/src/SD.h"

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

TEST_CASE("SPIFFS checks the config object passed in", "[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
    FSConfig f;
    SPIFFSConfig s;
    SDFSConfig d;

    REQUIRE_FALSE(SPIFFS.setConfig(f));
    REQUIRE(SPIFFS.setConfig(s));
    REQUIRE_FALSE(SPIFFS.setConfig(d));
}

TEST_CASE("SDFS checks the config object passed in", "[fs]")
{
    SDFS_MOCK_DECLARE();
    FSConfig f;
    SPIFFSConfig s;
    SDFSConfig d;

    REQUIRE_FALSE(SDFS.setConfig(f));
    REQUIRE_FALSE(SDFS.setConfig(s));
    REQUIRE(SDFS.setConfig(d));
}

TEST_CASE("FS can begin","[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
    SPIFFSConfig cfg;
    cfg.setAutoFormat(false);
    SPIFFS.setConfig(cfg);
    REQUIRE_FALSE(SPIFFS.begin());
    cfg.setAutoFormat(true);
    SPIFFS.setConfig(cfg);
    REQUIRE(SPIFFS.begin());
    REQUIRE_FALSE(SPIFFS.setConfig(cfg)); // Can't change config of mounted FS
}

TEST_CASE("FS can't begin with zero size","[fs]")
{
    SPIFFS_MOCK_DECLARE(0, 8, 512, "");
    REQUIRE_FALSE(SPIFFS.begin());
}

TEST_CASE("Before begin is called, open will fail","[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE_FALSE(SPIFFS.open("/foo", "w"));
}

TEST_CASE("FS can create file","[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(SPIFFS.begin());
    createFile("/test", "");
    REQUIRE(SPIFFS.exists("/test"));
}

TEST_CASE("Files can be written and appended to","[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
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
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(SPIFFS.begin());
    createFile("config1.txt", "file 1");

    SPIFFS_MOCK_RESET();
    REQUIRE(SPIFFS.begin());
    REQUIRE(readFile("config1.txt") == "file 1");
}


TEST_CASE("Filesystem is empty after format", "[fs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
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
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
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
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
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
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(SPIFFS.begin());
    createFile("/config", "some text");
    createFile("/data", "");
    readFile("/config");
    createFile("/data", "more text");
    listDir("/");
}

TEST_CASE("#1819 Can list all files with openDir(\"\")", "[fs][bugreport]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(SPIFFS.begin());
    createFile("/file1", "some text");
    createFile("/file2", "other text");
    createFile("file3", "more text");
    createFile("sorta-dir/file4", "\n");
    auto files = listDir("");
    REQUIRE(files.size() == 4);
}

TEST_CASE("truncate", "[fs][spiffs]")
{
    SPIFFS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(SPIFFS.begin());
    createFile("/file1", "some text");
    auto f = SPIFFS.open("/file1", "r");
    f.truncate(4);
    f.close();
    String s = readFile("/file1");
    REQUIRE( s == "some" );
}

TEST_CASE("SDFS", "[sdfs]")
{
    SDFS_MOCK_DECLARE();
    auto cfg = SDFSConfig(0, SD_SCK_MHZ(1));
    SDFS.setConfig(cfg);
    REQUIRE(SDFS.format());
    REQUIRE(SDFS.begin());
    REQUIRE_FALSE(SDFS.setConfig(cfg)); // Can't change config of mounted fs
    REQUIRE(SDFS.mkdir("/happy/face"));
    REQUIRE(SDFS.mkdir("/happy/nose"));
    REQUIRE(SDFS.rmdir("/happy/face"));
    auto f = SDFS.open("/this/is/a/long/name.txt", "w");
    f.printf("Hello\n");
    f.close();
    SDFS.end();
}

TEST_CASE("Files.ino example", "[sd]")
{
    SDFS_MOCK_DECLARE();
    SDFS.end();
    SDFS.setConfig(SDFSConfig(0, SD_SCK_MHZ(1)));
    REQUIRE(SDFS.format());
    REQUIRE(SD.begin(4));
    REQUIRE_FALSE(SD.exists("example.txt"));
    File myFile = SD.open("example.txt", FILE_WRITE);
    REQUIRE(myFile);
    myFile.close();
    REQUIRE(SD.exists("example.txt"));
    REQUIRE(SD.remove("example.txt"));
    REQUIRE_FALSE(SD.exists("example.txt"));
    SDFS.end();
}


static void createFileSD(const char* name, const char* content)
{
    auto f = SD.open(name, FILE_WRITE);
    REQUIRE(f);
    if (content) {
        f.print(content);
    }
}

static String readFileSD(const char* name)
{
    auto f = SD.open(name, FILE_READ);
    if (f) {
        return f.readString();
    }
    return String();
}

TEST_CASE("Listfiles.ino example", "[sd]")
{
    SDFS_MOCK_DECLARE();
    SDFS.setConfig(SDFSConfig(0, SD_SCK_MHZ(1)));
    REQUIRE(SDFS.format());
    REQUIRE(SD.begin(4));

    createFileSD("file1", "hello");
    createFileSD("file2", "hola");
    createFileSD("dir1/file3", "nihao");
    createFileSD("dir2/dir3/file4", "bonjour");

    File root = SD.open("/");
    File file1 = root.openNextFile();
    File file2 = root.openNextFile();
    File dir1 = root.openNextFile();
    File dir1_file3 = dir1.openNextFile();
    File dir2 = root.openNextFile();
    File dir2_dir3 = dir2.openNextFile();
    File dir2_dir3_file4 = dir2_dir3.openNextFile();

    bool ok;
    ok = root.isDirectory() && !root.isFile() && !strcmp(root.name(), "/");
    REQUIRE(ok);
    ok = !file1.isDirectory() && file1.isFile() && !strcmp(file1.name(), "file1");
    REQUIRE(ok);
    ok = !file2.isDirectory() && file2.isFile() && !strcmp(file2.name(), "file2");
    REQUIRE(ok);
    ok = dir1.isDirectory() && !dir1.isFile() && !strcmp(dir1.name(), "dir1");
    REQUIRE(ok);
    ok = !dir1_file3.isDirectory() && dir1_file3.isFile() && !strcmp(dir1_file3.name(), "file3") &&
         !strcmp(dir1_file3.fullName(), "dir1/file3");
    REQUIRE(ok);
    ok = dir2.isDirectory() && !dir2.isFile() && !strcmp(dir2.name(), "dir2");
    REQUIRE(ok);
    ok = dir2_dir3.isDirectory() && !dir2_dir3.isFile() && !strcmp(dir2_dir3.name(), "dir3");
    REQUIRE(ok);
    ok = !dir2_dir3_file4.isDirectory() && dir2_dir3_file4.isFile() && !strcmp(dir2_dir3_file4.name(), "file4") &&
         !strcmp(dir2_dir3_file4.fullName(), "dir2/dir3/file4");
    REQUIRE(ok);

    REQUIRE(readFileSD("/file1") == "hello");
    REQUIRE(readFileSD("file2") == "hola");
    REQUIRE(readFileSD("dir1/file3") == "nihao");
    REQUIRE(readFileSD("/dir2/dir3/file4") == "bonjour");
}

TEST_CASE("Multisplendored File::writes", "[fs]")
{
    SDFS_MOCK_DECLARE();
    SDFS.end();
    SDFS.setConfig(SDFSConfig(0, SD_SCK_MHZ(1)));
    REQUIRE(SDFS.format());
    REQUIRE(SD.begin(4));

    File f = SD.open("/file.txt", FILE_WRITE);
    f.write('a');
    f.write(65);
    f.write("bbcc");
    f.write("theend", 6);
    char block[3]={'x','y','z'};
    f.write(block, 3);
    uint32_t bigone = 0x40404040;
    f.write((const uint8_t*)&bigone, 4);
    f.close();
    REQUIRE(readFileSD("/file.txt") == "aAbbcctheendxyz@@@@");
    File g = SD.open("/file2.txt", FILE_WRITE);
    g.write(0);
    g.close();
    g = SD.open("/file2.txt", FILE_READ);
    uint8_t u = 0x66;
    g.read(&u, 1);
    g.close();
    REQUIRE(u == 0);
}
