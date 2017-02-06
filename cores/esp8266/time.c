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

#include <time.h>
#include <sys/reent.h>
#include "sntp.h"


#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
struct timeval {
  time_t      tv_sec;
  suseconds_t tv_usec;
};
#endif

extern char* sntp_asctime(const struct tm *t);
extern struct tm* sntp_localtime(const time_t *clock);

// time gap in seconds from 01.01.1900 (NTP time) to 01.01.1970 (UNIX time)
#define DIFF1900TO1970 2208988800UL

static int s_daylightOffset_sec = 0;
static long s_timezone_sec = 0;
static time_t s_bootTime = 0;

// calculate offset used in gettimeofday
static void ensureBootTimeIsSet()
{
    if (!s_bootTime)
    {
        time_t now = sntp_get_current_timestamp();
        if (now)
        {
            s_bootTime =  now - millis() / 1000;
        }
    }
}

static void setServer(int id, const char* name_or_ip)
{
    if (name_or_ip)
    {
        //TODO: check whether server is given by name or IP
        sntp_setservername(id, (char*) name_or_ip);
    }
}

void configTime(int timezone, int daylightOffset_sec, const char* server1, const char* server2, const char* server3)
{
    sntp_stop();

    setServer(0, server1);
    setServer(1, server2);
    setServer(2, server3);

    s_timezone_sec = timezone;
    s_daylightOffset_sec = daylightOffset_sec;
    sntp_set_timezone(timezone/3600);
    sntp_init();
}

int clock_gettime(clockid_t unused, struct timespec *tp)
{
    (void) unused;
    tp->tv_sec  = millis() / 1000;
    tp->tv_nsec = micros() * 1000;
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
        ensureBootTimeIsSet();
        tp->tv_sec  = s_bootTime + millis() / 1000;
        tp->tv_usec = micros() * 1000;
    }
    return 0;
}
