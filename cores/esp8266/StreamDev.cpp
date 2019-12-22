
#include <Arduino.h>
#include <assert.h>

using esp8266::polledTimeout::oneShotFastMs;
using esp8266::polledTimeout::periodicFastMs;

size_t Stream::to (Print* to,
                   const ssize_t len,
                   oneShotFastMs::timeType timeout,
                   int readUntilChar)
{
    _last_to = TOSTREAM_SUCCESS;

    if (len == 0)
        return 0;   // avoid timeout for no requested data

    // There are two timeouts:
    // - read (network, serial, ...)
    // - write (network, serial, ...)
    // However
    // - getTimeout() is for reading only
    // - there is no getOutputTimeout() api
    // So we use getTimeout() for both,
    // (also when inputTimeoutPossible() is false)

    // "neverExpires (default, impossible)" is translated to default timeout
    oneShotFastMs timedOut(timeout == oneShotFastMs::neverExpires? getTimeout(): timeout);

    // yield about every 5ms (XXX SHOULD BE A SYSTEM-WIDE CONSTANT?)
    periodicFastMs yieldNow(5);

    size_t written = 0;

    // len==-1 => maxLen=0 <=> until starvation
    size_t maxLen = std::max((decltype(len))0, len);

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
                if (w != 1)
                {
                    _last_to = TOSTREAM_WRITE_ERROR;
                    break;
                }
                written += 1;
                timedOut.reset();
                if (c == readUntilChar)
                    break;
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
            w = std::min(w, (decltype(w))64); //XXX FIXME 64 is a constant
            if (w)
            {
                char temp[w];
                ssize_t r = readNow(temp, w);
                if (r < 0)
                {
                    _last_to = TOSTREAM_READ_ERROR;
                    break;
                }
                if ((size_t)r < w)
                {
                    Serial.printf(":to read(=%zd)<available(=%zd) stream=%p\n", r, w, this);
                    break;
                }
                w = to->write(temp, r);
                written += w;
                if ((size_t)r != w)
                {
                    _last_to = TOSTREAM_WRITE_ERROR;
                    break;
                }
                if (w)
                    timedOut.reset();
            }
            if (timedOut)
                break;
            if (yieldNow)
                yield();
        }

    if (_last_to == TOSTREAM_SUCCESS)
    {
        if (timedOut)
            _last_to = TOSTREAM_TIMED_OUT;
        else if (len > 0 && written != len)
            _last_to = TOSTREAM_SHORT;
    }
    return written;
}
