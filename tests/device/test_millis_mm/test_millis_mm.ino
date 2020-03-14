//
// Millis() Runtime Benchmarke
//
//  Code to determine the runtime in 'usec' of various millis()
//  functions.
//

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <stdio.h>
#include <BSTest.h>

// Include API-Headers
extern "C" {                  // SDK functions for Arduino IDE access
#include "osapi.h"
#include "user_interface.h"
}  //end, 'extern C'

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

//                 Here                    // In 'core_esp8266_wiring.c'
static os_timer_t  us_ovf_timer;           // 'micros_overflow_timer'
static uint32_t    us_cnt = 0;             // 'm' in 'micros_overflow_tick()'
static uint32_t    us_ovflow = 0;          // 'micros_overflow_count'
static uint32_t    us_at_last_ovf = 0;     // 'micros_at_last_overflow_tick'

static bool        fixed_systime = false;  // Testing vars
static bool        debugf = false;
static uint32_t    us_systime = 0;
static float       nsf = 0;                // Normalization factor
static uint32_t    cntref = 0;             // Ref. comparision count

//---------------------------------------------------------------------------
// Interrupt code lifted directly from "cores/core_esp8266_wiring.c",
// with some variable renaming
//---------------------------------------------------------------------------

// Callback for usec counter overflow timer interrupt
void  us_overflow_tick ( void* arg )
{
  (void) arg;

  us_cnt = system_get_time();

  // Check for usec counter overflow
  if ( us_cnt < us_at_last_ovf ) {
    ++us_ovflow;
  } //end-if
  us_at_last_ovf = us_cnt;

} //us_overflow_tick

//---------------------------------------------------------------------------
#define  REPEAT 1
#define  ONCE   0

void us_count_init ( void )
{
  os_timer_setfn( &us_ovf_timer, (os_timer_func_t*)&us_overflow_tick, 0 );
  os_timer_arm( &us_ovf_timer, 60000, REPEAT );

} //us_count_init

//---------------------------------------------------------------------------
// Wrapper(s) for our benchmark
//---------------------------------------------------------------------------
// Set a fixed value of usec system time
void  set_systime ( uint32_t usec )
{
  us_systime = usec;
} //set_systime

//---------------------------------------------------------------------------
// Wrapper to return a fixed system time
uint32_t system_get_timeA ( void )
{
  return ( fixed_systime ? us_systime : system_get_time() );
} //system_get_timeA

//---------------------------------------------------------------------------
// Functions to be tested
//---------------------------------------------------------------------------
// Print integer list as hex
void viewhex( uint16_t *p, uint8_t n )
{
  Serial.print( "0x" );
  for ( uint8_t i = 0; i < n; i++ )
  {
    Serial.printf( "%04X ", p[ (n - 1) - i ] );
  }

} //viewhex

//---------------------------------------------------------------------------
// Support routine for 'millis_test_DEBUG()'
// Print accumulator value along interm summed into it
void view_accsum ( const char *desc, uint16_t *acc, uint16_t *itrm )
{
   Serial.print( "acc:  " );
   viewhex( acc, 4 );
   Serial.printf( "  %s = ", desc );
   viewhex( itrm, 4 );
   Serial.println();
   
} //view_accsum

//---------------------------------------------------------------------------
// FOR BENCHTEST
// Original millis() function
unsigned long ICACHE_RAM_ATTR millis_orig ( void )
{
  // Get usec system time, usec overflow conter
  uint32_t  m = system_get_time();
  uint32_t  c = us_ovflow + ((m < us_at_last_ovf) ? 1 : 0);

  return ( (c * 4294967) + m / 1000 );

} //millis_orig

//---------------------------------------------------------------------------
// FOR DEBUG
// Corrected millis(), 64-bit arithmetic gold standard
// truncated to 32-bits by return
unsigned long ICACHE_RAM_ATTR millis_corr_DEBUG( void )
{
  // Get usec system time, usec overflow conter
  uint32_t  m = system_get_timeA();   // DEBUG
  uint32_t  c = us_ovflow + ((m < us_at_last_ovf) ? 1 : 0);

  return ( (c * 4294967296 + m) / 1000 );

} //millis_corr_DEBUG

