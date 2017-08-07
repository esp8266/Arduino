
// this is from lwip1.4-espressif's sntp.c which was a patched version of a formet lwip's sntp
// patch is extracted for the new lwip2's sntp

#include "lwip/apps/sntp.h"
#include "osapi.h"
#include "os_type.h"

static uint32 realtime_stamp = 0;
static sint8 time_zone = 8;
LOCAL os_timer_t sntp_timer;
/*****************************************/
#define SECSPERMIN	60L
#define MINSPERHOUR	60L
#define HOURSPERDAY	24L
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY	(SECSPERHOUR * HOURSPERDAY)
#define DAYSPERWEEK	7
#define MONSPERYEAR	12

#define YEAR_BASE	1900
#define EPOCH_YEAR      1970
#define EPOCH_WDAY      4
#define EPOCH_YEARS_SINCE_LEAP 2
#define EPOCH_YEARS_SINCE_CENTURY 70
#define EPOCH_YEARS_SINCE_LEAP_CENTURY 370

#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

int __tznorth;
int __tzyear;
char reult[100];
static const int mon_lengths[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
} ;

static const int year_lengths[2] = {
  365,
  366
} ;
struct tm
{
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_mday;
  int	tm_mon;
  int	tm_year;
  int	tm_wday;
  int	tm_yday;
  int	tm_isdst;
};

struct tm res_buf;
typedef struct __tzrule_struct
{
  char ch;
  int m;
  int n;
  int d;
  int s;
  time_t change;
  int offset;
} __tzrule_type;

__tzrule_type sntp__tzrule[2];
struct tm *
sntp_mktm_r(const time_t * tim_p ,struct tm *res ,int is_gmtime)
{
  long days, rem;
  time_t lcltime;
  int y;
  int yleap;
  const int *ip;

  /* base decision about std/dst time on current time */
  lcltime = *tim_p;

  days = ((long)lcltime) / SECSPERDAY;
  rem = ((long)lcltime) % SECSPERDAY;
  while (rem < 0)
    {
      rem += SECSPERDAY;
      --days;
    }
  while (rem >= SECSPERDAY)
    {
      rem -= SECSPERDAY;
      ++days;
    }

  /* compute hour, min, and sec */
  res->tm_hour = (int) (rem / SECSPERHOUR);
  rem %= SECSPERHOUR;
  res->tm_min = (int) (rem / SECSPERMIN);
  res->tm_sec = (int) (rem % SECSPERMIN);

  /* compute day of week */
  if ((res->tm_wday = ((EPOCH_WDAY + days) % DAYSPERWEEK)) < 0)
    res->tm_wday += DAYSPERWEEK;

  /* compute year & day of year */
  y = EPOCH_YEAR;
  if (days >= 0)
    {
      for (;;)
	{
	  yleap = isleap(y);
	  if (days < year_lengths[yleap])
	    break;
	  y++;
	  days -= year_lengths[yleap];
	}
    }
  else
    {
      do
	{
	  --y;
	  yleap = isleap(y);
	  days += year_lengths[yleap];
	} while (days < 0);
    }

  res->tm_year = y - YEAR_BASE;
  res->tm_yday = days;
  ip = mon_lengths[yleap];
  for (res->tm_mon = 0; days >= ip[res->tm_mon]; ++res->tm_mon)
    days -= ip[res->tm_mon];
  res->tm_mday = days + 1;

  if (!is_gmtime)
    {
      int offset;
      int hours, mins, secs;

//      TZ_LOCK;
//      if (_daylight)
//	{
//	  if (y == __tzyear || __tzcalc_limits (y))
//	    res->tm_isdst = (__tznorth
//			     ? (*tim_p >= __tzrule[0].change && *tim_p < __tzrule[1].change)
//			     : (*tim_p >= __tzrule[0].change || *tim_p < __tzrule[1].change));
//	  else
//	    res->tm_isdst = -1;
//	}
//      else
      res->tm_isdst = -1;

      offset = (res->tm_isdst == 1 ? sntp__tzrule[1].offset : sntp__tzrule[0].offset);

      hours = offset / SECSPERHOUR;
      offset = offset % SECSPERHOUR;

      mins = offset / SECSPERMIN;
      secs = offset % SECSPERMIN;

      res->tm_sec -= secs;
      res->tm_min -= mins;
      res->tm_hour -= hours;

      if (res->tm_sec >= SECSPERMIN)
	{
	  res->tm_min += 1;
	  res->tm_sec -= SECSPERMIN;
	}
      else if (res->tm_sec < 0)
	{
	  res->tm_min -= 1;
	  res->tm_sec += SECSPERMIN;
	}
      if (res->tm_min >= MINSPERHOUR)
	{
	  res->tm_hour += 1;
	  res->tm_min -= MINSPERHOUR;
	}
      else if (res->tm_min < 0)
	{
	  res->tm_hour -= 1;
	  res->tm_min += MINSPERHOUR;
	}
      if (res->tm_hour >= HOURSPERDAY)
	{
	  ++res->tm_yday;
	  ++res->tm_wday;
	  if (res->tm_wday > 6)
	    res->tm_wday = 0;
	  ++res->tm_mday;
	  res->tm_hour -= HOURSPERDAY;
	  if (res->tm_mday > ip[res->tm_mon])
	    {
	      res->tm_mday -= ip[res->tm_mon];
	      res->tm_mon += 1;
	      if (res->tm_mon == 12)
		{
		  res->tm_mon = 0;
		  res->tm_year += 1;
		  res->tm_yday = 0;
		}
	    }
	}
       else if (res->tm_hour < 0)
	{
	  res->tm_yday -= 1;
	  res->tm_wday -= 1;
	  if (res->tm_wday < 0)
	    res->tm_wday = 6;
	  res->tm_mday -= 1;
	  res->tm_hour += 24;
	  if (res->tm_mday == 0)
	    {
	      res->tm_mon -= 1;
	      if (res->tm_mon < 0)
		{
		  res->tm_mon = 11;
		  res->tm_year -= 1;
		  res->tm_yday = 365 + isleap(res->tm_year);
		}
	      res->tm_mday = ip[res->tm_mon];
	    }
	}
//      TZ_UNLOCK;
    }
  else
    res->tm_isdst = 0;
//  os_printf("res %d %d %d %d %d\n",res->tm_year,res->tm_mon,res->tm_mday,res->tm_yday,res->tm_hour);
  return (res);
}
struct tm *
sntp_localtime_r(const time_t * tim_p ,
		struct tm *res)
{
  return sntp_mktm_r (tim_p, res, 0);
}

