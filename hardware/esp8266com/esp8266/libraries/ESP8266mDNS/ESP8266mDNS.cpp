/*

ESP8266 Multicast DNS (port of CC3000 Multicast DNS library)
Version 1.1
Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)
MDNS-SD Suport 2015 Hristo Gochkov


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

#define LWIP_OPEN_SRC

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



//#define MDNS_DEBUG_ERR
//#define MDNS_DEBUG_TX
//#define MDNS_DEBUG_RX

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

MDNSResponder::MDNSResponder() : _conn(0) { _services = 0; }
MDNSResponder::~MDNSResponder() {}

bool MDNSResponder::begin(const char* domain){ 
  // Open the MDNS socket if it isn't already open.
  
  size_t n = strlen(domain);
  if (n > 255) { // Can only handle domains that are 255 chars in length.
    return false;
  }
  
  // Copy in domain characters as lowercase
  for (int i = 0; i < n; ++i)
    _hostName[i] = tolower(domain[i]);
  _hostName[n] = '\0';
  
  os_strcpy(_boardName, ARDUINO_BOARD);
  
  // Open the MDNS socket if it isn't already open.
  if (!_conn) {
    uint32_t ourIp = _getOurIp();
    if(ourIp == 0){
      return false;
    }
    
    ip_addr_t ifaddr;
    ifaddr.addr = ourIp;
    ip_addr_t multicast_addr;
    multicast_addr.addr = (uint32_t) MDNS_MULTICAST_ADDR;

    if (igmp_joingroup(&ifaddr, &multicast_addr)!= ERR_OK) {
      return false;
    }

    _conn = new UdpContext;
    _conn->ref();

    if (!_conn->listen(*IP_ADDR_ANY, MDNS_PORT)) {
      return false;
    }
    _conn->setMulticastInterface(ifaddr);
    _conn->setMulticastTTL(MDNS_MULTICAST_TTL);
    _conn->onRx(std::bind(&MDNSResponder::update, this));
    _conn->connect(multicast_addr, MDNS_PORT);
  }
  return true;
}

void MDNSResponder::update() {
  if (!_conn->next()) {
    return; 
  }
  _parsePacket();
}

void MDNSResponder::addService(char *name, char *proto, uint16_t port){
  if(_getServicePort(name, proto) != 0) return;
  if(os_strlen(name) > 32 || os_strlen(proto) != 3) return; //bad arguments
  struct MDNSService *srv = (struct MDNSService*)(os_malloc(sizeof(struct MDNSService)));
  os_strcpy(srv->_name, name);
  os_strcpy(srv->_proto, proto);
  srv->_port = port;
  srv->_next = 0;
  if(_services) _services->_next = srv;
  else _services = srv;
}

uint16_t MDNSResponder::_getServicePort(char *name, char *proto){
  MDNSService* servicePtr;
  for (servicePtr = _services; servicePtr; servicePtr = servicePtr->_next) {
    if(servicePtr->_port > 0 && strcmp(servicePtr->_name, name) == 0 && strcmp(servicePtr->_proto, proto) == 0){
      return servicePtr->_port;
    }
  }
  return 0;
}

uint32_t MDNSResponder::_getOurIp(){
  if(wifi_get_opmode() & STATION_MODE){
    struct ip_info staIpInfo;
    wifi_get_ip_info(STATION_IF, &staIpInfo);
    return staIpInfo.ip.addr;
  }
#ifdef MDNS_DEBUG_ERR
    os_printf("ERR_NO_LOCAL_IP\n");
#endif
  return 0;
}

void MDNSResponder::_parsePacket(){
  int i;
  char tmp;
  bool serviceParsed = false;
  bool protoParsed = false;
  bool localParsed = false;
  
  char hostName[255];
  uint8_t hostNameLen;
  
  char serviceName[32];
  uint8_t serviceNameLen;
  uint16_t servicePort;
  
  char protoName[32];
  uint8_t protoNameLen;
  
  uint16_t packetHeader[6];
  
  for(i=0; i<6; i++) packetHeader[i] = _conn_read16();

  if((packetHeader[1] & 0x8000) != 0){ //not parsing responses yet
    _conn->flush();
    return;
  }
  
  // PARSE REQUEST NAME
  
  hostNameLen = _conn_read8();
  _conn_readS(hostName, hostNameLen);
  hostName[hostNameLen] = '\0';
  
  if(hostName[0] == '_'){
    serviceParsed = true;
    memcpy(serviceName, hostName+1, hostNameLen);
    serviceNameLen = hostNameLen-1;
    hostNameLen = 0;
  }
  
  if(hostNameLen > 0 && strcmp(_hostName, hostName) != 0){
#ifdef MDNS_DEBUG_ERR
    os_printf("ERR_NO_HOST: %s\n", hostName);
#endif
    _conn->flush();
    return;
  }
  
  if(!serviceParsed){
    serviceNameLen = _conn_read8();
    _conn_readS(serviceName, serviceNameLen);
    serviceName[serviceNameLen] = '\0';
    
    if(serviceName[0] == '_'){
      memcpy(serviceName, serviceName+1, serviceNameLen);
      serviceNameLen--;
      serviceParsed = true;
    } else if(serviceNameLen == 5 && strcmp("local", serviceName) == 0){
      tmp = _conn_read8();
      if(tmp == 0){
        serviceParsed = true;
        serviceNameLen = 0;
        protoParsed = true;
        protoNameLen = 0;
        localParsed = true;
      } else {
#ifdef MDNS_DEBUG_ERR
        os_printf("ERR_FQDN: %s\n", serviceName);
#endif
        _conn->flush();
        return;
      }
    } else {
#ifdef MDNS_DEBUG_ERR
      os_printf("ERR_SERVICE: %s\n", serviceName);
#endif
      _conn->flush();
      return;
    }
  }
  
  if(!protoParsed){
    protoNameLen = _conn_read8();
    _conn_readS(protoName, protoNameLen);
    protoName[protoNameLen] = '\0';
    if(protoNameLen == 4 && protoName[0] == '_'){
      memcpy(protoName, protoName+1, protoNameLen);
      protoNameLen--;
      protoParsed = true;
    } else {
#ifdef MDNS_DEBUG_ERR
      os_printf("ERR_PROTO: %s\n", protoName);
#endif
      _conn->flush();
      return;
    }
  }
  
  if(!localParsed){
    char localName[32];
    uint8_t localNameLen = _conn_read8();
    _conn_readS(localName, localNameLen);
    localName[localNameLen] = '\0';
    tmp = _conn_read8();
    if(localNameLen == 5 && strcmp("local", localName) == 0 && tmp == 0){
      localParsed = true;
    } else {
#ifdef MDNS_DEBUG_ERR
      os_printf("ERR_FQDN: %s\n", localName);
#endif
      _conn->flush();
      return;
    }
  }
  
  if(serviceNameLen > 0 && protoNameLen > 0){
    servicePort = _getServicePort(serviceName, protoName);
    if(servicePort == 0){
#ifdef MDNS_DEBUG_ERR
      os_printf("ERR_NO_SERVICE: %s\n", serviceName);
#endif
      _conn->flush();
      return;
    }
  } else if(serviceNameLen > 0 || protoNameLen > 0){
#ifdef MDNS_DEBUG_ERR
    os_printf("ERR_SERVICE_PROTO: %s\n", serviceName);
#endif
    _conn->flush();
    return;
  }
  
  // RESPOND
  
#ifdef MDNS_DEBUG_RX
  os_printf("RX: REQ, ID:%u, Q:%u, A:%u, NS:%u, ADD:%u\n", packetHeader[0], packetHeader[2], packetHeader[3], packetHeader[4], packetHeader[5]);
#endif

  uint16_t currentType;
  uint16_t currentClass;
  
  int numQuestions = packetHeader[2];
  if(numQuestions > 4) numQuestions = 4;
  uint16_t questions[4];
  int question = 0;

  while(numQuestions--){
    currentType = _conn_read16();
    if(currentType & MDNS_NAME_REF){ //new header handle it better!
      currentType = _conn_read16();
    }
    currentClass = _conn_read16();
    if(currentClass & MDNS_CLASS_IN) questions[question++] = currentType;
    
    if(numQuestions > 0){
      if(_conn_read16() != 0xC00C){//new question but for another host/service
        _conn->flush();
        numQuestions = 0;
      }
    }
    
#ifdef MDNS_DEBUG_RX
    os_printf("REQ: ");
    if(hostNameLen > 0) os_printf("%s.", hostName);
    if(serviceNameLen > 0) os_printf("_%s.", serviceName);
    if(protoNameLen > 0) os_printf("_%s.", protoName);
    os_printf("local. ");
  
    if(currentType == MDNS_TYPE_AAAA) os_printf("  AAAA ");
    else if(currentType == MDNS_TYPE_A) os_printf("  A ");
    else if(currentType == MDNS_TYPE_PTR) os_printf("  PTR ");
    else if(currentType == MDNS_TYPE_SRV) os_printf("  SRV ");
    else if(currentType == MDNS_TYPE_TXT) os_printf("  TXT ");
    else os_printf("  0x%04X ", currentType);

    if(currentClass == MDNS_CLASS_IN) os_printf("  IN ");
    else if(currentClass == MDNS_CLASS_IN_FLUSH_CACHE) os_printf("  IN[F] ");
    else os_printf("  0x%04X ", currentClass);
  
    os_printf("\n");
#endif
  }
  uint8_t responseMask = 0;
  for(i=0;i<question;i++){
    if(questions[i] == MDNS_TYPE_A) responseMask |= 0x1;
    else if(questions[i] == MDNS_TYPE_SRV) responseMask |= 0x3;
    else if(questions[i] == MDNS_TYPE_TXT) responseMask |= 0x4;
    else if(questions[i] == MDNS_TYPE_PTR) responseMask |= 0xF;
  }
  
  return _reply(responseMask, (serviceName), (protoName), servicePort);
}



void MDNSResponder::_reply(uint8_t replyMask, char * service, char *proto, uint16_t port){
  int i;
  if(replyMask == 0) return;
  
#ifdef MDNS_DEBUG_TX
    os_printf("TX: mask:%01X, service:%s, proto:%s, port:%u\n", replyMask, service, proto, port);
#endif
  
  char nameLen = os_strlen(_hostName);
  size_t serviceLen = os_strlen(service);
  
  uint8_t answerCount = 0;
  for(i=0;i<4;i++){
    if(replyMask & (1 << i)) answerCount++;
  }

  _conn->flush();
  uint8_t head[12] = {
    0x00, 0x00, //ID = 0
    0x84, 0x00, //Flags = response + authoritative answer
    0x00, 0x00, //Question count
    0x00, answerCount, //Answer count
    0x00, 0x00, //Name server records
    0x00, 0x00, //Additional records
  };
  _conn->append(reinterpret_cast<const char*>(head), 12);
  
  if((replyMask & 0x8) == 0){
    _conn->append(reinterpret_cast<const char*>(&nameLen), 1);
    _conn->append(reinterpret_cast<const char*>(_hostName), nameLen);
  }
  
  if(replyMask & 0xE){
    uint8_t servHead[2] = {(uint8_t)(serviceLen+1), '_'};
    uint8_t protoHead[2] = {0x4, '_'};
    _conn->append(reinterpret_cast<const char*>(servHead), 2);
    _conn->append(reinterpret_cast<const char*>(service), serviceLen);
    _conn->append(reinterpret_cast<const char*>(protoHead), 2);
    _conn->append(reinterpret_cast<const char*>(proto), 3);
  }
  
  uint8_t local[7] = {
    0x05, //strlen(_local)
    0x6C, 0x6F, 0x63, 0x61, 0x6C, //local
    0x00, //End of domain
  };
  _conn->append(reinterpret_cast<const char*>(local), 7);
  
  // PTR Response
  if(replyMask & 0x8){
    uint8_t ptr[10] = {
      0x00, 0x0c, //PTR record query
      0x00, 0x01, //Class IN
      0x00, 0x00, 0x11, 0x94, //TTL 4500
      0x00, (uint8_t)(3 + nameLen), //***DATA LEN (3 + strlen(host))
    };
    _conn->append(reinterpret_cast<const char*>(ptr), 10);
    _conn->append(reinterpret_cast<const char*>(&nameLen), 1);
    _conn->append(reinterpret_cast<const char*>(_hostName), nameLen);
    uint8_t ptrTail[2] = {0xC0, 0x0C};
    _conn->append(reinterpret_cast<const char*>(ptrTail), 2);
  }
  
  // TXT Response
  if(replyMask & 0x4){
    if(replyMask & 0x8){//send the name
      uint8_t txtHead[2] = {0xC0, (uint8_t)(36 + serviceLen)};
      _conn->append(reinterpret_cast<const char*>(txtHead), 2);
    }
    
    uint8_t boardNameLen = os_strlen(_boardName);
    
    uint8_t txt[24] = {
      0x00, 0x10, //Type TXT
      0x80, 0x01, //Class IN, with cache flush
      0x00, 0x00, 0x11, 0x94, //TTL 4500
      0x00, 0x0e, //DATA LEN
      (uint8_t)(6 + boardNameLen), //strlen(board=) + strlen(boardName)
      0x62, 0x6f, 0x61, 0x72, 0x64, 0x3d //board=
    };
    _conn->append(reinterpret_cast<const char*>(txt), 17);
    _conn->append(reinterpret_cast<const char*>(_boardName), boardNameLen);
    
  }
  
  // SRV Response
  if(replyMask & 0x2){
    if(replyMask & 0xC){//send the name
      uint8_t srvHead[2] = {0xC0, 0x0C};
      if(replyMask & 0x8)
        srvHead[1] = 36 + serviceLen;
      _conn->append(reinterpret_cast<const char*>(srvHead), 2);
    }
    
    uint8_t srv[16] = {
      0x00, 0x21, //Type SRV
      0x80, 0x01, //Class IN, with cache flush
      0x00, 0x00, 0x00, 0x78, //TTL 120
      0x00, (uint8_t)(9 + nameLen), //DATA LEN (9 + strlen(host))
      0x00, 0x00, //Priority 0
      0x00, 0x00, //Weight 0
      (uint8_t)((port >> 8) & 0xFF), (uint8_t)(port & 0xFF)
    };
    _conn->append(reinterpret_cast<const char*>(srv), 16);
    _conn->append(reinterpret_cast<const char*>(&nameLen), 1);
    _conn->append(reinterpret_cast<const char*>(_hostName), nameLen);
    uint8_t srvTail[2] = {0xC0, (uint8_t)(20 + serviceLen + nameLen)};
    if(replyMask & 0x8)
      srvTail[1] = 19 + serviceLen;
    _conn->append(reinterpret_cast<const char*>(srvTail), 2);
  }
  
  // A Response
  if(replyMask & 0x1){
    uint32_t ip = _getOurIp();
    if(replyMask & 0x2){//send the name (no compression for now)
      _conn->append(reinterpret_cast<const char*>(&nameLen), 1);
      _conn->append(reinterpret_cast<const char*>(_hostName), nameLen);
      _conn->append(reinterpret_cast<const char*>(local), 7);
    }
    
    uint8_t aaa[14] = {
      0x00, 0x01, //TYPE A
      0x80, 0x01, //Class IN, with cache flush
      0x00, 0x00, 0x00, 0x78, //TTL 120
      0x00, 0x04, //DATA LEN
      (uint8_t)(ip & 0xFF), (uint8_t)((ip >> 8) & 0xFF), (uint8_t)((ip >> 16) & 0xFF), (uint8_t)((ip >> 24) & 0xFF)
    };
    _conn->append(reinterpret_cast<const char*>(aaa), 14);
  }
  _conn->send();
}

MDNSResponder MDNS = MDNSResponder();
