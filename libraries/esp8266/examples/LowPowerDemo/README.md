# <center>Low-Power Demo</center>

There is a lot of confusion, out-of-date information, and poor or non-working examples of how to use the 5 basic low-power modes of the ESP8266.  This demo code shows you how to use them reliably.  If you're here for very low power, then the 2 Light Sleep modes and Deep Sleep are what you want.

The two relevant reference manuals from Espressif are the [Low-Power Solutions](https://www.espressif.com/sites/default/files/documentation/9b-esp8266-low_power_solutions__en.pdf) and the [Non-OS SDK API Reference](https://www.espressif.com/sites/default/files/documentation/2c-esp8266_non_os_sdk_api_reference_en.pdf).  There is more information in the two PDFs than is presented here, so you'll want both of them for your reference.


The table below is an expanded version of Table 1.1 from the Low-Power Solutions PDF.  The amperages listed are absolute minimums, and most people will not get that low with typical hardware and programs.

|          item         | Automatic Modem Sleep | Forced Modem Sleep | Automatic Light Sleep | Timed / Forced Light Sleep |  Forced Deep Sleep |
|:---------------------:|:---------------------:|:------------------:|:---------------------:|:------------------:|:------------------:|
|   WiFi connectivity   |       Connected       |    Disconnected    |       Connected       |    Disconnected    |    Disconnected    |
|       GPIO state      |       Unchanged       |      Unchanged     |       Unchanged       |      Unchanged     | Low amperage (2 uA) |
|          WiFi         |           ON          |         OFF        |           ON          |         OFF        |         OFF        |
|      System Clock     |           ON          |         ON         |        CYCLING        |         OFF        |         OFF        |
|          RTC          |           ON          |         ON         |           ON          |         ON (1)        |         ON (2)     |
|          CPU          |           ON          |         ON         |           ON          |         ON         |         OFF        |
|   Substrate Amperage   |         15 mA         |        15 mA       |        1-15 mA (3)    |       0.4 mA       |        20 uA       |
|  Avg Amperage DTIM = 1 |        16.2 mA        |                    |        (1.8 mA)       |                    |                    |
|  Avg Amperage DTIM = 3 |        15.4 mA        |                    |        (0.9 mA)       |                    |                    |
| Avg Amperage DTIM = 10 |        15.2 mA        |                    |       (0.55 mA)       |                    |                    |

Notes: 

(1) setting a sleep time of 0xFFFFFFF for Light Sleep disconnects the RTC, requiring a GPIO interrupt to wake the CPU

(2) setting a sleep time of 0 for Deep Sleep disconnects the RTC, requiring an external RESET to wake the CPU

(3) minimum amperage was never measured less than ~ 1 mA and is frequently 15 mA between TIM beacons

The Average Amperage with different DTIM settings is unverified, and will likely be higher in a real-world environment.  All of the amperages listed in this README are for the ESP8266 chip only, compiled for 80 MHz CPU Frequency as 160 MHz uses even more power.  Modules that have voltage regulators, USB chips, LEDs or other hardware will draw additional amperage.

---

## Basic Tests in the Demo

1. Unconfigured modem
2. Automatic Modem Sleep
3. Forced Modem Sleep
4. Automatic Light Sleep
5. Timed Light Sleep - stop the CPU for (x microseconds)
6. Forced Light Sleep, wake with GPIO interrupt
7. Deep Sleep for 10 seconds, wake with default modem power settings
8. Deep Sleep for 10 seconds, wake with RFCAL
9. Deep Sleep Instant for 10 seconds, wake with NO_RFCAL
10. Deep Sleep Instant for 10 seconds, wake with RF_DISABLED

---

### Test 1 - Unconfigured modem

This is typical for programs that don't use WiFi, and is a high continuous drain of at least 67 mA.  This isn't a test as much as setting a baseline or reference point for comparing the power savings.  You can stop during any test while the CPU is halted or the LED is blinking to measure the amperage.

### Test 2 - Automatic Modem Sleep

This is the default power saving mode when you have an active WiFi connection.  You don't need to add anything to your code to get this mode.  The only time the modem sleeps is when your program spends time in delay() frequently in STA mode.  Any delay() time works as long as it happens frequently.  The test is doing **delay(100)** to get the modem to sleep.  While in delay() your sketch isn't doing anything worthwhile.  Amperage during Automatic Modem Sleep is 15 mA minimum.  Without a delay() the amperage is > 67 mA with brief spikes > 250-350 mA as transmissions occur.  When the WiFi has traffic (even a couple of pings), the modem can turn on for over 2 seconds continuous at 67 mA, and it may stay on for a second after the traffic.  In a high traffic environment you won't get any power savings with either of the 2 Automatic modes.  Automatic Modem Sleep turns on 7-8 seconds after an active connection is established.

### Test 3 - Forced Modem Sleep

Turns off the modem (losing the connection), and reducing the amperage by > 50 mA.  This test uses a WiFi library function and saves the WiFi connection state for faster reconnection later in the tests.  Forced Modem Sleep is good if there is a long interval with no expected WiFi traffic, as you can do other things while only drawing 15 to 20 mA.  The longer you spend in delay(), the closer the amperage approaches 15 mA.  The test loops on delay(100) until you press the button.  The CPU will be drawing 15 to 16 mA during the looped delay(), and 19-20 mA without a delay().  Doing WiFi.forceSleepWake() to wake the modem later can take twice as long as a re-initialize of WiFi.

### Test 4 - Automatic Light Sleep

Like Automatic Modem Sleep, with similar restrictions.  Once configured it's immediately active when a connection is established.  During periods of long delay() the amperage can drop to ~ 2 mA average.  In a network with sparse traffic you might get something near 2-5 mA average.  The LED blinks more slowly during this test as it's doing delay(350) to get the modem to sleep.  With delay() times shorter than the TIM beacon interval (100 mS beacons for these tests) the modem only goes into Automatic Modem Sleep, and with a longer delay() it will go more fully into Automatic Light Sleep.  Although the CPU clock is cycling on and off to achieve low power, millis() and micros() are correct.  You can't stop OS_timers to get the low amperage recorded by Espressif as WiFi needs system timers running.

### Test 5 - Timed Light Sleep

Similar to timed Deep Sleep, but it wakes with an interrupt at the next line in your code and continues.  The chip sleeps at 0.4 mA amperage until it is woken by the RTC timer.  If you have a design that needs to be woken more often than every 2 seconds then you should consider using Timed Light Sleep.  For sleep periods longer than 2 seconds, Deep Sleep will be more energy efficient.  The chip wakes after an interrupt in about 3 to 5.5 mS (regardless of CPU speed), but WiFi was turned off to enter timed Light Sleep so you will need to re-initialize it if you are using WiFi.  Any timers (including OS_timers and PWM) will keep the chip from going into timed Light Sleep, and it will fall through to the next line in your code if any timers are running.  If you do a print() before Sleep, be sure to do Serial.flush() to stop the UART.  The interrupt callback is recommended, and you can set a optional GPIO interrupt to wake the CPU as well as the RTC timer.

### Test 6 - Forced Light Sleep, wake with GPIO interrupt

Similar to ESP.deepSleep(0).  The chip sleeps at 0.4 mA amperage until it is woken by an external interrupt.  The only allowed interrupts are high level and low level; edge interrupts won't work.  If you have a design that needs to be woken more often than every 2 seconds then you should consider using Forced Light Sleep.  For sleep periods longer than 2 seconds, Deep Sleep will be more energy efficient.  The chip wakes after an interrupt in about 3 to 5.5 mS (regardless of CPU speed), but WiFi was turned off to enter Forced Light Sleep so you will need to re-initialize it if you are using WiFi.  Any user timers (including PWM) will keep the chip from going fully into Forced Light Sleep, and amperage will be ~ 2 mA with timers enabled.

### Test 7 - Deep Sleep, wake with RF_DEFAULT

In Deep Sleep almost everything is turned off, and the chip draws ~ 20 uA.  If you have D0/GPIO16 connected to RST, you can use the RTC timer to wake the chip up at a timed interval.  You can also wake it solely with an external RESET with ESP.deepSleep(0, wake option), which disconnects the timer.  Waking with RF_DEFAULT means it will do an RFCAL if it needs to.  Doing **ESP.deepSleep(time)** without the mode variable uses this wake mode.  These first two Deep Sleep tests use the standard Deep Sleep function, so the WiFi connection is closed and the modem turned off, which takes up to 270 mS before Deep Sleep begins.  Deep Sleep ends with a RESET, and the boot time after that is ~ 130 mS.  Any Deep Sleep less than 2 seconds is wasting energy due to the modem shut-off and boot times, and Forced Light Sleep will be a better choice as it recovers in < 5.5 mS from the previous program state.  The Deep Sleep tests will not go into Automatic Modem Sleep because delay() is not used.

Note that a RESET during Deep Sleep (either external or from D0/GPIO16) does not clear the GPIO pins; some of them hold their previous state.  It's unknown how much else survives a reset, as it's not well documented.

### Test 8 - Deep Sleep, wake with RFCAL

Identical to the test above, but the modem always does an RF power calibration when booting.  In normal use, most people would do WAKE_RF_DEFAULT instead to avoid the extra RFCAL power burst coming out of Deep Sleep if it's not needed.  Note that most of the time both of these modes (WAKE_RF_DEFAULT and WAKE_RFCAL) do a 100 mS long RFCAL *before* going into Deep Sleep (the RFCAL after Deep Sleep is much shorter).  If the modem is shut down, this long RFCAL doesn't happen.

### Test 9 - Deep Sleep Instant, wake with NO_RFCAL

This variation doesn't do an RF calibration on return, so power requirements will be slightly less.  Additionally, frequently it immediately goes into Deep Sleep without turning off the modem (that's the INSTANT part).  There's another bug in SDK 2, and the SDK functions the WiFi-class calls occasionally do a modem shut-down before Deep Sleep; it's not always Instant.  When it doesn't do the modem shut-down it saves an extra 270 mS of power.  With the modem turned off (Forced Modem Sleep) you **always** get an instant Deep Sleep; doing WiFi.mode(WIFI_OFF) doesn't help, as the SDK still spends 270 mS of time shutting the modem down before going into Deep Sleep.

