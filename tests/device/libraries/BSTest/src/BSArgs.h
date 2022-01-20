/* Splitting string into tokens, taking quotes and escape sequences into account.
 * From https://github.com/espressif/esp-idf/blob/master/components/console/split_argv.c
 * Copyright 2016-2017 Espressif Systems (Shanghai) PTE LTD
 * Licensed under the Apache License 2.0.
 */

#ifndef BS_ARGS_H
#define BS_ARGS_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>

namespace bs
{
namespace protocol
{
#define SS_FLAG_ESCAPE 0x8

    typedef enum
    {
        /* parsing the space between arguments */
        SS_SPACE = 0x0,
        /* parsing an argument which isn't quoted */
        SS_ARG = 0x1,
        /* parsing a quoted argument */
        SS_QUOTED_ARG = 0x2,
        /* parsing an escape sequence within unquoted argument */
        SS_ARG_ESCAPED = SS_ARG | SS_FLAG_ESCAPE,
        /* parsing an escape sequence within a quoted argument */
        SS_QUOTED_ARG_ESCAPED = SS_QUOTED_ARG | SS_FLAG_ESCAPE,
    } split_state_t;

/* helper macro, called when done with an argument */
#define END_ARG()                      \
    do                                 \
    {                                  \
        char_out     = 0;              \
        argv[argc++] = next_arg_start; \
        state        = SS_SPACE;       \
    } while (0);

    /**
 * @brief Split command line into arguments in place
 *
 * - This function finds whitespace-separated arguments in the given input line.
 *
 *     'abc def 1 20 .3' -> [ 'abc', 'def', '1', '20', '.3' ]
 *
 * - Argument which include spaces may be surrounded with quotes. In this case
 *   spaces are preserved and quotes are stripped.
 *
 *     'abc "123 456" def' -> [ 'abc', '123 456', 'def' ]
 *
 * - Escape sequences may be used to produce backslash, double quote, and space:
 *
 *     'a\ b\\c\"' -> [ 'a b\c"' ]
 *
 * Pointers to at most argv_size - 1 arguments are returned in argv array.
 * The pointer after the last one (i.e. argv[argc]) is set to NULL.
 *
 * @param line pointer to buffer to parse; it is modified in place
 * @param argv array where the pointers to arguments are written
 * @param argv_size number of elements in argv_array (max. number of arguments will be argv_size - 1)
 * @return number of arguments found (argc)
 */
    inline size_t split_args(char* line, char** argv, size_t argv_size)
    {
        const int     QUOTE          = '"';
        const int     ESCAPE         = '\\';
        const int     SPACE          = ' ';
        split_state_t state          = SS_SPACE;
        size_t        argc           = 0;
        char*         next_arg_start = line;
        char*         out_ptr        = line;
        for (char* in_ptr = line; argc < argv_size - 1; ++in_ptr)
        {
            int char_in = (unsigned char)*in_ptr;
            if (char_in == 0)
            {
                break;
            }
            int char_out = -1;

            switch (state)
            {
            case SS_SPACE:
                if (char_in == SPACE)
                {
                    /* skip space */
                }
                else if (char_in == QUOTE)
                {
                    next_arg_start = out_ptr;
                    state          = SS_QUOTED_ARG;
                }
                else if (char_in == ESCAPE)
                {
                    next_arg_start = out_ptr;
                    state          = SS_ARG_ESCAPED;
                }
                else
                {
                    next_arg_start = out_ptr;
                    state          = SS_ARG;
                    char_out       = char_in;
                }
                break;

            case SS_QUOTED_ARG:
                if (char_in == QUOTE)
                {
                    END_ARG();
                }
                else if (char_in == ESCAPE)
                {
                    state = SS_QUOTED_ARG_ESCAPED;
                }
                else
                {
                    char_out = char_in;
                }
                break;

            case SS_ARG_ESCAPED:
            case SS_QUOTED_ARG_ESCAPED:
                if (char_in == ESCAPE || char_in == QUOTE || char_in == SPACE)
                {
                    char_out = char_in;
                }
                else
                {
                    /* unrecognized escape character, skip */
                }
                state = (split_state_t)(state & (~SS_FLAG_ESCAPE));
                break;

            case SS_ARG:
                if (char_in == SPACE)
                {
                    END_ARG();
                }
                else if (char_in == ESCAPE)
                {
                    state = SS_ARG_ESCAPED;
                }
                else
                {
                    char_out = char_in;
                }
                break;
            }
            /* need to output anything? */
            if (char_out >= 0)
            {
                *out_ptr = char_out;
                ++out_ptr;
            }
        }
        /* make sure the final argument is terminated */
        *out_ptr = 0;
        /* finalize the last argument */
        if (state != SS_SPACE && argc < argv_size - 1)
        {
            argv[argc++] = next_arg_start;
        }
        /* add a NULL at the end of argv */
        argv[argc] = NULL;

        return argc;
    }

}  // namespace protocol

}  // namespace bs

#endif  //BS_ARGS_H
