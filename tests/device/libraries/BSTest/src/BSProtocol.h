#ifndef BS_PROTOCOL_H
#define BS_PROTOCOL_H

#include "BSArgs.h"

#define BS_LINE_PREFIX ">>>>>bs_test_"

extern bool pretest();

namespace bs
{
namespace protocol
{
    template<typename IO>
    void output_test_start(IO& io, const char* file, size_t line, const char* name,
                           const char* desc)
    {
        io.printf(BS_LINE_PREFIX "start file=\"%s\" line=%d name=\"%s\" desc=\"%s\"\n", file, line,
                  name, desc);
    }

    template<typename IO>
    void output_check_failure(IO& io, size_t line)
    {
        io.printf(BS_LINE_PREFIX "check_failure line=%d\n", line);
    }

    template<typename IO>
    void output_test_end(IO& io, bool success, size_t checks, size_t failed_checks, size_t line = 0)
    {
        io.printf(BS_LINE_PREFIX "end line=%d result=%d checks=%d failed_checks=%d\n", line,
                  success, checks, failed_checks);
    }

    template<typename IO>
    void output_menu_begin(IO& io)
    {
        io.printf(BS_LINE_PREFIX "menu_begin\n");
    }

    template<typename IO>
    void output_menu_item(IO& io, int index, const char* name, const char* desc)
    {
        io.printf(BS_LINE_PREFIX "item id=%d name=\"%s\" desc=\"%s\"\n", index, name, desc);
    }

    template<typename IO>
    void output_menu_end(IO& io)
    {
        io.printf(BS_LINE_PREFIX "menu_end\n");
    }

    template<typename IO>
    void output_setenv_result(IO& io, const char* key, const char* value)
    {
        io.printf(BS_LINE_PREFIX "setenv key=\"%s\" value=\"%s\"\n", key, value);
    }

    template<typename IO>
    void output_getenv_result(IO& io, const char* key, const char* value)
    {
        (void)key;
        io.printf(BS_LINE_PREFIX "getenv value=\"%s\"\n", value);
    }

    template<typename IO>
    void output_pretest_result(IO& io, bool res)
    {
        io.printf(BS_LINE_PREFIX "pretest result=%d\n", res ? 1 : 0);
    }

    template<typename IO>
    bool input_handle(IO& io, char* line_buf, size_t line_buf_size, int& test_num)
    {
        int cb_read = io.read_line(line_buf, line_buf_size);
        if (cb_read == 0 || line_buf[0] == '\n')
        {
            return false;
        }
        char*  argv[4];
        size_t argc = split_args(line_buf, argv, sizeof(argv) / sizeof(argv[0]));
        if (argc == 0)
        {
            return false;
        }
        if (strcmp(argv[0], "setenv") == 0)
        {
            if (argc != 3)
            {
                return false;
            }
            setenv(argv[1], argv[2], 1);
            output_setenv_result(io, argv[1], argv[2]);
            test_num = -1;
            return false; /* we didn't get the test number yet, so return false */
        }
        if (strcmp(argv[0], "getenv") == 0)
        {
            if (argc != 2)
            {
                return false;
            }
            const char* value = getenv(argv[1]);
            output_getenv_result(io, argv[1], (value != NULL) ? value : "");
            return false;
        }
        if (strcmp(argv[0], "pretest") == 0)
        {
            if (argc != 1)
            {
                return false;
            }
            bool res = ::pretest();
            output_pretest_result(io, res);
            return false;
        }
        /* not one of the commands, try to parse as test number */
        char* endptr;
        test_num = (int)strtol(argv[0], &endptr, 10);
        if (endptr != argv[0] + strlen(argv[0]))
        {
            return false;
        }
        return true;
    }

}  // namespace protocol
}  // namespace bs

#endif  // BS_PROTOCOL_H
