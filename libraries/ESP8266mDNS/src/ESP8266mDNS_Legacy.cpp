/*

    ESP8266 Multicast DNS (port of CC3000 Multicast DNS library)
    Version 1.1
    Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
    ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)
    MDNS-SD Suport 2015 Hristo Gochkov
    Extended MDNS-SD support 2016 Lars Englund (lars.englund@gmail.com)


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

// Important RFC's for reference:
// - DNS request and response: http://www.ietf.org/rfc/rfc1035.txt
// - Multicast DNS: http://www.ietf.org/rfc/rfc6762.txt
// - MDNS-SD: https://tools.ietf.org/html/rfc6763

#ifndef LWIP_OPEN_SRC
#define LWIP_OPEN_SRC
#endif

#include "ESP8266mDNS.h"
#include <functional>

#include "debug.h"

extern "C" {
#include "osapi.h"
#include "ets_sys.h"
#include "user_interface.h"
}

#include "WiFiUdp.h"
#include "lwip/opt.h"
#include "lwip/udp.h"
#include "lwip/inet.h"
#include "lwip/igmp.h"
#include "lwip/mem.h"
#include "include/UdpContext.h"



namespace Legacy_MDNSResponder
{


#ifdef DEBUG_ESP_MDNS
#define DEBUG_ESP_MDNS_ERR
#define DEBUG_ESP_MDNS_TX
#define DEBUG_ESP_MDNS_RX
#endif

#define MDNS_NAME_REF   0xC000

#define MDNS_TYPE_AAAA  0x001C
#define MDNS_TYPE_A     0x0001
#define MDNS_TYPE_PTR   0x000C
#define MDNS_TYPE_SRV   0x0021
#define MDNS_TYPE_TXT   0x0010

#define MDNS_CLASS_IN             0x0001
#define MDNS_CLASS_IN_FLUSH_CACHE 0x8001

#define MDNS_ANSWERS_ALL  0x0F
#define MDNS_ANSWER_PTR   0x08
#define MDNS_ANSWER_TXT   0x04
#define MDNS_ANSWER_SRV   0x02
#define MDNS_ANSWER_A     0x01

#define _conn_read32() (((uint32_t)_conn->read() << 24) | ((uint32_t)_conn->read() << 16) | ((uint32_t)_conn->read() << 8) | _conn->read())
#define _conn_read16() (((uint16_t)_conn->read() << 8) | _conn->read())
#define _conn_read8() _conn->read()
#define _conn_readS(b,l) _conn->read((char*)(b),l);

static const IPAddress MDNS_MULTICAST_ADDR(224, 0, 0, 251);
static const int MDNS_MULTICAST_TTL = 1;
static const int MDNS_PORT = 5353;

struct MDNSService
{
    MDNSService* _next;
    char _name[32];
    char _proto[4];
    uint16_t _port;
    uint16_t _txtLen; // length of all txts
    struct MDNSTxt * _txts;
};

struct MDNSTxt
{
    MDNSTxt * _next;
    String _txt;
};

struct MDNSAnswer
{
    MDNSAnswer* next;
    uint8_t ip[4];
    uint16_t port;
    char *hostname;
};

struct MDNSQuery
{
    char _service[32];
    char _proto[4];
};


MDNSResponder::MDNSResponder() : _conn(0)
{
    _services = 0;
    _instanceName = "";
    _answers = 0;
    _query = 0;
    _newQuery = false;
    _waitingForAnswers = false;
}
MDNSResponder::~MDNSResponder()
{
    if (_query != 0)
    {
        os_free(_query);
        _query = 0;
    }

    // Clear answer list
    MDNSAnswer *answer;
    int numAnswers = _getNumAnswers();
    for (int n = numAnswers - 1; n >= 0; n--)
    {
        answer = _getAnswerFromIdx(n);
        os_free(answer->hostname);
        os_free(answer);
        answer = 0;
    }
    _answers = 0;

    if (_conn)
    {
        _conn->unref();
    }
}

bool MDNSResponder::begin(const char* hostname)
{
    size_t n = strlen(hostname);
    if (n > 63)   // max size for a single label.
    {
        return false;
    }

    // Copy in hostname characters as lowercase
    _hostName = hostname;
    _hostName.toLowerCase();

    // If instance name is not already set copy hostname to instance name
    if (_instanceName.equals(""))
    {
        _instanceName = hostname;
    }

    _gotIPHandler = WiFi.onStationModeGotIP([this](const WiFiEventStationModeGotIP & event)
    {
        (void) event;
        _restart();
    });

    _disconnectedHandler = WiFi.onStationModeDisconnected([this](const WiFiEventStationModeDisconnected & event)
    {
        (void) event;
        _restart();
    });

    return _listen();
}

void MDNSResponder::notifyAPChange()
{
    _restart();
}

void MDNSResponder::_restart()
{
    if (_conn)
    {
        _conn->unref();
        _conn = nullptr;
    }
    _listen();
}

bool MDNSResponder::_listen()
{
    // Open the MDNS socket if it isn't already open.
    if (!_conn)
    {
#ifdef DEBUG_ESP_MDNS_RX
        DEBUG_ESP_PORT.println("MDNS listening");
#endif

        IPAddress mdns(MDNS_MULTICAST_ADDR);

        if (igmp_joingroup(IP4_ADDR_ANY4, mdns) != ERR_OK)
        {
            return false;
        }

        _conn = new UdpContext;
        _conn->ref();

        if (!_conn->listen(IP_ADDR_ANY, MDNS_PORT))
        {
            return false;
        }
        _conn->setMulticastTTL(MDNS_MULTICAST_TTL);
        _conn->onRx(std::bind(&MDNSResponder::update, this));
        _conn->connect(mdns, MDNS_PORT);
    }
    return true;
}

void MDNSResponder::update()
{
    if (!_conn || !_conn->next())
    {
        return;
    }
    _parsePacket();
}


void MDNSResponder::setInstanceName(String name)
{
    if (name.length() > 63)
    {
        return;
    }
    _instanceName = name;
}


bool MDNSResponder::addServiceTxt(char *name, char *proto, char *key, char *value)
{
    MDNSService* servicePtr;

    uint8_t txtLen = os_strlen(key) + os_strlen(value) + 1; // Add one for equals sign
    txtLen += 1; //accounts for length byte added when building the txt responce
    //Find the service
    for (servicePtr = _services; servicePtr; servicePtr = servicePtr->_next)
    {
        //Checking Service names
        if (strcmp(servicePtr->_name, name) == 0 && strcmp(servicePtr->_proto, proto) == 0)
        {
            //found a service name match
            if (servicePtr->_txtLen + txtLen > 1300)
            {
                return false;    //max txt record size
            }
            MDNSTxt *newtxt = new MDNSTxt;
            newtxt->_txt = String(key) + '=' + String(value);
            newtxt->_next = 0;
            if (servicePtr->_txts == 0)  //no services have been added
            {
                //Adding First TXT to service
                servicePtr->_txts = newtxt;
                servicePtr->_txtLen += txtLen;
                return true;
            }
            else
            {
                MDNSTxt * txtPtr = servicePtr->_txts;
                while (txtPtr->_next != 0)
                {
                    txtPtr = txtPtr->_next;
                }
                //adding another TXT to service
                txtPtr->_next = newtxt;
                servicePtr->_txtLen += txtLen;
                return true;
            }
        }
    }
    return false;
}

void MDNSResponder::addService(char *name, char *proto, uint16_t port)
{
    if (_getServicePort(name, proto) != 0)
    {
        return;
    }
    if (os_strlen(name) > 32 || os_strlen(proto) != 3)
    {
        return;    //bad arguments
    }
    struct MDNSService *srv = (struct MDNSService*)(os_malloc(sizeof(struct MDNSService)));
    os_strcpy(srv->_name, name);
    os_strcpy(srv->_proto, proto);
    srv->_port = port;
    srv->_next = 0;
    srv->_txts = 0;
    srv->_txtLen = 0;

    if (_services == 0)
    {
        _services = srv;
    }
    else
    {
        MDNSService* servicePtr = _services;
        while (servicePtr->_next != 0)
        {
            servicePtr = servicePtr->_next;
        }
        servicePtr->_next = srv;
    }

}

int MDNSResponder::queryService(char *service, char *proto)
{
#ifdef DEBUG_ESP_MDNS_TX
    DEBUG_ESP_PORT.printf("queryService %s %s\n", service, proto);
#endif
    while (_answers != 0)
    {
        MDNSAnswer *currAnswer = _answers;
        _answers = _answers->next;
        os_free(currAnswer->hostname);
        os_free(currAnswer);
        currAnswer = 0;
    }
    if (_query != 0)
    {
        os_free(_query);
        _query = 0;
    }
    _query = (struct MDNSQuery*)(os_malloc(sizeof(struct MDNSQuery)));
    os_strcpy(_query->_service, service);
    os_strcpy(_query->_proto, proto);
    _newQuery = true;

    char underscore[] = "_";

    // build service name with _
    char serviceName[os_strlen(service) + 2];
    os_strcpy(serviceName, underscore);
    os_strcat(serviceName, service);
    size_t serviceNameLen = os_strlen(serviceName);

    //build proto name with _
    char protoName[5];
    os_strcpy(protoName, underscore);
    os_strcat(protoName, proto);
    size_t protoNameLen = 4;

    //local string
    char localName[] = "local";
    size_t localNameLen = 5;

    //terminator
    char terminator[] = "\0";

    // Only supports sending one PTR query
    uint8_t questionCount = 1;

    _waitingForAnswers = true;
    for (int itfn = 0; itfn < 2; itfn++)
    {
        struct ip_info ip_info;

        wifi_get_ip_info((!itfn) ? SOFTAP_IF : STATION_IF, &ip_info);
        if (!ip_info.ip.addr)
        {
            continue;
        }
        _conn->setMulticastInterface(IPAddress(ip_info.ip.addr));

        // Write the header
        _conn->flush();
        uint8_t head[12] =
        {
            0x00, 0x00, //ID = 0
            0x00, 0x00, //Flags = response + authoritative answer
            0x00, questionCount, //Question count
            0x00, 0x00, //Answer count
            0x00, 0x00, //Name server records
            0x00, 0x00 //Additional records
        };
        _conn->append(reinterpret_cast<const char*>(head), 12);

        // Only supports sending one PTR query
        // Send the Name field (eg. "_http._tcp.local")
        _conn->append(reinterpret_cast<const char*>(&serviceNameLen), 1);          // lenght of "_" + service
        _conn->append(reinterpret_cast<const char*>(serviceName), serviceNameLen); // "_" + service
        _conn->append(reinterpret_cast<const char*>(&protoNameLen), 1);            // lenght of "_" + proto
        _conn->append(reinterpret_cast<const char*>(protoName), protoNameLen);     // "_" + proto
        _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);            // lenght of "local"
        _conn->append(reinterpret_cast<const char*>(localName), localNameLen);     // "local"
        _conn->append(reinterpret_cast<const char*>(&terminator), 1);              // terminator

        //Send the type and class
        uint8_t ptrAttrs[4] =
        {
            0x00, 0x0c, //PTR record query
            0x00, 0x01 //Class IN
        };
        _conn->append(reinterpret_cast<const char*>(ptrAttrs), 4);
        _conn->send();
    }

#ifdef DEBUG_ESP_MDNS_TX
    DEBUG_ESP_PORT.println("Waiting for answers..");
#endif
    delay(1000);

    _waitingForAnswers = false;

    return _getNumAnswers();
}

String MDNSResponder::hostname(int idx)
{
    MDNSAnswer *answer = _getAnswerFromIdx(idx);
    if (answer == 0)
    {
        return String();
    }
    return answer->hostname;
}

IPAddress MDNSResponder::IP(int idx)
{
    MDNSAnswer *answer = _getAnswerFromIdx(idx);
    if (answer == 0)
    {
        return IPAddress();
    }
    return IPAddress(answer->ip);
}

uint16_t MDNSResponder::port(int idx)
{
    MDNSAnswer *answer = _getAnswerFromIdx(idx);
    if (answer == 0)
    {
        return 0;
    }
    return answer->port;
}

MDNSAnswer* MDNSResponder::_getAnswerFromIdx(int idx)
{
    MDNSAnswer *answer = _answers;
    while (answer != 0 && idx-- > 0)
    {
        answer = answer->next;
    }
    if (idx > 0)
    {
        return 0;
    }
    return answer;
}

int MDNSResponder::_getNumAnswers()
{
    int numAnswers = 0;
    MDNSAnswer *answer = _answers;
    while (answer != 0)
    {
        numAnswers++;
        answer = answer->next;
    }
    return numAnswers;
}

MDNSTxt * MDNSResponder::_getServiceTxt(char *name, char *proto)
{
    MDNSService* servicePtr;
    for (servicePtr = _services; servicePtr; servicePtr = servicePtr->_next)
    {
        if (servicePtr->_port > 0 && strcmp(servicePtr->_name, name) == 0 && strcmp(servicePtr->_proto, proto) == 0)
        {
            if (servicePtr->_txts == 0)
            {
                return nullptr;
            }
            return servicePtr->_txts;
        }
    }
    return nullptr;
}

uint16_t MDNSResponder::_getServiceTxtLen(char *name, char *proto)
{
    MDNSService* servicePtr;
    for (servicePtr = _services; servicePtr; servicePtr = servicePtr->_next)
    {
        if (servicePtr->_port > 0 && strcmp(servicePtr->_name, name) == 0 && strcmp(servicePtr->_proto, proto) == 0)
        {
            if (servicePtr->_txts == 0)
            {
                return false;
            }
            return servicePtr->_txtLen;
        }
    }
    return 0;
}

uint16_t MDNSResponder::_getServicePort(char *name, char *proto)
{
    MDNSService* servicePtr;
    for (servicePtr = _services; servicePtr; servicePtr = servicePtr->_next)
    {
        if (servicePtr->_port > 0 && strcmp(servicePtr->_name, name) == 0 && strcmp(servicePtr->_proto, proto) == 0)
        {
            return servicePtr->_port;
        }
    }
    return 0;
}

IPAddress MDNSResponder::_getRequestMulticastInterface()
{
    struct ip_info ip_info;
    bool match_ap = false;
    if (wifi_get_opmode() & SOFTAP_MODE)
    {
        const IPAddress& remote_ip = _conn->getRemoteAddress();
        wifi_get_ip_info(SOFTAP_IF, &ip_info);
        IPAddress infoIp(ip_info.ip);
        IPAddress infoMask(ip_info.netmask);
        if (ip_info.ip.addr && ip_addr_netcmp((const ip_addr_t*)remote_ip, (const ip_addr_t*)infoIp, ip_2_ip4((const ip_addr_t*)infoMask)))
        {
            match_ap = true;
        }
    }
    if (!match_ap)
    {
        wifi_get_ip_info(STATION_IF, &ip_info);
    }
    return IPAddress(ip_info.ip.addr);
}

void MDNSResponder::_parsePacket()
{
    int i;
    char tmp;
    bool serviceParsed = false;
    bool protoParsed = false;
    bool localParsed = false;

    char hostName[255];
    uint8_t hostNameLen;

    char serviceName[32];
    uint8_t serviceNameLen;
    uint16_t servicePort = 0;

    char protoName[32];
    protoName[0] = 0;
    uint8_t protoNameLen = 0;

    uint16_t packetHeader[6];

    for (i = 0; i < 6; i++)
    {
        packetHeader[i] = _conn_read16();
    }

    if ((packetHeader[1] & 0x8000) != 0)   // Read answers
    {
#ifdef DEBUG_ESP_MDNS_RX
        DEBUG_ESP_PORT.printf("Reading answers RX: REQ, ID:%u, Q:%u, A:%u, NS:%u, ADD:%u\n", packetHeader[0], packetHeader[2], packetHeader[3], packetHeader[4], packetHeader[5]);
#endif

        if (!_waitingForAnswers)
        {
#ifdef DEBUG_ESP_MDNS_RX
            DEBUG_ESP_PORT.println("Not expecting any answers right now, returning");
#endif
            _conn->flush();
            return;
        }

        int numAnswers = packetHeader[3] + packetHeader[5];
        // Assume that the PTR answer always comes first and that it is always accompanied by a TXT, SRV, AAAA (optional) and A answer in the same packet.
        if (numAnswers < 4)
        {
#ifdef DEBUG_ESP_MDNS_RX
            DEBUG_ESP_PORT.printf("Expected a packet with 4 or more answers, got %u\n", numAnswers);
#endif
            _conn->flush();
            return;
        }

        uint8_t tmp8;
        uint16_t answerPort = 0;
        uint8_t answerIp[4] = { 0, 0, 0, 0 };
        char answerHostName[255];
        bool serviceMatch = false;
        MDNSAnswer *answer;
        uint8_t partsCollected = 0;
        uint8_t stringsRead = 0;

        answerHostName[0] = '\0';

        // Clear answer list
        if (_newQuery)
        {
            int oldAnswers = _getNumAnswers();
            for (int n = oldAnswers - 1; n >= 0; n--)
            {
                answer = _getAnswerFromIdx(n);
                os_free(answer->hostname);
                os_free(answer);
                answer = 0;
            }
            _answers = 0;
            _newQuery = false;
        }

        while (numAnswers--)
        {
            // Read name
            stringsRead = 0;
            size_t last_bufferpos = 0;
            do
            {
                tmp8 = _conn_read8();
                if (tmp8 == 0x00)   // End of name
                {
                    break;
                }
                if (tmp8 & 0xC0)   // Compressed pointer
                {
                    uint16_t offset = ((((uint16_t)tmp8) & ~0xC0) << 8) | _conn_read8();
                    if (_conn->isValidOffset(offset))
                    {
                        if (0 == last_bufferpos)
                        {
                            last_bufferpos  = _conn->tell();
                        }
#ifdef DEBUG_ESP_MDNS_RX
                        DEBUG_ESP_PORT.print("Compressed pointer, jumping from ");
                        DEBUG_ESP_PORT.print(last_bufferpos);
                        DEBUG_ESP_PORT.print(" to ");
                        DEBUG_ESP_PORT.println(offset);
#endif
                        _conn->seek(offset);
                        tmp8 = _conn_read8();
                    }
                    else
                    {
#ifdef DEBUG_ESP_MDNS_RX
                        DEBUG_ESP_PORT.print("Skipping malformed compressed pointer");
#endif
                        tmp8 = _conn_read8();
                        break;
                    }
                }
                if (stringsRead > 3)
                {
#ifdef DEBUG_ESP_MDNS_RX
                    DEBUG_ESP_PORT.println("failed to read the response name");
#endif
                    _conn->flush();
                    return;
                }
                _conn_readS(serviceName, tmp8);
                serviceName[tmp8] = '\0';
#ifdef DEBUG_ESP_MDNS_RX
                DEBUG_ESP_PORT.printf(" %d ", tmp8);
                for (int n = 0; n < tmp8; n++)
                {
                    DEBUG_ESP_PORT.printf("%c", serviceName[n]);
                }
                DEBUG_ESP_PORT.println();
#endif
                if (serviceName[0] == '_')
                {
                    if (strcmp(&serviceName[1], _query->_service) == 0)
                    {
                        serviceMatch = true;
#ifdef DEBUG_ESP_MDNS_RX
                        DEBUG_ESP_PORT.printf("found matching service: %s\n", _query->_service);
#endif
                    }
                }
                stringsRead++;
            } while (true);
            if (last_bufferpos > 0)
            {
                _conn->seek(last_bufferpos);
#ifdef DEBUG_ESP_MDNS_RX
                DEBUG_ESP_PORT.print("Compressed pointer, jumping back to ");
                DEBUG_ESP_PORT.println(last_bufferpos);
#endif
            }

            uint16_t answerType = _conn_read16(); // Read type
            uint16_t answerClass = _conn_read16(); // Read class
            uint32_t answerTtl = _conn_read32(); // Read ttl
            uint16_t answerRdlength = _conn_read16(); // Read rdlength

            (void) answerClass;
            (void) answerTtl;

            if (answerRdlength > 255)
            {
                if (answerType == MDNS_TYPE_TXT && answerRdlength < 1460)
                {
                    while (--answerRdlength)
                    {
                        _conn->read();
                    }
                }
                else
                {
#ifdef DEBUG_ESP_MDNS_RX
                    DEBUG_ESP_PORT.printf("Data len too long! %u\n", answerRdlength);
#endif
                    _conn->flush();
                    return;
                }
            }

#ifdef DEBUG_ESP_MDNS_RX
            DEBUG_ESP_PORT.printf("type: %04x rdlength: %d\n", answerType, answerRdlength);
#endif

            if (answerType == MDNS_TYPE_PTR)
            {
                partsCollected |= 0x01;
                _conn_readS(hostName, answerRdlength); // Read rdata
                if (hostName[answerRdlength - 2] & 0xc0)
                {
                    memcpy(answerHostName, hostName + 1, answerRdlength - 3);
                    answerHostName[answerRdlength - 3] = '\0';
                }
#ifdef DEBUG_ESP_MDNS_RX
                DEBUG_ESP_PORT.printf("PTR %d ", answerRdlength);
                for (int n = 0; n < answerRdlength; n++)
                {
                    DEBUG_ESP_PORT.printf("%c", hostName[n]);
                }
                DEBUG_ESP_PORT.println();
#endif
            }

            else if (answerType == MDNS_TYPE_TXT)
            {
                partsCollected |= 0x02;
                _conn_readS(hostName, answerRdlength); // Read rdata
#ifdef DEBUG_ESP_MDNS_RX
                DEBUG_ESP_PORT.printf("TXT %d ", answerRdlength);
                for (int n = 0; n < answerRdlength; n++)
                {
                    DEBUG_ESP_PORT.printf("%c", hostName[n]);
                }
                DEBUG_ESP_PORT.println();
#endif
            }

            else if (answerType == MDNS_TYPE_SRV)
            {
                partsCollected |= 0x04;
                uint16_t answerPrio = _conn_read16(); // Read priority
                uint16_t answerWeight = _conn_read16(); // Read weight
                answerPort = _conn_read16(); // Read port
                last_bufferpos = 0;

                (void) answerPrio;
                (void) answerWeight;

                // Read hostname
                tmp8 = _conn_read8();
                if (tmp8 & 0xC0)   // Compressed pointer
                {
                    uint16_t offset = ((((uint16_t)tmp8) & ~0xC0) << 8) | _conn_read8();
                    if (_conn->isValidOffset(offset))
                    {
                        last_bufferpos = _conn->tell();
#ifdef DEBUG_ESP_MDNS_RX
                        DEBUG_ESP_PORT.print("Compressed pointer, jumping from ");
                        DEBUG_ESP_PORT.print(last_bufferpos);
                        DEBUG_ESP_PORT.print(" to ");
                        DEBUG_ESP_PORT.println(offset);
#endif
                        _conn->seek(offset);
                        tmp8 = _conn_read8();
                    }
                    else
                    {
#ifdef DEBUG_ESP_MDNS_RX
                        DEBUG_ESP_PORT.print("Skipping malformed compressed pointer");
#endif
                        tmp8 = _conn_read8();
                        break;
                    }
                }
                _conn_readS(answerHostName, tmp8);
                answerHostName[tmp8] = '\0';
#ifdef DEBUG_ESP_MDNS_RX
                DEBUG_ESP_PORT.printf("SRV %d ", tmp8);
                for (int n = 0; n < tmp8; n++)
                {
                    DEBUG_ESP_PORT.printf("%02x ", answerHostName[n]);
                }
                DEBUG_ESP_PORT.printf("\n%s\n", answerHostName);
#endif
                if (last_bufferpos > 0)
                {
                    _conn->seek(last_bufferpos);
                    tmp8 = 2; // Size of compression octets
#ifdef DEBUG_ESP_MDNS_RX
                    DEBUG_ESP_PORT.print("Compressed pointer, jumping back to ");
                    DEBUG_ESP_PORT.println(last_bufferpos);
#endif
                }
                if (answerRdlength - (6 + 1 + tmp8) > 0)   // Skip any remaining rdata
                {
                    _conn_readS(hostName, answerRdlength - (6 + 1 + tmp8));
                }
            }

            else if (answerType == MDNS_TYPE_A)
            {
                partsCollected |= 0x08;
                for (int i = 0; i < 4; i++)
                {
                    answerIp[i] = _conn_read8();
                }
            }
            else
            {
#ifdef DEBUG_ESP_MDNS_RX
                DEBUG_ESP_PORT.printf("Ignoring unsupported type %02x\n", tmp8);
#endif
                for (int n = 0; n < answerRdlength; n++)
                {
                    (void)_conn_read8();
                }
            }

            if ((partsCollected == 0x0F) && serviceMatch)
            {
#ifdef DEBUG_ESP_MDNS_RX
                DEBUG_ESP_PORT.println("All answers parsed, adding to _answers list..");
#endif
                // Add new answer to answer list
                if (_answers == 0)
                {
                    _answers = (struct MDNSAnswer*)(os_malloc(sizeof(struct MDNSAnswer)));
                    answer = _answers;
                }
                else
                {
                    answer = _answers;
                    while (answer->next != 0)
                    {
                        answer = answer->next;
                    }
                    answer->next = (struct MDNSAnswer*)(os_malloc(sizeof(struct MDNSAnswer)));
                    answer = answer->next;
                }
                answer->next = 0;
                answer->hostname = 0;

                // Populate new answer
                answer->port = answerPort;
                for (int i = 0; i < 4; i++)
                {
                    answer->ip[i] = answerIp[i];
                }
                answer->hostname = (char *)os_malloc(strlen(answerHostName) + 1);
                os_strcpy(answer->hostname, answerHostName);
                _conn->flush();
                return;
            }
        }

        _conn->flush();
        return;
    }

    // PARSE REQUEST NAME

    hostNameLen = _conn_read8() % 255;
    _conn_readS(hostName, hostNameLen);
    hostName[hostNameLen] = '\0';

    if (hostName[0] == '_')
    {
        serviceParsed = true;
        memcpy(serviceName, hostName + 1, hostNameLen);
        serviceNameLen = hostNameLen - 1;
        hostNameLen = 0;
    }

    if (hostNameLen > 0 && !_hostName.equals(hostName) && !_instanceName.equals(hostName))
    {
#ifdef DEBUG_ESP_MDNS_ERR
        DEBUG_ESP_PORT.printf("ERR_NO_HOST: %s\n", hostName);
        DEBUG_ESP_PORT.printf("hostname: %s\n", _hostName.c_str());
        DEBUG_ESP_PORT.printf("instance: %s\n", _instanceName.c_str());
#endif
        _conn->flush();
        return;
    }

    if (!serviceParsed)
    {
        serviceNameLen = _conn_read8() % 255;
        _conn_readS(serviceName, serviceNameLen);
        serviceName[serviceNameLen] = '\0';

        if (serviceName[0] == '_')
        {
            memmove(serviceName, serviceName + 1, serviceNameLen);
            serviceNameLen--;
            serviceParsed = true;
        }
        else if (serviceNameLen == 5 && strcmp("local", serviceName) == 0)
        {
            tmp = _conn_read8();
            if (tmp == 0)
            {
                serviceParsed = true;
                serviceNameLen = 0;
                protoParsed = true;
                protoNameLen = 0;
                localParsed = true;
            }
            else
            {
#ifdef DEBUG_ESP_MDNS_ERR
                DEBUG_ESP_PORT.printf("ERR_FQDN: %s\n", serviceName);
#endif
                _conn->flush();
                return;
            }
        }
        else
        {
#ifdef DEBUG_ESP_MDNS_ERR
            DEBUG_ESP_PORT.printf("ERR_SERVICE: %s\n", serviceName);
#endif
            _conn->flush();
            return;
        }
    }

    if (!protoParsed)
    {
        protoNameLen = _conn_read8() % 255;
        _conn_readS(protoName, protoNameLen);
        protoName[protoNameLen] = '\0';
        if (protoNameLen == 4 && protoName[0] == '_')
        {
            memmove(protoName, protoName + 1, protoNameLen);
            protoNameLen--;
            protoParsed = true;
        }
        else if (strcmp("services", serviceName) == 0 && strcmp("_dns-sd", protoName) == 0)
        {
            _conn->flush();
            IPAddress interface = _getRequestMulticastInterface();
            _replyToTypeEnumRequest(interface);
            return;
        }
        else
        {
#ifdef DEBUG_ESP_MDNS_ERR
            DEBUG_ESP_PORT.printf("ERR_PROTO: %s\n", protoName);
#endif
            _conn->flush();
            return;
        }
    }

    if (!localParsed)
    {
        char localName[32];
        uint8_t localNameLen = _conn_read8() % 31;
        _conn_readS(localName, localNameLen);
        localName[localNameLen] = '\0';
        tmp = _conn_read8();
        if (localNameLen == 5 && strcmp("local", localName) == 0 && tmp == 0)
        {
            localParsed = true;
        }
        else
        {
#ifdef DEBUG_ESP_MDNS_ERR
            DEBUG_ESP_PORT.printf("ERR_FQDN: %s\n", localName);
#endif
            _conn->flush();
            return;
        }
    }

    if (serviceNameLen > 0 && protoNameLen > 0)
    {
        servicePort = _getServicePort(serviceName, protoName);
        if (servicePort == 0)
        {
#ifdef DEBUG_ESP_MDNS_ERR
            DEBUG_ESP_PORT.printf("ERR_NO_SERVICE: %s\n", serviceName);
#endif
            _conn->flush();
            return;
        }
    }
    else if (serviceNameLen > 0 || protoNameLen > 0)
    {
#ifdef DEBUG_ESP_MDNS_ERR
        DEBUG_ESP_PORT.printf("ERR_SERVICE_PROTO: %s\n", serviceName);
#endif
        _conn->flush();
        return;
    }

    // RESPOND

#ifdef DEBUG_ESP_MDNS_RX
    DEBUG_ESP_PORT.printf("RX: REQ, ID:%u, Q:%u, A:%u, NS:%u, ADD:%u\n", packetHeader[0], packetHeader[2], packetHeader[3], packetHeader[4], packetHeader[5]);
#endif

    uint16_t currentType;
    uint16_t currentClass;

    int numQuestions = packetHeader[2];
    if (numQuestions > 4)
    {
        numQuestions = 4;
    }
    uint16_t questions[4];
    int question = 0;

    while (numQuestions--)
    {
        currentType = _conn_read16();
        if (currentType & MDNS_NAME_REF) //new header handle it better!
        {
            currentType = _conn_read16();
        }
        currentClass = _conn_read16();
        if (currentClass & MDNS_CLASS_IN)
        {
            questions[question++] = currentType;
        }

        if (numQuestions > 0)
        {
            if (_conn_read16() != 0xC00C) //new question but for another host/service
            {
                _conn->flush();
                numQuestions = 0;
            }
        }

#ifdef DEBUG_ESP_MDNS_RX
        DEBUG_ESP_PORT.printf("REQ: ");
        if (hostNameLen > 0)
        {
            DEBUG_ESP_PORT.printf("%s.", hostName);
        }
        if (serviceNameLen > 0)
        {
            DEBUG_ESP_PORT.printf("_%s.", serviceName);
        }
        if (protoNameLen > 0)
        {
            DEBUG_ESP_PORT.printf("_%s.", protoName);
        }
        DEBUG_ESP_PORT.printf("local. ");

        if (currentType == MDNS_TYPE_AAAA)
        {
            DEBUG_ESP_PORT.printf("  AAAA ");
        }
        else if (currentType == MDNS_TYPE_A)
        {
            DEBUG_ESP_PORT.printf("  A ");
        }
        else if (currentType == MDNS_TYPE_PTR)
        {
            DEBUG_ESP_PORT.printf("  PTR ");
        }
        else if (currentType == MDNS_TYPE_SRV)
        {
            DEBUG_ESP_PORT.printf("  SRV ");
        }
        else if (currentType == MDNS_TYPE_TXT)
        {
            DEBUG_ESP_PORT.printf("  TXT ");
        }
        else
        {
            DEBUG_ESP_PORT.printf("  0x%04X ", currentType);
        }

        if (currentClass == MDNS_CLASS_IN)
        {
            DEBUG_ESP_PORT.printf("  IN ");
        }
        else if (currentClass == MDNS_CLASS_IN_FLUSH_CACHE)
        {
            DEBUG_ESP_PORT.printf("  IN[F] ");
        }
        else
        {
            DEBUG_ESP_PORT.printf("  0x%04X ", currentClass);
        }

        DEBUG_ESP_PORT.printf("\n");
#endif
    }
    uint8_t questionMask = 0;
    uint8_t responseMask = 0;
    for (i = 0; i < question; i++)
    {
        if (questions[i] == MDNS_TYPE_A)
        {
            questionMask |= 0x1;
            responseMask |= 0x1;
        }
        else if (questions[i] == MDNS_TYPE_SRV)
        {
            questionMask |= 0x2;
            responseMask |= 0x3;
        }
        else if (questions[i] == MDNS_TYPE_TXT)
        {
            questionMask |= 0x4;
            responseMask |= 0x4;
        }
        else if (questions[i] == MDNS_TYPE_PTR)
        {
            questionMask |= 0x8;
            responseMask |= 0xF;
        }
    }

    IPAddress interface = _getRequestMulticastInterface();
    return _replyToInstanceRequest(questionMask, responseMask, serviceName, protoName, servicePort, interface);
}


/**
    STRINGIZE
*/
#ifndef STRINGIZE
#define STRINGIZE(x) #x
#endif
#ifndef STRINGIZE_VALUE_OF
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
#endif


