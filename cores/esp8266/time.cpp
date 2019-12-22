/*
 * time.c - ESP8266-specific functions for SNTP
 * Copyright (c) 2015 Peter Dobler. All rights reserved.
 * This file is part of the esp8266 core for Arduino environment.
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <stdlib.h>
#include <../include/time.h> // See issue #6714
#include <sys/time.h>
#include <sys/reent.h>
#include "sntp.h"
#include "coredecls.h"

#include "sntp-lwip2.h"

extern "C" {

#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
struct timeval {
  time_t      tv_sec;
  suseconds_t tv_usec;
};
#endif

extern char* sntp_asctime(const struct tm *t);
extern struct tm* sntp_localtime(const time_t *clock);
extern uint64_t micros64();
extern int sntp_get_daylight();
extern void sntp_set_daylight(int daylight);

// time gap in seconds from 01.01.1900 (NTP time) to 01.01.1970 (UNIX time)
#define DIFF1900TO1970 2208988800UL

bool timeshift64_is_set = false;
static uint64_t timeshift64 = 0;

void tune_timeshift64 (uint64_t now_us)
{
     timeshift64 = now_us - micros64();
     timeshift64_is_set = true;
}

static void setServer(int id, const char* name_or_ip)
{
    if (name_or_ip)
    {
        // per current configuration,
        // lwIP can receive an IP address or a fqdn
        sntp_setservername(id, (char*) name_or_ip);
    }
}

int clock_gettime(clockid_t unused, struct timespec *tp)
{
    (void) unused;
    uint64_t m = micros64();
    tp->tv_sec = m / 1000000;
    tp->tv_nsec = (m % 1000000) * 1000;
    return 0;
}

time_t time(time_t * t)
{
    time_t seconds = sntp_get_current_timestamp();
    if (t)
    {
        *t = seconds;
    }
    return seconds;
}

int _gettimeofday_r(struct _reent* unused, struct timeval *tp, void *tzp)
{
    (void) unused;
    (void) tzp;
    if (tp)
    {
        if (!timeshift64_is_set)
            tune_timeshift64(sntp_get_current_timestamp() * 1000000ULL);
        uint64_t currentTime_us = timeshift64 + micros64();
        tp->tv_sec = currentTime_us / 1000000ULL;
        tp->tv_usec = currentTime_us % 1000000ULL;
    }
    return 0;
}

}; // extern "C"

void configTime(int timezone_sec, int daylightOffset_sec, const char* server1, const char* server2, const char* server3)
{
    sntp_stop();

    setServer(0, server1);
    setServer(1, server2);
    setServer(2, server3);

    sntp_set_timezone_in_seconds(timezone_sec);
    sntp_set_daylight(daylightOffset_sec);
    sntp_init();
}

void configTime(const char* tz, const char* server1, const char* server2, const char* server3)
{
    sntp_stop();

    setServer(0, server1);
    setServer(1, server2);
    setServer(2, server3);

    char tzram[strlen_P(tz) + 1];
    memcpy_P(tzram, tz, sizeof(tzram));
    setenv("TZ", tzram, 1/*overwrite*/);
    sntp_set_timezone_in_seconds(0);
    tzset();

    sntp_init();
}

void setDST(int daylightOffset_sec)
{
    sntp_stop();
    sntp_set_daylight(daylightOffset_sec);
    sntp_init();
}

int getDST()
{
    return sntp_get_daylight();
}
