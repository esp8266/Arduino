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
#include <StreamString.h>

TEST_CASE("String::move", "[core][String]")
{
    const char buffer[] = "this string goes over the sso limit";

    String target;
    String source(buffer);

    target = std::move(source);
    REQUIRE(source.c_str() != nullptr);
    REQUIRE(!source.length());
    REQUIRE(target == buffer);
}

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
    String flash = (F("hello from flash"));
    REQUIRE(flash == "hello from flash");
    const char textarray[6] = {'h', 'e', 'l', 'l', 'o', 0};
    String hello(textarray);
    REQUIRE(hello == "hello");
    String hello2;
    hello2 = textarray;
    REQUIRE(hello2 == "hello");
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
    str += (long)-123;
    REQUIRE(str == "abcdeabcde9872147483647-2147483648691969-123");
    str += (unsigned long)321;
    REQUIRE(str == "abcdeabcde9872147483647-2147483648691969-123321");
    str += (float)-1.01;
    REQUIRE(str == "abcdeabcde9872147483647-2147483648691969-123321-1.01");
    str += (double)1.01;
    REQUIRE(str == "abcdeabcde9872147483647-2147483648691969-123321-1.011.01");
    str = "clean";
    REQUIRE(str.concat(str) == true);
    REQUIRE(str == "cleanclean");
    // non-decimal negative #s should be as if they were unsigned
    str = String((int)-100, 16);
    REQUIRE(str == "ffffff9c");
    str = String((long)-101, 16);
    REQUIRE(str == "ffffff9b");
    str = String((int)-100, 10);
    REQUIRE(str == "-100");
    str = String((long)-100, 10);
    REQUIRE(str == "-100");
    // Non-zero-terminated array concatenation
    const char buff[] = "abcdefg";
    String n;
    n = "1234567890"; // Make it a SSO string, fill with non-0 data
    n = "1"; // Overwrite [1] with 0, but leave old junk in SSO space still
    n.concat(buff, 3);
    REQUIRE(n == "1abc"); // Ensure the trailing 0 is always present even w/this funky concat
    for (int i=0; i<20; i++)
        n.concat(buff, 1); // Add 20 'a's to go from SSO to normal string
    REQUIRE(n == "1abcaaaaaaaaaaaaaaaaaaaa");
    n = "";
    for (int i=0; i<=5; i++)
        n.concat(buff, i);
    REQUIRE(n == "aababcabcdabcde");
    n.concat(buff, 0); // And check no add'n
    REQUIRE(n == "aababcabcdabcde");
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
    memset(buff, 0, 4);
    s.getBytes(buff, 4, 6);
    bool ok = (buff[0] == 'E') && (buff[1] == 0) && (buff[2] == 0) && (buff[3] == 0);
    REQUIRE(ok == true);
}

TEST_CASE("String conversion", "[core][String]")
{
    String s = "12345";
    long l = s.toInt();
    REQUIRE(l == 12345);
    s = "2147483647";
    l = s.toInt();
    REQUIRE(l == INT_MAX);
    s = "-2147483647";
    l = s.toInt();
    REQUIRE(l == -2147483647);
    s = "-2147483648";
    l = s.toInt();
    REQUIRE(l == INT_MIN);
    s = "3.14159";
    float f = s.toFloat();
    REQUIRE( fabs(f - 3.14159) < 0.0001 );
}

TEST_CASE("String case", "[core][String]")
{
    String s = "aBc_123";
    s.toLowerCase();
    REQUIRE(s == "abc_123");
    s = "aBc_123";
    s.toUpperCase();
    REQUIRE(s == "ABC_123");
}

