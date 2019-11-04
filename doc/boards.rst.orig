Boards
======

Generic ESP8266 Module
----------------------

These modules come in different form factors and pinouts. See the page at ESP8266 community wiki for more info: `ESP8266 Module Family <http://www.esp8266.com/wiki/doku.php?id=esp8266-module-family>`__.

Usually these modules have no bootstapping resistors on board, insufficient decoupling capacitors, no voltage regulator, no reset circuit, and no USB-serial adapter. This makes using them somewhat tricky, compared to development boards which add these features.

In order to use these modules, make sure to observe the following:

-  **Provide sufficient power to the module.** For stable use of the ESP8266 a power supply with 3.3V and >= 250mA is required. Using the power available from USB to Serial adapter is not recommended, these adapters typically do not supply enough current to run ESP8266 reliably in every situation. An external supply or regulator alongwith filtering capacitors is preferred.

-  **Connect bootstapping resistors** to GPIO0, GPIO2, GPIO15 according to the schematics below.

-  **Put ESP8266 into bootloader mode** before uploading code.

Serial Adapter
--------------

There are many different USB to Serial adapters / boards. To be able to put ESP8266 into bootloader mode using serial handshaking lines, you need the adapter which breaks out RTS and DTR outputs. CTS and DSR are not useful for upload (they are inputs). Make sure the adapter can work with 3.3V IO voltage: it should have a jumper or a switch to select between 5V and 3.3V, or be marked as 3.3V only.

Adapters based around the following ICs should work:

-  FT232RL
-  CP2102
-  CH340G

PL2303-based adapters are known not to work on Mac OS X. See https://github.com/igrr/esptool-ck/issues/9 for more info.

Minimal Hardware Setup for Bootloading and Usage
------------------------------------------------

+-----------------+------------+------------------+
| PIN             | Resistor   | Serial Adapter   |
+=================+============+==================+
| VCC             |            | VCC (3.3V)       |
+-----------------+------------+------------------+
| GND             |            | GND              |
+-----------------+------------+------------------+
| TX or GPIO2\*   |            | RX               |
+-----------------+------------+------------------+
| RX              |            | TX               |
+-----------------+------------+------------------+
| GPIO0           | PullUp     | DTR              |
+-----------------+------------+------------------+
| Reset\*         | PullUp     | RTS              |
+-----------------+------------+------------------+
| GPIO15\*        | PullDown   |                  |
+-----------------+------------+------------------+
| CH\_PD          | PullUp     |                  |
+-----------------+------------+------------------+

-  Note
-  GPIO15 is also named MTDO
-  Reset is also named RSBT or REST (adding PullUp improves the
   stability of the module)
-  GPIO2 is alternative TX for the boot loader mode
-  **Directly connecting a pin to VCC or GND is not a substitute for a
   PullUp or PullDown resistor, doing this can break upload management
   and the serial console, instability has also been noted in some
   cases.**

ESP to Serial
-------------

.. figure:: ESP_to_serial.png
   :alt: ESP to Serial

   ESP to Serial

Minimal Hardware Setup for Bootloading only
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ESPxx Hardware

+---------------+------------+------------------+
| PIN           | Resistor   | Serial Adapter   |
+===============+============+==================+
| VCC           |            | VCC (3.3V)       |
+---------------+------------+------------------+
| GND           |            | GND              |
+---------------+------------+------------------+
| TX or GPIO2   |            | RX               |
+---------------+------------+------------------+
| RX            |            | TX               |
+---------------+------------+------------------+
| GPIO0         |            | GND              |
+---------------+------------+------------------+
| Reset         |            | RTS\*            |
+---------------+------------+------------------+
| GPIO15        | PullDown   |                  |
+---------------+------------+------------------+
| CH\_PD        | PullUp     |                  |
+---------------+------------+------------------+

-  Note
-  if no RTS is used a manual power toggle is needed

Minimal Hardware Setup for Running only
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ESPxx Hardware

+----------+------------+----------------+
| PIN      | Resistor   | Power supply   |
+==========+============+================+
| VCC      |            | VCC (3.3V)     |
+----------+------------+----------------+
| GND      |            | GND            |
+----------+------------+----------------+
| GPIO0    | PullUp     |                |
+----------+------------+----------------+
| GPIO15   | PullDown   |                |
+----------+------------+----------------+
| CH\_PD   | PullUp     |                |
+----------+------------+----------------+

Minimal
-------

.. figure:: ESP_min.png
   :alt: ESP min

   ESP min

Improved Stability
------------------

.. figure:: ESP_improved_stability.png
   :alt: ESP improved stability

   ESP improved stability

Boot Messages and Modes
-----------------------

