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
  OTA_END_ERROR,
  OTA_ERASE_SETTINGS_ERROR
} ota_error_t;

typedef enum {
  OTA_ERASE_CFG_NO = 0,
  OTA_ERASE_CFG_IGNORE_ERROR,
  OTA_ERASE_CFG_ABORT_ON_ERROR
} ota_erase_cfg_t;

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

    //Sets flag to erase WiFi Settings at reboot/reset. "eraseConfig" selects to
    //abort erase on failure or ignore error and erase.
    void setEraseConfig(ota_erase_cfg_t eraseConfig = OTA_ERASE_CFG_ABORT_ON_ERROR);

    //This callback will be called when OTA connection has begun
    void onStart(THandlerFunction fn);

    //This callback will be called when OTA has finished
    void onEnd(THandlerFunction fn);

    //This callback will be called when OTA encountered Error
    void onError(THandlerFunction_Error fn);

    //This callback will be called when OTA is receiving data
    void onProgress(THandlerFunction_Progress fn);

    //Starts the ArduinoOTA service
    void begin(bool useMDNS = true);

    //Ends the ArduinoOTA service
    void end();

    //Has the effect of the "+ WiFi Settings" in the Arduino IDE Tools "Erase
    //Flash" selection. Only returns on erase flash failure.
    void eraseConfigAndReset();

    //Call this in loop() to run the service. Also calls MDNS.update() when begin() or begin(true) is used.
    void handle();

    //Gets update command type after OTA has started. Either U_FLASH or U_FS
    int getCommand();

  private:
    void _runUpdate(void);
    void _onRx(void);
    int parseInt(void);
    String readStringUntil(char end);

    int _port = 0;
    String _password;
    String _hostname;
    String _nonce;
    UdpContext *_udp_ota = nullptr;
    bool _initialized = false;
    bool _rebootOnSuccess = true;
    bool _useMDNS = true;
    ota_erase_cfg_t _eraseConfig = OTA_ERASE_CFG_NO;
    ota_state_t _state = OTA_IDLE;
    int _size = 0;
    int _cmd = 0;
    uint16_t _ota_port = 0;
    uint16_t _ota_udp_port = 0;
    IPAddress _ota_ip;
    String _md5;

    THandlerFunction _start_callback = nullptr;
    THandlerFunction _end_callback = nullptr;
    THandlerFunction_Error _error_callback = nullptr;
    THandlerFunction_Progress _progress_callback = nullptr;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ARDUINOOTA)
extern ArduinoOTAClass ArduinoOTA;
#endif

#endif /* __ARDUINO_OTA_H */
