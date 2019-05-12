/*
 * time.h
 *
 *  Created on: May 31, 2016
 *      Author: liuhan
 */

#ifndef TIME_H_
#define TIME_H_
#include "osapi.h"
#include "os_type.h"
#include "lwip/sntp.h"

struct timeval {
  unsigned long  tv_sec;         /* seconds */
  unsigned long  tv_usec;        /* and microseconds */
};

/***************************RTC TIME OPTION***************************************/
// daylight settings
// Base calculated with value obtained from NTP server (64 bits)
#define sntp_base   (*((uint64_t*)RTC_STORE0))
// Timer value when base was obtained
#define TIM_REF_SET(value) WRITE_PERI_REG(RTC_STORE2, value)
#define TIM_REF_GET()      READ_PERI_REG(RTC_STORE2)

// Setters and getters for CAL, TZ and DST.
#define RTC_CAL_SET(val)    do {uint32 value = READ_PERI_REG(RTC_STORE3);\
    value |= ((val) & 0x0000FFFF);\
    WRITE_PERI_REG(RTC_STORE3, value);\
    }while(0)
#define RTC_DST_SET(val)    do {uint32 value = READ_PERI_REG(RTC_STORE3);\
    value |= (((val)<<16) & 0x00010000);\
    WRITE_PERI_REG(RTC_STORE3, value);\
    }while(0)
#define RTC_TZ_SET(val)     do {uint32 value = READ_PERI_REG(RTC_STORE3);\
    value |= (((val)<<24) & 0xFF000000);\
    WRITE_PERI_REG(RTC_STORE3, value);\
    }while(0)

#define RTC_CAL_GET()       (READ_PERI_REG(RTC_STORE3) & 0x0000FFFF)
#define RTC_DST_GET()       ((READ_PERI_REG(RTC_STORE3) & 0x00010000)>>16)
#define RTC_TZ_GET()        ((((int)READ_PERI_REG(RTC_STORE3)) & ((int)0xFF000000))>>24)
void system_update_rtc(time_t t, uint32_t us);
time_t sntp_get_rtc_time(sint32_t *us);

int  gettimeofday(struct timeval* t, void* timezone);
void updateTime(uint32 ms);
bool configTime(int timezone, int daylightOffset, char *server1, char *server2, char *server3, bool enable);
time_t time(time_t *t);
unsigned long millis(void);
unsigned long micros(void);
#endif /* TIME_H_ */
