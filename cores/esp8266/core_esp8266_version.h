
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

#define STRHELPER(x) #x
#define STR(x) STRHELPER(x)

#ifdef __cplusplus
extern "C++"
{

// Following constexpr functions are compiled and executed
// *after* pre-processing and *during* compilation
//
// Their result is treated like a numeric constant in final binary code.
// git tags must be in the form:
// - <major>.<minor>.<revision>       (2.4.2) (2.5.0)
// - <major>.<minor>.<revision>-rc<rc> (2.5.0-rc1) (2.5.0-rc2)
//
// "git describe" = ARDUINO_ESP8266_GIT_DESC will thus be in the form:
// - <tag> (2.4.2) (2.5.0)
// - <tag>-<numberOfCommits>-g<git-hash> (2.4.2-91-gcb05b86d) (2.5.0-rc3-1-gcb05b86d)
//
// Examples:
// case 2.4.2 (fresh version/tag)
//     esp8266CoreVersionSubRevision() is 0   Numeric is: 20402000
// case 2.4.2-91-gcb05b86d:
//     esp8266CoreVersionSubRevision() is -91 Numeric is: 20402091
// case 2.5.0-rc3-1-gcb05b86d:
//     esp8266CoreVersionSubRevision() is 3   Numeric is: 20499903
// case 2.5.0:
//     esp8266CoreVersionSubRevision() is 0   Numeric is: 20500000

constexpr
bool constexpr_isDecimal (const char c)
{
    return c >= '0' && c <= '9';
}

template<unsigned N> constexpr
bool constexpr_isMinus (const char (&arr) [N], unsigned i)
{
    return arr[i] == '-' && constexpr_isDecimal(arr[i+1]);
}

template<unsigned N> constexpr
int constexpr_atoi (const char (&arr) [N], unsigned i)
{
    return ({ // <= c++11 requires a "return statement"
        int ret = 0;
        int sign = 1;
        if (arr[i] == '-')
        {
            sign = -1;
            i++;
        }
        while (constexpr_isDecimal(arr[i]))
            ret = 10*ret + arr[i++] - '0';
        ret * sign;
    });
}

template<unsigned N> constexpr
int constexpr_extract_int (const char (&arr) [N], unsigned f)
{
    return ({ // <= c++11 requires a "return statement"
        unsigned i = 0;
        while (f && arr[i])
        {
            if (constexpr_isMinus(arr, i))
                i++;
            for (; constexpr_isDecimal(arr[i]); i++);
            f--;
            for (; arr[i] && !constexpr_isMinus(arr, i) && !constexpr_isDecimal(arr[i]); i++);
        }
        constexpr_atoi(arr, i);
    });
}

/*
 * version major
 */
constexpr
int esp8266CoreVersionMajor ()
{
    return constexpr_extract_int(STR(ARDUINO_ESP8266_GIT_DESC), 0);
}

/*
 * version minor
 */
constexpr
int esp8266CoreVersionMinor ()
{
    return constexpr_extract_int(STR(ARDUINO_ESP8266_GIT_DESC), 1);
}

/*
 * version revision
 */
constexpr
int esp8266CoreVersionRevision ()
{
    return constexpr_extract_int(STR(ARDUINO_ESP8266_GIT_DESC), 2);
}

/*
 * git commit number since last tag (negative)
 * or RC-number (positive)
 */
constexpr
int esp8266CoreVersionSubRevision ()
{
    return constexpr_extract_int(STR(ARDUINO_ESP8266_GIT_DESC), 3);
}

/*
 * unique revision indentifier (never decreases)
 */
constexpr
int esp8266CoreVersionNumeric ()
{
    return   esp8266CoreVersionMajor()    * 10000000
           + esp8266CoreVersionMinor()    *   100000
           + esp8266CoreVersionRevision() *     1000
           + (esp8266CoreVersionSubRevision() < 0 ?
               -esp8266CoreVersionSubRevision() :
               esp8266CoreVersionSubRevision() ?
                   esp8266CoreVersionSubRevision() - 100 :
                   0);
}

} // extern "C++"
#endif // __cplusplus
#endif // __CORE_ESP8266_ESP8266_VERSION_H
