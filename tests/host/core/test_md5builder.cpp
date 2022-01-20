/*
 test_md5builder.cpp - MD5Builder tests
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
#include <string.h>
#include <MD5Builder.h>
#include <StreamString.h>

TEST_CASE("MD5Builder::add works as expected", "[core][MD5Builder]")
{
    MD5Builder builder;
    builder.begin();
    builder.add("short");
    builder.calculate();
    REQUIRE(builder.toString() == "4f09daa9d95bcb166a302407a0e0babe");

    builder.begin();
    builder.add("longlonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglong");
    builder.calculate();
    REQUIRE(builder.toString() == "9edb67f2b22c604fab13e2fd1d6056d7");
}

TEST_CASE("MD5Builder::addHexString works as expected", "[core][MD5Builder]")
{
    WHEN("A char array is parsed")
    {
        MD5Builder builder;
        builder.begin();
        const char* myPayload = "1234567890abcdeffedcba98765432106469676974616c7369676e61747572656170706c69636174696F6e73";
        builder.addHexString(myPayload);
        builder.calculate();
        REQUIRE(builder.toString() == "47b937a6f9f12a4c389fa5854e023efb");
    }

    WHEN("A Arduino String is parsed")
    {
        MD5Builder builder;
        builder.begin();
        builder.addHexString(String("1234567890abcdeffedcba98765432106469676974616c7369676e61747572656170706c69636174696f6e73"));
        builder.calculate();
        REQUIRE(builder.toString() == "47b937a6f9f12a4c389fa5854e023efb");
    }
}

TEST_CASE("MD5Builder::addStream works", "[core][MD5Builder]")
{
    MD5Builder  builder;
    const char* str = "MD5Builder::addStream_works_longlonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglong";
    {
        StreamString stream;
        stream.print(str);
        builder.begin();
        builder.addStream(stream, stream.available());
        builder.calculate();
        REQUIRE(builder.toString() == "bc4a2006e9d7787ee15fe3d4ef9cdb46");
    }
    {
        StreamString stream;
        stream.print(str);
        builder.begin();
        builder.addStream(stream, 20);
        builder.calculate();
        REQUIRE(builder.toString() == "c9ad2a3d64b9a877831a67b3bfd34228");
    }
    {
        StreamString stream;
        stream.print(str);
        builder.begin();
        builder.addStream(stream, 120);
        builder.calculate();
        REQUIRE(builder.toString() == "bc4a2006e9d7787ee15fe3d4ef9cdb46");
    }
}
