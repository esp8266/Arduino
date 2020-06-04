# Graph readme

## What is this sketch about ?
This example consists of a web page that displays misc ESP8266 information, namely values of GPIOs, ADC measurement and free heap 
using http requests and a html/javascript frontend.
A similar functionality used to be hidden in previous versions of the FSBrowser example.

## How to use it ?
1. Uncomment one of the `#define USE_xxx` directives in the sketch to select the ESP filesystem to store the index.htm file on
2. Provide the credentials of your WiFi network (search for `STASSID`)
3. Compile and upload the sketch to your ESP8266 device
4. For normal use, copy the contents of the `data` folder to the filesystem. To do so:
- for SDFS, copy that contents (not the data folder itself, just its contents) to the root of a FAT/FAT32-formated SD card connected to the SPI port of the ESP8266
- for SPIFFS or LittleFS, please follow the instructions at https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#uploading-files-to-file-system
5. Once the data and sketch have been uploaded, access the page by pointing your browser to http://graph.local

## What does it demonstrate ?
1. Use of the ESP8266WebServer library to return files from the ESP filesystem
2. Use of the ESP8266WebServer library to return a dynamic JSON document
3. Querying the state of ESP I/Os as well as free heap
4. Ajax calls to a JSON API of the ESP from a webpage
5. Rendering of "real-time" data in a webpage

## Usage
- the page should start showing samples right away
- the sampling period (interval between requests to the ESP) can be selected. If the system cannot keep up with the rhythm, the interval will get longer (and the period input field will turn red to indicate it). Note that the X-axis is the time since ESP bootup, in milliseconds.
- the maximum number of samples can be selected. Warning: this uses up browser memory and power, so a large number might increase the sampling period.
- sampling can be paused or restarted, and graph can be cleared during pause
- the list of GPIOs to be displayed can be customized from Arduino code by changing the gpioMask value included in the json document
- in that list, some GPIOs can be temporarily hidden by clicking on their labels on top
- analog and heap graphs can be zoomed in using the mouse wheel. A click resets the zoom level

## Options
This sample is "fully compatible" with the FSBrowser sample. In other words, if you copy the `espData` handler over from this sketch to the FSBrowser example sketch, and upload the index.htm page to its filesystem, you will be able to use both the FSBrowser and the graph page at the same time.

## Dependency
The html page requires the [Chart.js](https://www.chartjs.org/) (v2.9.3 at the time of writing) library, which is loaded from a CDN, as well as its [zoom plugin](https://github.com/chartjs/chartjs-plugin-zoom/blob/master/README.md) (v0.7.7) and [hammer.js](http://hammerjs.github.io/) (v2.0.8) for gesture capture.
Consequently, internet access from your web browser is required for this app to work as-is.
If your browser has no web access (e.g. if you are connected to the ESP8266 as an access-point), you can download those three files locally and upload them along with the index.htm page, and uncomment the block at the top of the index.htm page

## Notes
- The code in the loop is just to demonstrate that the app is working by toggling a few I/Os. 
However, values have been particularly chosen to be meaningful for the [Witty Cloud](https://gregwareblog.wordpress.com/2016/01/10/esp-witty/) board, rotating colors of the RGB led. 
When placed close to a reflecting area, the light sensor (LDR) of the board also shows an analog image of the RGB led power.
The button rotates mode between "RGB rotate", "RGB stop", "RGB off" (and corresponding GPIOs disappearing from the graph), .
- If you use SDFS, if your card's CS pin is not connected to the default pin (4), uncomment the `fileSystemConfig.setCSPin(chipSelectPin);` line and specify the GPIO the CS pin is connected to
- `index.htm` is the default index returned if your URL does not end with a filename (works on subfolders as well)