TEST_CASE("String nulls", "[core][String]")
{
    String s;
    REQUIRE(s == "");
    REQUIRE(s.toFloat() == 0);
    REQUIRE(s.toInt() == 0);
    s.trim();
    s.toUpperCase();
    s.toLowerCase();
    s.remove(1,1);
    s.remove(10);
    s.replace("taco", "burrito");
    s.replace('a', 'b');
    REQUIRE(s.substring(10, 20) == "");
    REQUIRE(s.lastIndexOf("tacos", 1) == -1);
    REQUIRE(s.lastIndexOf("tacos") == -1);
    REQUIRE(s.lastIndexOf('t', 0) == -1);
    REQUIRE(s.lastIndexOf('t') == -1);
    REQUIRE(s.indexOf(String("tacos"), 1) == -1);
    REQUIRE(s.indexOf(String("tacos")) == -1);
    REQUIRE(s.indexOf(F("tacos"), 1) == -1);
    REQUIRE(s.indexOf(F("tacos")) == -1);
    REQUIRE(s.indexOf("tacos", 1) == -1);
    REQUIRE(s.indexOf("tacos") == -1);
    REQUIRE(s.indexOf('t', 1) == -1);
    REQUIRE(s.indexOf('t') == -1);
    s.getBytes(NULL, 100, 0);
    s[0] = 't';
    REQUIRE(s == "");
    REQUIRE(s.length() == 0);
    s.setCharAt(1, 't');
    REQUIRE(s.startsWith("abc",0) == false);
    REQUIRE(s.startsWith("def") == false);
    REQUIRE(s.equalsConstantTime("def") == false);
    REQUIRE(s.equalsConstantTime("") == true);
    REQUIRE(s.equalsConstantTime(s) == true);
    REQUIRE(s.equalsIgnoreCase(s) == true);
    REQUIRE(s.equals("def") == false);
    REQUIRE(s.equals("") == true);
    REQUIRE(s.equals(s) == true);
    String t = s;
    REQUIRE(s.equals(t) == true);
    REQUIRE((s <= ""));
    REQUIRE(!(s < ""));
    REQUIRE((s >= ""));
    REQUIRE(!(s > ""));
    s += "abc";
    REQUIRE(s == "abc");
}

TEST_CASE("String sizes near 8b", "[core][String]")
{
    // Test that proper amount of space allocated (including trailing 0)
    // Need valgrind to verify no out-of-bounds errors, the strcmp()s will
    // access each byte and cause an exception of the space was not properly
    // allocated.
    String s7("123456");
    String s8("1234567");
    String s9("12345678");
    String s15("12345678901234");
    String s16("123456789012345");
    String s17("1234567890123456");
    REQUIRE(!strcmp(s7.c_str(),"123456"));
    REQUIRE(!strcmp(s8.c_str(),"1234567"));
    REQUIRE(!strcmp(s9.c_str(),"12345678"));
    REQUIRE(!strcmp(s15.c_str(),"12345678901234"));
    REQUIRE(!strcmp(s16.c_str(),"123456789012345"));
    REQUIRE(!strcmp(s17.c_str(),"1234567890123456"));
    s7 += '_';
    s8 += '_';
    s9 += '_';
    s15 += '_';
    s16 += '_';
    s17 += '_';
    REQUIRE(!strcmp(s7.c_str(),"123456_"));
    REQUIRE(!strcmp(s8.c_str(),"1234567_"));
    REQUIRE(!strcmp(s9.c_str(),"12345678_"));
    REQUIRE(!strcmp(s15.c_str(),"12345678901234_"));
    REQUIRE(!strcmp(s16.c_str(),"123456789012345_"));
    REQUIRE(!strcmp(s17.c_str(),"1234567890123456_"));
}

