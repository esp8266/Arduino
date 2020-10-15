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
 * reworked for newlib and lwIP-v2:
 * time source is SNTP/settimeofday()
 * system time is micros64() / NONOS-SDK's system_get_time()
 * synchronisation of the two through timeshift64
 */

#include <stdlib.h>
#include <../include/time.h> // See issue #6714
#include <sys/time.h>
#include <sys/reent.h>
#include <errno.h>

#include <sntp.h> // nonos-sdk
#include <coredecls.h>
#include <Schedule.h>

#include <Arduino.h> // configTime()

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
extern void sntp_set_daylight(int daylight);

static uint64_t timeshift64 = 0;

void tune_timeshift64 (uint64_t now_us)
{
     timeshift64 = now_us - micros64();
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

///////////////////////////////////////////
// backport legacy nonos-sdk Espressif api

bool sntp_set_timezone_in_seconds (int32_t timezone_sec)
{
    configTime(timezone_sec, 0, sntp_getservername(0), sntp_getservername(1), sntp_getservername(2));
    return true;
}

bool sntp_set_timezone(sint8 timezone_in_hours)
{
    return sntp_set_timezone_in_seconds(3600 * ((int)timezone_in_hours));
}

char* sntp_get_real_time(time_t t)
{
    return ctime(&t);
}

uint32 sntp_get_current_timestamp()
{
    return time(nullptr);
}

// backport legacy nonos-sdk Espressif api
///////////////////////////////////////////

time_t time(time_t * t)
{
    time_t currentTime_s = (micros64() + timeshift64) / 1000000ULL;
    if (t)
    {
        *t = currentTime_s;
    }
    return currentTime_s;
}

int _gettimeofday_r(struct _reent* unused, struct timeval *tp, void *tzp)
{
    (void) unused;
    (void) tzp;
    if (tp)
    {
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

    // There is no way to tell when DST starts or stop with this API
    // So DST is always integrated in TZ
    // The other API should be preferred

    /*** portable version using posix API
         (calls sprintf here, then sscanf internally)

    int tzs = daylightOffset_sec + timezone_sec;
    int tzh = tzs / 3600;
    tzs -= tzh * 3600;
    int tzm = tzs / 60;
    tzs -= tzm * 60;

    // man tzset:
    char tzstr [64];
    snprintf(tzstr, sizeof tzstr, "ESPUSER<%+d:%02d:%02d>", tzh, tzm, tzs);
    return configTime(tzstr, server1, server2, server3);

    Replaced by light code found from
    newlib inspection and internal structure hacking
    (no sprintf, no sscanf, -7584 flash bytes):

    *** hack starts here: ***/

    static char gmt[] = "GMT";

    _timezone = timezone_sec + daylightOffset_sec;
    _daylight = 0;
    _tzname[0] = gmt;
    _tzname[1] = gmt;
    auto tz = __gettzinfo();
    tz->__tznorth = 1;
    tz->__tzyear = 0;
    for (int i = 0; i < 2; i++)
    {
        auto tzr = &tz->__tzrule[i];
        tzr->ch = 74;
        tzr->m = 0;
        tzr->n = 0;
        tzr->d = 0;
        tzr->s = 0;
        tzr->change = 0;
        tzr->offset = -_timezone;
    }

    /*** end of hack ***/

    // sntp servers
    setServer(0, server1);
    setServer(1, server2);
    setServer(2, server3);

    sntp_init();
}

void setTZ(const char* tz){
	
    char tzram[strlen_P(tz) + 1];
    memcpy_P(tzram, tz, sizeof(tzram));
    setenv("TZ", tzram, 1/*overwrite*/);
    tzset();
}

void configTime(const char* tz, const char* server1, const char* server2, const char* server3)
{
    sntp_stop();

    setServer(0, server1);
    setServer(1, server2);
    setServer(2, server3);
	setTZ(tz);
	
    sntp_init();
}

static TrivialCB _settimeofday_cb;

void settimeofday_cb (TrivialCB&& cb)
{
    _settimeofday_cb = std::move(cb);
}

void settimeofday_cb (const TrivialCB& cb)
{
    _settimeofday_cb = cb;
}

extern "C" {

#include <lwip/apps/sntp.h>

int settimeofday(const struct timeval* tv, const struct timezone* tz)
{
    if (tz || !tv)
        // tz is obsolete (cf. man settimeofday)
        return EINVAL;

    // reset time subsystem
    tune_timeshift64(tv->tv_sec * 1000000ULL + tv->tv_usec);

    if (_settimeofday_cb)
        schedule_recurrent_function_us([](){ _settimeofday_cb(); return false; }, 0);

    return 0;
}

};
