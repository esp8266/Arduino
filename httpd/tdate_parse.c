/*
 *  Copyright(C) 2007 Cameron Rich
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "axhttp.h"

struct day_mon_map 
{
    const char* s;
    uint8_t l;
};

static struct day_mon_map wday_tab[] = 
{
    { "Sun", 0 }, { "Mon", 1 }, { "Tue", 2 }, { "Wed", 3 },
    { "Thu", 4 }, { "Fri", 5 }, { "Sat", 6 }, 
};

static struct day_mon_map mon_tab[] = 
{
    { "Jan", 0 }, { "Feb", 1 }, { "Mar", 2 }, { "Apr", 3 },
    { "May", 4 }, { "Jun", 5 }, { "Jul", 6 }, { "Aug", 7 },
    { "Sep", 8 }, { "Oct", 9 }, { "Nov", 10 }, { "Dec", 11 },
};

static int day_mon_map_compare(const char *v1, const char *v2)
{
    return strcmp(((struct day_mon_map*)v1)->s, ((struct day_mon_map*)v2)->s);
}

void tdate_init(void)
{
    qsort(wday_tab, sizeof(wday_tab)/sizeof(struct day_mon_map),
            sizeof(struct day_mon_map), 
            (int (*)(const void *, const void *))day_mon_map_compare);
    qsort(mon_tab, sizeof(mon_tab)/sizeof(struct day_mon_map),
            sizeof(struct day_mon_map), 
            (int (*)(const void *, const void *))day_mon_map_compare);
}

static int8_t day_mon_map_search(const char* str, 
                            const struct day_mon_map* tab, int n)
{
    struct day_mon_map *search = bsearch(&str, tab, n,
            sizeof(struct day_mon_map), 
                (int (*)(const void *, const void *))day_mon_map_compare);
    return search ? search->l : -1;
}

time_t tdate_parse(const char* str)
{
    struct tm tm;
    char str_mon[4], str_wday[4];
    int tm_sec, tm_min, tm_hour, tm_mday, tm_year;

    /* Initialize. */
    memset(&tm, 0, sizeof(struct tm));

    /* wdy, DD mth YY HH:MM:SS GMT */
    if ((sscanf(str, "%3[a-zA-Z], %d %3[a-zA-Z] %d %d:%d:%d GMT",
                str_wday, &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
                    &tm_sec) == 7) ||
    /* wdy mth DD HH:MM:SS YY */
        (sscanf(str, "%3[a-zA-Z] %3[a-zA-Z] %d %d:%d:%d %d",
                str_wday, str_mon, &tm_mday, &tm_hour, &tm_min, &tm_sec,
                    &tm_year) == 7))
    {
        int8_t tm_wday = day_mon_map_search(str_wday, wday_tab, 
                        sizeof(wday_tab)/sizeof(struct day_mon_map));
        int8_t tm_mon = day_mon_map_search(str_mon, mon_tab, 
                        sizeof(mon_tab)/sizeof(struct day_mon_map));

        if (tm_wday < 0 || tm_mon < 0)
            return -1;

        tm.tm_wday = tm_wday;
        tm.tm_mon = tm_mon;
        tm.tm_mday = tm_mday;
        tm.tm_hour = tm_hour;
        tm.tm_min = tm_min;
        tm.tm_sec = tm_sec;
        tm.tm_year = tm_year - 1900;
        return mktime(&tm);
    }

    return -1;  /* error */
}
