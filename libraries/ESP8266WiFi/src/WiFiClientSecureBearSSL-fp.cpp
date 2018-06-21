
#include <WiFiClientSecureBearSSL.h>

namespace BearSSL {

static uint8_t htoi (unsigned char c)
{
    return c >= '0' && c <= '9'? c - '0'
         : c >= 'A' && c <= 'F'? c - 'A' + 10
         : c >= 'a' && c <= 'f'? c - 'a' + 10
         : 255;
}

void WiFiClientSecure::setFingerprint(const String& fingerprint) {

    uint8_t fp [20];
    uint8_t c, d;
    int idx = 0;
    const char* _fingerprint = fingerprint.c_str();

    while (idx < 20 && (c = *_fingerprint++) && (d = *_fingerprint++)) {
        c = htoi(c);
        d = htoi(d);
        if (c > 15 || d > 15) {
            // skip separator
            _fingerprint--;
            continue;
        }
        fp[idx++] = (c << 4) + d;
    }

    if (idx == 20)
        setFingerprint(fp);
}

} // namespace