//---------------------------------------------------------------------------
// FOR BENCHMARK
unsigned long ICACHE_RAM_ATTR millis_corr ( void )
{
  // Get usec system time, usec overflow conter
  uint32_t  m = system_get_time();
  uint32_t  c = us_ovflow + ((m < us_at_last_ovf) ? 1 : 0);

  return ( (c * 4294967296 + m) / 1000 );

} //millis_corr

//---------------------------------------------------------------------------
// FOR DEBUG
// millis() 'magic multiplier' approximation
//
// This function corrects the cumlative (296us / usec overflow) drift
// seen in the orignal 'millis()' function.
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
//   unsigned long ICACHE_RAM_ATTR millis_corr_DEBUG( void )
//   {
//     // Get usec system time, usec overflow conter
//     ......
//     return ( (c * 4294967296 + m) / 1000 );  // 64-bit division is SLOW
//   } //millis_corr
//
// 5) See this link for a good discussion on magic multipliers:
//    http://ridiculousfish.com/blog/posts/labor-of-division-episode-i.html
//

#define  M_USEC_MAX     0xFFFFFFFF
#define  C_COUNT_MAX    0x00400000

#define  MAGIC_1E3_wLO  0x4bc6a7f0    // LS part
#define  MAGIC_1E3_wHI  0x00418937    // MS part, magic multiplier

unsigned long ICACHE_RAM_ATTR millis_test_DEBUG ( void )
{
  union {
    uint64_t  q;     // Accumulator, 64-bit, little endian
    uint32_t  a[2];  // ..........., 32-bit  segments
  } acc;
  acc.a[1] = 0;      // Zero high-acc
  
  uint64_t  prd;     // Interm product
  
  // Get usec system time, usec overflow counter
  uint32_t  m = system_get_timeA();
  uint32_t  c = us_ovflow + ((m < us_at_last_ovf) ? 1 : 0);

  // DEBUG: Show input vars
  if ( debugf )
     Serial.printf( "Test  m: 0x%08X  c: 0x%08X\n", m, c );

  // (a) Init. low-acc with high-word of 1st product. The right-shift
  //     falls on a byte boundary, hence is relatively quick.

  acc.q  = ( (prd = (uint64_t)( m * (uint64_t)MAGIC_1E3_wLO )) >> 32 );

  // DEBUG: Show both accumulator and interm product
  if( debugf )
     view_accsum( "m kl", (uint16_t *)&acc.q, (uint16_t *)&prd );

  // (b) Offset sum, low-acc
  acc.q += ( prd = ( m * (uint64_t)MAGIC_1E3_wHI ) );

  // DEBUG: Show both accumulator and interm product
  if( debugf )
     view_accsum( "m kh", (uint16_t *)&acc.q, (uint16_t *)&prd );

  // (c) Offset sum, low-acc
  acc.q += ( prd = ( c * (uint64_t)MAGIC_1E3_wLO ) );

  // DEBUG: Show both accumulator and interm product
  if( debugf )
     view_accsum( "c kl", (uint16_t *)&acc.q, (uint16_t *)&prd );
  
  // (d) Truncated sum, high-acc
  acc.a[1] += (uint32_t)( prd = ( c * (uint64_t)MAGIC_1E3_wHI ) );

  // DEBUG: Show both accumulator and interm product
  if( debugf )
     view_accsum( "c kh", (uint16_t *)&acc.q, (uint16_t *)&prd );

  return ( acc.a[1] );  // Extract result, high-acc

} //millis_test_DEBUG

//---------------------------------------------------------------------------
// FOR BENCHTEST
unsigned long ICACHE_RAM_ATTR millis_test ( void )
{
  union {
    uint64_t  q;     // Accumulator, 64-bit, little endian
    uint32_t  a[2];  // ..........., 32-bit  segments
  } acc;
  acc.a[1] = 0;      // Zero high-acc

  // Get usec system time, usec overflow counter
  uint32_t  m = system_get_time();
  uint32_t  c = us_ovflow + ((m < us_at_last_ovf) ? 1 : 0);

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

} //millis_test

//---------------------------------------------------------------------------
// Execution time benchmark
//---------------------------------------------------------------------------

// Print benchmark result
void  millis_rtms_print ( const char *pream, uint32_t cntx, const char *desc )
{
  Serial.print( pream );
  Serial.print( nsf * (float)cntx );
  Serial.print( "   " );
  Serial.print( (float)cntx / (float)cntref );
  Serial.print( "     " );  
  Serial.println( desc );
  
} //millis_rtms_print