TEST_CASE("String SSO works", "[core][String]")
{
  // This test assumes that SSO_SIZE==8, if that changes the test must as well
  String s;
  s += "0";
  REQUIRE(s == "0");
  REQUIRE(s.length() == 1);
  const char *savesso = s.c_str();
  s += 1;
  REQUIRE(s.c_str() == savesso);
  REQUIRE(s == "01");
  REQUIRE(s.length() == 2);
  s += "2";
  REQUIRE(s.c_str() == savesso);
  REQUIRE(s == "012");
  REQUIRE(s.length() == 3);
  s += 3;
  REQUIRE(s.c_str() == savesso);
  REQUIRE(s == "0123");
  REQUIRE(s.length() == 4);
  s += "4";
  REQUIRE(s.c_str() == savesso);
  REQUIRE(s == "01234");
  REQUIRE(s.length() == 5);
  s += "5";
  REQUIRE(s.c_str() == savesso);
  REQUIRE(s == "012345");
  REQUIRE(s.length() == 6);
  s += "6";
  REQUIRE(s.c_str() == savesso);
  REQUIRE(s == "0123456");
  REQUIRE(s.length() == 7);
  s += "7";
  REQUIRE(s.c_str() == savesso);
  REQUIRE(s == "01234567");
  REQUIRE(s.length() == 8);
  s += "8";
  REQUIRE(s.c_str() == savesso);
  REQUIRE(s == "012345678");
  REQUIRE(s.length() == 9);
  s += "9";
  REQUIRE(s.c_str() == savesso);
  REQUIRE(s == "0123456789");
  REQUIRE(s.length() == 10);
  if (sizeof(savesso) == 4) {
    s += "a";
    REQUIRE(s.c_str() != savesso);
    REQUIRE(s == "0123456789a");
    REQUIRE(s.length() == 11);
    s += "b";
    REQUIRE(s.c_str() != savesso);
    REQUIRE(s == "0123456789ab");
    REQUIRE(s.length() == 12);
    s += "c";
    REQUIRE(s.c_str() != savesso);
    REQUIRE(s == "0123456789abc");
    REQUIRE(s.length() == 13);
  } else {
  s += "a";
    REQUIRE(s.c_str() == savesso);
    REQUIRE(s == "0123456789a");
    REQUIRE(s.length() == 11);
    s += "bcde";
    REQUIRE(s.c_str() == savesso);
    REQUIRE(s == "0123456789abcde");
    REQUIRE(s.length() == 15);
    s += "fghi";
    REQUIRE(s.c_str() != savesso);
    REQUIRE(s == "0123456789abcdefghi");
    REQUIRE(s.length() == 19);
    s += "j";
    REQUIRE(s.c_str() != savesso);
    REQUIRE(s == "0123456789abcdefghij");
    REQUIRE(s.length() == 20);
    s += "k";
    REQUIRE(s.c_str() != savesso);
    REQUIRE(s == "0123456789abcdefghijk");
    REQUIRE(s.length() == 21);
    s += "l";
    REQUIRE(s.c_str() != savesso);
    REQUIRE(s == "0123456789abcdefghijkl");
    REQUIRE(s.length() == 22);
    s += "m";
    REQUIRE(s.c_str() != savesso);
    REQUIRE(s == "0123456789abcdefghijklm");
    REQUIRE(s.length() == 23);
    s += "nopq";
    REQUIRE(s.c_str() != savesso);
    REQUIRE(s == "0123456789abcdefghijklmnopq");
    REQUIRE(s.length() == 27);
    s += "rstu";
    REQUIRE(s.c_str() != savesso);
    REQUIRE(s == "0123456789abcdefghijklmnopqrstu");
    REQUIRE(s.length() == 31);
  }
  s = "0123456789abcde";
  s = s.substring(s.indexOf('a'));
  REQUIRE(s == "abcde");
  REQUIRE(s.length() == 5);
}

#include <new>
void repl(const String& key, const String& val, String& s, boolean useURLencode)
{
    s.replace(key, val);
}