void MDNSResponder::enableArduino(uint16_t port, bool auth)
{

    addService("arduino", "tcp", port);
    addServiceTxt("arduino", "tcp", "tcp_check", "no");
    addServiceTxt("arduino", "tcp", "ssh_upload", "no");
    addServiceTxt("arduino", "tcp", "board", STRINGIZE_VALUE_OF(ARDUINO_BOARD));
    addServiceTxt("arduino", "tcp", "auth_upload", (auth) ? "yes" : "no");
}

void MDNSResponder::_replyToTypeEnumRequest(IPAddress multicastInterface)
{
    MDNSService* servicePtr;
    for (servicePtr = _services; servicePtr; servicePtr = servicePtr->_next)
    {
        if (servicePtr->_port > 0)
        {
            char *service = servicePtr->_name;
            char *proto = servicePtr->_proto;
            //uint16_t port = servicePtr->_port;

#ifdef DEBUG_ESP_MDNS_TX
            DEBUG_ESP_PORT.printf("TX: service:%s, proto:%s\n", service, proto);
#endif

            char sdHostName[] = "_services";
            size_t sdHostNameLen = 9;
            char sdServiceName[] = "_dns-sd";
            size_t sdServiceNameLen = 7;
            char sdProtoName[] = "_udp";
            size_t sdProtoNameLen = 4;

            char underscore[]  = "_";

            // build service name with _
            char serviceName[os_strlen(service) + 2];
            os_strcpy(serviceName, underscore);
            os_strcat(serviceName, service);
            size_t serviceNameLen = os_strlen(serviceName);

            //build proto name with _
            char protoName[5];
            os_strcpy(protoName, underscore);
            os_strcat(protoName, proto);
            size_t protoNameLen = 4;

            //local string
            char localName[] = "local";
            size_t localNameLen = 5;

            //terminator
            char terminator[] = "\0";

            //Write the header
            _conn->flush();
            uint8_t head[12] =
            {
                0x00, 0x00, //ID = 0
                0x84, 0x00, //Flags = response + authoritative answer
                0x00, 0x00, //Question count
                0x00, 0x01, //Answer count
                0x00, 0x00, //Name server records
                0x00, 0x00, //Additional records
            };
            _conn->append(reinterpret_cast<const char*>(head), 12);

            // Send the Name field (ie. "_services._dns-sd._udp.local")
            _conn->append(reinterpret_cast<const char*>(&sdHostNameLen), 1);         // length of "_services"
            _conn->append(reinterpret_cast<const char*>(sdHostName), sdHostNameLen); // "_services"
            _conn->append(reinterpret_cast<const char*>(&sdServiceNameLen), 1);      // length of "_dns-sd"
            _conn->append(reinterpret_cast<const char*>(sdServiceName), sdServiceNameLen);// "_dns-sd"
            _conn->append(reinterpret_cast<const char*>(&sdProtoNameLen), 1);        // length of "_udp"
            _conn->append(reinterpret_cast<const char*>(sdProtoName), sdProtoNameLen); // "_udp"
            _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);          // length "local"
            _conn->append(reinterpret_cast<const char*>(localName), localNameLen);   // "local"
            _conn->append(reinterpret_cast<const char*>(&terminator), 1);            // terminator

            //Send the type, class, ttl and rdata length
            uint8_t ptrDataLen = serviceNameLen + protoNameLen + localNameLen + 4;   // 4 is three label sizes and the terminator
            uint8_t ptrAttrs[10] =
            {
                0x00, 0x0c,             //PTR record query
                0x00, 0x01,             //Class IN
                0x00, 0x00, 0x11, 0x94, //TTL 4500
                0x00, ptrDataLen,       //RData length
            };
            _conn->append(reinterpret_cast<const char*>(ptrAttrs), 10);

            //Send the RData (ie. "_http._tcp.local")
            _conn->append(reinterpret_cast<const char*>(&serviceNameLen), 1);        // length of "_http"
            _conn->append(reinterpret_cast<const char*>(serviceName), serviceNameLen); // "_http"
            _conn->append(reinterpret_cast<const char*>(&protoNameLen), 1);          // length of "_tcp"
            _conn->append(reinterpret_cast<const char*>(protoName), protoNameLen);   // "_tcp"
            _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);          // length "local"
            _conn->append(reinterpret_cast<const char*>(localName), localNameLen);   // "local"
            _conn->append(reinterpret_cast<const char*>(&terminator), 1);            // terminator

            _conn->setMulticastInterface(multicastInterface);
            _conn->send();
        }
    }
}

