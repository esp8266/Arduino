----------------------------- Delete below -----------------------------

If your issue is a general question, starts similar to "How do I..", is related to 3rd party libs, or is related to hardware, please discuss at a community forum like esp8266.com.

INSTRUCTIONS
============
If you do not follow these instructions, your issue may be dismissed.

1. Follow the checklist under Basic Infos and fill in the [ ] spaces with an X.
2. Fill in all the fields under Platform and Settings in IDE marked with [ ] (pick the correct option for you in each case, delete the others).
3. If you haven't already done so, test your issue against current master branch (aka latest git), because it may have been already fixed.
4. Describe your problem.
5. If you have a STACK DUMP decode it:

https://arduino-esp8266.readthedocs.io/en/latest/Troubleshooting/stack_dump.html

6. Include a Minimal Complete Reproducible Example sketch that shows your issue. Do not include your entire project, or a huge piece of code.
7. Include debug messages:

https://arduino-esp8266.readthedocs.io/en/latest/Troubleshooting/debugging.html 

8. Use markup (buttons above) and the Preview tab to check what the issue will look like.
9. Delete these instructions from the above to the below marker lines before submitting this issue.


----------------------------- Delete above -----------------------------

### Basic Infos

- [ ] This issue complies with the [issue POLICY doc](https://github.com/esp8266/Arduino/blob/master/POLICY.md).
- [ ] I have read the documentation at [readthedocs](https://arduino-esp8266.readthedocs.io/en/latest) and the issue is not addressed there.
- [ ] I have tested that the issue is present in current master branch (aka latest git).
- [ ] I have searched the issue tracker for a similar issue.
- [ ] If there is a stack dump, I have decoded it.
- [ ] I have filled out all fields below.

#### Platform

- Hardware:					[ESP-12|ESP-01|ESP-07|ESP8285 device|other]
- Core Version:			[latest git hash or date]
- Development Env:		[Arduino IDE|Platformio|Make|other]
- Operating System:		[Windows|Ubuntu|MacOS]

### Settings in IDE

- Module:  			[Generic ESP8266 Module|Wemos D1 mini r2|Nodemcu|other]
- Flash Mode:			[qio|dio|other]
- Flash Size: 		[4MB/1MB]
- lwip Variant:   [v1.4|v2 Lower Memory|Higher Bandwidth]
- Reset Method:		[ck|nodemcu]
- Flash Frequency:	[40Mhz]
- CPU Frequency:		[80Mhz|160MHz]
- Upload Using:		[OTA|SERIAL]
- Upload Speed:   [115200|other] (serial upload only)


### Problem Description

Detailed problem description goes here.


### [MCVE](https://stackoverflow.com/help/mcve) Sketch

```cpp

#include <Arduino.h>

void setup() {

}

void loop() {

}

```

### Debug Messages

```
Debug messages go here
```


