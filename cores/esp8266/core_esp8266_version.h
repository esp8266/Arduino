
/*
 core_esp8266_version.h - parse "git describe" at compile time
 Copyright (c) 2018 david gauchard. All rights reserved.
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

#ifndef __CORE_ESP8266_VERSION_H
#define __CORE_ESP8266_VERSION_H

#include <core_version.h>
// examples:
//#define ARDUINO_ESP8266_GIT_DESC 2.4.2-91-gcb05b86d
//#define ARDUINO_ESP8266_GIT_DESC 2.5.0rc3-1-gcb05b86d
//#define ARDUINO_ESP8266_GIT_DESC 2.5.0

#define STRHELPER(x) #x
#define STR(x) STRHELPER(x)

#ifdef __cplusplus
extern "C++"
{

template<unsigned N> constexpr
int eVAL (const char (&arr) [N], unsigned i)
{
    return ({ // <= c++11 requires a "return statement"
        int ret = 0;
        int sign = 1;
        if (arr[i] == '-') 
        {
            sign = -1;
            i++;
        }
        while (arr[i] >= '0' && arr[i] <= '9')
            ret = 10*ret + arr[i++] - '0';
        ret * sign;
    });
}

template<unsigned N> constexpr
int fIELDEVAL (const char (&arr) [N], unsigned f)
{
    return ({ // <= c++11 requires a "return statement"
        unsigned i = 0;
        while (f && arr[i])
        {
            if (arr[i] == '-')
                i++;
            for (; arr[i] >= '0' && arr[i] <= '9'; i++);
            f--;
            for (; arr[i] && arr[i] != '-' && (arr[i] < '0' || arr[i] > '9'); i++);
        }
        eVAL(arr, i);
    });
}

/*
 * version major
 */
constexpr int mAJOR () { return fIELDEVAL(STR(ARDUINO_ESP8266_GIT_DESC), 0); }

/*
 * version minor
 */
constexpr int mINOR () { return fIELDEVAL(STR(ARDUINO_ESP8266_GIT_DESC), 1); }

/*
 * version revision
 */
constexpr int rEV   () { return fIELDEVAL(STR(ARDUINO_ESP8266_GIT_DESC), 2); }

/*
 * git commit number since last tag (negative) or RC-number (positive)
 */
constexpr int gIT   () { return fIELDEVAL(STR(ARDUINO_ESP8266_GIT_DESC), 3); }

/*
 * unique revision indentifier (never decreases)
 */
constexpr int uNIQ  ()
{
    return   mAJOR() * 100000
           + mINOR() *  10000
           + rEV()   *   1000
           + (gIT() < 0? -gIT(): gIT()? gIT() - 1000: 0);
}

} // extern "C++"
#endif // __cplusplus

#if TESTMEWITHGCC

/*
"2.4.2-91-gcb05b86d" => 2 . 4 . 2 - -91 = 242091
"2.5.0rc3-1-gcb05b86d" => 2 . 5 . 0 - 3 = 249003
"2.5.0" =>                2 . 5 . 0 - 0 = 250000
*/

int main (void)
{
    printf("%d . %d . %d - %d = %d\n",
        mAJOR(),
        mINOR(),
        rEV(),
        gIT(),
        uNIQ());
    return 0;
}
#endif // testme

#endif // __CORE_ESP8266_ESP8266_VERSION_H
