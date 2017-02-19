FAQ
===

The purpose of this FAQ / Troubleshooting is to respond to questions
commonly asked in `Issues <https://github.com/esp8266/Arduino/issues>`__
section and on `ESP8266 Community forum <http://www.esp8266.com/>`__.

Where possible we are going right to the answer and provide it within
one or two paragraphs. If it takes more than that, you will see a link
:arrow\_right: to more details.

Please feel free to contribute if you believe that some frequent issues
are not covered below.


I am getting "espcomm\_sync failed" error when trying to upload my ESP. How to resolve this issue?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This message indicates issue with uploading ESP module over a serial
connection. There are couple of possible causes, that depend on the type
of your module, if you use separate USB to serial converter.

:doc:`Read more <a01-espcomm_sync-failed>`.

Why esptool is not listed in "Programmer" menu? How do I upload ESP without it?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Do not worry about "Programmer" menu of Arduino IDE. It doesn't matter
what is selected in it — upload now always defaults to using esptool.

Ref. `#138 <https://github.com/esp8266/Arduino/issues/138>`__,
`#653 <https://github.com/esp8266/Arduino/issues/653>`__ and
`#739 <https://github.com/esp8266/Arduino/issues/739>`__.

My ESP crashes running some code. How to troubleshoot it?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The code may crash because of s/w bug or issue with your h/w. Before
entering an issue report, please perform initial troubleshooting.

:doc:`Read more <a02-my-esp-crashes>`.

This Arduino library doesn't work on ESP. How do I make it working?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You would like to use this Arduino library with ESP8266 and it does not
perform. It is not listed among libraries verified to work with ESP8266.

:doc:`Read more <a03-library-does-not-work>`.

In the IDE, for ESP-12E that has 4M flash, I can choose 4M (1M SPIFFS) or 4M (3M SPIFFS). No matter what I select, the IDE tells me the maximum code space is about 1M. Where does my flash go?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The reason we cannot have more than 1MB of code in flash has to do with
a hardware limitation. Flash cache hardware on the ESP8266 only allows
mapping 1MB of code into the CPU address space at any given time. You
can switch mapping offset, so technically you can have more than 1MB
total, but switching such "banks" on the fly is not easy and efficient,
so we don't bother doing that. Besides, no one has so far complained
about 1MB of code space being insufficient for practical purposes.

The option to choose 4M or 1M SPIFFS is to optimize the upload time.
Uploading 3MB takes a long time so sometimes you can just use 1MB. Other
2MB of flash can still be used with ``ESP.flashRead`` and
``ESP.flashWrite`` APIs if necessary.

I have observed a case when ESP.restart() doesn't work. What is the reason for that?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You will see this issue only if serial upload was not followed by a
physical reset (e.g. power-on reset). For a device being in that state
``ESP.restart`` will not work. Apparently the issue is caused by `one of
internal registers not being properly updated until physical
reset <https://github.com/esp8266/Arduino/issues/1017#issuecomment-200605576>`__.
This issue concerns only serial uploads. OTA uploads are not affected.
If you are using ``ESP.restart``, the work around is to reset ESP once
after each serial upload.

Ref. `#1017 <https://github.com/esp8266/Arduino/issues/1017>`__,
`#1107 <https://github.com/esp8266/Arduino/issues/1107>`__,
`#1782 <https://github.com/esp8266/Arduino/issues/1782>`__

How to resolve "Board generic (platform esp8266, package esp8266) is unknown" error?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This error may pop up after switching between
`staging <https://github.com/esp8266/Arduino#staging-version->`__ and
`stable <https://github.com/esp8266/Arduino#stable-version->`__ esp8266
/ Arduino package installations, or after upgrading the package version
:doc:`Read more <a04-board-generic-is-unknown>`.
