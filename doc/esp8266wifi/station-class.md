---
title: ESP8266WiFi Station Class
---

[ESP8266WiFi Library :back:](readme.md#station)


## Station Class

The number of features provided by ESP8266 in the station mode is far more extensive than covered in original [Arduino WiFi library](https://www.arduino.cc/en/Reference/WiFi). Therefore, instead of supplementing original documentation, we have decided to write a new one from scratch.

Description of station class has been broken down into four parts. First discusses methods to establish connection to an access point. Second provides methods to manage connection like e.g. `reconnect` or `isConnected`. Third covers properties to obtain information about connection like MAC or IP address. Finally the fourth section provides alternate methods to connect like e.g. Wi-Fi Protected Setup (WPS).


## Table of Contents
  * [Start Here](#start-here)
    * [begin](#begin)
    * [config](#config)
  * [Manage Connection](#manage-connection)
    * [reconnect](#reconnect)
    * [disconnect](#disconnect)
    * [isConnected](#isconnected)
    * [setAutoConnect](#setautoconnect)
    * [getAutoConnect](#getautoconnect)
    * [setAutoReconnect](#setautoreconnect)
    * [waitForConnectResult](#waitforconnectresult)
  * [Configuration](#configuration)
    * [macAddress](#macAddress)
    * [localIP](#localip)
    * [subnetMask](#subnetmask)
    * [gatewayIP](#gatewayip)
    * [dnsIP](#dnsip)
    * [hostname](#hostname)
    * [status](#status)
    * [SSID](#ssid)
    * [psk](#psk)
    * [BSSID](#bssid)
    * [RSSI](#rssi)
  * [Connect Different](#connect-different)
    * [WPS](#wps)
    * [Smart Config](#smart-config)


Points below provide description and code snippets how to use particular methods. 

For more code samples please refer to separate section with [examples :arrow_right:](station-examples.md) dedicated specifically to the Station Class.


### Start Here

Switching the module to Station mode is done with `begin` function. Typical parameters passed to `begin` include SSID and password, so module can connect to specific Access Point.

```cpp
WiFi.begin(ssid, password)
```

By default, ESP will attempt to reconnect to Wi-Fi network whenever it is disconnected. There is no need to handle this by separate code. A good way to simulate disconnection would be to reset the access point. ESP will report disconnection, and then try to reconnect automatically.


#### begin

There are several version (called *[function overloads](https://en.wikipedia.org/wiki/Function_overloading)* in C++) of `begin` function. One was presented just above: `WiFi.begin(ssid, password)`. Overloads provide flexibility in number or type of accepted parameters.

The simplest overload of `begin` is as follows:

```cpp
WiFi.begin()
```

Calling it will instruct module to switch to the station mode and connect to the last used access point basing on configuration saved in flash memory. 

Below is the syntax of another overload of `begin` with the all possible parameters:

```cpp
WiFi.begin(ssid, password, channel, bssid, connect)
```

Meaning of parameters is as follows:
  * `ssid` - a character string containing the SSID of Access Point we would like to connect to, may have up to 32 characters
  * `password` to the access point, a character string that should be minimum 8 characters long and not longer than 64 characters
  * `channel` of AP, if we like to operate using specific channel, otherwise this parameter may be omitted
  * `bssid` - mac address of AP, this parameter is also optional
  * `connect` - a `boolean` parameter that if set to `false`, will instruct module just to save the other parameters without actually establishing connection to the access point


#### config

Disable [DHCP](https://en.wikipedia.org/wiki/Dynamic_Host_Configuration_Protocol) client (Dynamic Host Configuration Protocol) and set the IP configuration of station interface to user defined arbitrary values. The interface will be a static IP configuration instead of values provided by DHCP.

```cpp
WiFi.config(local_ip, gateway, subnet, dns1, dns2) 
```

Function will return `true` if configuration change is applied successfully. If configuration can not be applied, because e.g. module is not in station or station + soft access point mode, then `false` will be returned.

The following IP configuration may be provided:

 * `local_ip` - enter here IP address you would like to assign the ESP station's interface
 * `gateway` - should contain IP address of gateway (a router) to access external networks 
 * `subnet` - this is a mask that defines the range of IP addresses of the local network
 * `dns1`, `dns2` - optional parameters that define IP addresses of Domain Name Servers (DNS) that maintain a directory of domain names (like e.g. *www.google.co.uk*) and translate them for us to IP addresses

*Example code:*
```cpp
#include <ESP8266WiFi.h>

const char* ssid = "********";
const char* password = "********";

IPAddress staticIP(192,168,1,22);
IPAddress gateway(192,168,1,9);
IPAddress subnet(255,255,255,0);

void setup(void)
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  WiFi.config(staticIP, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {}
```
*Example output:*
```
Connecting to sensor-net
.
Connected, IP address: 192.168.1.22
```
Please note that station with static IP configuration usually connects to the network faster. In the above example it took about 500ms (one dot `.` displayed). This is because obtaining of IP configuration by DHCP client takes time and in this case this step is skipped.


### Manage Connection

#### reconnect

Reconnect the station. This is done by disconnecting from the access point an then initiating connection back to the same AP. 

```cpp
WiFi.reconnect() 
```

Notes:
  1. Station should be already connected to an access point. If this is not the case, then function will return `false` not performing any action.
  2. If `true` is returned it means that connection sequence has been successfully started. User should still check for connection status, waiting until `WL_CONNECTED` is reported:

```cpp
WiFi.reconnect();
while (WiFi.status() != WL_CONNECTED)
{
  delay(500);
  Serial.print(".");
}
```


#### disconnect

Sets currently configured SSID and password to `null` values and disconnects the station from an access point.

```cpp
WiFi.disconnect(wifioff) 
```

The `wifioff` is an optional `boolean` parameter. If set to `true`, then the station mode will be turned off. 


#### isConnected

Returns `true` if Station is connected to an access point or `false` if not. 

```cpp
WiFi.isConnected() 
```


#### setAutoConnect

Configure module to automatically connect on power on to the last used access point.

```cpp
WiFi.setAutoConnect(autoConnect) 
```

The `autoConnect` is an optional parameter. If set to `false` then auto connection functionality up will be disabled. If omitted or set to `true`, then auto connection will be enabled.


#### getAutoConnect

This is "companion" function to `setAutoConnect()`. It returns `true` if module is configured to automatically connect to last used access point on power on.

```cpp
WiFi.getAutoConnect()
```

If auto connection functionality is disabled, then function returns `false`.


#### setAutoReconnect

Set whether module will attempt to reconnect to an access point in case it is disconnected.

```cpp
WiFi.setAutoReconnect(autoReconnect)  
```

If parameter `autoReconnect` is set to `true`, then module will try to reestablish lost connection to the AP. If set to `false` then module will stay disconnected.

Note: running `setAutoReconnect(true)` when module is already disconnected will not make it reconnect to the access point. Instead `reconnect()` should be used.


#### waitForConnectResult
  
Wait until module connects to the access point. This function is intended for module configured in station or station + soft access point mode.  

```cpp
WiFi.waitForConnectResult()  
```

Function returns one of the following connection statuses:
  * `WL_CONNECTED` after successful connection is established
  * `WL_NO_SSID_AVAIL`in case configured SSID cannot be reached
  * `WL_CONNECT_FAILED` if password is incorrect
  * `WL_IDLE_STATUS` when Wi-Fi is in process of changing between statuses
  * `WL_DISCONNECTED` if module is not configured in station mode 


### Configuration

#### macAddress

Get the MAC address of the ESP station's interface. 

```cpp
WiFi.macAddress(mac) 
```  

Function should be provided with `mac` that is a pointer to memory location (an `uint8_t` array the size of 6 elements) to save the mac address. The same pointer value is returned by the function itself.

*Example code:*
```cpp
if (WiFi.status() == WL_CONNECTED)
{
  uint8_t macAddr[6];
  WiFi.macAddress(macAddr);
  Serial.printf("Connected, mac address: %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}
```
*Example output:*
```
Mac address: 5C:CF:7F:08:11:17
```

If you do not feel comfortable with pointers, then there is optional version of this function available. Instead of the pointer, it returns a formatted `String` that contains the same mac address.

```cpp
WiFi.macAddress() 
```  

*Example code:*
```cpp
if (WiFi.status() == WL_CONNECTED)
{
  Serial.printf("Connected, mac address: %s\n", WiFi.macAddress().c_str());
}
```


#### localIP

Function used to obtain IP address of ESP station's interface.

```cpp
WiFi.localIP() 
```
The type of returned value is [IPAddress](https://github.com/esp8266/Arduino/blob/master/cores/esp8266/IPAddress.h). There is a couple of methods available to display this type of data. They are presented in examples below that cover description of `subnetMask`, `gatewayIP` and `dnsIP` that return the IPAdress as well.

*Example code:*
```cpp
if (WiFi.status() == WL_CONNECTED)
{
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}
```
*Example output:*
```
Connected, IP address: 192.168.1.10
```


#### subnetMask

Get the subnet mask of the station's interface.

```cpp
WiFi.subnetMask()
```  
Module should be connected to the access point to obtain the subnet mask.

*Example code:*
```cpp
Serial.print("Subnet mask: ");
Serial.println(WiFi.subnetMask());
```
*Example output:*
```
Subnet mask: 255.255.255.0
```

#### gatewayIP
  
Get the IP address of the gateway.

```cpp
WiFi.gatewayIP()
```  

*Example code:*
```cpp
Serial.printf("Gataway IP: %s\n", WiFi.gatewayIP().toString().c_str());
```
*Example output:*
```
Gataway IP: 192.168.1.9
```

#### dnsIP

Get the IP addresses of Domain Name Servers (DNS).

```cpp
WiFi.dnsIP(dns_no)
```

With the input parameter `dns_no` we can specify which Domain Name Server's IP we need. This parameter is zero based and allowed values are none, 0 or 1. If no parameter is provided, then IP of DNS #1 is returned.

*Example code:*
```cpp
Serial.print("DNS #1, #2 IP: ");
WiFi.dnsIP().printTo(Serial);
Serial.print(", ");
WiFi.dnsIP(1).printTo(Serial);
Serial.println();
```  
*Example output:*
```
DNS #1, #2 IP: 62.179.1.60, 62.179.1.61
```

#### hostname

Get the DHCP hostname assigned to ESP station.

```cpp
WiFi.hostname()
```  
Function returns `String` type. Default hostname is in format `ESP_24xMAC`where 24xMAC are the last 24 bits of module's MAC address. 

The hostname may be changed using the following function:
 
```cpp
WiFi.hostname(aHostname) 
```  

Input parameter `aHostname` may be a type of `char*`, `const char*` or `String`. Maximum length of assigned hostname is 32 characters. Function returns either `true` or `false` depending on result. For instance, if the limit of 32 characters is exceeded, function will return `false` without assigning the new hostname.

*Example code:*
```cpp
Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
WiFi.hostname("Station_Tester_02");
Serial.printf("New hostname: %s\n", WiFi.hostname().c_str());
```  
*Example output:*
```
Default hostname: ESP_081117
New hostname: Station_Tester_02
```


#### status

Return the status of Wi-Fi connection.

```cpp
WiFi.status()
```  

Function returns one of the following connection statuses:
  * `WL_CONNECTED` after successful connection is established
  * `WL_NO_SSID_AVAIL`in case configured SSID cannot be reached
  * `WL_CONNECT_FAILED` if password is incorrect
  * `WL_IDLE_STATUS` when Wi-Fi is in process of changing between statuses
  * `WL_DISCONNECTED` if module is not configured in station mode 

Returned value is type of `wl_status_t` defined in [wl_definitions.h](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/include/wl_definitions.h)

*Example code:*
```cpp
#include <ESP8266WiFi.h>

void setup(void)
{
  Serial.begin(115200);
  Serial.printf("Connection status: %d\n", WiFi.status());
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  Serial.printf("Connection status: %d\n", WiFi.status());
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnection status: %d\n", WiFi.status());
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {}
```
*Example output:*
```
Connection status: 6
Connecting to sensor-net
Connection status: 6
......
Connection status: 3
Connected, IP address: 192.168.1.10
```
Particular connection statuses 6 and 3 may be looked up in [wl_definitions.h](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/include/wl_definitions.h) as follows:

```
3 - WL_CONNECTED
6 - WL_DISCONNECTED
```
Basing on this example, when running above code, module is initially disconnected from the network and returns connection status `6 - WL_DISCONNECTED`. It is also disconnected immediately after running ` WiFi.begin(ssid, password)`. Then after about 3 seconds (basing on number of dots displayed every 500ms), it finally gets connected returning status `3 - WL_CONNECTED`.


#### SSID

Return the name of Wi-Fi network, formally called [Service Set Identification (SSID)](http://www.juniper.net/techpubs/en_US/network-director1.1/topics/concept/wireless-ssid-bssid-essid.html#jd0e34). 

```cpp
WiFi.SSID()
```  
Returned value is of the `String` type.

*Example code:*
```cpp
Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
```
*Example output:*
```
SSID: sensor-net
```


#### psk  

Return current pre shared key (password) associated with the Wi-Fi network.

```cpp
WiFi.psk()
```

Function returns value of the `String` type.



#### BSSID
  
Return the mac address the access point where ESP module is connected to. This address is formally called [Basic Service Set Identification (BSSID)](http://www.juniper.net/techpubs/en_US/network-director1.1/topics/concept/wireless-ssid-bssid-essid.html#jd0e47).

```cpp
WiFi.BSSID()
```  

The `BSSID()` function returns a pointer to the memory location (an `uint8_t` array with the size of 6 elements) where the BSSID is saved.

Below is similar function, but returning BSSID but as a `String` type.

```cpp
WiFi.BSSIDstr()  
```  

*Example code:*
```cpp
Serial.printf("BSSID: %s\n", WiFi.BSSIDstr().c_str());
```
*Example output:*
```
BSSID: 00:1A:70:DE:C1:68
```


#### RSSI

Return the signal strength of Wi-Fi network, that is formally called [Received Signal Strength Indication (RSSI)](https://en.wikipedia.org/wiki/Received_signal_strength_indication). 

```cpp
WiFi.RSSI() 
```  

Signal strength value is provided in dBm. The type of returned value is `int32_t`.

*Example code:*
```cpp
Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
```
*Example output:*
```
RSSI: -68 dBm
```


### Connect Different
  
[ESP8266 SDK](http://bbs.espressif.com/viewtopic.php?f=51&t=1023) provides alternate methods to connect ESP station to an access point. Out of them [esp8266 / Arduino](https://github.com/esp8266/Arduino) core implements [WPS](#wps) and [Smart Config](#smart-config) as described in more details below.


#### WPS

The following `beginWPSConfig` function allows connecting to a network using [Wi-Fi Protected Setup (WPS)](https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup). Currently only [push-button configuration](http://www.wi-fi.org/knowledge-center/faq/how-does-wi-fi-protected-setup-work) (`WPS_TYPE_PBC` mode) is supported (SDK 1.5.4).

```cpp
WiFi.beginWPSConfig()
```

Depending on connection result function returns either `true` or `false` (`boolean` type). 

*Example code:*
```cpp
#include <ESP8266WiFi.h>

void setup(void)
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Wi-Fi mode set to WIFI_STA %s\n", WiFi.mode(WIFI_STA) ? "" : "Failed!");
  Serial.print("Begin WPS (press WPS button on your router) ... ");
  Serial.println(WiFi.beginWPSConfig() ? "Success" : "Failed");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {}
```
*Example output:*
```
Wi-Fi mode set to WIFI_STA 
Begin WPS (press WPS button on your router) ... Success
.........
Connected, IP address: 192.168.1.102
```


#### Smart Config

The Smart Config connection of an ESP module an access point is done by sniffing for special packets that contain SSID and password of desired AP. To do so the mobile device or computer should have functionality of broadcasting of encoded SSID and password.

The following three functions are provided to implement Smart Config. 

Start smart configuration mode by sniffing for special packets that contain SSID and password of desired Access Point. Depending on result either `true` or `false is returned.

```cpp
beginSmartConfig() 
```  

Query Smart Config status, to decide when stop configuration. Function returns either `true` or `false of `boolean` type.

```cpp
smartConfigDone()
```  

Stop smart config, free the buffer taken by `beginSmartConfig()`. Depending on result function return either `true` or `false` of `boolean` type.

```cpp
stopSmartConfig() 
```  

For additional details regarding Smart Config please refer to [ESP8266 API User Guide](http://bbs.espressif.com/viewtopic.php?f=51&t=1023).

