/*
 * Copyright (C) 2002 by Red Hat, Incorporated. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pgmspace.h>

#define MAX_1 50
#define memcmp memcmp_P
#define memcpy memcpy_P
#define memmem memmem_P
#define memchr memchr_P
#define strcat strcat_P
#define strncat strncat_P
#define strcpy strcpy_P
#define strncpy strncpy_P
#define strlen strlen_P
#define strnlen strnlen_P
#define strcmp strcmp_P
#define strncmp strncmp_P

#define MAX_2 (2 * MAX_1 + MAX_1 / 10)

void eprintf(int line, char* result, char* expected, int size)
{
    if (size != 0)
        printf("Failure at line %d, result is <%.*s>, should be <%s> of size %d\n",
            line, size, result, expected, size);
    else
        printf("Failure at line %d, result is <%s>, should be <%s>\n",
            line, result, expected);
}

void mycopy(char* target, char* source, int size)
{
    int i;

    for (i = 0; i < size; ++i)
    {
        target[i] = source[i];
    }
}

void myset(char* target, char ch, int size)
{
    int i;

    for (i = 0; i < size; ++i)
    {
        target[i] = ch;
    }
}

void tstring_main(void)
{
    char target[MAX_1] = "A";
    char first_char;
    char second_char;
    char array[] = "abcdefghijklmnopqrstuvwxz";
    char array2[] = "0123456789!@#$%^&*(";
    char buffer2[MAX_1];
    char buffer3[MAX_1];
    char buffer4[MAX_1];
    char buffer5[MAX_2];
    char buffer6[MAX_2];
    char buffer7[MAX_2];
    char expected[MAX_1];
    char *tmp1, *tmp2, *tmp3, *tmp4, *tmp5, *tmp6, *tmp7;
    int i, j, k, x, z, align_test_iterations;
    z = 0;

    int test_failed = 0;

    tmp1 = target;
    tmp2 = buffer2;
    tmp3 = buffer3;
    tmp4 = buffer4;
    tmp5 = buffer5;
    tmp6 = buffer6;
    tmp7 = buffer7;

    tmp2[0] = 'Z';
    tmp2[1] = '\0';

    if (memset(target, 'X', 0) != target || memcpy(target, "Y", 0) != target || memmove(target, "K", 0) != target || strncpy(tmp2, "4", 0) != tmp2 || strncat(tmp2, "123", 0) != tmp2 || strcat(target, "") != target)
    {
        eprintf(__LINE__, target, "A", 0);
        test_failed = 1;
    }

    if (strcmp(target, "A") || strlen(target) != 1 || memchr(target, 'A', 0) != NULL
        || memcmp(target, "J", 0) || strncmp(target, "A", 1) || strncmp(target, "J", 0) || tmp2[0] != 'Z' || tmp2[1] != '\0')
    {
        eprintf(__LINE__, target, "A", 0);
        test_failed = 1;
    }

    tmp2[2] = 'A';
    if (strcpy(target, "") != target || strncpy(tmp2, "", 4) != tmp2 || strcat(target, "") != target)
    {
        eprintf(__LINE__, target, "", 0);
        test_failed = 1;
    }

    if (target[0] != '\0' || strncmp(target, "", 1) || memcmp(tmp2, "\0\0\0\0", 4))
    {
        eprintf(__LINE__, target, "", 0);
        test_failed = 1;
    }

    tmp2[2] = 'A';
    if (strncat(tmp2, "1", 3) != tmp2 || memcmp(tmp2, "1\0A", 3))
    {
        eprintf(__LINE__, tmp2, "1\0A", 3);
        test_failed = 1;
    }

    if (strcpy(tmp3, target) != tmp3 || strcat(tmp3, "X") != tmp3 || strncpy(tmp2, "X", 2) != tmp2 || memset(target, tmp2[0], 1) != target)
    {
        eprintf(__LINE__, target, "X", 0);
        test_failed = 1;
    }

    if (strcmp(target, "X") || strlen(target) != 1 || memchr(target, 'X', 2) != target || strchr(target, 'X') != target || memchr(target, 'Y', 2) != NULL || strchr(target, 'Y') != NULL || strcmp(tmp3, target) || strncmp(tmp3, target, 2) || memcmp(target, "K", 0) || strncmp(target, tmp3, 3))
    {
        eprintf(__LINE__, target, "X", 0);
        test_failed = 1;
    }

    if (strcpy(tmp3, "Y") != tmp3 || strcat(tmp3, "Y") != tmp3 || memset(target, 'Y', 2) != target)
    {
        eprintf(__LINE__, target, "Y", 0);
        test_failed = 1;
    }

    target[2] = '\0';
    if (memcmp(target, "YY", 2) || strcmp(target, "YY") || strlen(target) != 2 || memchr(target, 'Y', 2) != target || strcmp(tmp3, target) || strncmp(target, tmp3, 3) || strncmp(target, tmp3, 4) || strncmp(target, tmp3, 2) || strchr(target, 'Y') != target)
    {
        eprintf(__LINE__, target, "YY", 2);
        test_failed = 1;
    }

    strcpy(target, "WW");
    if (memcmp(target, "WW", 2) || strcmp(target, "WW") || strlen(target) != 2 || memchr(target, 'W', 2) != target || strchr(target, 'W') != target)
    {
        eprintf(__LINE__, target, "WW", 2);
        test_failed = 1;
    }

    if (strncpy(target, "XX", 16) != target || memcmp(target, "XX\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16))
    {
        eprintf(__LINE__, target, "XX\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16);
        test_failed = 1;
    }

    if (strcpy(tmp3, "ZZ") != tmp3 || strcat(tmp3, "Z") != tmp3 || memcpy(tmp4, "Z", 2) != tmp4 || strcat(tmp4, "ZZ") != tmp4 || memset(target, 'Z', 3) != target)
    {
        eprintf(__LINE__, target, "ZZZ", 3);
        test_failed = 1;
    }

    target[3] = '\0';
    tmp5[0] = '\0';
    strncat(tmp5, "123", 2);
    if (memcmp(target, "ZZZ", 3) || strcmp(target, "ZZZ") || strcmp(tmp3, target) || strcmp(tmp4, target) || strncmp(target, "ZZZ", 4) || strncmp(target, "ZZY", 3) <= 0 || strncmp("ZZY", target, 4) >= 0 || memcmp(tmp5, "12", 3) || strlen(target) != 3)
    {
        eprintf(__LINE__, target, "ZZZ", 3);
        test_failed = 1;
    }

    target[2] = 'K';
    if (memcmp(target, "ZZZ", 2) || strcmp(target, "ZZZ") >= 0 || memcmp(target, "ZZZ", 3) >= 0 || strlen(target) != 3 || memchr(target, 'K', 3) != target + 2 || strncmp(target, "ZZZ", 2) || strncmp(target, "ZZZ", 4) >= 0 || strchr(target, 'K') != target + 2)
    {
        eprintf(__LINE__, target, "ZZK", 3);
        test_failed = 1;
    }

    strcpy(target, "AAA");
    if (memcmp(target, "AAA", 3) || strcmp(target, "AAA") || strncmp(target, "AAA", 3) || strlen(target) != 3)
    {
        eprintf(__LINE__, target, "AAA", 3);
        test_failed = 1;
    }

    j = 5;
    while (j < MAX_1)
    {
        for (i = j - 1; i <= j + 1; ++i)
        {
            /* don't bother checking unaligned data in the larger
	     sizes since it will waste time without performing additional testing */
            if ((size_t)i <= 16 * sizeof(long))
            {
                align_test_iterations = 2 * sizeof(long);
                if ((size_t)i <= 2 * sizeof(long) + 1)
                    z = 2;
                else
                    z = 2 * sizeof(long);
            }
            else
            {
                align_test_iterations = 1;
            }

            for (x = 0; x < align_test_iterations; ++x)
            {
                tmp1 = target + x;
                tmp2 = buffer2 + x;
                tmp3 = buffer3 + x;
                tmp4 = buffer4 + x;
                tmp5 = buffer5 + x;
                tmp6 = buffer6 + x;

                first_char = array[i % (sizeof(array) - 1)];
                second_char = array2[i % (sizeof(array2) - 1)];
                memset(tmp1, first_char, i);
                mycopy(tmp2, tmp1, i);
                myset(tmp2 + z, second_char, i - z - 1);
                if (memcpy(tmp1 + z, tmp2 + z, i - z - 1) != tmp1 + z)
                {
                    printf("error at line %d\n", __LINE__);
                    test_failed = 1;
                }

                tmp1[i] = '\0';
                tmp2[i] = '\0';
                if (strcpy(expected, tmp2) != expected)
                {
                    printf("error at line %d\n", __LINE__);
                    test_failed = 1;
                }
                tmp2[i - z] = first_char + 1;
                if (memmove(tmp2 + z + 1, tmp2 + z, i - z - 1) != tmp2 + z + 1 || memset(tmp3, first_char, i) != tmp3)
                {
                    printf("error at line %d\n", __LINE__);
                    test_failed = 1;
                }

                myset(tmp4, first_char, i);
                tmp5[0] = '\0';
                if (strncpy(tmp5, tmp1, i + 1) != tmp5 || strcat(tmp5, tmp1) != tmp5)
                {
                    printf("error at line %d\n", __LINE__);
                    test_failed = 1;
                }
                mycopy(tmp6, tmp1, i);
                mycopy(tmp6 + i, tmp1, i + 1);

                tmp7[2 * i + z] = second_char;
                strcpy(tmp7, tmp1);

                (void)strchr(tmp1, second_char);

                if (memcmp(tmp1, expected, i) || strcmp(tmp1, expected) || strncmp(tmp1, expected, i) || strncmp(tmp1, expected, i + 1) || strcmp(tmp1, tmp2) >= 0 || memcmp(tmp1, tmp2, i) >= 0 || strncmp(tmp1, tmp2, i + 1) >= 0 || (int)strlen(tmp1) != i || memchr(tmp1, first_char, i) != tmp1 || strchr(tmp1, first_char) != tmp1 || memchr(tmp1, second_char, i) != tmp1 + z || strchr(tmp1, second_char) != tmp1 + z || strcmp(tmp5, tmp6) || strncat(tmp7, tmp1, i + 2) != tmp7 || strcmp(tmp7, tmp6) || tmp7[2 * i + z] != second_char)
                {
                    eprintf(__LINE__, tmp1, expected, 0);
                    printf("x is %d\n", x);
                    printf("i is %d\n", i);
                    printf("tmp1 is <%p>\n", tmp1);
                    printf("tmp5 is <%p> <%s>\n", tmp5, tmp5);
                    printf("tmp6 is <%p> <%s>\n", tmp6, tmp6);
                    test_failed = 1;
                }

                for (k = 1; k <= align_test_iterations && k <= i; ++k)
                {
                    if (memcmp(tmp3, tmp4, i - k + 1) != 0 || strncmp(tmp3, tmp4, i - k + 1) != 0)
                    {
                        printf("Failure at line %d, comparing %.*s with %.*s\n",
                            __LINE__, i, tmp3, i, tmp4);
                        test_failed = 1;
                    }
                    tmp4[i - k] = first_char + 1;
                    if (memcmp(tmp3, tmp4, i) >= 0 || strncmp(tmp3, tmp4, i) >= 0 || memcmp(tmp4, tmp3, i) <= 0 || strncmp(tmp4, tmp3, i) <= 0)
                    {
                        printf("Failure at line %d, comparing %.*s with %.*s\n",
                            __LINE__, i, tmp3, i, tmp4);
                        test_failed = 1;
                    }
                    tmp4[i - k] = first_char;
                }
            }
        }
        j = ((2 * j) >> 2) << 2;
    }

    if (test_failed)
        abort();

    printf("ok\n");
}
