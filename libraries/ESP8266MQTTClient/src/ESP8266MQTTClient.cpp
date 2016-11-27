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

class MQTTTransportTraits
{
public:
    virtual ~MQTTTransportTraits()
    {
    }

    virtual std::unique_ptr<WiFiClient> create()
    {
        return std::unique_ptr<WiFiClient>(new WiFiClient());
    }

    virtual bool verify(WiFiClient& client, const char* host)
    {
        return true;
    }
};

class MQTTTLSTraits : public MQTTTransportTraits
{
public:
    MQTTTLSTraits(const String& fingerprint) :
        _fingerprint(fingerprint)
    {
    }

    std::unique_ptr<WiFiClient> create() override
    {
        return std::unique_ptr<WiFiClient>(new WiFiClientSecure());
    }

    bool verify(WiFiClient& client, const char* host) override
    {
        auto wcs = reinterpret_cast<WiFiClientSecure&>(client);
        return wcs.verify(_fingerprint.c_str(), host);
    }

protected:
    String _fingerprint;
};

/**
 * constructor
 */
MQTTClient::MQTTClient():
    _connected_cb(NULL),
    _disconnected_cb(NULL),
    _subscribe_cb(NULL),
    _publish_cb(NULL),
    _data_cb(NULL)
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
    parsed_url *puri = parse_uri(uri.c_str());
    MQTT_CHECK(puri->scheme == NULL, "ERROR: Protocol must not NULL\r\n", false);
    MQTT_CHECK(puri->host == NULL, "ERROR: Host must not NULL\r\n", false);
    _scheme = String(puri->scheme);
    _host = String(puri->host);
    _port = DEFAULT_MQTT_PORT;

    if(puri->port) {
        _port = atoi(puri->port);
    }

    if(puri->path) {
        _client_id = String(puri->path);
    } else {
        _client_id = String("ESP_") + ESP.getChipId();
    }

    if(puri->username)
        _username = String(puri->username);
    if(puri->password)
        _password = String(puri->password);

    http_parsed_url_free(puri);

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


    _mqtt_state.in_buffer = (uint8_t *)malloc(DEFAULT_MQTT_BUFFER_SIZE_BYTES);
    _mqtt_state.in_buffer_length = DEFAULT_MQTT_BUFFER_SIZE_BYTES;
    _mqtt_state.out_buffer =  (uint8_t *)malloc(DEFAULT_MQTT_BUFFER_SIZE_BYTES);
    _mqtt_state.out_buffer_length = DEFAULT_MQTT_BUFFER_SIZE_BYTES;
    _mqtt_state.connect_info = &_connect_info;

    mqtt_msg_init(&_mqtt_state.mqtt_connection,
                  _mqtt_state.out_buffer,
                  _mqtt_state.out_buffer_length);

    _transportTraits.reset(nullptr);
    _transportTraits = MQTTTransportTraitsPtr(new MQTTTransportTraits());

    LOG("%s:%s@%s:%d/%s", _username.c_str(), _password.c_str(), _host.c_str(), _port, _client_id.c_str());

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
    if(connected()) {
        LOG("[MQTT-Client] connect. already connected, try reuse!\n");
        while(_tcp->available() > 0) {
            _tcp->read();
        }
        return true;
    }

    if (!_transportTraits) {
        LOG("[MQTT-Client] connect: MQTTClient::begin was not called or returned error\n");
        return false;
    }

    if(_scheme == "mqtt")
        _tcp = _transportTraits->create();
    else {
        LOG("ERROR: currently only support mqtt over tcp\r\n");
        return false;
    }

    if(!_tcp->connect(_host.c_str(), _port)) {
        LOG("[MQTT-Client] failed connect to %s:%u\n", _host.c_str(), _port);
        return false;
    }

    LOG("[MQTT-Client] connected to %s:%u\n", _host.c_str(), _port);

    if (!_transportTraits->verify(*_tcp, _host.c_str())) {
        LOG("[MQTT-Client] transport level verify failed\n");
        _tcp->stop();
        return false;
    }

    _tcp->setNoDelay(true);

    _mqtt_state.outbound_message = mqtt_msg_connect(&_mqtt_state.mqtt_connection,
                                   _mqtt_state.connect_info);
    _mqtt_state.pending_msg_type = mqtt_get_type(_mqtt_state.outbound_message->data);
    _mqtt_state.pending_msg_id = mqtt_get_id(_mqtt_state.outbound_message->data,
                                 _mqtt_state.outbound_message->length);
    LOG("Sending MQTT CONNECT message, type: %d, id: %04X\r\n",
        _mqtt_state.pending_msg_type,
        _mqtt_state.pending_msg_id);
    write_len = _tcp->write(_mqtt_state.outbound_message->data,
                            _mqtt_state.outbound_message->length);
    LOG("Reading MQTT CONNECT response message\r\n");
    while (_tcp->available() == 0);
    read_len = _tcp->read(_mqtt_state.in_buffer, DEFAULT_MQTT_BUFFER_SIZE_BYTES);



    if (read_len < 0) {
        LOG("Error network response\r\n");
        return false;
    }
    if (mqtt_get_type(_mqtt_state.in_buffer) != MQTT_MSG_TYPE_CONNACK) {
        LOG("Invalid MSG_TYPE response: %d, read_len: %d\r\n", mqtt_get_type(_mqtt_state.in_buffer), read_len);
        return false;
    }
    connect_rsp_code = mqtt_get_connect_return_code(_mqtt_state.in_buffer);
    switch (connect_rsp_code) {
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
void MQTTClient::handle(void)
{
    mqtt_outbox *ob;
    while(!connected()) {
        if(connect()) {
            if(_connected_cb)
                _connected_cb();
            _keepalive_tick = millis();
            break;
        }
        delay(MQTT_RECONNECT_TIMEOUT);
    }
    process_read();

    if(millis() - _keepalive_tick > _keepalive / 2) {
        _keepalive_tick = millis();
        send_ping();
    }
    ob = ob_get_oldest_no_pending(_outbox);
    if(ob != NULL) {
        _tcp->write(ob->buffer,
                    ob->len);
        ob->pending = 1;
        if(ob->remove_on_sent) {
            _tcp->flush();
            ob_del_ob(ob);
        }
        LOG("Sent - Outbox size: %d\r\n", ob_get_size(_outbox));
    }

    ob_del_expired(_outbox, millis(), 60*60*1000); //remove all package not sent in 60 minutes
    ob_cleanup(_outbox, DEFAULT_MQTT_MAX_QUEUE); //keep outbox maximum is DEFAULT_MQTT_MAX_QUEUE(8*1024) bytes 
}

bool MQTTClient::deliver_publish(uint8_t *message)
{
    mqtt_event_data_t event_data;
    int more_data = 0, len_read_more = 0;
    String topic, data;
    char temp;
    _mqtt_state.message_length = mqtt_get_total_length(_mqtt_state.in_buffer, _mqtt_state.message_length_read);

    event_data.topic_length = _mqtt_state.message_length_read;
    event_data.topic = mqtt_get_publish_topic(message, &event_data.topic_length);
    event_data.data_length = _mqtt_state.message_length_read;
    event_data.data = mqtt_get_publish_data(message, &event_data.data_length);

    LOG("Data received, total package len: %d, publish package len: %d, data len: %d\r\n", _mqtt_state.message_length_read, _mqtt_state.message_length, event_data.data_length);
    if(_data_cb) {
        temp = event_data.topic[event_data.topic_length];
        event_data.topic[event_data.topic_length] = 0;
        topic = String(event_data.topic);
        event_data.topic[event_data.topic_length] = temp;
        event_data.data[event_data.data_length] = 0;
        data = String(event_data.data);
        _data_cb(topic, data, false);
    }
    while(_mqtt_state.message_length_read < _mqtt_state.message_length) {
        len_read_more = _tcp->read(_mqtt_state.in_buffer, DEFAULT_MQTT_BUFFER_SIZE_BYTES);
        LOG("Get more data: %d\r\n", len_read_more);
        if(len_read_more <= 0)
            break;
        if(_data_cb) {
            _mqtt_state.in_buffer[len_read_more] = 0;
            _data_cb(String((char*)event_data.topic), String((char*)_mqtt_state.in_buffer), true);
        }
        _mqtt_state.message_length_read += len_read_more;
    }

    return true;

}

int MQTTClient::process_read()
{
    int read_len;
    uint8_t msg_type;
    uint8_t msg_qos;
    uint16_t msg_id;
    mqtt_outbox *valid_msg;
    _tcp->setTimeout(DEFAULT_MQTT_READ_TIMEOUT);
    read_len = _tcp->read(_mqtt_state.in_buffer, DEFAULT_MQTT_BUFFER_SIZE_BYTES);
    if (read_len <= 0)
        return 0;
    _mqtt_state.message_length_read = read_len;
PROCESS_READ_AGAIN:
    msg_type = mqtt_get_type(_mqtt_state.in_buffer);
    msg_qos = mqtt_get_qos(_mqtt_state.in_buffer);
    msg_id = mqtt_get_id(_mqtt_state.in_buffer, _mqtt_state.in_buffer_length);
    LOG("Read len %d, id: %d, type: %d\r\n", read_len, msg_id, msg_type);
    switch (msg_type)
    {
        case MQTT_MSG_TYPE_SUBACK:
            valid_msg = ob_get(_outbox, msg_id);
            if (valid_msg->msg_type == MQTT_MSG_TYPE_SUBSCRIBE && valid_msg->msg_id == msg_id) {
                if(_subscribe_cb)
                    _subscribe_cb(msg_id);
                ob_del_id(_outbox, msg_id);
                _mqtt_state.message_length = mqtt_get_total_length(_mqtt_state.in_buffer, _mqtt_state.message_length_read);

                LOG("Subscribe successful, msgid: %d, outbox size: %d\r\n", msg_id, ob_get_size(_outbox));
            }
            break;
        case MQTT_MSG_TYPE_UNSUBACK:
            valid_msg = ob_get(_outbox, msg_id);
            if (valid_msg->msg_type == MQTT_MSG_TYPE_UNSUBSCRIBE && valid_msg->msg_id == msg_id)
                LOG("UnSubscribe successful\r\n");
            break;
        case MQTT_MSG_TYPE_PUBLISH:
            if (msg_qos == 1)
                _mqtt_state.outbound_message = mqtt_msg_puback(&_mqtt_state.mqtt_connection, msg_id);
            else if (msg_qos == 2)
                _mqtt_state.outbound_message = mqtt_msg_pubrec(&_mqtt_state.mqtt_connection, msg_id);




            deliver_publish(_mqtt_state.in_buffer);
            if(msg_qos == 0)
                ob_del_id(_outbox, msg_id);

            LOG("Outbox size: %d, msgid: %d\r\n", ob_get_size(_outbox), msg_id);
            if (msg_qos == 1 || msg_qos == 2) {
                LOG("Queue MQTT_MSG_TYPE_PUBACK/MQTT_MSG_TYPE_PUBREC: %d, delete on send\r\n", msg_qos);
                queue(msg_qos == 1); //delete after send
            }
            // deliver_publish(client, _mqtt_state.in_buffer, _mqtt_state.message_length_read);
            break;
        case MQTT_MSG_TYPE_PUBACK:
            valid_msg = ob_get(_outbox, msg_id);
            if (valid_msg && valid_msg->msg_type == MQTT_MSG_TYPE_PUBLISH && valid_msg->msg_id == msg_id) {
                LOG("Received MQTT_MSG_TYPE_PUBACK, finish QoS1 publish, msgid: %d, remove data outbox\r\n", msg_id);
                ob_del_id(_outbox, msg_id);
                LOG("Outbox size: %d\r\n", ob_get_size(_outbox));
            }

            break;
        case MQTT_MSG_TYPE_PUBREC:
            LOG("received MQTT_MSG_TYPE_PUBREC, msgid: %d\r\n", msg_id);
            valid_msg = ob_get(_outbox, msg_id);
            if (valid_msg && valid_msg->msg_type == MQTT_MSG_TYPE_PUBLISH && valid_msg->msg_id == msg_id) {
                LOG("Reply with MQTT_MSG_TYPE_PUBREL msg_id: %d, %d\r\n", msg_id);
                ob_del_id(_outbox, msg_id);
                _mqtt_state.outbound_message = mqtt_msg_pubrel(&_mqtt_state.mqtt_connection, msg_id);
                queue(0);
                LOG("Outbox size: %d\r\n", ob_get_size(_outbox));
            }
            break;
        case MQTT_MSG_TYPE_PUBREL:
            valid_msg = ob_get(_outbox, msg_id);
            LOG("Received MQTT_MSG_TYPE_PUBREL, msg_id: %d, %d\r\n", msg_id);
            if (valid_msg && valid_msg->msg_type == MQTT_MSG_TYPE_PUBREC && valid_msg->msg_id == msg_id) {
                LOG("Reply with MQTT_MSG_TYPE_PUBCOMP, remove on sent, msg_id: %d, %d\r\n", msg_id);
                ob_del_id(_outbox, msg_id);
                _mqtt_state.outbound_message = mqtt_msg_pubcomp(&_mqtt_state.mqtt_connection, msg_id);
                queue(1);
                LOG("Outbox size: %d\r\n", ob_get_size(_outbox));
            }


            break;
        case MQTT_MSG_TYPE_PUBCOMP:
            LOG("Received MQTT_MSG_TYPE_PUBCOMP, msg_id: %d\r\n", msg_id);
            valid_msg = ob_get(_outbox, msg_id);
            if (valid_msg && (valid_msg->msg_type == MQTT_MSG_TYPE_PUBCOMP || valid_msg->msg_type == MQTT_MSG_TYPE_PUBREL) && valid_msg->msg_id == msg_id) {
                
                ob_del_id(_outbox, msg_id);
                LOG("Outbox size: %d\r\n", ob_get_size(_outbox));
            }
            break;
        case MQTT_MSG_TYPE_PINGREQ:
            LOG("received MQTT_MSG_TYPE_PINGREQ\r\n");
            _mqtt_state.outbound_message = mqtt_msg_pingresp(&_mqtt_state.mqtt_connection);
            queue(1);
            break;
        case MQTT_MSG_TYPE_PINGRESP:
            LOG("MQTT_MSG_TYPE_PINGRESP\r\n");
            // Ignore
            break;
    }
    if(_mqtt_state.message_length < _mqtt_state.message_length_read) {
        _mqtt_state.message_length_read -= _mqtt_state.message_length;
        _mqtt_state.in_buffer += _mqtt_state.message_length;
        goto PROCESS_READ_AGAIN;
    }
    return 1;
}

void MQTTClient::queue(int remove_on_sent)
{
    _mqtt_state.pending_msg_type = mqtt_get_type(_mqtt_state.outbound_message->data);
    _mqtt_state.pending_msg_id = mqtt_get_id(_mqtt_state.outbound_message->data, _mqtt_state.outbound_message->length);
    LOG("Queue: msgid: %d, msgtype: %d\r\n", _mqtt_state.pending_msg_id, _mqtt_state.pending_msg_type);
    ob_put(_outbox,
           _mqtt_state.outbound_message->data,
           _mqtt_state.outbound_message->length,
           _mqtt_state.pending_msg_id,
           _mqtt_state.pending_msg_type,
           millis(),
           remove_on_sent);
    LOG("Outbox size: %d\r\n", ob_get_size(_outbox));
}
void MQTTClient::send_ping()
{

    _mqtt_state.outbound_message = mqtt_msg_pingreq(&_mqtt_state.mqtt_connection);
    _mqtt_state.pending_msg_type = mqtt_get_type(_mqtt_state.outbound_message->data);
    _mqtt_state.pending_msg_id = mqtt_get_id(_mqtt_state.outbound_message->data,
                                 _mqtt_state.outbound_message->length);
    LOG("Sending pingreq");
    _tcp->write(_mqtt_state.outbound_message->data,
                _mqtt_state.outbound_message->length);
}

int MQTTClient::subscribe(String topic)
{
    return subscribe(topic, 0);
}
int MQTTClient::subscribe(String topic, uint8_t qos)
{
    _mqtt_state.outbound_message = mqtt_msg_subscribe(&_mqtt_state.mqtt_connection,
                                   topic.c_str(), qos,
                                   &_mqtt_state.pending_msg_id);

    LOG("Queue subscribe, topic\"%s\", id: %d\r\n", topic.c_str(), _mqtt_state.pending_msg_id);
    queue(0);
    return _mqtt_state.pending_msg_id;
}
int MQTTClient::publish(String topic, String data)
{
    return publish(topic, data, 0, 0);
}
int MQTTClient::publish(String topic, String data, int qos, int retain)
{
    int remove_on_sent = 0;
    _mqtt_state.outbound_message = mqtt_msg_publish(&_mqtt_state.mqtt_connection,
                                   topic.c_str(), data.c_str(), data.length(),
                                   qos, retain,
                                   &_mqtt_state.pending_msg_id);
    LOG("Queue publish, topic\"%s\", id: %d\r\n", topic.c_str(), _mqtt_state.pending_msg_id);
    if(qos == 0)
        remove_on_sent = 1;

    queue(remove_on_sent);
    return _mqtt_state.pending_msg_id;
}