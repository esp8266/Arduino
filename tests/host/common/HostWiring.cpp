/*
    Arduino: wire emulation
    Copyright (c) 2018 david gauchard. All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal with the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimers.

    - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimers in the
    documentation and/or other materials provided with the distribution.

    - The names of its contributors may not be used to endorse or promote
    products derived from this Software without specific prior written
    permission.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
    THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS WITH THE SOFTWARE.
*/

#include <Arduino.h>

#ifdef DEBUG_ESP_CORE
#define VERBOSE(x...) fprintf(stderr, MOCK x)
#else
#define VERBOSE(x...) mockverbose(x)
#endif

void pinMode(uint8_t pin, uint8_t mode)
{
#define xxx(mode)            \
    case mode:               \
        m = STRHELPER(mode); \
        break
    const char* m;
    switch (mode)
    {
        case INPUT:
            m = "INPUT";
            break;
        case OUTPUT:
            m = "OUTPUT";
            break;
        case INPUT_PULLUP:
            m = "INPUT_PULLUP";
            break;
        case OUTPUT_OPEN_DRAIN:
            m = "OUTPUT_OPEN_DRAIN";
            break;
        case INPUT_PULLDOWN_16:
            m = "INPUT_PULLDOWN_16";
            break;
        case WAKEUP_PULLUP:
            m = "WAKEUP_PULLUP";
            break;
        case WAKEUP_PULLDOWN:
            m = "WAKEUP_PULLDOWN";
            break;
        default:
            m = "(special)";
    }
    VERBOSE("gpio%d: mode='%s'\n", pin, m);
}

void digitalWrite(uint8_t pin, uint8_t val)
{
    VERBOSE("digitalWrite(pin=%d val=%d)\n", pin, val);
}

void analogWrite(uint8_t pin, int val)
{
    VERBOSE("analogWrite(pin=%d, val=%d\n", pin, val);
}

int analogRead(uint8_t pin)
{
    (void)pin;
    return 512;
}

void analogWriteRange(uint32_t range)
{
    VERBOSE("analogWriteRange(range=%d)\n", range);
}

int digitalRead(uint8_t pin)
{
    VERBOSE("digitalRead(%d)\n", pin);

    // pin 0 is most likely a low active input
    return pin ? 0 : 1;
}
