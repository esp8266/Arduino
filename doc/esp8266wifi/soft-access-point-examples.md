---
title: ESP8266WiFi Soft Access Point Class - Sketch Examples
---

[ESP8266WiFi Library :back:](readme.md#soft-access-point)


## Soft Access Point

Example below presents how to configure ESP8266 to run in soft access point mode so Wi-Fi stations can connect to it. The Wi-Fi network established by the soft-AP will be identified with the SSID set during configuration. The network may be protected with a password. The network may be also open, if no password is set during configuration.


## Table of Contents
  * [The Sketch](#the-sketch)
  * [How to Use It?](#how-to-use-it)
  * [How Does it Work?](#how-does-it-work)
  * [Can we Make it Simpler?](#can-we-make-it-simpler)
  * [Conclusion](#conclusion)


### The Sketch

Setting up soft-AP with ESP8266 can be done with just couple lines of code.

```cpp
#include <ESP8266WiFi.h>

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP("ESPsoftAP_01", "pass-to-soft-AP");
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }
}

void loop()
{
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
}
```

### How to Use It?

In line `boolean result = WiFi.softAP("ESPsoftAP_01", "pass-to-soft-AP")` change `pass-to-soft-AP` to some meaningful password and upload sketch. Open serial monitor and you should see:

```
Setting soft-AP ... Ready
Stations connected = 0
Stations connected = 0
...
```

Then take your mobile phone or a PC, open the list of available access points, find ` ESPsoftAP_01 ` and connect to it. This should be reflected on serial monitor as a new station connected:

```
Stations connected = 1
Stations connected = 1
...
```

If you have another Wi-Fi station available then connect it as well. Check serial monitor again where you should now see two stations reported.


### How Does it Work?

Sketch is small so analysis shouldn't be difficult. In first line we are including ` ESP8266WiFi ` library:

```cpp
#include <ESP8266WiFi.h>
```

Setting up of the access point `ESPsoftAP_01` is done by executing:

```cpp
 boolean result = WiFi.softAP("ESPsoftAP_01", "pass-to-soft-AP");
 ```

If this operation is successful then `result` will be `true` or `false` if otherwise. Basing on that either `Ready` or `Failed!` will be printed out by the following `if - else` conditional statement. 


### Can we Make it Simpler?

Can we make this sketch even simpler? Yes, we can! 
We can do it by using alternate `if - else` statement as below:

```cpp
WiFi.softAP("ESPsoftAP_01", "pass-to-soft-AP") ? "Ready" : "Failed!"
```

Such statement will return either `Ready` or `Failed!` depending on result of `WiFi.softAP(...)`. This way we can considerably shorten our sketch without any changes to functionality:

```cpp
#include <ESP8266WiFi.h>

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("ESPsoftAP_01", "pass-to-soft-AP") ? "Ready" : "Failed!");
}

void loop()
{
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
}
```

I believe this is very neat piece of code. If `? :` conditional operator is new to you, I recommend to start using it and make your code shorter and more elegant.


### Conclusion

[ESP8266WiFi](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi) library makes it easy to turn ESP8266 into soft access point. 

Once you try above sketch check out [WiFiAccessPoint.ino](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiAccessPoint/WiFiAccessPoint.ino) as a next step. It demonstrates how to access ESP operating in soft-AP mode from a web browser.


For the list of functions to manage ESP module in soft-AP mode please refer to the [Soft Access Point Class :arrow_right:](soft-access-point-class.md) documentation.



