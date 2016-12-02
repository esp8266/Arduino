#ifndef BS_STDIO_H
#define BS_STDIO_H

#include <stdio.h>
#include <exception>

namespace bs
{
class StdIOHelper
{
public:
    StdIOHelper()
    {
    }

    size_t printf(const char *format, ...)
    {
        va_list arg;
        va_start(arg, format);
        size_t result = vprintf(format, arg);
        va_end(arg);
        return result;
    }

    bool read_int(int& result)
    {
        return scanf("%d", &result) == 1;
    }
};

typedef StdIOHelper IOHelper;

inline void fatal() {
    throw std::runtime_error("fatal error");
}

} // namespace bs

#endif //BS_STDIO_H
