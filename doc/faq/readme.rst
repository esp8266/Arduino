FAQ
===

The purpose of this FAQ / Troubleshooting is to respond to questions
commonly asked in `Issues <https://github.com/esp8266/Arduino/issues>`__
section and on `ESP8266 Community forum <https://www.esp8266.com/>`__.

Where possible we are going right to the answer and provide it within
one or two paragraphs. If it takes more than that, you will see a link
to "Read more" details.

Please feel free to contribute if you believe that some frequent issues
are not covered below.


I am getting "espcomm\_sync failed", "esptool.FatalError", etc. How to resolve this issue?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This message indicates issue with uploading ESP module over a serial
connection. There are couple of possible causes, that depend on the type
of your module, if you use separate USB to serial converter.

`Read more <a01-upload-failed.rst>`__.

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

`Read more <a02-my-esp-crashes.rst>`__.

How can I get some extra KBs in flash ?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Using ``*printf()`` with floats is enabled by default.  Some KBs of flash can
  be saved by using the option ``--nofloat`` with the boards generator:

  ``./tools/boards.txt.py --nofloat --boardsgen``

* Use the debug level option ``NoAssert-NDEBUG`` (in the Tools menu)

`Read more <a05-board-generator.rst>`__.

About WPS
~~~~~~~~~

From release 2.4.2 and ahead, not using WPS will give an extra ~4.5KB in
heap.

In release 2.4.2 only, WPS is disabled by default and the board generator is
required to enable it:

``./tools/boards.txt.py --allowWPS --boardsgen``

`Read more <a05-board-generator.rst>`__.

For platformIO (and maybe other build environments), you will also need to add the build flag: -D NO_EXTRA_4K_HEAP

This manual selection is not needed starting from 2.5.0 (and in git
version).  WPS is always available, and not using it will give an extra
~4.5KB compared to releases until 2.4.1 included.

This Arduino library doesn't work on ESP. How do I make it work?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You would like to use this Arduino library with ESP8266 and it does not
perform. It is not listed among libraries verified to work with ESP8266.

`Read more <a03-library-does-not-work.rst>`__.

In the IDE, for ESP-12E that has 4M flash, I can choose 4M (1M FS) or 4M (3M FS). No matter what I select, the IDE tells me the maximum code space is about 1M. Where does my flash go?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The reason we cannot have more than 1MB of code in flash has to do with
a hardware limitation. Flash cache hardware on the ESP8266 only allows
mapping 1MB of code into the CPU address space at any given time. You
can switch mapping offset, so technically you can have more than 1MB
total, but switching such "banks" on the fly is not easy and efficient,
so we don't bother doing that. Besides, no one has so far complained
about 1MB of code space being insufficient for practical purposes.

The option to choose 3M or 1M filesystem is to optimize the upload time.
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
`Read more <a04-board-generic-is-unknown.rst>`__.


How to clear TCP PCBs in time-wait state ?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is not needed anymore:

PCBs in time-wait state are limited to 5 and removed when that number is
exceeded.

Ref.  `<https://github.com/d-a-v/esp82xx-nonos-linklayer/commit/420960dfc0dbe07114f7364845836ac333bc84f7>`__

For reference:

Time-wait PCB state helps TCP not confusing two consecutive connections with the
same (s-ip,s-port,d-ip,d-port) when the first is already closed but still
having duplicate packets lost in internet arriving later during the second.
Artificially clearing them is a workaround to help saving precious heap.

.. code:: cpp

    // no need for #include
    struct tcp_pcb;
    extern struct tcp_pcb* tcp_tw_pcbs;
    extern "C" void tcp_abort (struct tcp_pcb* pcb);

    void tcpCleanup (void) {
      while (tcp_tw_pcbs)
        tcp_abort(tcp_tw_pcbs);
    }

Ref.  `#1923 <https://github.com/esp8266/Arduino/issues/1923>`__


Why is there a board generator and what about it ?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The board generator is a python script originally intended to ease the
Arduino IDE's `boards.txt` configuration file about the multitude of
available boards, especially when common parameters have to be updated for
all of them.

This script is also used to manage uncommon options that are currently not
available in the IDE menu.

`Read more <a05-board-generator.rst>`__.

My WiFi won't reconnect after deep sleep using ``WAKE_RF_DISABLED``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When you implement deep sleep using ``WAKE_RF_DISABLED``, this forces what
appears to be a bare metal disabling of WiFi functionality, which is not
restored using ``WiFi.forceSleepWake()`` or ``WiFi.mode(WIFI_STA)``. If you need
to implement deep sleep with ``WAKE_RF_DISABLED`` and later connect to WiFi, you
will need to implement an additional (short) deep sleep using
``WAKE_RF_DEFAULT``.

Ref.  `#3072 <https://github.com/esp8266/Arduino/issues/3072>`__

My WiFi was previously automatically connected right after booting, but isn't anymore
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This was WiFi persistence.  Starting from version 3 of this core, WiFi is
indeed off at boot and is powered on only when starting to be used with the
regular API.

Read more at `former WiFi persistent mode <../esp8266wifi/generic-class.rst#persistent>`__.

How to resolve "undefined reference to ``flashinit``" error ?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Please read `flash layout <../filesystem.rst>`__ documentation entry.

How to specify global build defines and options?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By using a uniquely named `.h` file, macro definitions can be created and
globally used. Additionally, compiler command-line options can be embedded in
this file as a unique block comment.

`Read more <a06-global-build-options.rst>`__.
