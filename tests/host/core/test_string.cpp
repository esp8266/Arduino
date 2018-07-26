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
#include <limits.h>

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
    String strnegi(-9999);
    String strnegf(-2.123, 3);
    REQUIRE(strnegi == "-9999");
    REQUIRE(strnegf == "-2.123");
    String strbase16l((long)999999,16);
    REQUIRE(strbase16l == "f423f");
}

TEST_CASE("String constructors", "[core][String]")
{
    String s0('c');
    REQUIRE(s0 == "c");
    String bin((unsigned char)5, 4);
    REQUIRE(bin == "11");
    String ib((unsigned int)999, 16);
    REQUIRE(ib == "3e7");
    String lb((unsigned long)3000000000, 8);
    REQUIRE(lb == "26264057000");
    String sl1((long)-2000000000, 10);
    REQUIRE(sl1 == "-2000000000");
    String s1("abcd");
    String s2(s1);
    REQUIRE(s1 == s2);
    String *s3 = new String("manos");
    s2 = *s3;
    delete s3;
    REQUIRE(s2 == "manos");
    s3 = new String("thisismuchlongerthantheother");
    s2 = s3->c_str();
    delete s3;
    REQUIRE(s2 == "thisismuchlongerthantheother");
    String strf((float)3.14159, 5);
    REQUIRE(strf == "3.14159");
    String ssh(strf + "_" + s1);
    REQUIRE(ssh == "3.14159_abcd");
}

TEST_CASE("String concantenation", "[core][String]")
{
    String str;
    REQUIRE(str.length() == 0);
    str.reserve(1000);
    REQUIRE(str.length() == 0);
    str.reserve(0);
    REQUIRE(str.length() == 0);
    str += 'a';
    str += "bcde";
    str += str;
    str += 987;
    str += (int)INT_MAX;
    str += (int)INT_MIN;
    REQUIRE(str == "abcdeabcde9872147483647-2147483648");
    str += (unsigned char)69;
    REQUIRE(str == "abcdeabcde9872147483647-214748364869");
    str += (unsigned int)1969;
    REQUIRE(str == "abcdeabcde9872147483647-2147483648691969");
}

TEST_CASE("String comparison", "[core][String]")
{
    String alpha("I like fish!");
    REQUIRE(alpha < "I like tacos!");
    REQUIRE(alpha > "I like bacon!");
    REQUIRE(alpha.equalsIgnoreCase("i LiKe FiSh!"));
    REQUIRE(alpha.equalsConstantTime("I like fish!"));
    REQUIRE(alpha != "I like fish?");
    REQUIRE(alpha.startsWith("I like"));
    REQUIRE(!alpha.startsWith("I lick"));
    REQUIRE(alpha.startsWith("fish", 7));
    REQUIRE(!alpha.startsWith("fish?", 7));
    REQUIRE(alpha.endsWith("!"));
    REQUIRE(alpha.endsWith("fish!"));
    REQUIRE(!alpha.endsWith("sh?"));
}

TEST_CASE("String byte access", "[core][String]")
{
    String s;
    s.reserve(1000);
    s = "Never Eat Soggy Waffles";
    REQUIRE(s[0] == 'N');
    REQUIRE(s[999] == 0);
    s[6] = 'C';
    REQUIRE(!strcmp(s.c_str(), "Never Cat Soggy Waffles"));
    unsigned char buff[4];
    s.getBytes(buff, 4, 6);
    REQUIRE(!memcmp(buff, "Cat", 4));
    s = "Never E";
    s.getBytes(buff, 4, 6);
    REQUIRE(!memcmp(buff, "E", 2));
}