The ESP module checks at every boot the Pins 0, 2 and 15. based on them its boots in different modes:

+----------+---------+---------+------------------------------------+
| GPIO15   | GPIO0   | GPIO2   | Mode                               |
+==========+=========+=========+====================================+
| 0V       | 0V      | 3.3V    | Uart Bootloader                    |
+----------+---------+---------+------------------------------------+
| 0V       | 3.3V    | 3.3V    | Boot sketch (SPI flash)            |
+----------+---------+---------+------------------------------------+
| 3.3V     | x       | x       | SDIO mode (not used for Arduino)   |
+----------+---------+---------+------------------------------------+

at startup the ESP prints out the current boot mode example:

::

    rst cause:2, boot mode:(3,6)

note: - GPIO2 is used as TX output and the internal Pullup is enabled on boot.

rst cause
~~~~~~~~~

+----------+------------------+
| Number   | Description      |
+==========+==================+
| 0        | unknown          |
+----------+------------------+
| 1        | normal boot      |
+----------+------------------+
| 2        | reset pin        |
+----------+------------------+
| 3        | software reset   |
+----------+------------------+
| 4        | watchdog reset   |
+----------+------------------+

boot mode
~~~~~~~~~

the first value respects the pin setup of the Pins 0, 2 and 15.

+----------+----------+---------+---------+-------------+
| Number   | GPIO15   | GPIO0   | GPIO2   | Mode        |
+==========+==========+=========+=========+=============+
| 0        | 0V       | 0V      | 0V      | Not valid   |
+----------+----------+---------+---------+-------------+
| 1        | 0V       | 0V      | 3.3V    | Uart        |
+----------+----------+---------+---------+-------------+
| 2        | 0V       | 3.3V    | 0V      | Not valid   |
+----------+----------+---------+---------+-------------+
| 3        | 0V       | 3.3V    | 3.3V    | Flash       |
+----------+----------+---------+---------+-------------+
| 4        | 3.3V     | 0V      | 0V      | SDIO        |
+----------+----------+---------+---------+-------------+
| 5        | 3.3V     | 0V      | 3.3V    | SDIO        |
+----------+----------+---------+---------+-------------+
| 6        | 3.3V     | 3.3V    | 0V      | SDIO        |
+----------+----------+---------+---------+-------------+
| 7        | 3.3V     | 3.3V    | 3.3V    | SDIO        |
+----------+----------+---------+---------+-------------+

note: - number = ((GPIO15 << 2) \| (GPIO0 << 1) \| GPIO2);

Generic ESP8285 Module
----------------------

ESP8285 (`datasheet <http://www.espressif.com/sites/default/files/0a-esp8285_datasheet_en_v1.0_20160422.pdf>`__) is a multi-chip package which contains ESP8266 and 1MB flash. All points related to bootstrapping resistors and recommended circuits listed above apply to ESP8285 as well.

Note that since ESP8285 has SPI flash memory internally connected in DOUT mode, pins 9 and 10 may be used as GPIO / I2C / PWM pins.

ESPDuino (ESP-13 Module)
------------------------

*TODO*

Adafruit Feather HUZZAH ESP8266
-------------------------------

The Adafruit Feather HUZZAH ESP8266 is an Arduino-compatible Wi-Fi development board powered by Ai-Thinker's ESP-12S, clocked at 80 MHz at 3.3V logic. A high-quality SiLabs CP2104 USB-Serial chip is included so that you can upload code at a blistering 921600 baud for fast development time. It also has auto-reset so no noodling with pins and reset button pressings. A 3.7V Lithium polymer battery connector is included, making it ideal for portable projects. The Adafruit Feather HUZZAH ESP8266 will automatically recharge a connected battery when USB power is available.

Product page: https://www.adafruit.com/product/2821

Invent One
----------

The Invent One is an Arduino-compatible Wi-Fi development board powered by Ai-Thinker's ESP-12F, clocked at 80 MHz at 3.3V logic. It has an onboard ADC (PCF8591) so that you can have multiple analog inputs to work with. More information can be found here: https://blog.inventone.ng

Product page: https://inventone.ng

XinaBox CW01
------------

The XinaBox CW01(ESP8266) is an Arduino-compatible Wi-Fi development board powered by an ESP-12F, clocked at 80 MHz at 3.3V logic. The CW01 has an onboard RGB LED and 3 xBUS connection ports.

Product page: https://xinabox.cc/products/CW01

ESPresso Lite 1.0
-----------------

