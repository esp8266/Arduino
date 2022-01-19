/*
    test_pgmspace.cpp - pgmspace tests
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
#include <pgmspace.h>

TEST_CASE("strstr_P works as strstr", "[core][pgmspace]")
{
    auto t = [](const char* h, const char* n)
    {
        const char* strstr_P_result = strstr_P(h, n);
        const char* strstr_result = strstr(h, n);
        REQUIRE(strstr_P_result == strstr_result);
    };

    // Test case data is from avr-libc, original copyright (c) 2007  Dmitry Xmelkov
    // See avr-libc/tests/simulate/pmstring/strstr_P.c
    t("", "");
    t("12345", "");
    t("ababac", "abac");
    t("", "a");
    t("b", "a");
    t("a", "a");
    t("abcbef", "a");
    t(".a", "a");
    t(".a.", "a");
    t("ABCDEFGH", "H");
    t("", "12");
    t("13", "12");
    t("32", "12");
    t("12", "12");
    t("123", "12");
    t("012", "12");
    t("01200", "12");
    t("a_ab_abc_abcd_abcde", "abcdef");
    t("a_ab_abc_abcd_abcde_abcdef", "abcdef");
    t("aababcabcdabcde", "abcdef");
    t("aababcabcdabcdeabcdef", "abcdef");
    t("abaabaaabaaaab", "aaaaab");
    t("abaabaaabaaaabaaaaab", "aaaaab");
    t("_foo_foo", "foo");
    t("A", "a");
}
