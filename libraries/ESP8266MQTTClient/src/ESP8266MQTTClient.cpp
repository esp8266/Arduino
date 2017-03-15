/*
ESP8266 MQTT Client library for ESP8266 Arduino
Version 0.1
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
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <StreamString.h>
#include <base64.h>
#include "ESP8266MQTTClient.h"
#include "MQTTTransport.h"

/**
 * constructor
 */
MQTTClient::MQTTClient():
    _connected_cb(NULL),
    _disconnected_cb(NULL),
    _subscribe_cb(NULL),
    _publish_cb(NULL),
    _data_cb(NULL),
    _secure_cb(NULL),
    _initialized(false),
    _reconnect_tick(0)
{
    _outbox = ob_create();

}

/**
 * destructor
 */
MQTTClient::~MQTTClient()
{
    if(_tcp) {
        _tcp->stop();
    }
    ob_destroy(_outbox);
}


bool MQTTClient::begin(String uri)
{
    return begin(uri, {.lwtTopic = "", .lwtMsg = "", .lwtQos = 0, .lwtRetain = 0}, DEFAULT_MQTT_KEEPALIVE, DEFAULT_MQTT_CLEAN_SESSION);
}
bool MQTTClient::begin(String uri, int keepalive, bool clean_session)
{
    return begin(uri, {.lwtTopic = "", .lwtMsg = "", .lwtQos = 0, .lwtRetain = 0}, keepalive, clean_session);
}
bool MQTTClient::begin(String uri, LwtOptions lwt)
{
    return begin(uri, lwt, DEFAULT_MQTT_KEEPALIVE, DEFAULT_MQTT_CLEAN_SESSION);
}
bool MQTTClient::begin(String uri, LwtOptions lwt, int keepalive, bool clean_session)
{
    parsed_uri_t *puri = parse_uri(uri.c_str());
    MQTT_CHECK(puri->scheme == NULL, "ERROR: Protocol is not NULL\r\n", false);
    MQTT_CHECK(puri->host == NULL, "ERROR: Host is not NULL\r\n", false);
    delay(1000);
    _scheme = String(puri->scheme);
    _host = String(puri->host);
    _port = DEFAULT_MQTT_PORT;
    _path = "/";

    if(puri->fragment) {
        _client_id = String(puri->fragment);
    } else {
        _client_id = String("ESP_") + ESP.getChipId();
    }
    LOG("MQTT ClientId: %s\r\n", _client_id.c_str());
    if(puri->port) {
        _port = atoi(puri->port);
    }

    if(puri->path) {
        _path += String(puri->path);
    }
    if(puri->username)
        _username = String(puri->username);
    if(puri->password)
        _password = String(puri->password);

    free_parsed_uri(puri);

    _lwt_topic = String(lwt.lwtTopic);
    _lwt_msg = String(lwt.lwtMsg);
    _lwt_qos = lwt.lwtQos;
    _lwt_retain = lwt.lwtRetain;

    _keepalive = keepalive;

    _connect_info.client_id = _client_id.c_str();
    _connect_info.username = _username.c_str();
    _connect_info.password = _password.c_str();
    _connect_info.will_topic = _lwt_topic.c_str();
    _connect_info.will_message = _lwt_msg.c_str();
    _connect_info.will_qos = _lwt_qos;
    _connect_info.will_retain = _lwt_retain;


    _connect_info.keepalive = _keepalive;
    _connect_info.clean_session = clean_session;


    _state.in_buffer = (uint8_t *)malloc(DEFAULT_MQTT_BUFFER_SIZE_BYTES);
    if(_state.in_buffer == NULL) {
        LOG("Not enought memory\r\n");
        return false;
    }
    _state.in_buffer_length = DEFAULT_MQTT_BUFFER_SIZE_BYTES;
    _state.out_buffer = (uint8_t *)malloc(DEFAULT_MQTT_BUFFER_SIZE_BYTES);
    if(_state.in_buffer == NULL) {
        free(_state.in_buffer);
        LOG("Not enought memory\r\n");
        return false;
    }
    _state.out_buffer_length = DEFAULT_MQTT_BUFFER_SIZE_BYTES;
    _state.connect_info = &_connect_info;

    mqtt_msg_init(&_state.connection,
                  _state.out_buffer,
                  _state.out_buffer_length);

    _transportTraits.reset(nullptr);


    if(_scheme == "mqtt") {
        _transportTraits = MQTTTransportTraitsPtr(new MQTTTransportTraits());
    } else if(_scheme == "mqtts") {
        _transportTraits = MQTTTransportTraitsPtr(new MQTTTLSTraits());
    } else if(_scheme == "ws") {
        _transportTraits = MQTTTransportTraitsPtr(new MQTTWSTraits());
    }
    else {
        free(_state.out_buffer);
        free(_state.in_buffer);
        LOG("ERROR: currently only support mqtt over tcp\r\n");
        return false;
    }
    _tcp = _transportTraits->create();
    _initialized = true;
    return true;
}

