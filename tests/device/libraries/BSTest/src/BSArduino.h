#ifndef BS_ARDUINO_H
#define BS_ARDUINO_H

#include <Arduino.h>
namespace bs
{
class ArduinoIOHelper
{
public:
    ArduinoIOHelper(Stream& stream) : m_stream(stream)
    {
    }

    size_t printf(const char *format, ...)
    {
        va_list arg;
        va_start(arg, format);
        char temp[128];
        char* buffer = temp;
        size_t len = vsnprintf(temp, sizeof(temp), format, arg);
        va_end(arg);
        if (len > sizeof(temp) - 1) {
            buffer = new char[len + 1];
            if (!buffer) {
                return 0;
            }
            va_start(arg, format);
            ets_vsnprintf(buffer, len + 1, format, arg);
            va_end(arg);
        }
        len = m_stream.write((const uint8_t*) buffer, len);
        if (buffer != temp) {
            delete[] buffer;
        }
        return len;
    }

    bool read_int(int& result)
    {
        // TODO: fix this for 0 value
        result = m_stream.parseInt();
        return result != 0;
    }

protected:
    Stream& m_stream;
};

typedef ArduinoIOHelper IOHelper;

inline void fatal() {
    ESP.restart();
}

} // namespace bs

#endif //BS_ARDUINO_H
