# WebServer example documentation and hints

This example shows different techniques on how to use and extend the ESP8266WebServer for specific purposes

It is a small project in it's own and has some files to use on the web server to show how to use simple REST based services.

It requires some space for a filesystem and runs fine on ESP8266 NodeMCU board with 4 MByte flash using the following options:
* Flash Size Option 4MB (FS:2MB)
* Debug Port Serial
* MMU 32+32 balanced

It features

* http access to the web server
* deliver all files from the file system
* deliver special built-in files
* implement services (list files, sysinfo)
* uploading files using drag & drop
* listing and deleting files using a SPA application
* Example of SPA and Web Service application 
* Only files in the root folder are supported for simplicity - no directories.




## Implementing a web server

The ESP8266WebServer library offers a simple path to implement a web server on a ESP8266 board.

The advantage on using the ESP8266WebServer instead of the plain simple WiFiServer is that the ESP8266WebServer
takes much care about the http protocol conventions and features and allows easily access to parameters.
It offers plug-in capabilities by registering specific functionalities that will be outlined below.


### Initialization

In the setup() function in the webserver.ino sketch file the following steps are implemented to make the webserver available on the local network.

* Create a webserver listening to port 80 for http requests.
* Initialize the access to the filesystem in the free flash memory (typically 2MByte).
* Connect to the local WiFi network. Here is only a straight-forward implementation hard-coding network name and passphrase. You may consider to use something like the WiFiManager library.
* Register the device in DNS using a known hostname.
* Registering several plug-ins (see below).
* Starting the web server.


### Running

In the loop() function the web server will be given time to receive and send network packages by calling
`server.handleClient();`.



## Registering simple functions to implement RESTful services

Registering function is the simplest integration mechanism available to add functionality. The server offers the `on(path, function)` methods that take the URL and the function as parameters.

There are 2 functions implemented that get registered to handle incoming GET requests for given URLs.

The JSON data format is used often for such services as it is the "natural" data format of the browser using javascript.

When the **handleSysInfo()** function is registered and a browser requests for <http://webserver/$sysinfo> the function will be called and can collect the requested information.

> ```CPP
> server.on("/$sysinfo", handleSysInfo);
> ```

The result in this case is a JSON object that is assembled in the result String variable and the returned as a response to the client also giving the information about the data format.

You can try this request in a browser by opening <http://webserver/$sysinfo> in the address bar.

> ```CPP
> server.on("/$sysinfo", handleList);
> ```

The function **handleList()** is registered the same way to return the list of files in the file system also returning a JSON object including name, size and the last modification timestamp.

You can try this request in a browser by opening <http://webserver/$list> in the address bar.


## Registering a function to send out some static content from a String

This is an example of registering a inline function in the web server.
The 2. parameter of the on() method is a so called CPP lamda function (without a name) 
that actually has only one line of functionality by sending a string as result to the client.

> ```CPP
> server.on("/$upload.htm", []() {
>   server.send(200, "text/html", FPSTR(uploadContent));
> });
> ```

Here the text from a static String with html code is returned instead of a file from the filesystem. 
The content of this string can be found in the file `builtinfiles.h`. It contains a small html+javascript implementation
that allows uploading new files into the empty filesystem.

Just open <http://webserver/$upload.htm> and drag some files from the data folder on the drop area.


## Registering a function to handle requests to the server without a path

Often servers are addressed by using the base URL like <http://webserver/> where no further path details is given.
Of course we like the user to be redirected to something usable. Therefore the `handleRoot()` function is registered:

> ```CPP
> server.on("/$upload.htm", handleRoot);
> ```

The `handleRoot()` function checks the filesystem for the file named **/index.htm** and creates a redirect to this file when the file exists.
Otherwise the redirection goes to the built-in **/$upload.htm** web page.



## Using the serveStatic plug-in

The **serveStatic** plug in is part of the library and handles delivering files from the filesystem to the client. It can be customized in some ways.

> ```CPP
> server.enableCORS(true);
> server.enableETag(true);
> server.serveStatic("/", LittleFS, "/");
> ```


### Cross-Origin Ressource Sharing (CORS)

The `enableCORS(true)` function adds a `Access-Control-Allow-Origin: *` http-header to all responses to the client
to inform that it is allowed to call URLs and services on this server from other web sites.

The feature is disabled by default (in the current version) and when you like to disable this then you should call `enableCORS(false)` during setup.

* Web sites providing high sensitive information like online banking this is disabled most of the times.
* Web sites providing advertising information or reusable scripts / images this is enabled.


### ETag support

The `enableETag(true)` function adds a ETag http header to the responses to the client that come from files from the filesystem
to enable better use of the cache in the browser.

When enabled by default the server reads the file content and creates a checksum using the md5 and base64 algorithm her called the ETag value
that changes whenever the file contains something different.

Once a browser has got the content of a file from the server including the ETag information it will add that ETag value is added in the following requests for the same resource.
Now the server can answer with a 'use the version from the cache' when the new calculated ETag value is equal to the ETag value in the request.

The calculation of the ETag value requires some time and processing but sending content is always slower.
So when you have the situation that a browser will use a web server multiple times this mechanism saves network and computing and makes web pages more responsive.

In the source code you can find another version of an algorithm to calculate a ETag value that uses the date&time from the filesystem.
This is a simpler and faster way but with a low risk of dismissing a file update as the timestamp is based on seconds and local time.
This can be enabled on demand, see inline comments.


## Registering a full-featured handler as plug-in

The example also implements the class `FileServerHandler` derived from the class `RequestHandler` to plug in functionality
that can handle more complex requests without giving a fixed URL.
It implements uploading and deleting files in the file system that is not implemented by the standard server.serveStatic functionality.

This class has to implements several functions and works in a more detailed way:

* The `canHandle()` method can inspect the given http method and url to decide weather the RequestFileHandler can handle the incoming request or not.

  In this case the RequestFileHandler will return true when the request method is an POST for upload or a DELETE for deleting files.

  The regular GET requests will be ignored and therefore handled by the also registered server.serveStatic handler.

* The function `handle()` then implements the real deletion of the file.

* The `canUpload()`and `upload()` methods work similar while the `upload()` method is called multiple times to create, append data and close the new file.


## Registering a special handler for "file not found"

Any other incoming request that was not handled by the registered plug-ins above can be detected by registering 

> ```CPP
> // handle cases when file is not found
> server.onNotFound([]() {
>   // standard not found in browser.
>   server.send(404, "text/html", FPSTR(notFoundContent));
> });
> ```

This allows sending back an "friendly" result for the browser. Here a sim ple html page is created from a static string.
You can easily change the html code in the file `builtinfiles.h`.


## customizations

You may like to change the hostname and the timezone in the lines:

> ```CPP
> #define HOSTNAME "webserver"
> #define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"
> ```


