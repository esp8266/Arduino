/* 
 core_esp8266_wiring.c - implementation of Wiring API for esp8266

 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
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

#include "wiring_private.h"
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "cont.h"

extern "C" {

extern void ets_delay_us(uint32_t us);
extern void esp_yield();
extern void esp_delay(unsigned long ms);

static os_timer_t micros_overflow_timer;
static uint32_t micros_at_last_overflow_tick = 0;
static uint32_t micros_overflow_count = 0;
#define ONCE 0
#define REPEAT 1

void __delay(unsigned long ms) {
    esp_delay(ms);
}

void delay(unsigned long ms) __attribute__ ((weak, alias("__delay"))); 

void micros_overflow_tick(void* arg) {
    (void) arg;
    uint32_t m = system_get_time();
    if(m < micros_at_last_overflow_tick)
        ++micros_overflow_count;
    micros_at_last_overflow_tick = m;
}

//---------------------------------------------------------------------------
// millis() 'magic multiplier' approximation
//
// This function corrects the cumulative (296us / usec overflow) drift
// seen in the original 'millis()' function.
//
// Input:
//    'm' - 32-bit usec counter,           0 <= m <= 0xFFFFFFFF
//    'c' - 32-bit usec overflow counter   0 <= c <  0x00400000
// Output:
//    Returns milliseconds in modulo 0x1,0000,0000 (0 to 0xFFFFFFFF)
//
// Notes:
//
// 1) This routine approximates the 64-bit integer division,
//
//    quotient =  ( 2^32 c + m ) / 1000,
//
//    through the use of 'magic' multipliers. A slow division is replaced by
//    a faster multiply using a scaled multiplicative inverse of the divisor:
//
//    quotient =~ ( 2^32 c + m ) * k,  where k = Ceiling[ 2^n / 1000 ]
//
//    The precision difference between multiplier and divisor sets the
//    upper-bound of the dividend which can be successfully divided.
//
//    For this application, n = 64, and the divisor (1000) has 10-bits of 
//    precision. This sets the dividend upper-bound to (64 - 10) = 54 bits,
//    and that of 'c' to (54 - 32) = 22 bits. This corresponds to a value
//    for 'c' = 0x0040,0000 , or +570 years of usec counter overflows.
//
// 2) A distributed multiply with offset-summing is used find k( 2^32 c + m ):
//
//      prd = (2^32 kh + kl) * ( 2^32 c + m )
//          = 2^64 kh c + 2^32 kl c + 2^32 kh m + kl m
//               (d)         (c)         (b)       (a)
//
//    Graphically, the offset-sums align in little endian like this:
//                  LS -> MS
//            32       64       96      128
//    | a[-1]  |  a[0]  |  a[1]  |  a[2]  |
//    |       m kl      |    0   |    0   |  a[-1] not needed
//    |        |       m kh      |        |
//    |        |       c kl      |        |  a[1] holds the result
//    |        |        |       c kh      |  a[2] can be discarded
//
//    As only the high-word of 'm kl' and low-word of 'c kh' contribute to the
//    overall result, only (2) 32-bit words are needed for the accumulator.
//
// 3) As C++ does not intrinsically test for addition overflows, one must
//    code specifically to detect them. This approximation skips these
//    overflow checks for speed, hence the sum,
//
//    highword( m kl ) + m kh + c kl  <  (2^64-1),  MUST NOT OVERFLOW.
//
//    To meet this criteria, not only do we have to pick 'k' to achieve our
//    desired precision, we also have to split 'k' appropriately to avoid
//    any addition overflows.
//
//    'k' should be also chosen to align the various products on byte
//    boundaries to avoid any 64-bit shifts before additions, as they incur
//    major time penalties. The 'k' chosen for this specific division by 1000
//    was picked primarily to avoid shifts as well as for precision.
//
//    For the reasons list above, this routine is NOT a general one.
//    Changing divisors could break the overflow requirement and force
//    picking a 'k' split which requires shifts before additions.
//
//              ** Test THOROUGHLY after making changes **
//
// 4) Results of time benchmarks run on an ESP8266 Huzzah feather are:
//
//         usec   x Orig   Comment
// Orig:   3.18   1.00     Original code
// Corr:  13.21   4.15     64-bit reference code
// Test:   4.60   1.45     64-bit magic multiply, 4x32
//
// The magic multiplier routine runs ~3x faster than the reference. Execution
// times can vary considerably with the numbers being multiplied, so one
// should derate this factor to around 2x, worst case.
//
//   Reference function: corrected millis(), 64-bit arithmetic,
//                       truncated to 32-bits by return
//   unsigned long IRAM_ATTR millis_corr_DEBUG( void )
//   {
//     // Get usec system time, usec overflow conter
//     ......
//     return ( (c * 4294967296 + m) / 1000 );  // 64-bit division is SLOW
//   } //millis_corr
//
// 5) See this link for a good discussion on magic multipliers:
//    http://ridiculousfish.com/blog/posts/labor-of-division-episode-i.html
//

#define  MAGIC_1E3_wLO  0x4bc6a7f0    // LS part
#define  MAGIC_1E3_wHI  0x00418937    // MS part, magic multiplier

unsigned long IRAM_ATTR millis()
{
  union {
     uint64_t  q;     // Accumulator, 64-bit, little endian
     uint32_t  a[2];  // ..........., 32-bit  segments
  } acc;
  acc.a[1] = 0;       // Zero high-acc
  
  // Get usec system time, usec overflow counter
  uint32_t  m = system_get_time();
  uint32_t  c = micros_overflow_count +
                   ((m < micros_at_last_overflow_tick) ? 1 : 0);

  // (a) Init. low-acc with high-word of 1st product. The right-shift
  //     falls on a byte boundary, hence is relatively quick.
  
  acc.q  = ( (uint64_t)( m * (uint64_t)MAGIC_1E3_wLO ) >> 32 );

  // (b) Offset sum, low-acc
  acc.q += ( m * (uint64_t)MAGIC_1E3_wHI );

  // (c) Offset sum, low-acc
  acc.q += ( c * (uint64_t)MAGIC_1E3_wLO );

  // (d) Truncated sum, high-acc
  acc.a[1] += (uint32_t)( c * (uint64_t)MAGIC_1E3_wHI );

  return ( acc.a[1] );  // Extract result, high-acc

} //millis

unsigned long IRAM_ATTR micros() {
    return system_get_time();
}

uint64_t IRAM_ATTR micros64() {
    uint32_t low32_us = system_get_time();
    uint32_t high32_us = micros_overflow_count + ((low32_us < micros_at_last_overflow_tick) ? 1 : 0);
    uint64_t duration64_us = (uint64_t)high32_us << 32 | low32_us;
    return duration64_us;
}

void IRAM_ATTR delayMicroseconds(unsigned int us) {
    os_delay_us(us);
}

void init() {
    initPins();
    timer1_isr_init();
    os_timer_setfn(&micros_overflow_timer, (os_timer_func_t*) &micros_overflow_tick, 0);
    os_timer_arm(&micros_overflow_timer, 60000, REPEAT);
}

};