TEST_CASE("String SSO handles junk in memory", "[core][String]")
{
  // We fill the SSO space with garbage then construct an object in it and check consistency
  // This is NOT how you want to use Strings outside of this testing!
  unsigned char space[64];
  String *s = (String*)space;
  memset(space, 0xff, 64);
  new(s) String;
  REQUIRE(*s == "");
  s->~String();

  // Tests from #5883
  bool useURLencode = false;
  const char euro[4] = {(char)0xe2, (char)0x82, (char)0xac, 0}; // Unicode euro symbol
  const char yen[3]   = {(char)0xc2, (char)0xa5, 0}; // Unicode yen symbol

  memset(space, 0xff, 64);
  new(s) String("%ssid%");
  repl(("%ssid%"), "MikroTik", *s, useURLencode);
  REQUIRE(*s == "MikroTik");
  s->~String();

  memset(space, 0xff, 64);
  new(s) String("{E}");
  repl(("{E}"), euro, *s, useURLencode);
  REQUIRE(*s == "€");
  s->~String();
  memset(space, 0xff, 64);
  new(s) String("&euro;");
  repl(("&euro;"), euro, *s, useURLencode);
  REQUIRE(*s == "€");
  s->~String();
  memset(space, 0xff, 64);
  new(s) String("{Y}");
  repl(("{Y}"), yen, *s, useURLencode);
  REQUIRE(*s == "¥");
  s->~String();
  memset(space, 0xff, 64);
  new(s) String("&yen;");
  repl(("&yen;"), yen, *s, useURLencode);
  REQUIRE(*s == "¥");
  s->~String();

  memset(space, 0xff, 64);
  new(s) String("%sysname%");
  repl(("%sysname%"), "CO2_defect", *s, useURLencode);
  REQUIRE(*s == "CO2_defect");
  s->~String();
}


TEST_CASE("Issue #5949 - Overlapping src/dest in replace", "[core][String]")
{
  String blah = "blah";
  blah.replace("xx", "y");
  REQUIRE(blah == "blah");
  blah.replace("x", "yy");
  REQUIRE(blah == "blah");
  blah.replace(blah, blah);
  REQUIRE(blah == "blah");
}


TEST_CASE("Issue #2736 - StreamString SSO fix", "[core][StreamString]")
{
    StreamString s;
    s.print('{');
    s.print('\"');
    s.print(String("message"));
    s.print('\"');
    REQUIRE(s == "{\"message\"");
}

TEST_CASE("Strings with NULs", "[core][String]")
{
  // The following should never be done in a real app! This is only to inject 0s in the middle of a string.
  // Fits in SSO...
  String str("01234567");
  REQUIRE(str.length() == 8);
  char *ptr = (char *)str.c_str();
  ptr[3] = 0;
  String str2;
  str2 = str;
  REQUIRE(str2.length() == 8);
  // Needs a buffer pointer
  str = "0123456789012345678901234567890123456789";
  ptr = (char *)str.c_str();
  ptr[3] = 0;
  str2 = str;
  REQUIRE(str2.length() == 40);
  String str3("a");
  ptr = (char *)str3.c_str();
  *ptr = 0;
  REQUIRE(str3.length() == 1);
  str3 += str3;
  REQUIRE(str3.length() == 2);
  str3 += str3;
  REQUIRE(str3.length() == 4);
  str3 += str3;
  REQUIRE(str3.length() == 8);
  str3 += str3;
  REQUIRE(str3.length() == 16);
  str3 += str3;
  REQUIRE(str3.length() == 32);
  str3 += str3;
  REQUIRE(str3.length() == 64);
  static char zeros[64] = {0};
  const char *p = str3.c_str();
  REQUIRE(!memcmp(p, zeros, 64));
}

TEST_CASE("Replace and string expansion", "[core][String]")
{
  String s, l;
  // Make these large enough to span SSO and non SSO
  String whole = "#123456789012345678901234567890";
  const char *res = "abcde123456789012345678901234567890";
  for (size_t i=1; i < whole.length(); i++) {
    s = whole.substring(0, i);
    l = s;
    l.replace("#", "abcde");
    char buff[64];
    strcpy(buff, res);
    buff[5 + i-1] = 0;
    REQUIRE(!strcmp(l.c_str(), buff));
    REQUIRE(l.length() == strlen(buff));
  }
}
