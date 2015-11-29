#ifndef __ARDUINO_OTA_H
#define __ARDUINO_OTA_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

class UdpContext;

#define OTA_CALLBACK(callback) void (*callback)()
#define OTA_CALLBACK_PROGRESS(callback)  void (*callback)(unsigned int, unsigned int)
#define OTA_CALLBACK_ERROR(callback)  void (*callback)(ota_error_t)

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
    ArduinoOTAClass();
    ~ArduinoOTAClass();
    void setPort(uint16_t port);
    void setHostname(const char *hostname);
    void setPassword(const char *password);
    void onStart(OTA_CALLBACK(fn));
    void onEnd(OTA_CALLBACK(fn));
    void onProgress(OTA_CALLBACK_PROGRESS(fn));
    void onError(OTA_CALLBACK_ERROR (fn));
    void begin();
    void handle();

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

    OTA_CALLBACK(_start_callback);
    OTA_CALLBACK(_end_callback);
    OTA_CALLBACK_ERROR(_error_callback);
    OTA_CALLBACK_PROGRESS(_progress_callback);

    void _runUpdate(void);
    void _onRx(void);
    int parseInt(void);
    String readStringUntil(char end);
};

extern ArduinoOTAClass ArduinoOTA;

#endif /* __ARDUINO_OTA_H */
