/*
 Arduino Hardware Serial emulation
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
#include <PolledTimeout.h>

#include <unistd.h> // write

HardwareSerial Serial(UART0);

HardwareSerial::HardwareSerial (int uart_nr)
{
	if (uart_nr != 0)
		fprintf(stderr, MOCK "FIXME HardwareSerial::HardwareSerial(%d)\n", uart_nr);
	_uart = (decltype(_uart))1; // not used, for 'while (!Serial);' to pass
}

void HardwareSerial::begin (unsigned long baud, SerialConfig config, SerialMode mode, uint8_t tx_pin)
{
	if (config != SERIAL_8N1 || mode != SERIAL_FULL || tx_pin != 1)
		fprintf(stderr, MOCK "FIXME HardwareSerial::begin(baud=%ld config=0x%x mode=0x%x)\n", baud, (int)config, (int)mode);
}

void HardwareSerial::setDebugOutput (bool on)
{
	(void)on;
}

int HardwareSerial::available (void)
{
	printf(MOCK "TODO HardwareSerial::available\n");
	return 0;
}

void HardwareSerial::flush ()
{
	//XXXTODO
	fflush(stdout);
}

size_t HardwareSerial::readBytes(char* buffer, size_t size)
{
    size_t got = 0;

    while (got < size)
    {
        esp8266::polledTimeout::oneShot timeOut(_timeout);
        size_t avail;
        while ((avail = available()) == 0 && !timeOut);
        if (avail == 0)
            break;
        got += read(buffer + got, std::min(size - got, avail));
    }
    return got;
}

// uart.c

extern "C"
{

size_t uart_write_char (uart_t* uart, char c)
{
	//XXXTODO
	(void)uart;
	return write(1, &c, 1);
}

int uart_peek_char (uart_t* uart)
{
	///XXXTODO
	static bool notimpl = false;
	if (!notimpl)
	{
		notimpl = true;
		fprintf(stderr, MOCK "FIXME uart_peek_char\n");
	}
	(void)uart;
	return -1;
}

int uart_read_char (uart_t* uart)
{
	///XXXTODO
	static bool notimpl = false;
	if (!notimpl)
	{
		notimpl = true;
		fprintf(stderr, MOCK "FIXME uart_read_char\n");
	}
	(void)uart;
	return -1;
}

size_t uart_write (uart_t* uart, const char* buf, size_t size)
{
	///XXXTODO
	(void)uart;
	return write(1, buf, size);
}

size_t uart_read(uart_t* uart, char* userbuffer, size_t usersize)
{
	///XXXTODO
	(void)uart;
	return read(0, userbuffer, usersize);
}

} // extern "C"
