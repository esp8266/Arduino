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

    size_t read_line(char* dest, size_t dest_size)
    {
        char* res = fgets(dest, dest_size, stdin);
        if (res == NULL) {
            return 0;
        }
        size_t len = strlen(dest);
        if (dest[len - 1] == '\n') {
            dest[len - 1] = 0;
            len--;
        }
        return len;
    }
};

typedef StdIOHelper IOHelper;

inline void fatal() {
    throw std::runtime_error("fatal error");
}

} // namespace bs

#endif //BS_STDIO_H
