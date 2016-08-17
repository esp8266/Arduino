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
    void setPort(uint16_t port);
    void setHostname(const char *hostname);
    String getHostname();
    void setPassword(const char *password);
    void setPasswordHash(const char *password);
    void onStart(THandlerFunction fn);
    void onEnd(THandlerFunction fn);
    void onError(THandlerFunction_Error fn);
    void onProgress(THandlerFunction_Progress fn);
    void begin();
    void handle();
    int getCommand(); // get update command type after OTA started- either U_FLASH or U_SPIFFS

  private:
    int _port;
    String _password;
    String _hostname;
    String _nonce;
    UdpContext *_udp_ota;
    bool _initialized;
    ota_state_t _state;
    int _size;
    int _cmd;
    int _ota_port;
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
