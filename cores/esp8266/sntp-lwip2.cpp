/*
 * sntp-lwip2.c - ESP8266-specific functions for SNTP and lwIP-v2
 * Copyright (c) 2015 Espressif (license is tools/sdk/lwip/src/core/sntp.c's)
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 *
 * History:
 * This code is extracted from lwip1.4-espressif's sntp.c
 * which is a patched version of the original lwip1's sntp.
 * (check the mix-up in tools/sdk/lwip/src/core/sntp.c)
 * It is moved here as-is and cleaned for maintainability and
 * because it does not belong to lwip.
 *
 * TODOs:
 * settimeofday(): handle tv->tv_usec
 * sntp_mktm_r(): review, fix DST handling (this one is currently untouched from lwip-1.4)
 * implement adjtime()
 */

#include <lwip/init.h>
#include <time.h>
#include <sys/time.h>
#include <osapi.h>
#include <os_type.h>
#include "coredecls.h"
#include "Schedule.h"

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

#if LWIP_VERSION_MAJOR == 1

#include <pgmspace.h>

static const char stod14[] PROGMEM = "settimeofday() can't set time!\n";
bool sntp_set_timezone(sint8 timezone);
bool sntp_set_timezone_in_seconds(sint32 timezone)
{
    return sntp_set_timezone((sint8)(timezone/(60*60))); //TODO: move this to the same file as sntp_set_timezone() in lwip1.4, and implement correctly over there.	
}
	
void sntp_set_daylight(int daylight);

int settimeofday(const struct timeval* tv, const struct timezone* tz)
{
    if (tz) /*before*/
    {
        sntp_set_timezone_in_seconds(tz->tz_minuteswest * 60);
        // apparently tz->tz_dsttime is a bitfield and should not be further used (cf man)
        sntp_set_daylight(0);
    }
    if (tv) /* after*/
    {
        // can't call lwip1.4's static sntp_set_system_time()
        os_printf(stod14);

        // reset time subsystem
        timeshift64_is_set = false;
        
        return -1;
    }
    return 0;
}

#endif // lwip 1.4 only

#if LWIP_VERSION_MAJOR == 2

#include <lwip/apps/sntp.h>

uint32_t sntp_real_timestamp = 0;
LOCAL os_timer_t sntp_timer;

void ICACHE_RAM_ATTR sntp_time_inc (void)
{
    sntp_real_timestamp++;
}

int settimeofday(const struct timeval* tv, const struct timezone* tz)
{
    (void)tz; // obsolete (cf. man settimeofday)
    if (tv) /*after*/
    {
        // reset time subsystem
        tune_timeshift64(tv->tv_sec * 1000000ULL + tv->tv_usec);

		sntp_real_timestamp = tv->tv_sec;
		os_timer_disarm(&sntp_timer);
		os_timer_setfn(&sntp_timer, (os_timer_func_t *)sntp_time_inc, NULL);
		os_timer_arm(&sntp_timer, 1000, 1);

        if (_settimeofday_cb)
            schedule_recurrent_function_us([](){ _settimeofday_cb(); return false; }, 0);
    }
    return 0;
}

#endif // lwip2 only

};