ESPresso Lite 1.0 (beta version) is an Arduino-compatible Wi-Fi development board powered by Espressif System's own ESP8266 WROOM-02 module. It has breadboard-friendly breakout pins with in-built LED, two reset/flash buttons and a user programmable button . The operating voltage is 3.3VDC, regulated with 800mA maximum current. Special distinctive features include on-board I2C pads that allow direct connection to OLED LCD and sensor boards.

ESPresso Lite 2.0
-----------------

ESPresso Lite 2.0 is an Arduino-compatible Wi-Fi development board based on an earlier V1 (beta version). Re-designed together with Cytron Technologies, the newly-revised ESPresso Lite V2.0 features the auto-load/auto-program function, eliminating the previous need to reset the board manually before flashing a new program. It also feature two user programmable side buttons and a reset button. The special distinctive features of on-board pads for I2C sensor and actuator is retained.

Phoenix 1.0
-----------

Product page: http://www.espert.co

Phoenix 2.0
-----------

Product page: http://www.espert.co

NodeMCU 0.9 (ESP-12 Module)
---------------------------

Pin mapping
~~~~~~~~~~~

Pin numbers written on the board itself do not correspond to ESP8266 GPIO pin numbers. Constants are defined to make using this board easier:

.. code:: c++

    static const uint8_t D0   = 16;
    static const uint8_t D1   = 5;
    static const uint8_t D2   = 4;
    static const uint8_t D3   = 0;
    static const uint8_t D4   = 2;
    static const uint8_t D5   = 14;
    static const uint8_t D6   = 12;
    static const uint8_t D7   = 13;
    static const uint8_t D8   = 15;
    static const uint8_t D9   = 3;
    static const uint8_t D10  = 1;

If you want to use NodeMCU pin 5, use D5 for pin number, and it will be translated to 'real' GPIO pin 14.

NodeMCU 1.0 (ESP-12E Module)
----------------------------

This module is sold under many names for around $6.50 on AliExpress and it's one of the cheapest, fully integrated ESP8266 solutions.

It's an open hardware design with an ESP-12E core and 4 MB of SPI flash.

According to the manufacturer, "with a micro USB cable, you can connect NodeMCU devkit to your laptop and flash it without any trouble". This is more or less true: the board comes with a CP2102 onboard USB to serial adapter which just works, well, the majority of the time. Sometimes flashing fails and you have to reset the board by holding down FLASH +
RST, then releasing FLASH, then releasing RST. This forces the CP2102 device to power cycle and to be re-numbered by Linux.

The board also features a NCP1117 voltage regulator, a blue LED on GPIO16 and a 220k/100k Ohm voltage divider on the ADC input pin.
The ESP-12E usually has a led connected on GPIO2.

Full pinout and PDF schematics can be found `here <https://github.com/nodemcu/nodemcu-devkit-v1.0>`__

Olimex MOD-WIFI-ESP8266(-DEV)
-----------------------------

This board comes with 2 MB of SPI flash and optional accessories (e.g. evaluation board ESP8266-EVB or BAT-BOX for batteries).

The basic module has three solder jumpers that allow you to switch the operating mode between SDIO, UART and FLASH.

The board is shipped for FLASH operation mode, with jumpers TD0JP=0, IO0JP=1, IO2JP=1.

Since jumper IO0JP is tied to GPIO0, which is PIN 21, you'll have to ground it before programming with a USB to serial adapter and reset the board by power cycling it.

UART pins for programming and serial I/O are GPIO1 (TXD, pin 3) and GPIO3 (RXD, pin 4).

You can find the board schematics `here <https://github.com/OLIMEX/ESP8266/blob/master/HARDWARE/MOD-WIFI-ESP8266-DEV/MOD-WIFI-ESP8266-DEV_schematic.pdf>`__

SparkFun ESP8266 Thing
----------------------

Product page: https://www.sparkfun.com/products/13231

SparkFun ESP8266 Thing Dev
--------------------------

Product page: https://www.sparkfun.com/products/13711

SweetPea ESP-210
----------------

*TODO*

LOLIN(WEMOS) D1 R2 & mini
-------------------------

Product page: https://www.wemos.cc/

LOLIN(WEMOS) D1 mini Pro
------------------------

Product page: https://www.wemos.cc/

LOLIN(WEMOS) D1 mini Lite
-------------------------

Parameters in Arduino IDE:
~~~~~~~~~~~~~~~~~~~~~~~~~~

- Card: "WEMOS D1 Mini Lite"
- Flash Size: "1M (512K SPIFFS)"
- CPU Frequency: "80 Mhz"

Power:
~~~~~~

- 5V pin : 4.7V 500mA output when the board is powered by USB ; 3.5V-6V input
- 3V3 pin : 3.3V 500mA regulated output
- Digital pins : 3.3V 30mA.

