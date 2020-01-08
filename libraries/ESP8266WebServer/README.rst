
ESP8266 Web Server
==================

The WebServer class found in ``ESP8266WebServer.h`` header, is a simple web server that knows how to handle HTTP requests such as GET and POST and can only support one simultaneous client.

Usage
-----

Class Constructor
~~~~~~~~~~~~~~~~~

.. code:: cpp

    ESP8266WebServer server(80);

Creates the ESP8266WebServer class object.

*Parameters:* 

host IP address: ``IPaddress addr`` (optional)
  
host port number: ``int port`` (default is the standard HTTP port 80)

Basic Operations
~~~~~~~~~~~~~~~~

Starting the server
^^^^^^^^^^^^^^^^^^^

.. code:: cpp

  void begin();

Handling incoming client requests
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: cpp

  void handleClient();

Disabling the server
^^^^^^^^^^^^^^^^^^^^

.. code:: cpp

  void close();
  void stop();

Both methods function the same

Client request handlers
^^^^^^^^^^^^^^^^^^^^^^^

.. code:: cpp

  void on();
  void addHandler();
  void onNotFound();
  void onFileUpload();	

*Example:* 

.. code:: cpp

  server.on("/", handlerFunction);
  server.onNotFound(handlerFunction); // called when handler is not assigned
  server.onFileUpload(handlerFunction); // handle file uploads

Sending responses to the client
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: cpp

  void send();
  void send_P();

*Parameters:*

``code`` - HTTP response code, can be ``200`` or ``404``, etc.

``content_type`` - HTTP content type, like ``"text/plain"`` or ``"image/png"``, etc.

``content`` - actual content body

Advanced Options
~~~~~~~~~~~~~~~~

Getting information about request arguments
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: cpp

  const String & arg();
  const String & argName();
  int args();
  bool hasArg();

``arg`` - get request argument value, use ``arg("plain")`` to get POST body
	
``argName`` - get request argument name
	
``args`` - get arguments count
	
``hasArg`` - check if argument exist

Getting information about request headers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: cpp

  const String & header();
  const String & headerName();
  const String & hostHeader();
  int headers();
  bool hasHeader();
  

``header`` - get request header value

``headerName`` - get request header name

``hostHeader`` - get request host header if available, else empty string
  
``headers`` - get header count
	
``hasHeader`` - check if header exist

Authentication
^^^^^^^^^^^^^^

.. code:: cpp

  bool authenticate();
  void requestAuthentication();

``authenticate`` - server authentication, returns true if client is authenticated else false

``requestAuthentication`` - sends authentication failure response to the client

*Example Usage:*

.. code:: cpp

  if(!server.authenticate(username, password)){
    server.requestAuthentication();
  }


Other Function Calls
~~~~~~~~~~~~~~~~~~~~

.. code:: cpp

  const String & uri(); // get the current uri
  HTTPMethod  method(); // get the current method 
  WiFiClient client(); // get the current client
  HTTPUpload & upload(); // get the current upload
  void setContentLength(); // set content length
  void sendHeader(); // send HTTP header
  void sendContent(); // send content
  void sendContent_P(); 
  void collectHeaders(); // set the request headers to collect
  void serveStatic();
  size_t streamFile();

For code samples enter `here <https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/examples>`__ .

