#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "ESP8266HTTPUpdateServer.h"


const char* ESP8266HTTPUpdateServer::_serverIndex =
R"(<html><body><form method='POST' action='' enctype='multipart/form-data'>
                  <input type='file' name='update'>
                  <input type='submit' value='Update'>
               </form>
         </body></html>)";
const char* ESP8266HTTPUpdateServer::_failedResponse = R"(Update Failed!)";
const char* ESP8266HTTPUpdateServer::_successResponse = "<META http-equiv=\"refresh\" content=\"15;URL=\">Update Success! Rebooting...";

template<class ...Ts>
void ESP8266HTTPUpdateServer::debug(const char *fmt, Ts... args) {
    if (_serial_output) {
        Serial.printf(fmt, args...);
    }
}

ESP8266HTTPUpdateServer::ESP8266HTTPUpdateServer(bool serial_debug)
{
    _serial_output = serial_debug;
    _server = NULL;
    _username = NULL;
    _password = NULL;
    _authenticated = false;
}

void ESP8266HTTPUpdateServer::setup(ESP8266WebServer *server, const char * path, const char * username, const char * password)
{
    _server = server;
    _username = (char *)username;
    _password = (char *)password;

    // handler for the /update form page
    _server->on(path, HTTP_GET, [&]() {
        if (_username != NULL && _password != NULL && !_server->authenticate(_username, _password)) {
            return _server->requestAuthentication();
        }
        _server->send(200, "text/html", _serverIndex);
    });

    // handler for the /update form POST (once file upload finishes)
    _server->on(path, HTTP_POST, [&]() {
        if (!_authenticated) {
            return _server->requestAuthentication();
        }
        _server->send(200, "text/html", Update.hasError() ? _failedResponse : _successResponse);
        ESP.restart();
    }, [&]() {
        // handler for the file upload, get's the sketch bytes, and writes
        // them through the Update object
        HTTPUpload& upload = _server->upload();
        if (upload.status == UPLOAD_FILE_START) {
            if (_serial_output) {
                Serial.setDebugOutput(true);
            }
            handle_start();

            _authenticated = (_username == NULL || _password == NULL || _server->authenticate(_username, _password));
            if (!_authenticated) {
                debug("Unauthenticated Update\n");
                handle_error(1);
                return;
            }

            WiFiUDP::stopAll();
            debug("Update: %s\n", upload.filename.c_str());
            uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace)) {//start with max available size
                if (_serial_output) {
                    Update.printError(Serial);
                    handle_error(2);
                }
            }
            handle_progress(0, upload.totalSize);
        } else if (_authenticated && upload.status == UPLOAD_FILE_WRITE) {
            debug(".");
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                if (_serial_output) {
                    Update.printError(Serial);
                    handle_error(3);
                }
            }
            handle_progress(upload.currentSize, upload.totalSize);
        } else if (_authenticated && upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) { //true to set the size to the current progress
                debug("Update Success: %u\nRebooting...\n", upload.totalSize);
                handle_progress(upload.totalSize, upload.totalSize);
                handle_end();
            } else {
                if (_serial_output) {
                    Update.printError(Serial);
                    handle_error(4);
                }
            }
            if (_serial_output) {
                Serial.setDebugOutput(false);
            }
        } else if (_authenticated && upload.status == UPLOAD_FILE_ABORTED) {
            Update.end();
            debug("Update was aborted");
            handle_error(5);
        }
        delay(0);
    });
}

void ESP8266HTTPUpdateServer::onStart(THandlerFunction fn)
{
    _cb_start = fn;
}

void ESP8266HTTPUpdateServer::onEnd(THandlerFunction fn)
{
    _cb_end = fn;
}

void ESP8266HTTPUpdateServer::onError(THandlerFunction_Error fn)
{
    _cb_error = fn;
}

void ESP8266HTTPUpdateServer::onProgress(THandlerFunction_Progress fn)
{
    _cb_progress = fn;
}

void ESP8266HTTPUpdateServer::handle_start()
{
    if (_cb_start) {
        _cb_start();
    }
}

void ESP8266HTTPUpdateServer::handle_end()
{
    if (_cb_end) {
        _cb_end();
    }
}

void ESP8266HTTPUpdateServer::handle_error(int err)
{
    if (_cb_error) {
        _cb_error(err);
    }
}

void ESP8266HTTPUpdateServer::handle_progress(unsigned int i, unsigned int j)
{
    if (_cb_progress) {
        _cb_progress(i, j);
    }
}
