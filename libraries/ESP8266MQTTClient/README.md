# MQTT Client library for ESP8266 Arduino

This is MQTT client library for ESP8266, using mqtt_msg package from [MQTT client library for Contiki](https://github.com/esar/contiki-mqtt) and use for ESP8266 NON-OS SDK [esp_mqtt](https://github.com/tuanpmt/esp_mqtt)

Features:

- Support subscribing, publishing, authentication, will messages, keep alive pings and all 3 QoS levels (it should be a fully functional client).

## Requirements
- ESP8266WiFi
- WiFiClientSecure

## Status
- Support 3 type of qos (0, 1, 2) and outbox
- only mqtt over TCP

## MQTT URI Scheme

- `mqtt://[username][:password]@hostname[:port][/clientId]`
    + `mqtt`, `mqtts` for MQTT over TCP & SSL support - only support `mqtt`
    + `ws`, `wss` for MQTT over Websocket & SSL support - currently not support.
- Example:
    + **Full** `mqtt://username:password@test.mosquitto.org:1883/my_client_id`
    + **Minimal** `mqtt://test.mosquitto.org`, with `user`, `pass` = NULL, port = 1883, client id = "ESP_" + ESP.getChipId()

## API 
### Setup  
- bool begin(String uri);
- bool begin(String uri, int keepalive, bool clean_session); 
- bool begin(String uri, LwtOptions lwt);
- bool begin(String uri, LwtOptions lwt, int keepalive, bool clean_session)  

### Events
- void onConnect(THandlerFunction fn);
- void onDisconnect(THandlerFunction fn);
- void onSubscribe(THandlerFunction_PubSub fn);
- void onPublish(THandlerFunction_PubSub fn);
- void onData(THandlerFunction_Data fn);

### Pub/Sub 
- int subscribe(String topic);
- int subscribe(String topic, uint8_t qos);
- int publish(String topic, String data);
- int publish(String topic, String data, int qos, int retain);

## License

Copyright (c) 2016 Tuan PM (https//twitter.com/tuanpmt) 
ESP8266 port (c) 2016 Ivan Grokhotkov (ivan@esp8266.com)

License (MIT license):
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.