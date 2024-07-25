:orphan:

This Arduino library doesn't work on ESP. How do I make it working?
-------------------------------------------------------------------

-  `Introduction <#introduction>`__
-  `Identify the Issues <#identify-the-issues>`__
-  `Fix it Yourself <#fix-it-yourself>`__
-  `Compilation Errors <#compilation-errors>`__
-  `Exceptions / Watchdog Resets <#exceptions-watchdog-resets>`__
-  `Functionality Issues <#functionality-issues>`__
-  `Conclusion <#conclusion>`__

Introduction
~~~~~~~~~~~~

You would like to use this Arduino library with ESP8266 and it doesn't
perform. It is not listed among `libraries verified to work with
ESP8266 <../libraries.rst#other-libraries-not-included-with-the-ide>`__.
You couldn't find any evidence on internet that it is compatible.

What are the odds to make it working?

Identify the Issues
~~~~~~~~~~~~~~~~~~~

Start with looking for all the symptoms that it is not compatible with
ESP8266. Ideally use example sketches provided with the library. Then
list all the issues you are able to identify.

You are likely to see one or more of the following: \* Compilation drops
errors \* There are no issues with compilation but application restarts
because of exception or watchdog (wdt) \* Application seems to work, but
does not function as expected, e.g. calculation results are incorrect.

Armed with the list of issues, contact the library author asking for
comments. If issues are legitimate, then ask for his / her support to
implement it for ESP8266. Being specific you have bigger chances
convincing the author to help you either by updating the library or
guiding you how to resolve the issues.

Fix it Yourself
~~~~~~~~~~~~~~~

If library author is unable to provide support, then assess the chances
of fixing it yourself.

Compilation Errors
^^^^^^^^^^^^^^^^^^

*Issue:* Compiler complains about usage of AVR registers (PORTx, PINx,
TCR1A, etc).

*Solution:* Check if usage of registers is well localized in a few
functions, try to replace GPIO registers usage with digitalRead /
digitalWrite, timer registers usage with timerX\_ functions. If usage of
AVR registers happens all over the code, this library might not be worth
the effort. Also may be worth checking if someone got the library
working on ARM (Due/STM), PIC, etc. If this is the case, maybe there
already is a version of the library which uses Arduino APIs instead of
raw registers.

*Issue:* Compiler complains about ``<avr/pgmspace.h>``.

*Solution:* modify the library by adding conditional include of ESP's
pgmspace.h.

::

    #ifdef ESP8266
      #include <pgmspace.h>
    #else
      #include <avr/pgmspace.h>
    #endif

Exceptions / Watchdog Resets
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To troubleshoot resets follow FAQ item `My ESP crashes running some
code <a02-my-esp-crashes.rst>`__.

Functionality Issues
^^^^^^^^^^^^^^^^^^^^

*Issue:* Application works but returns weird numerical values.

*Solution:*: Check the usage of ``int`` type in the library. On AVRs
integers are 16 bit, and on ESPs they are 32 bit (just like on ARM).

*Issue:* Some device with time critical control like a servo drive or a
strip of LEDs does not operate smoothly and tends to randomly change
position or displayed pattern.

*Solution:*: Check for usage of interrupts that may get in conflict with
Wi-Fi activity of ESP8266. You may temporarily disable Wi-Fi
communication ``WiFi.mode(WIFI_OFF);`` to check if it helps.

Conclusion
~~~~~~~~~~

Identify compatibility issues and ask library author for support. If
left on your own, then check for usage of controller's low level access
functionality. Use `Esp Exception
Decoder <https://github.com/me-no-dev/EspExceptionDecoder>`__ if
confronted with exceptions / watchdogs resets.

The good news is that the number of libraries which aren't supported on
the ESP8266 is shrinking. Community of ESP8266 enthusiasts is growing.
If you are unable to resolve the issues yourself, there are very good
odds that you will be able to find somebody else to help you.

`FAQ list :back: <readme.rst>`__
