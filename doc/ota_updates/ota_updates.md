---
title: OTA Update
---

## Table of Contents
 * [Basic Requirements](#basic-requirements)
 * [Arduino IDE](#arduino-ide)
 * [HTTP Server](#http-server)
 * [Stream Interface](#stream-interface)

## Introduction

OTA (Over the Air) update is the process of loading the firmware to ESP module using WiFi connection rather that a serial port. Such functionality became extremely useful in case of limited or no physical access to the module.

OTA may be done from:
 - [Arduino IDE](#arduino-ide)
 - [HTTP server](#http-server)

In any case first firmware upload have to be done over a serial port. If OTA routines are correctly implemented in sketch, then all subsequent uploads may be done over the air.

There is no imposed security on OTA process from being hacked. It is up to developer to ensure that updates are allowed only from legitimate / trusted source. Once update is complete module restarts and new code is executed. Developer should ensure that application running on module is shut down and restarted in safe manner. Chapters below provide additinal information  regarding security and safety of OTA process.

### Security

Module has to be exposed wirelessly to get it updated with a new code. That poses chances of module being violently hacked and loaded with some other firmware. To reduce likelihood of being hacked consider protecting your uploads with a password, selecting certain OTA port, etc. 

Check functionality provided with [ArduinoOTA](https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA) library that may improve security:
```cpp
void setPort(uint16_t port);
void setHostname(const char *hostname);
void setPassword(const char *password);
```
If possible implement other means of protection from being hacked, e.g. exposing module for uploads only according to specific schedule, trigger OTA only be user pressing dedicated “Update” button, etc.

### Safety

OTA process takes ESP’s resources and bandwidth during upload. Then module is restarted and a new sketch executed. Analyse and test how it affects functionality of your existing and new sketch. 

If ESP is placed in remote location and controlling some equipment, you should put additional attention what happens if operation of this equipment is suddenly interrupted by update process. Therefore decide how to put this equipment into safe state before starting the update.  For instance your module may be controlling a garden watering system in a sequence. If this sequence is not properly shut down and a water valve left open, your garden may be flooded if this valve is not closed after OTA is finished and module restarts.  

The following functions are provided with [ArduinoOTA](https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA) library and intended to handle functionality of your application during specific stages of OTA on or on an OTA error:
```cpp
void onStart(OTA_CALLBACK(fn));
void onEnd(OTA_CALLBACK(fn));
void onProgress(OTA_CALLBACK_PROGRESS(fn));
void onError(OTA_CALLBACK_ERROR (fn));
```

The following chapters provide more details and specific methods of doing OTA.


## Basic Requirements

- Flash chip size is 2x the size of the sketch.

Two separate, same size memory locations are used alternately. While existing sketch is executed from one location, the new sketch is saved to the other. After reset the new sketch is executed directly from where saved. Location of old sketch will be used for next OTA upload.


## Arduino IDE

Uploading modules wirelessly from Arduino IDE is intended for the following typical scenarios:
- during firmware development as a quicker alternative to loading over a serial
- for updating small quantity of modules
- only if modules are available on the same network as the computer with Arduino IDE

#### Requirements
 - The ESP and the computer must be connected to the same network.

#### Let's Do It

Currently there are two software configurations that support OTA updates
- [Classic OTA](#classic-ota-configuration): Arduino IDE 1.6.5 and [stable](https://github.com/esp8266/Arduino#staging-version-) (July 23, 2015) or [staging](https://github.com/esp8266/Arduino#staging-version-) (Sep 30, 2015) platform package that provides first OTA implementation, yet without support for [ArduinoOTA](https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA) library. This particular configuration is intended for less experienced users. It soon will be depreciated once implementation below is fully released. 
- [ArduinoOTA](#arduinoota-configuration): Arduino-PR-4107-BUILD-421 and latest git version of platform package that includes [ArduinoOTA](https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA) library. This configuration features preliminary build of Arduino IDE and is intended for more experienced users. Please mid your step.

Instructions below demonstrate how to configure both [Classic OTA](#classic-ota-configuration) and [ArduinoOTA](#arduinoota-configuration) using NodeMCU 1.0 board with ESP-12E.

##### Classic OTA Configuration

1. Before you begin, please make sure that you have the following installed:
 - Arduino IDE and ESP8266 board support as described under https://github.com/esp8266/Arduino#installing-with-boards-manager
 - [Python](https://www.python.org/) 2.7.10 (do not install Python 3.5.0 that is not supported):

    **Note:** Windows users should select “Add python.exe to Path” (see below – this option is not selected by default)

    ![Python installation set up](ota-ide-python-configuration.png)

2. Now prepare the sketch and configuration for the upload over a serial port.

 - Start Arduino IDE and load sketch DNS_SD_Arduino_OTA.ino  available under File >  Examples > ESP8266mDNS

    ![OTA sketch selection](ota-ide-sketch-selection.png)

    **Note:** This sketch is available only for stable (July 23, 2015) and staging (Sep 30, 2015) releases installed in Arduino IDE using https://github.com/esp8266/Arduino#installing-with-boards-manager. It was removed in [#980](https://github.com/esp8266/Arduino/pull/980) from Github repository.

 - Update ssid and pass in the sketch  so the module can join your WiFi network

    ![ssid and pass entry](ota-ide-ssid-pass-entry.png)

 - Configure upload parameters as below (you may need to adjust configuration if you are using a different module):

    ![configuration of serial upload](ota-ide-serial-upload-configuration.png)

3. Upload the sketch (Ctrl+U). Once done open Serial Monitor (Ctrl+Shift+M) and check if the module has joined your WiFi network. 

 ![check if module joined network](ota-ide-module-joined-wifi.png)

4. Only if module is connected to network, after a couple of seconds, the esp8266-ota port will show up in Arduino IDE:

 ![selection og OTA port](ota-ide-ota-port-selection.png)

5. Now get ready for your first OTA upload by changing configuration settings as follows:

 ![configuration of OTA upload](ota-ide-ota-upload-configuration.png)

 **Note:** If you do not see “Upload Using: OTA” option available for “NodeMCU 1.0 (ESP-12E Module)” board, please upload the latest [boards.txt](https://github.com/esp8266/Arduino/blob/master/boards.txt) file from Github repository, replace existing file and restart Arduino IDE.

6. If you have successfully completed all the above steps, you can upload (Ctrl+U) the same (or any other) sketch over OTA:

 ![OTA upload complete](ota-ide-ota-upload-complete.png)

**Note** To be able to upload your sketch over and over again using OTA, you need to embed OTA routines inside. Please use DNS_SD_Arduino_OTA.ino as an example.

##### ArduinoOTA Configuration

1. Get the following software:
 - Arduino-PR-4107-BUILD-421 - https://github.com/esp8266/Arduino/pull/984#issuecomment-155905800
 - Latest git version of pacakge - https://github.com/esp8266/Arduino#using-git-version-
 - Python 2.7.10

2. Proceed to step 2 under [Classic OTA Configuration](#classic-ota-configuration) using BasicOTA.ino or OTALeds.ino skech instead.

3. Carry on with remaining steps.


## HTTP Server

```ESPhttpUpdate``` class can check for updates and download a binary file from HTTP web server.
It is possible to download updates from every IP or domain address on the network or Internet.

#### Requirements
 - web server

#### Arduino code

##### Simple updater

Simple updater downloads the file every time the function is called.

```cpp
ESPhttpUpdate.update("192.168.0.2", 80, "/arduino.bin");
```

##### Advanced updater

Its possible to point update function to a script at the server.
If version string argument is given, it will be sent to the server.
Server side script can use this to check if update should be performed.

Server side script can respond as follows:
- response code 200, and send the firmware image,
- or response code 304 to notify ESP that no update is required.

```cpp
t_httpUpdate_return ret = ESPhttpUpdate.update("192.168.0.2", 80, "/esp/update/arduino.php", "optional current version string here");
switch(ret) {
	case HTTP_UPDATE_FAILED:
		Serial.println("[update] Update failed.");
		break;
	case HTTP_UPDATE_NO_UPDATES:
		Serial.println("[update] Update no Update.");
		break;
	case HTTP_UPDATE_OK:
		Serial.println("[update] Update ok."); // may not called we reboot the ESP
		break;
}
```

#### Server request handling

##### Simple updater

For the simple updater the server only needs to deliver the binary file for update.

##### Advanced updater

For advanced update management a script needs to run at the server side, for example a PHP script.
At every update request the the ESP sends some information in HTTP headers to the server.

Example header data:
```
	[HTTP_USER_AGENT] => ESP8266-http-Update
    [HTTP_X_ESP8266_STA_MAC] => 18:FE:AA:AA:AA:AA
    [HTTP_X_ESP8266_AP_MAC] => 1A:FE:AA:AA:AA:AA
    [HTTP_X_ESP8266_FREE_SPACE] => 671744
    [HTTP_X_ESP8266_SKETCH_SIZE] => 373940
    [HTTP_X_ESP8266_CHIP_SIZE] => 524288
    [HTTP_X_ESP8266_SDK_VERSION] => 1.3.0
    [HTTP_X_ESP8266_VERSION] => DOOR-7-g14f53a19
```

With this information the script now can check if a update is needed. It is also possible to deliver different binaries based on the MAC address for example.

Script example:
```php
<?PHP

header('Content-type: text/plain; charset=utf8', true);

function check_header($name, $value = false) {
	if(!isset($_SERVER[$name])) {
		return false;
	}
	if($value && $_SERVER[$name] != $value) {
		return false;
	}
	return true;
}

function sendFile($path) {
	header($_SERVER["SERVER_PROTOCOL"].' 200 OK', true, 200);
	header('Content-Type: application/octet-stream', true);
	header('Content-Disposition: attachment; filename='.basename($path));
	header('Content-Length: '.filesize($path), true);
	readfile($path);
}

if(!check_header('HTTP_USER_AGENT', 'ESP8266-http-Update')) {
	header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
	echo "only for ESP8266 updater!\n";
	exit();
}

if(
	!check_header('HTTP_X_ESP8266_STA_MAC') ||
	!check_header('HTTP_X_ESP8266_AP_MAC') ||
	!check_header('HTTP_X_ESP8266_FREE_SPACE') ||
	!check_header('HTTP_X_ESP8266_SKETCH_SIZE') ||
	!check_header('HTTP_X_ESP8266_CHIP_SIZE') ||
	!check_header('HTTP_X_ESP8266_SDK_VERSION') ||
	!check_header('HTTP_X_ESP8266_VERSION')
) {
	header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
	echo "only for ESP8266 updater! (header)\n";
	exit();
}

$db = array(
	"18:FE:AA:AA:AA:AA" => "DOOR-7-g14f53a19",
	"18:FE:AA:AA:AA:BB" => "TEMP-1.0.0"
);

if(isset($db[$_SERVER['HTTP_X_ESP8266_STA_MAC']])) {
	if($db[$_SERVER['HTTP_X_ESP8266_STA_MAC']] != $_SERVER['HTTP_X_ESP8266_VERSION']) ) {
		sendFile("./bin/".$db[$_SERVER['HTTP_X_ESP8266_STA_MAC']]."bin");
	} else {
		header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
	}
	exit();
}

header($_SERVER["SERVER_PROTOCOL"].' 500 no version for ESP MAC', true, 500);

```


## Updater class

TODO describe Updater class