bool MQTTClient::connected(void)
{
    if(_tcp) {
        return (_tcp->connected() || (_tcp->available() > 0));
    }
    return false;
}

bool MQTTClient::connect(void)
{
    int write_len, read_len, connect_rsp_code;
    int connect_tick = millis();
    if(connected()) {
        LOG("[MQTT-Client] connect. already connected, try reuse!\n");
        while(_tcp->available() > 0) {
            _tcp->read();
        }
        return true;
    }

    if(!_transportTraits) {
        LOG("[MQTT-Client] connect: MQTTClient::begin was not called or returned error\n");
        return false;
    }
    _tcp->setNoDelay(true);
    //if(!_tcp->connect(_host.c_str(), _port)) { //=>works
    if(!_transportTraits->connect(_tcp.get(), _host.c_str(), _port)) { //=not works
        LOG("[MQTT-Client] failed connect to %s:%u\n", _host.c_str(), _port);
        return false;
    }

    LOG("[MQTT-Client] connected to %s:%u\n", _host.c_str(), _port);
    

    if(_secure_cb && (_scheme == "wss" || _scheme == "mqtts")) {
        LOG("[MQTT-Client] begin verifying %s:%u\n", _host.c_str(), _port);
        auto wcs = reinterpret_cast<WiFiClientSecure&>(*_tcp);
        if(!_secure_cb(wcs, _host)) {
            _tcp->stop();
            LOG("[MQTT-Client] failed verify to %s:%u\n", _host.c_str(), _port);
            return false;
        }
    }
    if(!_tcp->connected())
            return false;
    _state.outbound_message = mqtt_msg_connect(&_state.connection,
                              _state.connect_info);
    _state.pending_msg_type = mqtt_get_type(_state.outbound_message->data);
    _state.pending_msg_id = mqtt_get_id(_state.outbound_message->data,
                                        _state.outbound_message->length);
    LOG("Sending MQTT CONNECT message, type: %d, id: %04X, len: %d\r\n",
        _state.pending_msg_type,
        _state.pending_msg_id,
        _state.outbound_message->length);
    write_len = _transportTraits->write(_tcp.get(), _state.outbound_message->data,
                                        _state.outbound_message->length);
    connect_tick = millis();
    while(!_tcp->available()) {
        if(!_tcp->connected())
            return false;
        yield();
        if(millis() - connect_tick > MQTT_CONNECT_TIMEOUT) {
            _tcp->stop();
            return false;
        }
    }
    LOG("Reading MQTT CONNECT response message\r\n");
    read_len = _transportTraits->read(_tcp.get(), _state.in_buffer, DEFAULT_MQTT_BUFFER_SIZE_BYTES);
    if(read_len < 0) {
        LOG("Error network response\r\n");
        return false;
    }
    if(mqtt_get_type(_state.in_buffer) != MQTT_MSG_TYPE_CONNACK) {
        LOG("Invalid MSG_TYPE response: %d, read_len: %d\r\n", mqtt_get_type(_state.in_buffer), read_len);
        _tcp->stop();
        return false;
    }
    connect_rsp_code = mqtt_get_connect_return_code(_state.in_buffer);
    switch(connect_rsp_code) {
        case CONNECTION_ACCEPTED:
            LOG("Connected\r\n");
            return connected();
        case CONNECTION_REFUSE_PROTOCOL:
        case CONNECTION_REFUSE_SERVER_UNAVAILABLE:
        case CONNECTION_REFUSE_BAD_USERNAME:
        case CONNECTION_REFUSE_NOT_AUTHORIZED:
            LOG("Connection refuse, reason code: %d\r\n", connect_rsp_code);
            return false;
        default:
            LOG("Connection refuse, Unknow reason\r\n");
            return false;
    }
    return false;
}