links:
~~~~~~

- Product page: https://www.wemos.cc/
- Board schematic: https://wiki.wemos.cc/_media/products:d1:sch_d1_mini_lite_v1.0.0.pdf
- ESP8285 datasheet: https://www.espressif.com/sites/default/files/0a-esp8285_datasheet_en_v1.0_20160422.pdf
- Voltage regulator datasheet: http://pdf-datasheet.datasheet.netdna-cdn.com/pdf-down/M/E/6/ME6211-Microne.pdf

WeMos D1 R1
-----------

Product page: https://www.wemos.cc/

ESPino (ESP-12 Module)
----------------------

ESPino integrates the ESP-12 module with a 3.3v regulator, CP2104 USB-Serial bridge and a micro USB connector for easy programming. It is designed for fitting in a breadboard and has an RGB Led and two buttons for easy prototyping.

For more information about the hardware, pinout diagram and programming procedures, please see the `datasheet <https://github.com/makerlabmx/ESPino-tools/raw/master/Docs/ESPino-Datasheet-EN.pdf>`__.

Product page: http://www.espino.io/en

ThaiEasyElec's ESPino
---------------------

ESPino by ThaiEasyElec using WROOM-02 module from Espressif Systems with 4 MB Flash.

We will update an English description soon. - Product page:
http://thaieasyelec.com/products/wireless-modules/wifi-modules/espino-wifi-development-board-detail.html
- Schematics:
www.thaieasyelec.com/downloads/ETEE052/ETEE052\_ESPino\_Schematic.pdf -
Dimensions:
http://thaieasyelec.com/downloads/ETEE052/ETEE052\_ESPino\_Dimension.pdf
- Pinouts:
http://thaieasyelec.com/downloads/ETEE052/ETEE052\_ESPino\_User\_Manual\_TH\_v1\_0\_20160204.pdf (Please see pg. 8)

WifInfo
-------

WifInfo integrates the ESP-12 or ESP-07+Ext antenna module with a 3.3v regulator and the hardware to be able to measure French telemetry issue from ERDF powering meter serial output. It has a USB connector for powering, an RGB WS2812 Led, 4 pins I2C connector to fit OLED or sensor, and two buttons + FTDI connector and auto reset feature.

For more information, please see WifInfo related `blog <http://hallard.me/category/wifinfo/>`__ entries, `github <https://github.com/hallard/WifInfo>`__ and `community <https://community.hallard.me/category/16/wifinfo>`__ forum.

Arduino
-------

*TODO*

4D Systems gen4 IoD Range
-------------------------

gen4-IoD Range of ESP8266 powered Display Modules by 4D Systems.

2.4", 2.8" and 3.2" TFT LCD with uSD card socket and Resistive Touch. Chip Antenna + uFL Connector.

Datasheet and associated downloads can be found on the 4D Systems product page.

The gen4-IoD range can be programmed using the Arduino IDE and also the 4D Systems Workshop4 IDE, which incorporates many additional graphics benefits. GFX4d library is available, along with a number of demo applications.

- Product page: http://www.4dsystems.com.au/product/gen4-IoD

Digistump Oak
-------------

The Oak requires an `Serial Adapter`_ for a serial connection or flashing; its micro USB port is only for power.

To make a serial connection, wire the adapter's **TX to P3**, **RX to P4**, and **GND** to **GND**.  Supply 3.3v from the serial adapter if not already powered via USB.

To put the board into bootloader mode, configure a serial connection as above, connect **P2 to GND**, then re-apply power.  Once flashing is complete, remove the connection from P2 to GND, then re-apply power to boot into normal mode.

WiFiduino
---------

Product page: https://wifiduino.com/esp8266

Amperka WiFi Slot
-----------------

Product page: http://wiki.amperka.ru/wifi-slot

Seeed Wio Link
--------------

Wio Link is designed to simplify your IoT development. It is an ESP8266 based open-source Wi-Fi development board to create IoT applications by virtualizing plug-n-play modules to RESTful APIs with mobile APPs. Wio Link is also compatible with the Arduino IDE.

Please DO NOTICE that you MUST pull up pin 15 to enable the power for Grove ports, the board is designed like this for the purpose of peripherals power management.

Product page: https://www.seeedstudio.com/Wio-Link-p-2604.html

ESPectro Core
-------------

ESPectro Core is ESP8266 development board as the culmination of our 3+ year experience in exploring and developing products with ESP8266 MCU.

Initially designed for kids in mind, everybody should be able to use it. Yet it's still hacker-friendly as we break out all ESP8266 ESP-12F pins.

More details at https://shop.makestro.com/product/espectrocore/

