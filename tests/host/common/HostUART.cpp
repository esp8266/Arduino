/*
 HostUART.cpp - esp8266 UART HAL bound to host UART

 Copyright (c) 2021 David Gauchard. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

// uses libserialport https://sigrok.org/api/libserialport

#include <libserialport.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct sp_port*       hostPort;
static constexpr auto timeout_ms   = 1000;
int                   hostBaudrate = 0;

static int spcheck(const char* what, enum sp_return result)
{
    /* For this example we'll just exit on any error by calling abort(). */
    char* error_message;

    switch (result)
    {
    case SP_ERR_ARG:
        fprintf(stderr, "libserialport: %s: Invalid argument.\n", what);
        abort();
    case SP_ERR_FAIL:
        error_message = sp_last_error_message();
        fprintf(stderr, "libserialport: %s: Failed: %s\n", what, error_message);
        sp_free_error_message(error_message);
        abort();
    case SP_ERR_SUPP:
        fprintf(stderr, "libserialport: %s: Not supported.\n", what);
        abort();
    case SP_ERR_MEM:
        fprintf(stderr, "libserialport: %s: Couldn't allocate memory.\n", what);
        abort();
    case SP_OK:
    default:
        return result;
    }
}

void hostsp_open(const char* port, int baud, int bits, int parity, int stop, int control)
{
    static_assert(SP_PARITY_NONE == 0);       // 0 = default for parity
    static_assert(SP_FLOWCONTROL_NONE == 0);  // 0 = default for control

    spcheck("get port", sp_get_port_by_name(port, &hostPort));
    spcheck("open r/w", sp_open(hostPort, SP_MODE_READ_WRITE));
    spcheck("set baudrate", sp_set_baudrate(hostPort, baud));
    spcheck("set bits", sp_set_bits(hostPort, bits));
    spcheck("set parity", sp_set_parity(hostPort, (sp_parity)parity));
    spcheck("set stopbit", sp_set_stopbits(hostPort, stop));
    spcheck("set flowcontrol", sp_set_flowcontrol(hostPort, (sp_flowcontrol)control));
}

size_t hostsp_write(char c)
{
    spcheck("write(1)", sp_blocking_write(hostPort, &c, 1, timeout_ms));
    return 1;
}

size_t hostsp_write(const void* data, size_t len)
{
    spcheck("write(buf)", sp_blocking_write(hostPort, data, len, timeout_ms));
    return len;
}

char hostsp_read()
{
    char c;
    spcheck("read(1)", sp_blocking_read(hostPort, &c, 1, timeout_ms));
    return c;
}

size_t hostsp_read(void* data, size_t len)
{
    spcheck("read(len)", sp_blocking_read(hostPort, data, len, timeout_ms));
    return len;
}

size_t hostsp_availableForWrite()
{
    ssize_t ret = sp_output_waiting(hostPort);
    if (ret < 0)
        spcheck("available()", (sp_return)ret);
    return 128 - ret;
}

size_t hostsp_available()  // for read
{
    ssize_t ret = sp_input_waiting(hostPort);
    if (ret < 0)
        spcheck("available()", (sp_return)ret);
    return ret;
}