void MQTTClient::onConnect(THandlerFunction fn)
{
    _connected_cb = fn;
}
void MQTTClient::onSubscribe(THandlerFunction_PubSub fn)
{
    _subscribe_cb = fn;
}
void MQTTClient::onPublish(THandlerFunction_PubSub fn)
{
    _publish_cb = fn;
}
void MQTTClient::onData(THandlerFunction_Data fn)
{
    _data_cb = fn;
}
void MQTTClient::onSecure(THandlerFunction_Secure fn)
{
    _secure_cb = fn;
}
void MQTTClient::handle(void)
{
    mqtt_outbox *ob;
    if(!_initialized)
        return;
    if(!connected()) {
        if(_reconnect_tick != 0 && millis() - _reconnect_tick < MQTT_RECONNECT_TIMEOUT)
            return;

        _reconnect_tick = millis();
        if(connect()) {
            if(_connected_cb)
                _connected_cb();
            _keepalive_tick = millis();
        } else {
            return;
        }
    }

    processRead();

    if(millis() - _keepalive_tick > _keepalive / 2) {
        _keepalive_tick = millis();
        sendPing();
    }
    ob = ob_get_oldest_no_pending(_outbox);
    if(ob != NULL) {
        _transportTraits->write(_tcp.get(), (unsigned char*)ob->buffer,
                                ob->len);
        ob->pending = 1;
        if(ob->remove_on_sent) {
            _tcp->flush();
            ob_del_ob(ob);
        }
        LOG("Sent - Outbox size: %d\r\n", ob_get_size(_outbox));
    }

    ob_del_expired(_outbox, millis(), 60 * 60 * 1000); //remove all package not sent in 60 minutes
    ob_cleanup(_outbox, DEFAULT_MQTT_MAX_QUEUE); //keep outbox maximum is DEFAULT_MQTT_MAX_QUEUE(8*1024) bytes
}

bool MQTTClient::deliverPublish(uint8_t *message)
{
    mqtt_event_data_t event_data;
    int more_data = 0, len_read_more = 0;
    String topic, data;
    char temp;
    _state.message_length = mqtt_get_total_length(_state.in_buffer, _state.message_length_read);

    event_data.topic_length = _state.message_length_read;
    event_data.topic = mqtt_get_publish_topic(message, &event_data.topic_length);
    event_data.data_length = _state.message_length_read;
    event_data.data = mqtt_get_publish_data(message, &event_data.data_length);

    LOG("Data received, total package len: %d, publish package len: %d, data len: %d\r\n", _state.message_length_read, _state.message_length, event_data.data_length);
    if(_data_cb) {
        temp = event_data.topic[event_data.topic_length];
        event_data.topic[event_data.topic_length] = 0;
        topic = String(event_data.topic);
        event_data.topic[event_data.topic_length] = temp;
        event_data.data[event_data.data_length] = 0;
        data = String(event_data.data);
        _data_cb(topic, data, false);
    }
    while(_state.message_length_read < _state.message_length) {
        len_read_more = _transportTraits->read(_tcp.get(), _state.in_buffer, DEFAULT_MQTT_BUFFER_SIZE_BYTES);
        LOG("Get more data: %d\r\n", len_read_more);
        if(len_read_more <= 0)
            break;
        if(_data_cb) {
            _state.in_buffer[len_read_more] = 0;
            _data_cb(String((char*)event_data.topic), String((char*)_state.in_buffer), true);
        }
        _state.message_length_read += len_read_more;
    }
    return true;
}

