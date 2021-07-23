/**
 * @file WebServer.ino
 * @brief Example implementation using the ESP8266 WebServer.
 * 
 * See also README.md for instructions and hints.
 * 
 * Changelog:
 * 21.07.2021 creation, first version
 */

#include <Arduino.h>
#include <ESP8266WebServer.h>

#include "secrets.h" // add WLAN Credentials in here.

#include <FS.h>       // File System for Web Server Files
#include <LittleFS.h> // This file system is used.

// mark parameters not used in example
#define UNUSED __attribute__((unused))

// TRACE output simplified, can be deactivated here
#define TRACE(...) Serial.printf(__VA_ARGS__)

// name of the server. You reach it using http://webserver
#define HOSTNAME "webserver"

// local time zone definition (Berlin)
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

// need a WebServer for http access on port 80.
ESP8266WebServer server(80);

// The text of builtin files are in this header file
#include "builtinfiles.h"


// ===== Simple functions used to answer simple GET requests =====

// This function is called when the WebServer was requested without giving a filename.
// This will redirect to the file index.htm when it is existing otherwise to the built-in $upload.htm page
void handleRedirect()
{
  TRACE("Redirect...");
  String url = "/index.htm";

  if (!LittleFS.exists(url)) {
    url = "/$update.htm";
  }

  server.sendHeader("Location", url, true);
  server.send(302);
} // handleRedirect()


// This function is called when the WebServer was requested to list all existing files in the filesystem.
// a JSON array with file information is returned.
void handleListFiles()
{
  Dir dir = LittleFS.openDir("/");
  String result;

  result += "[\n";
  while (dir.next()) {
    if (result.length() > 4)
      result += ",";
    result += "  {";
    result += " \"name\": \"" + dir.fileName() + "\", ";
    result += " \"size\": " + String(dir.fileSize()) + ", ";
    result += " \"time\": " + String(dir.fileTime());
    result += " }\n";
    // jc.addProperty("size", dir.fileSize());
  } // while
  result += "]";
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/javascript; charset=utf-8", result);
} // handleListFiles()


// This function is called when the sysInfo service was requested.
void handleSysInfo()
{
  String result;

  FSInfo fs_info;
  LittleFS.info(fs_info);

  result += "{\n";
  result += "  \"flashSize\": " + String(ESP.getFlashChipSize()) + ",\n";
  result += "  \"freeHeap\": " + String(ESP.getFreeHeap()) + ",\n";
  result += "  \"fsTotalBytes\": " + String(fs_info.totalBytes) + ",\n";
  result += "  \"fsUsedBytes\": " + String(fs_info.usedBytes) + ",\n";
  result += "}";

  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/javascript; charset=utf-8", result);
} // handleSysInfo()


// ===== Request Handler class used to answer more complex requests =====

// The FileServerHandler is registered to the web server to support DELETE and UPLOAD of files into the filesystem.
class FileServerHandler : public RequestHandler
{
public:
  /**
   * @brief Construct a new File Server Handler object
   * @param fs The file system to be used.
   * @param path Path to the root folder in the file system that is used for
   * Serving static data down and upload.
   * @param cache_header Cache Header to be used in replies.
   */
  FileServerHandler()
  {
    TRACE("FileServerHandler is registered\n");
  }


  /**
    @brief check wether the request can be handled by this implementation.
    @param requestMethod method of the http request line.
    @param requestUri request ressource from the http request line.
    @return true when method can be handled.
  */
  bool canHandle(HTTPMethod requestMethod, const String UNUSED &_uri) override
  {
    // can handle POST for uploads and DELETE.
    return ((requestMethod == HTTP_POST) || (requestMethod == HTTP_DELETE));
  } // canHandle()


  bool canUpload(const String &uri) override
  {
    // only allow upload on root fs level.
    return (uri == "/");
  } // canUpload()


  bool handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) override
  {
    // ensure that filename starts with '/'
    String fName = requestUri;
    if (!fName.startsWith("/")) {
      fName = "/" + fName;
    }

    if (requestMethod == HTTP_POST) {
      // all done in upload. no other forms.

    } else if (requestMethod == HTTP_DELETE) {
      if (LittleFS.exists(fName)) {
        LittleFS.remove(fName);
      }
    } // if

    server.send(200); // all done.
    return (true);
  } // handle()


  // uploading process
  void upload(ESP8266WebServer UNUSED &server, const String UNUSED &_requestUri, HTTPUpload &upload) override
  {
    // ensure that filename starts with '/'
    String fName = upload.filename;
    if (!fName.startsWith("/")) {
      fName = "/" + fName;
    }

    if (upload.status == UPLOAD_FILE_START) {
      // Open the file
      if (LittleFS.exists(fName)) {
        LittleFS.remove(fName);
      } // if
      _fsUploadFile = LittleFS.open(fName, "w");

    } else if (upload.status == UPLOAD_FILE_WRITE) {
      // Write received bytes
      if (_fsUploadFile)
        _fsUploadFile.write(upload.buf, upload.currentSize);

    } else if (upload.status == UPLOAD_FILE_END) {
      // Close the file
      if (_fsUploadFile) {
        _fsUploadFile.close();
      }
    } // if
  }   // upload()

protected:
  File _fsUploadFile;
};


/**
 * Setup everything to make the webserver work.
 */
void setup(void)
{
  delay(3000); // wait for serial monitor to start completely.

  // Use Serial port for some trace information from the example
  Serial.begin(115200);
  Serial.setDebugOutput(false);

  TRACE("Starting WebServer example...\n");

  TRACE("Mounting the filesystem...\n");
  if (!LittleFS.begin()) {
    TRACE("could not mount the filesystem...\n");
    delay(2000);
    ESP.restart();
  }

  // start WiFI
  WiFi.mode(WIFI_STA);
  if (strlen(ssid) == 0) {
    WiFi.begin();
  } else {
    WiFi.begin(ssid, passPhrase);
  }

  // allow to address the device by the given name e.g. http://webserver
  WiFi.setHostname(HOSTNAME);

  TRACE("Connect to WiFi...\n");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    TRACE(".");
  }
  TRACE("connected.\n");

  // Ask for the current time using NTP request builtin into ESP firmware.
  TRACE("Setup ntp...\n");
  configTime(TIMEZONE, "pool.ntp.org");

  TRACE("Register service handlers...\n");

  // serve a built-in htm page
  server.on("/$upload.htm", []() {
    server.send(200, "text/html", FPSTR(uploadContent));
  });

  // register a redirect handler when only domain name is given.
  server.on("/", HTTP_GET, handleRedirect);

  // register some REST services
  server.on("/$list", HTTP_GET, handleListFiles);
  server.on("/$sysinfo", HTTP_GET, handleSysInfo);

  // UPLOAD and DELETE of files in the file system using a request handler.
  server.addHandler(new FileServerHandler());

  // enable CORS header in webserver results
  server.enableCORS(true);

  // enable ETAG header in webserver results from serveStatic handler
  server.enableETag(true);

  // serve all static files
  server.serveStatic("/", LittleFS, "/");

  // handle cases when file is not found
  server.onNotFound([]() {
    // standard not found in browser.
    server.send(404, "text/html", FPSTR(notFoundContent));
  });

  server.begin();
  TRACE("hostname=%s\n", WiFi.getHostname());
} // setup


// run the server...
void loop(void)
{
  server.handleClient();
} // loop()

// end.
