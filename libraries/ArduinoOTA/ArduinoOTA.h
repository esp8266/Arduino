#ifndef __ARDUINO_OTA_H
#define __ARDUINO_OTA_H

class WiFiUDP;

#define OTA_CALLBACK(callback) void (*callback)()
#define OTA_CALLBACK_PROGRESS(callback)  void (*callback)(unsigned int, unsigned int)

class ArduinoOTA
{
    int _port;
    String* _mdns_host;
    WiFiUDP* _udp_ota;
    bool _serial_debug;

    OTA_CALLBACK(_start_callback);
    OTA_CALLBACK(_end_callback);
    OTA_CALLBACK(_error_callback);

    OTA_CALLBACK_PROGRESS(_progress_callback);

    public:
        ArduinoOTA(const char *mdns_host="ESP8266-OTA-",
                   int port=8266,
                   bool serial_debug=true);
        ~ArduinoOTA();
        void setup();
        void handle();
        void onStart(OTA_CALLBACK(fn));
        void onEnd(OTA_CALLBACK(fn));
        void onProgress(OTA_CALLBACK_PROGRESS(fn));
        void onError(OTA_CALLBACK (fn));
};

#endif /* __ARDUINO_OTA_H */
