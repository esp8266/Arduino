#ifndef BS_ARDUINO_H
#define BS_ARDUINO_H

#include <Arduino.h>
namespace bs
{
class ArduinoIOHelper
{
public:
    ArduinoIOHelper(Stream& stream) : m_stream(stream) { }

    size_t printf(const char* format, ...)
    {
        va_list arg;
        va_start(arg, format);
        char   temp[128];
        char*  buffer = temp;
        size_t len    = vsnprintf(temp, sizeof(temp), format, arg);
        va_end(arg);
        if (len > sizeof(temp) - 1)
        {
            buffer = new char[len + 1];
            if (!buffer)
            {
                return 0;
            }
            va_start(arg, format);
            ets_vsnprintf(buffer, len + 1, format, arg);
            va_end(arg);
        }
        len = m_stream.write((const uint8_t*)buffer, len);
        if (buffer != temp)
        {
            delete[] buffer;
        }
        return len;
    }

    size_t read_line(char* dest, size_t dest_size)
    {
        size_t len = 0;
        // Can't use Stream::readBytesUntil here because it can't tell the
        // difference between timing out and receiving the terminator.
        while (len < dest_size - 1)
        {
            int c = m_stream.read();
            if (c < 0)
            {
                delay(1);
                continue;
            }
            if (c == '\r')
            {
                continue;
            }
            if (c == '\n')
            {
                dest[len] = 0;
                break;
            }
            dest[len++] = c;
        }
        return len;
    }

protected:
    Stream& m_stream;
};

typedef ArduinoIOHelper IOHelper;

inline void fatal() { ESP.restart(); }

}  // namespace bs

#endif  // BS_ARDUINO_H