### Test 10 - Deep Sleep Instant, wake with RF_DISABLED

This last variation also uses Deep Sleep Instant, but it wakes up with the modem disabled so amperage after Deep Sleep is only 15 mA.  Each of the 4 WAKE modes has their own use, depending on what you need.

---

All of the Deep Sleep modes end with a RESET, so you must re-initialize nearly everything.  You can store *some* information in the RTC memory to survive a Deep Sleep reset, which was done in this demo to illustrate the RTC memory.  See the **RTCUserMemory** and **WiFiShutdown** examples for more on this feature. 

Since SDK 2.1 the maximum Deep Sleep time has changed.  The old maximum was based on uint32_t(micros) or ~71.58 minutes (2^32-1 microseconds).  The new maximum is calculated from the RTC clock, which drifts with temperature from 5 to 7 timer ticks per microsecond, and will return a different number each time you read **system_rtc_clock_cali_proc()**.  Depending on CPU and temperature, you will get between 3 and 4 hours maximum Deep Sleep.  You can read the current theoretical maximum with **uint64_t deepSleepMax = ESP.deepSleepMax();** although you should set your time a little less than that due to the RTC drift.  If you go over the maximum time when you finally enter Deep Sleep, it disconnects the timer and won't wake without an external RESET. 

If you need a longer sleep time than 3 hours, you can pass zero as the time variable to Deep Sleep and it disconnects the RTC.  The only way to wake it at that point is an external RESET; D0 can't do it.  Both Forced Light Sleep and Deep Sleep(0) are woken by an external signal, so short delays are more efficient with Forced Light Sleep, and longer delays are more energy efficient with Deep Sleep.


