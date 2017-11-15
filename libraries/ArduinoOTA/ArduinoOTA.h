#ifndef __ARDUINO_OTA_H
#define __ARDUINO_OTA_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <functional>

class UdpContext;

typedef enum {
  OTA_IDLE,
  OTA_WAITAUTH,
  OTA_RUNUPDATE
} ota_state_t;

typedef enum {
  OTA_AUTH_ERROR,
  OTA_BEGIN_ERROR,
  OTA_CONNECT_ERROR,
  OTA_RECEIVE_ERROR,
  OTA_END_ERROR
} ota_error_t;

class ArduinoOTAClass
{
  public:
	typedef std::function<void(void)> THandlerFunction;
	typedef std::function<void(ota_error_t)> THandlerFunction_Error;
	typedef std::function<void(unsigned int, unsigned int)> THandlerFunction_Progress;

    ArduinoOTAClass();
    ~ArduinoOTAClass();

    //Sets the service port. Default 8266
    void setPort(uint16_t port);

    //Sets the device hostname. Default esp8266-xxxxxx
    void setHostname(const char *hostname);
    String getHostname();

    //Sets the password that will be required for OTA. Default NULL
    void setPassword(const char *password);

    //Sets the password as above but in the form MD5(password). Default NULL
    void setPasswordHash(const char *password);

    //Sets if the device should be rebooted after successful update. Default true
    void setRebootOnSuccess(bool reboot);

    //This callback will be called when OTA connection has begun
    void onStart(THandlerFunction fn);

    //This callback will be called when OTA has finished
    void onEnd(THandlerFunction fn);

    //This callback will be called when OTA encountered Error
    void onError(THandlerFunction_Error fn);

    //This callback will be called when OTA is receiving data
    void onProgress(THandlerFunction_Progress fn);

    //Starts the ArduinoOTA service
    void begin();

    //Call this in loop() to run the service
    void handle();

    //Gets update command type after OTA has started. Either U_FLASH or U_SPIFFS
    int getCommand();

  private:
    int _port;
    String _password;
    String _hostname;
    String _nonce;
    UdpContext *_udp_ota;
    bool _initialized;
    bool _rebootOnSuccess;
    ota_state_t _state;
    int _size;
    int _cmd;
    uint16_t _ota_port;
    uint16_t _ota_udp_port;
    IPAddress _ota_ip;
    String _md5;

    THandlerFunction _start_callback;
    THandlerFunction _end_callback;
    THandlerFunction_Error _error_callback;
    THandlerFunction_Progress _progress_callback;

    void _runUpdate(void);
    void _onRx(void);
    int parseInt(void);
    String readStringUntil(char end);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ARDUINOOTA)
extern ArduinoOTAClass ArduinoOTA;
#endif

#endif /* __ARDUINO_OTA_H */