int MQTTClient::processRead()
{
    int read_len;
    uint8_t msg_type;
    uint8_t msg_qos;
    uint16_t msg_id;
    mqtt_outbox *valid_msg;
    if(!connected())
        return 0;
    _tcp->setTimeout(DEFAULT_MQTT_READ_TIMEOUT);
    read_len = _transportTraits->read(_tcp.get(), _state.in_buffer, DEFAULT_MQTT_BUFFER_SIZE_BYTES);
    if(read_len <= 0)
        return 0;
    _state.message_length_read = read_len;
PROCESS_READ_AGAIN:
    msg_type = mqtt_get_type(_state.in_buffer);
    msg_qos = mqtt_get_qos(_state.in_buffer);
    msg_id = mqtt_get_id(_state.in_buffer, _state.in_buffer_length);
    LOG("Read len %d, id: %d, type: %d\r\n", read_len, msg_id, msg_type);
    switch(msg_type)
    {
        case MQTT_MSG_TYPE_SUBACK:
            valid_msg = ob_get(_outbox, msg_id);
            if(valid_msg->msg_type == MQTT_MSG_TYPE_SUBSCRIBE && valid_msg->msg_id == msg_id) {
                if(_subscribe_cb)
                    _subscribe_cb(msg_id);
                ob_del_id(_outbox, msg_id);
                _state.message_length = mqtt_get_total_length(_state.in_buffer, _state.message_length_read);

                LOG("Subscribe successful, msgid: %d, outbox size: %d\r\n", msg_id, ob_get_size(_outbox));
            }
            break;
        case MQTT_MSG_TYPE_UNSUBACK:
            valid_msg = ob_get(_outbox, msg_id);
            if(valid_msg && valid_msg->msg_type == MQTT_MSG_TYPE_UNSUBSCRIBE && valid_msg->msg_id == msg_id) {
                LOG("UnSubscribe successful\r\n");
                ob_del_id(_outbox, msg_id);
            }
            break;
        case MQTT_MSG_TYPE_PUBLISH:
            if(msg_qos == 1)
                _state.outbound_message = mqtt_msg_puback(&_state.connection, msg_id);
            else if(msg_qos == 2)
                _state.outbound_message = mqtt_msg_pubrec(&_state.connection, msg_id);

            deliverPublish(_state.in_buffer);
            if(msg_qos == 0)
                ob_del_id(_outbox, msg_id);

            LOG("Outbox size: %d, msgid: %d\r\n", ob_get_size(_outbox), msg_id);
            if(msg_qos == 1 || msg_qos == 2) {
                LOG("Queue MQTT_MSG_TYPE_PUBACK/MQTT_MSG_TYPE_PUBREC: %d, delete on send\r\n", msg_qos);
                queue(msg_qos == 1); //delete after send
            }
            break;
        case MQTT_MSG_TYPE_PUBACK:
            valid_msg = ob_get(_outbox, msg_id);
            if(valid_msg && valid_msg->msg_type == MQTT_MSG_TYPE_PUBLISH && valid_msg->msg_id == msg_id) {
                LOG("Received MQTT_MSG_TYPE_PUBACK, finish QoS1 publish, msgid: %d, remove data outbox\r\n", msg_id);
                ob_del_id(_outbox, msg_id);
                LOG("Outbox size: %d\r\n", ob_get_size(_outbox));
            }

            break;
        case MQTT_MSG_TYPE_PUBREC:
            LOG("received MQTT_MSG_TYPE_PUBREC, msgid: %d\r\n", msg_id);
            valid_msg = ob_get(_outbox, msg_id);
            if(valid_msg && valid_msg->msg_type == MQTT_MSG_TYPE_PUBLISH && valid_msg->msg_id == msg_id) {
                LOG("Reply with MQTT_MSG_TYPE_PUBREL msg_id: %d, %d\r\n", msg_id);
                ob_del_id(_outbox, msg_id);
                _state.outbound_message = mqtt_msg_pubrel(&_state.connection, msg_id);
                queue(0);
                LOG("Outbox size: %d\r\n", ob_get_size(_outbox));
            }
            break;
        case MQTT_MSG_TYPE_PUBREL:
            valid_msg = ob_get(_outbox, msg_id);
            LOG("Received MQTT_MSG_TYPE_PUBREL, msg_id: %d, %d\r\n", msg_id);
            if(valid_msg && valid_msg->msg_type == MQTT_MSG_TYPE_PUBREC && valid_msg->msg_id == msg_id) {
                LOG("Reply with MQTT_MSG_TYPE_PUBCOMP, remove on sent, msg_id: %d, %d\r\n", msg_id);
                ob_del_id(_outbox, msg_id);
                _state.outbound_message = mqtt_msg_pubcomp(&_state.connection, msg_id);
                queue(1);
                LOG("Outbox size: %d\r\n", ob_get_size(_outbox));
            }
            break;
        case MQTT_MSG_TYPE_PUBCOMP:
            LOG("Received MQTT_MSG_TYPE_PUBCOMP, msg_id: %d\r\n", msg_id);
            valid_msg = ob_get(_outbox, msg_id);
            if(valid_msg && (valid_msg->msg_type == MQTT_MSG_TYPE_PUBCOMP || valid_msg->msg_type == MQTT_MSG_TYPE_PUBREL) && valid_msg->msg_id == msg_id) {

                ob_del_id(_outbox, msg_id);
                LOG("Outbox size: %d\r\n", ob_get_size(_outbox));
            }
            break;
        case MQTT_MSG_TYPE_PINGREQ:
            LOG("received MQTT_MSG_TYPE_PINGREQ\r\n");
            _state.outbound_message = mqtt_msg_pingresp(&_state.connection);
            queue(1);
            break;
        case MQTT_MSG_TYPE_PINGRESP:
            LOG("MQTT_MSG_TYPE_PINGRESP\r\n");
            // Ignore
            break;
    }
    if(_state.message_length < _state.message_length_read) {
        _state.message_length_read -= _state.message_length;
        _state.in_buffer += _state.message_length;
        goto PROCESS_READ_AGAIN;
    }
    return 1;
}

