/*
 test_string.cpp - String tests
 Copyright © 2018 Earle F. Philhower, III

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
#include <WString.h>

TEST_CASE("String::trim", "[core][String]")
{
    String str;
    str = "    abcd123   ";
    str.trim();
    REQUIRE(str == "abcd123");
}

TEST_CASE("String::replace", "[core][String]")
{
    String str;
    str = "The quick brown fox jumped over the lazy dog.";
    String find = "fox";
    String replace = "vulpes vulpes";
    str.replace(find, replace);
    REQUIRE(str == "The quick brown vulpes vulpes jumped over the lazy dog.");
}

TEST_CASE("String(value, base)", "[core][String]")
{
    String strbase2(9999,2);
    String strbase8(9999,8);
    String strbase10(9999,10);
    String strbase16(9999,16);
    REQUIRE(strbase2 == "10011100001111");
    REQUIRE(strbase8 == "23417");
    REQUIRE(strbase10 == "9999");
    REQUIRE(strbase16 == "270f");
}


