//
//  test_fs.cpp
//  esp8266-host-tests
//
//  Created by Ivan Grokhotkov on 02/03/16.
//  Copyright © 2016 esp8266.com. All rights reserved.
//

#include <catch.hpp>
#include <FS.h>
#include "../common/spiffs_mock.h"

#define SPIFFS_SIZE (64*1024)
#define SPIFFS_BLOCK (2*4096)
#define SPIFFS_PAGE (512)
#define SPIFFS_MOCK_DECLARE(size_kb, block_kb, page_b) SpiffsMock mock(size_kb * 1024, block_kb * 1024, page_b)

TEST_CASE("FS can begin","[fs]")
{
    SPIFFS_MOCK_DECLARE(1024, 8, 512);
    REQUIRE(SPIFFS.begin());
}

TEST_CASE("FS can create file","[fs]")
{
    SPIFFS_MOCK_DECLARE(1024, 8, 512);
    REQUIRE(SPIFFS.begin());
    {
        File f = SPIFFS.open("config.txt", "w");
        REQUIRE(f);
    }
    REQUIRE(SPIFFS.exists("config.txt"));
}

TEST_CASE("Files can be written and appended to","[fs]")
{
    SPIFFS_MOCK_DECLARE(1024, 8, 512);
    REQUIRE(SPIFFS.begin());
    {
        File f = SPIFFS.open("config1.txt", "w");
        f.println("file 1");
        REQUIRE(f);
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