void MQTTClient::queue(int remove_on_sent)
{
    _state.pending_msg_type = mqtt_get_type(_state.outbound_message->data);
    _state.pending_msg_id = mqtt_get_id(_state.outbound_message->data, _state.outbound_message->length);
    LOG("Queue: msgid: %d, msgtype: %d\r\n", _state.pending_msg_id, _state.pending_msg_type);
    ob_put(_outbox,
           _state.outbound_message->data,
           _state.outbound_message->length,
           _state.pending_msg_id,
           _state.pending_msg_type,
           millis(),
           remove_on_sent);
    LOG("Outbox size: %d\r\n", ob_get_size(_outbox));
}
void MQTTClient::sendPing()
{
    _state.outbound_message = mqtt_msg_pingreq(&_state.connection);
    _state.pending_msg_type = mqtt_get_type(_state.outbound_message->data);
    _state.pending_msg_id = mqtt_get_id(_state.outbound_message->data,
                                        _state.outbound_message->length);
    LOG("Sending pingreq");
    _transportTraits->write(_tcp.get(), _state.outbound_message->data,
                            _state.outbound_message->length);
}

int MQTTClient::subscribe(String topic)
{
    return subscribe(topic, 0);
}
int MQTTClient::subscribe(String topic, uint8_t qos)
{
    _state.outbound_message = mqtt_msg_subscribe(&_state.connection,
                              topic.c_str(), qos,
                              &_state.pending_msg_id);

    LOG("Queue subscribe, topic\"%s\", id: %d\r\n", topic.c_str(), _state.pending_msg_id);
    queue(0);
    return _state.pending_msg_id;
}
int MQTTClient::unSubscribe(String topic)
{
    _state.outbound_message = mqtt_msg_unsubscribe(&_state.connection,
                              topic.c_str(),
                              &_state.pending_msg_id);

    LOG("Queue unsubscribe, topic\"%s\", id: %d\r\n", topic.c_str(), _state.pending_msg_id);
    queue(0);
    return _state.pending_msg_id;
}
int MQTTClient::publish(String topic, String data)
{
    return publish(topic, data, 0, 0);
}
int MQTTClient::publish(String topic, String data, int qos, int retain)
{
    int remove_on_sent = 0;
    _state.outbound_message = mqtt_msg_publish(&_state.connection,
                              topic.c_str(), data.c_str(), data.length(),
                              qos, retain,
                              &_state.pending_msg_id);
    LOG("Queue publish, topic\"%s\", id: %d\r\n", topic.c_str(), _state.pending_msg_id);
    if(qos == 0)
        remove_on_sent = 1;

    queue(remove_on_sent);
    return _state.pending_msg_id;
}
