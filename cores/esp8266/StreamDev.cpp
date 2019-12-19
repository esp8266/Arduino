
#include <Arduino.h>
#include <assert.h>

using esp8266::polledTimeout::oneShotFastMs;
using esp8266::polledTimeout::periodicFastMs;

size_t Stream::to (Print* to,
                   ssize_t len,
                   oneShotFastMs::timeType timeout,
                   int readUntilChar)
{
    if (len == 0)
        return 0;   // avoid timeout for no requested data

    // There are two useful timeout:
    // - read (network, serial, ...)
    // - write (network, serial, ...)
    // However
    // - getTimeout() is for reading only
    // - there is no getOutputTimeout() api
    // So we use getTimeout() for both,
    // (also when inputTimeoutPossible() is false)

    oneShotFastMs timedOut(timeout == oneShotFastMs::neverExpires? getTimeout(): timeout);
    periodicFastMs yieldNow(5); // yield about every 5ms
    size_t written = 0;
    size_t maxLen = std::max((decltype(len))0, len);

#define D 0
#if D
    Serial.printf("#### %dms: pb=%d ruc=%d ml=%d to=%d otp=%d itp=%d avr=%d avp=%d avw=%d\n",
        (int)millis(),
        (int)peekBufferAPI(),
        (int)readUntilChar,
        (int)maxLen,
        (int)timedOut.getTimeout(),
        (int)inputTimeoutPossible(),
        (int)outputTimeoutPossible(),
        (int)available(),
        (int)availableForPeek(),
        (int)to->availableForWrite());
#endif

    if (peekBufferAPI())

        // peek-buffer API

        while (!maxLen || written < maxLen)
        {
            size_t avpk = availableForPeek();
            if (avpk == 0 && !inputTimeoutPossible())
                // no more data to read, ever
                break;

            size_t w = to->availableForWrite();
            if (w == 0 && !outputTimeoutPossible())
                // no more data can be written, ever
                break;

            w = std::min(w, avpk);
            if (maxLen)
                w = std::min(w, maxLen - written);
            if (w)
            {
                const char* directbuf = peekBuffer();
                bool foundChar = false;
                if (readUntilChar >= 0)
                {
                    const char* last = (const char*)memchr(directbuf, readUntilChar, w);
                    if (last)
                    {
                        w = std::min((size_t)(last - directbuf + 1), w);
                        foundChar = true;
                    }
                }
                if (w && ((w = to->write(directbuf, w))))
                {
                    peekConsume(w);
                    written += w;
                    timedOut.reset();
                    if (foundChar)
                        break;
                }
            }
            else if (timedOut)
                break;
            if (yieldNow)
                yield();
        }

    else if (readUntilChar >= 0)

        // regular Stream API
        // no other choice than reading byte by byte

        while (!maxLen || written < maxLen)
        {
            size_t avpk = availableForPeek();
            if (avpk == 0 && !inputTimeoutPossible())
                // no more data to read, ever
                break;

            size_t w = to->availableForWrite();
            if (w == 0 && !outputTimeoutPossible())
                // no more data can be written, ever
                break;

            int c = read();
            if (c != -1)
            {
                w = to->write(c);
                if (c == readUntilChar)
                    break;
                assert(w);
                written += 1;
                timedOut.reset();
            }
            else if (timedOut)
                break;
            if (yieldNow)
                yield();
        }

    else

        // regular Stream API
        // use an intermediary buffer

        while (!maxLen || written < maxLen)
        {
            size_t avr = available();
            if (avr == 0 && !inputTimeoutPossible())
                // no more data to read, ever
                break;

            size_t w = to->availableForWrite();
            if (w == 0 && !to->outputTimeoutPossible())
                // no more data can be written, ever
                break;
            w = std::min(w, avr);
            w = std::min(w, (decltype(w))64);
            if (w)
            {
                char temp[w];
                ssize_t r = readNow(temp, w);
                if (r < 0)
                {
                    // do something?
                    break;
                }
                if ((size_t)r < w)
                {
                    Serial.printf(":to read(=%zd)<available(=%zd) stream=%p\n", r, w, this);
                    break;
                }
                w = to->write(temp, r);
                written += w;
                assert((size_t)r == w);
                if (w)
                    timedOut.reset();
            }
            if (timedOut)
                break;
            if (yieldNow)
                yield();
        }

#if D
    Serial.printf("#### %dms: transf=%d\n", (int)millis(), (int)written);
    if (timedOut)
        Serial.printf("#### Timeout!\n");
#endif

    return written;
}