void MDNSResponder::_replyToInstanceRequest(uint8_t questionMask, uint8_t responseMask, char * service, char *proto, uint16_t port, IPAddress multicastInterface)
{
    int i;
    if (questionMask == 0)
    {
        return;
    }
    if (responseMask == 0)
    {
        return;
    }

#ifdef DEBUG_ESP_MDNS_TX
    DEBUG_ESP_PORT.printf("TX: qmask:%01X, rmask:%01X, service:%s, proto:%s, port:%u\n", questionMask, responseMask, service, proto, port);
#endif


    String instanceName = _instanceName;
    size_t instanceNameLen = instanceName.length();

    String hostName = _hostName;
    size_t hostNameLen = hostName.length();

    char underscore[]  = "_";

    // build service name with _
    char serviceName[os_strlen(service) + 2];
    os_strcpy(serviceName, underscore);
    os_strcat(serviceName, service);
    size_t serviceNameLen = os_strlen(serviceName);

    //build proto name with _
    char protoName[5];
    os_strcpy(protoName, underscore);
    os_strcat(protoName, proto);
    size_t protoNameLen = 4;

    //local string
    char localName[] = "local";
    size_t localNameLen = 5;

    //terminator
    char terminator[] = "\0";

    uint8_t answerMask = responseMask & questionMask;
    uint8_t answerCount = 0;
    uint8_t additionalMask = responseMask & ~questionMask;
    uint8_t additionalCount = 0;
    for (i = 0; i < 4; i++)
    {
        if (answerMask & (1 << i))
        {
            answerCount++;
        }
        if (additionalMask & (1 << i))
        {
            additionalCount++;
        }
    }


    //Write the header
    _conn->flush();
    uint8_t head[12] =
    {
        0x00, 0x00, //ID = 0
        0x84, 0x00, //Flags = response + authoritative answer
        0x00, 0x00, //Question count
        0x00, answerCount, //Answer count
        0x00, 0x00, //Name server records
        0x00, additionalCount, //Additional records
    };
    _conn->append(reinterpret_cast<const char*>(head), 12);

    for (int responseSection = 0; responseSection < 2; ++responseSection)
    {

        // PTR Response
        if ((responseSection == 0 ? answerMask : additionalMask) & 0x8)
        {
            // Send the Name field (ie. "_http._tcp.local")
            _conn->append(reinterpret_cast<const char*>(&serviceNameLen), 1);        // length of "_http"
            _conn->append(reinterpret_cast<const char*>(serviceName), serviceNameLen); // "_http"
            _conn->append(reinterpret_cast<const char*>(&protoNameLen), 1);          // length of "_tcp"
            _conn->append(reinterpret_cast<const char*>(protoName), protoNameLen);   // "_tcp"
            _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);          // length "local"
            _conn->append(reinterpret_cast<const char*>(localName), localNameLen);   // "local"
            _conn->append(reinterpret_cast<const char*>(&terminator), 1);            // terminator

            //Send the type, class, ttl and rdata length
            uint8_t ptrDataLen = instanceNameLen + serviceNameLen + protoNameLen + localNameLen + 5; // 5 is four label sizes and the terminator
            uint8_t ptrAttrs[10] =
            {
                0x00, 0x0c,             //PTR record query
                0x00, 0x01,             //Class IN
                0x00, 0x00, 0x00, 0x78, //TTL 120
                0x00, ptrDataLen,       //RData length
            };
            _conn->append(reinterpret_cast<const char*>(ptrAttrs), 10);

            //Send the RData (ie. "My IOT device._http._tcp.local")
            _conn->append(reinterpret_cast<const char*>(&instanceNameLen), 1);       // length of "My IOT device"
            _conn->append(reinterpret_cast<const char*>(instanceName.c_str()), instanceNameLen);// "My IOT device"
            _conn->append(reinterpret_cast<const char*>(&serviceNameLen), 1);        // length of "_http"
            _conn->append(reinterpret_cast<const char*>(serviceName), serviceNameLen); // "_http"
            _conn->append(reinterpret_cast<const char*>(&protoNameLen), 1);          // length of "_tcp"
            _conn->append(reinterpret_cast<const char*>(protoName), protoNameLen);   // "_tcp"
            _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);          // length "local"
            _conn->append(reinterpret_cast<const char*>(localName), localNameLen);   // "local"
            _conn->append(reinterpret_cast<const char*>(&terminator), 1);            // terminator
        }

        //TXT Responce
        if ((responseSection == 0 ? answerMask : additionalMask) & 0x4)
        {
            //Send the name field (ie. "My IOT device._http._tcp.local")
            _conn->append(reinterpret_cast<const char*>(&instanceNameLen), 1);       // length of "My IOT device"
            _conn->append(reinterpret_cast<const char*>(instanceName.c_str()), instanceNameLen);// "My IOT device"
            _conn->append(reinterpret_cast<const char*>(&serviceNameLen), 1);        // length of "_http"
            _conn->append(reinterpret_cast<const char*>(serviceName), serviceNameLen); // "_http"
            _conn->append(reinterpret_cast<const char*>(&protoNameLen), 1);          // length of "_tcp"
            _conn->append(reinterpret_cast<const char*>(protoName), protoNameLen);   // "_tcp"
            _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);          // length "local"
            _conn->append(reinterpret_cast<const char*>(localName), localNameLen);   // "local"
            _conn->append(reinterpret_cast<const char*>(&terminator), 1);            // terminator

            //Send the type, class, ttl and rdata length
            uint8_t txtDataLen = _getServiceTxtLen(service, proto);
            uint8_t txtAttrs[10] =
            {
                0x00, 0x10,             //TXT record query
                0x80, 0x01,             //Class IN, with cache flush
                0x00, 0x00, 0x11, 0x94, //TTL 4500
                0x00, txtDataLen,       //RData length
            };
            _conn->append(reinterpret_cast<const char*>(txtAttrs), 10);

            //Send the RData
            MDNSTxt * txtPtr = _getServiceTxt(service, proto);
            while (txtPtr != 0)
            {
                uint8_t txtLen = txtPtr->_txt.length();
                _conn->append(reinterpret_cast<const char*>(&txtLen), 1);                  // length of txt
                _conn->append(reinterpret_cast<const char*>(txtPtr->_txt.c_str()), txtLen);// the txt
                txtPtr = txtPtr->_next;
            }
        }


        //SRV Responce
        if ((responseSection == 0 ? answerMask : additionalMask) & 0x2)
        {
            //Send the name field (ie. "My IOT device._http._tcp.local")
            _conn->append(reinterpret_cast<const char*>(&instanceNameLen), 1);       // length of "My IOT device"
            _conn->append(reinterpret_cast<const char*>(instanceName.c_str()), instanceNameLen);// "My IOT device"
            _conn->append(reinterpret_cast<const char*>(&serviceNameLen), 1);        // length of "_http"
            _conn->append(reinterpret_cast<const char*>(serviceName), serviceNameLen); // "_http"
            _conn->append(reinterpret_cast<const char*>(&protoNameLen), 1);          // length of "_tcp"
            _conn->append(reinterpret_cast<const char*>(protoName), protoNameLen);   // "_tcp"
            _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);          // length "local"
            _conn->append(reinterpret_cast<const char*>(localName), localNameLen);   // "local"
            _conn->append(reinterpret_cast<const char*>(&terminator), 1);            // terminator

            //Send the type, class, ttl, rdata length, priority and weight
            uint8_t srvDataSize = hostNameLen + localNameLen + 3; // 3 is 2 lable size bytes and the terminator
            srvDataSize += 6; // Size of Priority, weight and port
            uint8_t srvAttrs[10] =
            {
                0x00, 0x21,             //Type SRV
                0x80, 0x01,             //Class IN, with cache flush
                0x00, 0x00, 0x00, 0x78, //TTL 120
                0x00, srvDataSize,      //RData length
            };
            _conn->append(reinterpret_cast<const char*>(srvAttrs), 10);

            //Send the RData Priority weight and port
            uint8_t srvRData[6] =
            {
                0x00, 0x00,             //Priority 0
                0x00, 0x00,             //Weight 0
                (uint8_t)((port >> 8) & 0xFF), (uint8_t)(port & 0xFF)
            };
            _conn->append(reinterpret_cast<const char*>(srvRData), 6);
            //Send the RData (ie. "esp8266.local")
            _conn->append(reinterpret_cast<const char*>(&hostNameLen), 1);           // length of "esp8266"
            _conn->append(reinterpret_cast<const char*>(hostName.c_str()), hostNameLen);// "esp8266"
            _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);          // length "local"
            _conn->append(reinterpret_cast<const char*>(localName), localNameLen);   // "local"
            _conn->append(reinterpret_cast<const char*>(&terminator), 1);            // terminator

        }

        // A Response
        if ((responseSection == 0 ? answerMask : additionalMask) & 0x1)
        {
            //Send the RData (ie. "esp8266.local")
            _conn->append(reinterpret_cast<const char*>(&hostNameLen), 1);           // length of "esp8266"
            _conn->append(reinterpret_cast<const char*>(hostName.c_str()), hostNameLen);// "esp8266"
            _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);          // length "local"
            _conn->append(reinterpret_cast<const char*>(localName), localNameLen);   // "local"
            _conn->append(reinterpret_cast<const char*>(&terminator), 1);            // terminator

            uint8_t aaaAttrs[10] =
            {
                0x00, 0x01,             //TYPE A
                0x80, 0x01,             //Class IN, with cache flush
                0x00, 0x00, 0x00, 0x78, //TTL 120
                0x00, 0x04,             //DATA LEN
            };
            _conn->append(reinterpret_cast<const char*>(aaaAttrs), 10);

            // Send RData
            uint32_t ip = multicastInterface;
            uint8_t aaaRData[4] =
            {
                (uint8_t)(ip & 0xFF),         //IP first octet
                (uint8_t)((ip >> 8) & 0xFF),  //IP second octet
                (uint8_t)((ip >> 16) & 0xFF), //IP third octet
                (uint8_t)((ip >> 24) & 0xFF)  //IP fourth octet
            };
            _conn->append(reinterpret_cast<const char*>(aaaRData), 4);
        }
    }

    _conn->setMulticastInterface(multicastInterface);
    _conn->send();
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MDNS)
MDNSResponder MDNS;
#endif

}	// namespace Legacy_MDNSResponder




