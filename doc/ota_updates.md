---
title: OTA Update
---

## Table of Contents
 * [Requirements](#Requirements)
 * [Arduino IDE](#arduino-ide)
 * [HTTP Server](#http-server)
 * [Stream Interface](#stream-interface)
 
## Requirements

Basic requirement:
- Flash chip size is 2x the size of the sketch
 
## Arduino IDE

TODO describe Arduino IDE OTA process

### Requirements
 - The ESP and the Computer must be connected to the Same network.


## HTTP Server

the ```ESPhttpUpdate``` class can check for updates and download a binary file form a HTTP web server.
It is possible to download updates from every IP or domain address on the Network or Internet.


### Requirements
 - web server


### Arduino code

simple updater:
```cpp
ESPhttpUpdate.update("192.168.0.2", 80, "/arduino.bin");
```

advanced:
```cpp
t_httpUpdate_return ret = ESPhttpUpdate.update("192.168.0.2", 80, "/esp/update/arduino.php", "optional current version string here");
switch(ret) {
	case HTTP_UPDATE_FAILD:
		Serial.println("[update] Update fail.");
		break;
	case HTTP_UPDATE_NO_UPDATES:
		Serial.println("[update] Update no Update.");
		break;
	case HTTP_UPDATE_OK:
		Serial.println("[update] Update ok."); // may not called we reboot the ESP 
		break;
}
```

### Server request handling

TODO server side 

## Stream Interface

TODO describe Stream Interface update proccess

```cpp
	ESP.updateSketch(client, length);
```
 