struct tm *
sntp_localtime(const time_t * tim_p)
{
  return sntp_localtime_r (tim_p, &res_buf);
}


int
sntp__tzcalc_limits(int year)
{
  int days, year_days, years;
  int i, j;

  if (year < EPOCH_YEAR)
    return 0;

  __tzyear = year;

  years = (year - EPOCH_YEAR);

  year_days = years * 365 +
    (years - 1 + EPOCH_YEARS_SINCE_LEAP) / 4 - (years - 1 + EPOCH_YEARS_SINCE_CENTURY) / 100 +
    (years - 1 + EPOCH_YEARS_SINCE_LEAP_CENTURY) / 400;

  for (i = 0; i < 2; ++i)
    {
      if (sntp__tzrule[i].ch == 'J')
	days = year_days + sntp__tzrule[i].d + (isleap(year) && sntp__tzrule[i].d >= 60);
      else if (sntp__tzrule[i].ch == 'D')
	days = year_days + sntp__tzrule[i].d;
      else
	{
	  int yleap = isleap(year);
	  int m_day, m_wday, wday_diff;
	  const int *ip = mon_lengths[yleap];

	  days = year_days;

	  for (j = 1; j < sntp__tzrule[i].m; ++j)
	    days += ip[j-1];

	  m_wday = (EPOCH_WDAY + days) % DAYSPERWEEK;

	  wday_diff = sntp__tzrule[i].d - m_wday;
	  if (wday_diff < 0)
	    wday_diff += DAYSPERWEEK;
	  m_day = (sntp__tzrule[i].n - 1) * DAYSPERWEEK + wday_diff;

	  while (m_day >= ip[j-1])
	    m_day -= DAYSPERWEEK;

	  days += m_day;
	}

      /* store the change-over time in GMT form by adding offset */
      sntp__tzrule[i].change = days * SECSPERDAY + sntp__tzrule[i].s + sntp__tzrule[i].offset;
    }

  __tznorth = (sntp__tzrule[0].change < sntp__tzrule[1].change);

  return 1;
}

char *
sntp_asctime_r(struct tm *tim_p ,char *result)
{
  static const char day_name[7][4] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static const char mon_name[12][4] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  os_sprintf (result, "%s %s %02d %02d:%02d:%02d %02d\n",
	   day_name[tim_p->tm_wday],
	   mon_name[tim_p->tm_mon],
	   tim_p->tm_mday, tim_p->tm_hour, tim_p->tm_min,
	   tim_p->tm_sec, 1900 + tim_p->tm_year);
  return result;
}
char *
sntp_asctime(struct tm *tim_p)
{

    return sntp_asctime_r (tim_p, reult);
}

uint32 sntp_get_current_timestamp()
{
#if 0
	if(realtime_stamp == 0){
		os_printf("please start sntp first !\n");
		return 0;
	} else
#endif
		return realtime_stamp;
}

char* sntp_get_real_time(time_t t)
{
	return sntp_asctime(sntp_localtime (&t));
}
/**
 * SNTP get time_zone default GMT + 8
 */
sint8
sntp_get_timezone(void)
{
	return time_zone;
}
/**
 * SNTP set time_zone default GMT + 8
 */

bool
sntp_set_timezone(sint8 timezone)
{
	if(timezone >= -11 || timezone <= 13) {
		time_zone = timezone;
		return true;
	} else {
		return false;
	}

}

void sntp_set_daylight(int daylight)
{
#if 0
	if (sntp_get_timetype()) // always false in espressif's lwip1.4
		RTC_DST_SET(daylight);
#else
	(void)daylight;
#endif
}

void sntp_time_inc (void)
{
	realtime_stamp++;
}

void sntp_set_system_time (uint32_t t)
{
	realtime_stamp = t;
	os_timer_disarm(&sntp_timer);
	os_timer_setfn(&sntp_timer, (os_timer_func_t *)sntp_time_inc, NULL);
	os_timer_arm(&sntp_timer, 1000, 1);
}
