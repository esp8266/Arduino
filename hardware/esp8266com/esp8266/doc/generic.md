
## Upload via serial port
Pick the correct serial port.
You need to put ESP8266 into bootloader mode before uploading code.

## Power Supply

For stable use of the ESP8266 a power supply with 3.3V and >= 250mA is required.

* Note
 - Using the Power available from the USB to Serial adapter is not reccomended, these adapters typically do not supply enough current to run the ESP8266 reliably in every situation, an external supply or regulator is preferred.

## Serial Adapter

There are many different USB to Serial adapters / boards.

* Note
 - for full upload management you need RTS and DTR
 - the chip need to have 3.3V TTL (5V may damage the chip)
 - not all board have all pins of the ICs as breakout (check before order)
 - CTS and DSR are not useful for upload (they are Inputs)

* Working ICs
 - FT232RL
 - CP2102
 - may others (drop a comment)

## Minimal Hardware Setup for Bootloading and Usage

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

## Minimal Hardware Setup for Bootloading only ##
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

## Minimal Hardware Setup for Running only ##

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
