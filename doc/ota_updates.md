---
title: OTA Update
---

## Table of Contents
 * [Basic Requirements](#basic-requirements)
 * [Arduino IDE](#arduino-ide)
 * [HTTP Server](#http-server)
 * [Stream Interface](#stream-interface)
 
## Basic Requirements

- Flash chip size is 2x the size of the sketch
 
## Arduino IDE

TODO describe Arduino IDE OTA process

#### Requirements
 - The ESP and the Computer must be connected to the Same network.


## HTTP Server

the ```ESPhttpUpdate``` class can check for updates and download a binary file form a HTTP web server.
It is possible to download updates from every IP or domain address on the Network or Internet.


#### Requirements
 - web server


#### Arduino code

##### simple updater

the Simple Updater downloads the File every time the function is called.

```cpp
ESPhttpUpdate.update("192.168.0.2", 80, "/arduino.bin");
```

##### advanced updater

Its possible to point to a script at the server.
If a version String is delivered to the Function this String will be send to the server.
A Server side Update check is now possible.

the Server can return a binary file for update (Header 200)
or it return header 304 to notify the ESP that no Update is needed.

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

#### Server request handling

##### simple updater

for the simple Updater the Server only needs to deliver the binary file for update.

##### advanced updater

for advanced update management a Script needs to run at the Server side, for example a PHP script.
at every Update request the the ESP sends some informations in the Header to the Server

example Header data:
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

with this information the script now can check if a update is needed.
It is also possible to deliver different binary´s based on the MAC address for example.

script example:
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
}

header($_SERVER["SERVER_PROTOCOL"].' 500 no version for ESP MAC', true, 500);

```


## Stream Interface

TODO describe Stream Interface update proccess

```cpp
	ESP.updateSketch(client, length);
```
 