//---------------------------------------------------------------------------
void  Millis_RunTimes ( void )
{
  Serial.println();
  Serial.println( "Millis() RunTime Benchmarks" );

  uint32_t lc = 100000;          // Samples
  nsf = 1 / float(lc);           // Normalization (global)
 
  uint32_t bgn;
  uint32_t cnto  = 0, cntv = 0;
  uint32_t cntcx = 0, cntc = 0;
  uint32_t cntfx = 0, cntf = 0;

  // Setup timer values
  fixed_systime = true; 
  us_ovflow      =  C_COUNT_MAX;
  us_at_last_ovf = (M_USEC_MAX - (20 * 1000000));   // Max. less 20 sec

// No printing, systime active
  debugf = false; fixed_systime = false;

  for (uint32_t i = 0; i < lc; i++ )
  {
    bgn = system_get_time();
    millis_orig();      
    cnto += system_get_time() - bgn;

    bgn = system_get_time();
    millis();      
    cntv += system_get_time() - bgn;

    bgn = system_get_time();
    millis_corr_DEBUG();
    cntcx += system_get_time() - bgn;
        
    bgn = system_get_time();
    millis_corr();
    cntc += system_get_time() - bgn;

    bgn = system_get_time();
    millis_test_DEBUG();
    cntfx += system_get_time() - bgn;

    bgn = system_get_time();
    millis_test();
    cntf += system_get_time() - bgn;

    yield();
  } //end-for

  cntref = cnto;  // Set global ref. count

  Serial.println();
  Serial.println( "         usec   x Orig   Comment" );

  millis_rtms_print( " Orig:   ",  cntref, "Original code" ); 
  millis_rtms_print( " Core:   ",  cntv,   "Current  core" ); 
  Serial.println();

  millis_rtms_print( "  Ref:  ",   cntcx,  "64-bit reference code,      DEBUG" );
  millis_rtms_print( " Test:   ",  cntfx,  "64-bit magic multiply, 4x32 DEBUG" );  
  Serial.println();

  millis_rtms_print( "  Ref:  ",   cntc,   "64-bit reference code" );  
  millis_rtms_print( " Test:   ",  cntf,   "64-bit magic multiply, 4x32" );
  Serial.println();

  Serial.println( F("*** End, Bench Test ***") );
  Serial.println();

} //Millis_RunTimes

//---------------------------------------------------------------------------
// Debug millis_test()
//---------------------------------------------------------------------------

bool  Debug_Millis_Test ( void )
{
  uint32_t  m, msc, mstx;
  int32_t   diff;

// Switch over to fixed system time, enable printing
  fixed_systime = true;  debugf = true;
  
  us_ovflow    = C_COUNT_MAX;
  m            = (M_USEC_MAX - (0 * 1000000));
  set_systime( m );
  us_at_last_ovf = m - 1;  // Disables 'c' bump

// Millis() comparison, test vs. reference
  Serial.println();
  mstx = millis_test_DEBUG();
  msc  = millis_corr_DEBUG();
  diff = (int32_t)(mstx - msc);

  Serial.println();
  Serial.println( "         m         Test    Reference   Difference" );
  Serial.printf( "0x%08x   0x%08x   0x%08X    %9d\n", m, mstx, msc, diff );
  Serial.println();

// No printing, variable systime
  debugf = false; fixed_systime = false;

  return( (bool)( diff == 0 ) );   // Good test, matches reference

} //Debug_Millis_Test

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

BS_ENV_DECLARE();

void setup ()
{
  Serial.begin(115200);
  WiFi.mode( WIFI_OFF );
  BS_RUN(Serial);
} //setup


//---------------------------------------------------------------------------
bool pretest ()
{
  us_count_init();        // Start up timer overflow sampling  
  return true;
} //pretest

//---------------------------------------------------------------------------
void loop(void)
{
   yield();
} //loop

//---------------------------------------------------------------------------
// Test cases
//---------------------------------------------------------------------------
TEST_CASE( "Millis RunTime Benchmarks", "[bs]" )
{
    Millis_RunTimes();
} //testcase1

TEST_CASE( "Debug 'millis_test()' Code", "[bs]" )
{
    bool ok = Debug_Millis_Test();
    CHECK( ok );
} //testcase2

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
