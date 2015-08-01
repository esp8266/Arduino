---
title: Supported Hardware
---

- [Adafruit HUZZAH ESP8266 (ESP-12)](#adafruit-huzzah-esp8266-esp-12)
- [NodeMCU 0.9](#nodemcu-0-9)
- [NodeMCU 1.0](#nodemcu-1-0)
- [Olimex MOD-WIFI-ESP8266](#olimex-mod-wifi-esp8266)
- [SparkFun ESP8266 Thing](#sparkfun-esp8266-thing)
- [SweetPea ESP-210](#sweetpea-esp-210)
- [Generic ESP8266 modules](#generic-esp8266-modules)

### Adafruit HUZZAH ESP8266 (ESP-12)

*TODO: add notes*

### NodeMCU 0.9 <a name="nodemcu-0-9"></a>

#### Pin mapping

Pin numbers written on the board itself do not correspond to ESP8266 GPIO pin numbers. Constants are defined to make using this board easier:

```C++
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
```

If you want to use NodeMCU pin 5, use D5 for pin number, and it will be translated to 'real' GPIO pin 14.

### NodeMCU 1.0 <a name="nodemcu-1-0"></a>

*TODO: add notes*

### Olimex MOD-WIFI-ESP8266

*TODO: add notes*

### SparkFun ESP8266 Thing ###

Product page: https://www.sparkfun.com/products/13231

*TODO: add notes*

### SweetPea ESP-210

*TODO: add notes*

### Generic ESP8266 modules

These modules come in different form factors and pinouts. See the page at ESP8266 community wiki for more info:
[ESP8266 Module Family](http://www.esp8266.com/wiki/doku.php?id=esp8266-module-family).

Usually these modules have no bootstapping resistors on board, insufficient decoupling capacitors, no voltage regulator, no reset circuit, and no USB-serial adapter. This makes using them somewhat tricky, compared to development boards which add these features.

In order to use these modules, make sure to observe the following:

- **Provide sufficient power to the module.** For stable use of the ESP8266 a power supply with 3.3V and >= 250mA is required. Using the power available from USB to Serial adapter is not recommended, these adapters typically do not supply enough current to run ESP8266 reliably in every situation. An external supply or regulator along with filtering capacitors is preferred.

- **Connect bootstapping resistors** to GPIO0, GPIO2, GPIO15 according to the schematics below.

- **Put ESP8266 into bootloader mode** before uploading code.

### Serial Adapter

There are many different USB to Serial adapters / boards.

* Note
 - for full upload management you need RTS and DTR
 - the chip need to have 3.3V TTL (5V may damage the chip)
 - not all board have all pins of the ICs as breakout (check before order)
 - CTS and DSR are not useful for upload (they are Inputs)

* Working ICs
 - FT232RL
 - CP2102
 - CH340G
 - maybe others (drop a comment)

### Minimal Hardware Setup for Bootloading and Usage

ESPxx Hardware

| PIN           | Resistor | Serial Adapter |
| ------------- | -------- | -------------- |
| VCC           |          | VCC (3.3V)     |
| GND           |          | GND            |
| TX or GPIO2*  |          | RX             |
| RX            |          | TX             |
| GPIO0         | PullUp   | DTR            |
| Reset*        | PullUp   | RTS            |
| GPIO15*       | PullDown |                |
| CH_PD         | PullUp   |                |

* Note
 - GPIO15 is also named MTDO
 - Reset is also named RSBT or REST (adding PullUp improves the stability of the Module)
 - GPIO2 is alternative TX for the boot loader mode
 - **Directly connecting a pin to VCC or GND is not a substitute for a PullUp or PullDown resistor, doing this can break upload management and the serial console, instability has also been noted in some cases.**

### ESP to Serial
![ESP to Serial](https://raw.githubusercontent.com/Links2004/Arduino/esp8266/docs/ESP_to_serial.png)

#### Minimal Hardware Setup for Bootloading only ##
ESPxx Hardware

| PIN           | Resistor | Serial Adapter  |
| ------------- | -------- | --------------- |
| VCC           |          | VCC (3.3V)      |
| GND           |          | GND             |
| TX or GPIO2   |          | RX              |
| RX            |          | TX              |
| GPIO0         |          | GND             |
| Reset         |          | RTS*            |
| GPIO15        | PullDown |                 |
| CH_PD         | PullUp   |                 |

* Note
	- if no RTS is used a manual power toggle is needed

#### Minimal Hardware Setup for Running only ##

ESPxx Hardware

| PIN           | Resistor | Power supply    |
| ------------- | -------- | --------------- |
| VCC           |          | VCC (3.3V)      |
| GND           |          | GND             |
| GPIO0         | PullUp   |                 |
| GPIO15        | PullDown |                 |
| CH_PD         | PullUp   |                 |

### Minimal
![ESP min](https://raw.githubusercontent.com/Links2004/Arduino/esp8266/docs/ESP_min.png)

### Improved Stability
![ESP improved stability](https://raw.githubusercontent.com/Links2004/Arduino/esp8266/docs/ESP_improved_stability.png)
