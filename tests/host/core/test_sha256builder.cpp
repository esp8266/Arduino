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
#include <SHA256Builder.h>
#include <StreamString.h>

TEST_CASE("SHA256Builder::add works as expected", "[core][SHA256Builder]")
{
    SHA256Builder builder;
    builder.begin();
    builder.add("short");
    builder.calculate();
    REQUIRE(builder.toString() == "f9b0078b5df596d2ea19010c001bbd009e651de2c57e8fb7e355f31eb9d3f739");

    builder.begin();
    builder.add("longlonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglong");
    builder.calculate();
    REQUIRE(builder.toString() == "0435a3add4cc74a0b15d05f9b7c53b6773f8b062735343a7f8128c28d8e1e4a4");
}


TEST_CASE("SHA256Builder::addHexString works as expected", "[core][SHA256Builder]")
{
    SHA256Builder builder;
    builder.begin();
    builder.addHexString("1234567890abcdeffedcba98765432106469676974616c7369676e61747572656170706c69636174696f6e73");
    builder.calculate();
    REQUIRE(builder.toString() == "a2526a45572e9d0b3e97b1bbd1f23fe79d93297f787f78e2df84021cd0ff03bf");
}

TEST_CASE("SHA256Builder::addStream works", "[core][SHA256Builder]"){
    SHA256Builder builder;
    const char* str = "MD5Builder::addStream_works_longlonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglong";
    {
        StreamString stream;
        stream.print(str);
        builder.begin();
        REQUIRE( builder.addStream(stream, stream.available()) );
        builder.calculate();
        REQUIRE(builder.toString() == "12d9e218343a2cfda7e966591c3a858db66db2a47f253711c1c6d34a57128ad9");
    }
    {
        StreamString stream;
        stream.print(str);
        builder.begin();
        REQUIRE( builder.addStream(stream, 20)) ;
        builder.calculate();
        REQUIRE(builder.toString() == "d1417bbca9aa9454a7a70fd05aa72e1fab385d1aee1785d37d36f7df4513430a");
    }
    {
        StreamString stream;
        stream.print(str);
        builder.begin();
        REQUIRE( builder.addStream(stream, 120));
        builder.calculate();
        REQUIRE(builder.toString() == "12d9e218343a2cfda7e966591c3a858db66db2a47f253711c1c6d34a57128ad9");
    }
}
