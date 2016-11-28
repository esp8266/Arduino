/*
ESP8266 MQTT Client library for ESP8266 Arduino
Version 1.1
Copyright (c) 2016 Tuan PM (tuanpm@live.com)
ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)
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
*/

#ifndef ESP8266MQTTClient_H_
#define ESP8266MQTTClient_H_

#include <memory>
#include <Arduino.h>
#include <WiFiClient.h>
#include <functional>
#include "uri_parser.h"
#include "mqtt_msg.h"
#include "mqtt_outbox.h"

#ifdef DEBUG_ESP_MQTT_CLIENT
#ifdef DEBUG_ESP_PORT
#define LOG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#endif
#endif

#ifndef LOG
#define LOG(...)
#endif

#define MQTT_CHECK(cond, msg, ret)	if(cond){LOG(msg); return ret;}
#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_KEEPALIVE 120000
#define DEFAULT_MQTT_BUFFER_SIZE_BYTES	1024
#define DEFAULT_MQTT_CLEAN_SESSION 1
#define MQTT_RECONNECT_TIMEOUT 5000
#define DEFAULT_MQTT_READ_TIMEOUT 200
#define DEFAULT_MQTT_MAX_QUEUE (1024*8)
typedef struct {
	String lwtTopic;
	String lwtMsg;
	int lwtQos;
	int lwtRetain;
} LwtOptions;

typedef struct mqtt_state_t
{
	uint16_t port;
	int auto_reconnect;
	mqtt_connect_info_t* connect_info;
	uint8_t* in_buffer;
	uint8_t* out_buffer;
	int in_buffer_length;
	int out_buffer_length;
	uint16_t message_length;
	uint16_t message_length_read;
	mqtt_message_t* outbound_message;
	mqtt_connection_t connection;
	uint16_t pending_msg_id;
	int pending_msg_type;
	int pending_publish_qos;
} mqtt_state_t;
typedef struct mqtt_event_data_t
{
	uint8_t type;
	char* topic;
	char* data;
	uint16_t topic_length;
	uint16_t data_length;
	uint16_t data_offset;
	uint16_t data_total_length;
} mqtt_event_data_t;


class MQTTTransportTraits;
typedef std::unique_ptr<MQTTTransportTraits> MQTTTransportTraitsPtr;

class MQTTClient
{
public:
	typedef std::function<void(void)> THandlerFunction;
	typedef std::function<void(int)> THandlerFunction_Error;
	typedef std::function<void(int)> THandlerFunction_PubSub;
	typedef std::function<void(String, String, bool)> THandlerFunction_Data;

	MQTTClient();
	~MQTTClient();
	void onConnect(THandlerFunction fn);
	void onDisconnect(THandlerFunction fn);
	void onSubscribe(THandlerFunction_PubSub fn);
	void onPublish(THandlerFunction_PubSub fn);
	void onData(THandlerFunction_Data fn);

	bool begin(String uri);
	bool begin(String uri, int keepalive, bool clean_session); 
	bool begin(String uri, LwtOptions lwt);
	bool begin(String uri, LwtOptions lwt, int keepalive, bool clean_session);
	void handle();
	bool connect();
	int subscribe(String topic);
	int unsubscribe(String topic);
	int subscribe(String topic, uint8_t qos);
	int publish(String topic, String data);
	int publish(String topic, String data, int qos, int retain);
protected:
	std::unique_ptr<WiFiClient> _tcp;
	MQTTTransportTraitsPtr _transportTraits;
	bool connected();
	int process_read();
	void queue(int remove_on_sent);
	void send_ping();
	bool deliver_publish(uint8_t *message);

	mqtt_state_t  _state;
	mqtt_connect_info_t _connect_info;
	mqtt_outbox *_outbox;
	uint32_t _keepalive_tick;


	String _scheme;
	String _host;
	int _port;
	String _client_id;
	String _username;
	String _password;
	String _lwt_topic;
	String _lwt_msg;
	int _lwt_qos;
	int _lwt_retain;
	int _clean_session;
	int _keepalive;
	bool _initialized;

	THandlerFunction _connected_cb;
	THandlerFunction _disconnected_cb;
	THandlerFunction_PubSub _subscribe_cb;
	THandlerFunction_PubSub _publish_cb;
	THandlerFunction_Data _data_cb;
};
#endif /* ESP8266MQTTClient_H_ */