---

### Lower Power without the WiFi library (Forced Modem Sleep):

If all you want to do is reduce power for a sketch that doesn't need WiFi, add these SDK 2 functions to your code:

At the top of your program, add:
```c
#include <user_interface.h>
```

and in setup() add:
```c
  wifi_set_opmode_current(NULL_MODE);  // set Wi-Fi working mode to unconfigured, don't save to flash
  wifi_fpm_set_sleep_type(MODEM_SLEEP_T);  // set the sleep type to modem sleep
  wifi_fpm_open();  // enable Forced Modem Sleep
  wifi_fpm_do_sleep(0xFFFFFFF);  // force the modem to enter sleep mode
  delay(10);  // without a minimum of delay(1) here it doesn't reliably enter sleep
```
This code allows you to shut down the modem *without* loading the WiFi library, dropping your amperage by > 50 mA, or ~ 1/4th of the initial power.  It doesn't time out at 268 seconds, as you might think from the (0xFFFFFFF).  If you put a delay value smaller than 0xFFFFFFF it will end Modem Sleep at the number of uS you have entered: 10E6 would be 10 seconds of Modem Sleep.  The Forced Modem Sleep test does the same thing with a WiFi library call that encapsulates something similar to the code above.

If you want to reduce the start-up power even more, see https://github.com/esp8266/Arduino/issues/6642#issuecomment-578462867

You can also use the Deep Sleep modes without loading the WiFi library, as Deep Sleep use ESP API functions.  The Deep Sleep tests above turn the WiFi on to show you the differences after the 4 reset modes. but WiFi is not required for Deep Sleep.

