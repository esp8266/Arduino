#ifndef BS_PROTOCOL_H
#define BS_PROTOCOL_H

#define BS_LINE_PREFIX ">>>>>bs_test_"

namespace bs
{
namespace protocol
{
template<typename IO>
void output_test_start(IO& io, const char* file, size_t line, const char* name, const char* desc)
{
    io.printf(BS_LINE_PREFIX "start file=\"%s\" line=%d name=\"%s\" desc=\"%s\"\n", file, line, name, desc);
}

template<typename IO>
void output_check_failure(IO& io, size_t line)
{
    io.printf(BS_LINE_PREFIX "check_failure line=%d\n", line);
}

template<typename IO>
void output_test_end(IO& io, bool success, size_t checks, size_t failed_checks, size_t line=0)
{
    io.printf(BS_LINE_PREFIX "end line=%d result=%d checks=%d failed_checks=%d\n", line, success, checks, failed_checks);
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
bool input_menu_choice(IO& io, int& result)
{
    return io.read_int(result);
}

} // ::protocol
} // ::bs

#endif //BS_PROTOCOL_H
