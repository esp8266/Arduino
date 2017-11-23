:orphan:

Client Secure Class
-------------------

Methods and properties described in this section are specific to ESP8266. They are not covered in `Arduino WiFi library <https://www.arduino.cc/en/Reference/WiFi>`__ documentation. Before they are fully documented please refer to information below.

loadCertificate
~~~~~~~~~~~~~~~

Load client certificate from file system.

.. code:: cpp

    loadCertificate(file) 

*Declarations*

.. code:: cpp

    #include <FS.h>
    #include <ESP8266WiFi.h>
    #include <WiFiClientSecure.h>

    const char* certyficateFile = "/client.cer";

*setup() or loop()*

.. code:: cpp

    if (!SPIFFS.begin()) 
    {
      Serial.println("Failed to mount the file system");
      return;
    }

    Serial.printf("Opening %s", certyficateFile);
    File crtFile = SPIFFS.open(certyficateFile, "r");
    if (!crtFile)
    {
      Serial.println(" Failed!");
    }

    WiFiClientSecure client;

    Serial.print("Loading %s", certyficateFile);
    if (!client.loadCertificate(crtFile))
    {
      Serial.println(" Failed!");
    }

    // proceed with connecting of client to the host

setCertificate
~~~~~~~~~~~~~~

Load client certificate from C array.

.. code:: cpp

    setCertificate (array, size) 

For a practical example please check `this interesting blog <https://nofurtherquestions.wordpress.com/2016/03/14/making-an-esp8266-web-accessible/>`__.

Other Function Calls
~~~~~~~~~~~~~~~~~~~~

.. code:: cpp

    bool  verify (const char *fingerprint, const char *domain_name) 
    void  setPrivateKey (const uint8_t *pk, size_t size) 
    bool  loadCertificate (Stream &stream, size_t size) 
    bool  loadPrivateKey (Stream &stream, size_t size) 
    template<typename TFile >  bool  loadPrivateKey (TFile &file)

Documentation for the above functions is not yet prepared.

For code samples please refer to separate section with `examples <client-secure-examples.md>`__ dedicated specifically to the Client Secure Class.
