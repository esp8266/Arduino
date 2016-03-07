/*

ESP8266 Multicast DNS (port of CC3000 Multicast DNS library)
Version 1.1
Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)
MDNS-SD Suport 2015 Hristo Gochkov
Extended MDNS-SD support 2016 Lars Englund


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

struct MDNSService {
  MDNSService* _next;
  char _name[32];
  char _proto[3];
  uint16_t _port;
  struct MDNSTxt * _txts;
  uint16_t _txtLen; // length of all txts 
};

struct MDNSTxt{
  MDNSTxt * _next;
  String _txt;
};



MDNSResponder::MDNSResponder() : _conn(0) { 
  _services = 0;
  _instanceName = ""; 
  p_answer_function_ = NULL;
}
MDNSResponder::~MDNSResponder() {}

bool MDNSResponder::begin(const char* hostname){
  // Open the MDNS socket if it isn't already open.

  size_t n = strlen(hostname);
  if (n > 63) { // max size for a single label.
    return false;
  }

  // Copy in hostname characters as lowercase
  _hostName = hostname;
  _hostName.toLowerCase();

  // If instance name is not already set copy hostname to instance name
  if (_instanceName.equals("") ) _instanceName=hostname; 

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
  if (!_conn || !_conn->next()) {
    return;
  }
  _parsePacket();
}


void MDNSResponder::setInstanceName(String name){
  if (name.length() > 63) return;
  else _instanceName = name;
}


bool MDNSResponder::addServiceTxt(char *name, char *proto, char *key, char *value){
  MDNSService* servicePtr;
  
  uint8_t txtLen = os_strlen(key) + os_strlen(value) + 1; // Add one for equals sign 
  txtLen+=1; //accounts for length byte added when building the txt responce
  //Find the service
  for (servicePtr = _services; servicePtr; servicePtr = servicePtr->_next) {
    //Checking Service names
    if(strcmp(servicePtr->_name, name) == 0 && strcmp(servicePtr->_proto, proto) == 0){
      //found a service name match
      if (servicePtr->_txtLen + txtLen > 1300) return false;  //max txt record size
      MDNSTxt *newtxt = new MDNSTxt;
      newtxt->_txt = String(key) + "=" + String(value);
      newtxt->_next = 0;
      if(servicePtr->_txts == 0) { //no services have been added
        //Adding First TXT to service
        servicePtr->_txts = newtxt;
        servicePtr->_txtLen += txtLen;
        return true;
      }
      else{
        MDNSTxt * txtPtr = servicePtr->_txts;
        while(txtPtr->_next !=0) {
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

void MDNSResponder::addService(char *name, char *proto, uint16_t port){
  if(_getServicePort(name, proto) != 0) return;
  if(os_strlen(name) > 32 || os_strlen(proto) != 3) return; //bad arguments
  struct MDNSService *srv = (struct MDNSService*)(os_malloc(sizeof(struct MDNSService)));
  os_strcpy(srv->_name, name);
  os_strcpy(srv->_proto, proto);
  srv->_port = port;
  srv->_next = 0;
  srv->_txts = 0;
  srv->_txtLen = 0;
  
  if(_services == 0) _services = srv;
  else{
    MDNSService* servicePtr = _services;
    while(servicePtr->_next !=0) servicePtr = servicePtr->_next;
    servicePtr->_next = srv;
  }
  
}

void MDNSResponder::queryService(char *service, void(*p_answer_function)(const char*)) {
  Serial.print("queryService: ");
  Serial.println(service);
  p_answer_function_ = p_answer_function;
  
  // TODO: copy _reply() to new method called _query() that sends out a PTR query
  // Relevant RFCs
  // https://tools.ietf.org/html/rfc1035
  // https://tools.ietf.org/html/rfc6762
  // https://tools.ietf.org/html/rfc6763

  
// Example query sent by node.js for _lalala._tcp.local  
//0000   00 00 00 00 00 01 00 00 00 00 00 00 07 5f 6c 61  ............._la
//0010   6c 61 6c 61 04 5f 74 63 70 05 6c 6f 63 61 6c 00  lala._tcp.local.
//0020   00 0c 00 01                                      ....

  //String instanceName = _instanceName;
  //size_t instanceNameLen = instanceName.length();

  //String hostName = _hostName;
  //size_t hostNameLen = hostName.length();

  //char underscore[] = "_";

  // build service name with _
  char serviceName[] = "_esp";
  size_t serviceNameLen = 4;

  //build proto name with _
  char protoName[] = "_tcp";
  size_t protoNameLen = 4;

  //local string
  char localName[] = "local";
  size_t localNameLen = 5;

  //terminator
  char terminator[] = "\0";

  uint8_t questionCount = 1;

  // Write the header
  _conn->flush();
  uint8_t head[12] = {
    0x00, 0x00, //ID = 0
    0x00, 0x00, //Flags = response + authoritative answer
    0x00, questionCount, //Question count
    0x00, 0x00, //Answer count
    0x00, 0x00, //Name server records
    0x00, 0x00 //Additional records
  };
  _conn->append(reinterpret_cast<const char*>(head), 12);

  // PTR Query
  //if(replyMask & 0x8){
    // Send the Name field (eg. "_http._tcp.local")
    _conn->append(reinterpret_cast<const char*>(&serviceNameLen), 1);          // lenght of "_" + service
    _conn->append(reinterpret_cast<const char*>(serviceName), serviceNameLen); // "_" + service
    _conn->append(reinterpret_cast<const char*>(&protoNameLen), 1);            // lenght of "_" + proto
    _conn->append(reinterpret_cast<const char*>(protoName), protoNameLen);     // "_" + proto
    _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);            // lenght "local"
    _conn->append(reinterpret_cast<const char*>(localName), localNameLen);     // "local"
    _conn->append(reinterpret_cast<const char*>(&terminator), 1);              // terminator
    
    //Send the type and class
    //uint8_t ptrDataLen = instanceNameLen + serviceNameLen + protoNameLen + localNameLen + 5; // 5 is four label sizes and the terminator
    uint8_t ptrAttrs[4] = {
      0x00, 0x0c,             //PTR record query
      0x00, 0x01             //Class IN
    };
    _conn->append(reinterpret_cast<const char*>(ptrAttrs), 4);
  //}
  
  _conn->send();
  
  /*const char *answer = "Hello from MDNSResponder!";
  
  if (p_answer_function_) {
    // Since a callback function has been registered, execute it.
    p_answer_function_(answer);
  }*/
}

MDNSTxt * MDNSResponder::_getServiceTxt(char *name, char *proto){
  MDNSService* servicePtr;
  for (servicePtr = _services; servicePtr; servicePtr = servicePtr->_next) {
    if(servicePtr->_port > 0 && strcmp(servicePtr->_name, name) == 0 && strcmp(servicePtr->_proto, proto) == 0){
      if (servicePtr->_txts == 0) return false;
      else{
        return servicePtr->_txts;
      }
    }
  }
  return 0;
}

uint16_t MDNSResponder::_getServiceTxtLen(char *name, char *proto){
  MDNSService* servicePtr;
  for (servicePtr = _services; servicePtr; servicePtr = servicePtr->_next) {
    if(servicePtr->_port > 0 && strcmp(servicePtr->_name, name) == 0 && strcmp(servicePtr->_proto, proto) == 0){
      if (servicePtr->_txts == 0) return false;
      else{
        return servicePtr->_txtLen;
      }
    }
  }
  return 0;
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
  int mode = wifi_get_opmode();
  if(mode & STATION_MODE){
    struct ip_info staIpInfo;
    wifi_get_ip_info(STATION_IF, &staIpInfo);
    return staIpInfo.ip.addr;
  } else if (mode & SOFTAP_MODE) {
    struct ip_info staIpInfo;
    wifi_get_ip_info(SOFTAP_IF, &staIpInfo);
    return staIpInfo.ip.addr;
  } else {
#ifdef MDNS_DEBUG_ERR
    Serial.printf("ERR_NO_LOCAL_IP\n");
#endif
    return 0;
  }
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
  uint16_t servicePort = 0;

  char protoName[32];
  protoName[0] = 0;
  uint8_t protoNameLen = 0;

  uint16_t packetHeader[6];

  for(i=0; i<6; i++) packetHeader[i] = _conn_read16();

  if((packetHeader[1] & 0x8000) != 0){ //not parsing responses yet
  
    Serial.println("Response parsing");
    Serial.printf("RX: REQ, ID:%u, Q:%u, A:%u, NS:%u, ADD:%u\n", packetHeader[0], packetHeader[2], packetHeader[3], packetHeader[4], packetHeader[5]);
    
    int numAnswers = packetHeader[3];

    // TODO: parse answers and call p_answer_function_ with host (.local), IP, port,  
    // http://www.ietf.org/rfc/rfc1035.txt
    
    // TODO: Parse 4 answers in 1 packet (that's what this library sends out, check if node.js mqtt advertisment sends out different answers (use esp8266_mdns to check)): 
    // 1. PTR - ignore for now (gives domain name (instance.service) (ex. Domain Name: ESP_81CC47._lalala._tcp.local))
    // 2. TXT - ignore for now
    // 3. SRV - gives service name, protocol, port and target host
    // 4. A   - gives ip addr of target host
  
/*
PTR
0000   07 5f 6c 61 6c 61 6c 61 04 5f 74 63 70 05 6c 6f  ._lalala._tcp.lo
0010   63 61 6c 00 00 0c 00 01 00 00 11 94 00 1f 0a 45  cal............E
0020   53 50 5f 38 31 43 43 34 37 07 5f 6c 61 6c 61 6c  SP_81CC47._lalal
0030   61 04 5f 74 63 70 05 6c 6f 63 61 6c 00           a._tcp.local.
*/
    uint8_t tmp8;
    uint16_t tmp16;
    uint32_t tmp32;

    while (numAnswers--) {
      Serial.println("Parsing answer..");
      // Skip name
      tmp8 = _conn_read8();
      if (tmp8 & 0xC0) { // Compressed
        tmp8 = _conn_read8();
      }
      else { // Not compressed
        while (tmp8 != 0x00) {
          Serial.print(" ");
          Serial.print(tmp8);
          _conn_readS(hostName, tmp8);
          Serial.printf("%s ", hostName);
          tmp8 = _conn_read8();
          if (tmp8 & 0xC0) { // Compressed
            tmp8 = _conn_read8();
            break;
          }
        }
      }
      uint16_t type = _conn_read16(); // Read type
      if (type == MDNS_TYPE_PTR) {
        Serial.println("Got a PTR answer!");
      }
      if (type == MDNS_TYPE_TXT) {
        Serial.println("Got a TXT answer!");
      }
      if (type == MDNS_TYPE_SRV) {
        Serial.println("Got a SRV answer!");
      }
      if (type == MDNS_TYPE_A) {
        Serial.println("Got a A answer!");
      }
      tmp16 = _conn_read16(); // Skip class
      tmp32 = _conn_read32(); // Skip ttl
      tmp16 = _conn_read16(); // Skip rdlength
      Serial.println("\nskip ");
      Serial.print(tmp16);
      _conn_readS(hostName, tmp16); // Skip rdata
      Serial.printf("%s ", hostName);
    }
    Serial.println("All answers parsed!");
/*
TXT
0000   0a 45 53 50 5f 38 31 43 43 34 37 07 5f 6c 61 6c  .ESP_81CC47._lal
0010   61 6c 61 04 5f 74 63 70 05 6c 6f 63 61 6c 00 00  ala._tcp.local..
0020   10 00 01 00 00 11 94 00 00                       .........
*/

/*
SRV
0000   0a 45 53 50 5f 38 31 43 43 34 37 07 5f 6c 61 6c  .ESP_81CC47._lal
0010   61 6c 61 04 5f 74 63 70 05 6c 6f 63 61 6c 00 00  ala._tcp.local..
0020   21 80 01 00 00 00 78 00 18 00 00 00 00 1f 90 0a  !.....x.........
0030   65 73 70 5f 38 31 63 63 34 37 05 6c 6f 63 61 6c  esp_81cc47.local
0040   00                                               .
*/
/*
A
0000   0a 65 73 70 5f 38 31 63 63 34 37 05 6c 6f 63 61  .esp_81cc47.loca
0010   6c 00 00 01 80 01 00 00 00 78 00 04 c0 a8 01 0c  l........x......
*/

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

  if(hostNameLen > 0 && !_hostName.equals(hostName) && !_instanceName.equals(hostName)){
#ifdef MDNS_DEBUG_ERR
    Serial.printf("ERR_NO_HOST: %s\n", hostName);
    Serial.printf("hostname: %s\n", _hostName.c_str() );
    Serial.printf("instance: %s\n", _instanceName.c_str() );
#endif
    _conn->flush();
    return;
  }

  if(!serviceParsed){
    serviceNameLen = _conn_read8();
    _conn_readS(serviceName, serviceNameLen);
    serviceName[serviceNameLen] = '\0';

    if(serviceName[0] == '_'){
      memmove(serviceName, serviceName+1, serviceNameLen);
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
        Serial.printf("ERR_FQDN: %s\n", serviceName);
#endif
        _conn->flush();
        return;
      }
    } else {
#ifdef MDNS_DEBUG_ERR
      Serial.printf("ERR_SERVICE: %s\n", serviceName);
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
      memmove(protoName, protoName+1, protoNameLen);
      protoNameLen--;
      protoParsed = true;
    } else if(strcmp("services", serviceName) == 0 && strcmp("_dns-sd", protoName) == 0){
      _conn->flush();
      advertiseServices();
      return;
    } else {
#ifdef MDNS_DEBUG_ERR
      Serial.printf("ERR_PROTO: %s\n", protoName);
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
      Serial.printf("ERR_FQDN: %s\n", localName);
#endif
      _conn->flush();
      return;
    }
  }

  if(serviceNameLen > 0 && protoNameLen > 0){
    servicePort = _getServicePort(serviceName, protoName);
    if(servicePort == 0){
#ifdef MDNS_DEBUG_ERR
      Serial.printf("ERR_NO_SERVICE: %s\n", serviceName);
#endif
      _conn->flush();
      return;
    }
  } else if(serviceNameLen > 0 || protoNameLen > 0){
#ifdef MDNS_DEBUG_ERR
    Serial.printf("ERR_SERVICE_PROTO: %s\n", serviceName);
#endif
    _conn->flush();
    return;
  }

  // RESPOND

#ifdef MDNS_DEBUG_RX
  Serial.printf("RX: REQ, ID:%u, Q:%u, A:%u, NS:%u, ADD:%u\n", packetHeader[0], packetHeader[2], packetHeader[3], packetHeader[4], packetHeader[5]);
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
    Serial.printf("REQ: ");
    if(hostNameLen > 0) Serial.printf("%s.", hostName);
    if(serviceNameLen > 0) Serial.printf("_%s.", serviceName);
    if(protoNameLen > 0) Serial.printf("_%s.", protoName);
    Serial.printf("local. ");

    if(currentType == MDNS_TYPE_AAAA) Serial.printf("  AAAA ");
    else if(currentType == MDNS_TYPE_A) Serial.printf("  A ");
    else if(currentType == MDNS_TYPE_PTR) Serial.printf("  PTR ");
    else if(currentType == MDNS_TYPE_SRV) Serial.printf("  SRV ");
    else if(currentType == MDNS_TYPE_TXT) Serial.printf("  TXT ");
    else Serial.printf("  0x%04X ", currentType);

    if(currentClass == MDNS_CLASS_IN) Serial.printf("  IN ");
    else if(currentClass == MDNS_CLASS_IN_FLUSH_CACHE) Serial.printf("  IN[F] ");
    else Serial.printf("  0x%04X ", currentClass);

    Serial.printf("\n");
#endif
  }
  uint8_t responseMask = 0;
  for(i=0;i<question;i++){
    if(questions[i] == MDNS_TYPE_A) responseMask |= 0x1;
    else if(questions[i] == MDNS_TYPE_SRV) responseMask |= 0x3;
    else if(questions[i] == MDNS_TYPE_TXT) responseMask |= 0x4;
    else if(questions[i] == MDNS_TYPE_PTR) responseMask |= 0xF;
  }

  return _reply(responseMask, serviceName, protoName, servicePort);
}

void MDNSResponder::enableArduino(uint16_t port, bool auth){

  addService("arduino", "tcp", port);
  addServiceTxt("arduino", "tcp", "tcp_check", "no");
  addServiceTxt("arduino", "tcp", "ssh_upload", "no");
  addServiceTxt("arduino", "tcp", "board", ARDUINO_BOARD);
  addServiceTxt("arduino", "tcp", "auth_upload", (auth) ? "yes":"no");
}

size_t MDNSResponder::advertiseServices(){
  MDNSService* servicePtr;
  size_t i = 0;
  for (servicePtr = _services; servicePtr; servicePtr = servicePtr->_next) {
    if(servicePtr->_port > 0){
      _reply(0x0F, servicePtr->_name, servicePtr->_proto, servicePtr->_port);
      i++;
    }
  }
  return i;
}

void MDNSResponder::_reply(uint8_t replyMask, char * service, char *proto, uint16_t port){
  int i;
  if(replyMask == 0) return;

#ifdef MDNS_DEBUG_TX
    Serial.printf("TX: mask:%01X, service:%s, proto:%s, port:%u\n", replyMask, service, proto, port);
#endif

  
  String instanceName = _instanceName;
  size_t instanceNameLen = instanceName.length();

  String hostName = _hostName;
  size_t hostNameLen = hostName.length();

  char underscore[]  = "_";
 
  // build service name with _
  char serviceName[os_strlen(service)+2];
  os_strcpy(serviceName,underscore);
  os_strcat(serviceName, service);
  size_t serviceNameLen = os_strlen(serviceName);

  //build proto name with _
  char protoName[5];
  os_strcpy(protoName,underscore);
  os_strcat(protoName, proto);
  size_t protoNameLen = 4; 
  
  //local string
  char localName[] = "local";
  size_t localNameLen = 5; 
  
  //terminator
  char terminator[] = "\0";

  uint8_t answerCount = 0;
  for(i=0;i<4;i++){
    if(replyMask & (1 << i)) answerCount++;
  }


 //Write the header
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

  // PTR Response
  if(replyMask & 0x8){
    // Send the Name field (ie. "_http._tcp.local")
    _conn->append(reinterpret_cast<const char*>(&serviceNameLen), 1);          // lenght of "_http"
    _conn->append(reinterpret_cast<const char*>(serviceName), serviceNameLen); // "_http"
    _conn->append(reinterpret_cast<const char*>(&protoNameLen), 1);            // lenght of "_tcp"
    _conn->append(reinterpret_cast<const char*>(protoName), protoNameLen);     // "_tcp"
    _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);            // lenght "local"
    _conn->append(reinterpret_cast<const char*>(localName), localNameLen);     // "local"
    _conn->append(reinterpret_cast<const char*>(&terminator), 1);              // terminator
    
    //Send the type, class, ttl and rdata length
    uint8_t ptrDataLen = instanceNameLen + serviceNameLen + protoNameLen + localNameLen + 5; // 5 is four label sizes and the terminator
    uint8_t ptrAttrs[10] = {
      0x00, 0x0c,             //PTR record query
      0x00, 0x01,             //Class IN
      0x00, 0x00, 0x11, 0x94, //TTL 4500
      0x00, ptrDataLen,       //RData length
    };
    _conn->append(reinterpret_cast<const char*>(ptrAttrs), 10);
    
    //Send the RData (ie. "My IOT device._http._tcp.local")
    _conn->append(reinterpret_cast<const char*>(&instanceNameLen), 1);         // lenght of "My IOT device"
    _conn->append(reinterpret_cast<const char*>(instanceName.c_str()), instanceNameLen);// "My IOT device"
    _conn->append(reinterpret_cast<const char*>(&serviceNameLen), 1);          // lenght of "_http"
    _conn->append(reinterpret_cast<const char*>(serviceName), serviceNameLen); // "_http"
    _conn->append(reinterpret_cast<const char*>(&protoNameLen), 1);            // lenght of "_tcp"
    _conn->append(reinterpret_cast<const char*>(protoName), protoNameLen);     // "_tcp"
    _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);            // lenght "local"
    _conn->append(reinterpret_cast<const char*>(localName), localNameLen);     // "local"
    _conn->append(reinterpret_cast<const char*>(&terminator), 1);              // terminator
  }

  //TXT Responce
  if(replyMask & 0x4){
    //Send the name field (ie. "My IOT device._http._tcp.local")
    _conn->append(reinterpret_cast<const char*>(&instanceNameLen), 1);         // lenght of "My IOT device"
    _conn->append(reinterpret_cast<const char*>(instanceName.c_str()), instanceNameLen);// "My IOT device"
    _conn->append(reinterpret_cast<const char*>(&serviceNameLen), 1);          // lenght of "_http"
    _conn->append(reinterpret_cast<const char*>(serviceName), serviceNameLen); // "_http"
    _conn->append(reinterpret_cast<const char*>(&protoNameLen), 1);            // lenght of "_tcp"
    _conn->append(reinterpret_cast<const char*>(protoName), protoNameLen);     // "_tcp"
    _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);            // lenght "local"
    _conn->append(reinterpret_cast<const char*>(localName), localNameLen);     // "local"
    _conn->append(reinterpret_cast<const char*>(&terminator), 1);              // terminator

    //Send the type, class, ttl and rdata length
    uint8_t txtDataLen = _getServiceTxtLen(service,proto);
    uint8_t txtAttrs[10] = {
      0x00, 0x10,             //TXT record query
      0x00, 0x01,             //Class IN
      0x00, 0x00, 0x11, 0x94, //TTL 4500
      0x00, txtDataLen,       //RData length
    };
    _conn->append(reinterpret_cast<const char*>(txtAttrs), 10);

    //Send the RData
    MDNSTxt * txtPtr = _getServiceTxt(service,proto);
    while(txtPtr !=0){
      uint8_t txtLen = txtPtr->_txt.length();
      _conn->append(reinterpret_cast<const char*>(&txtLen), 1);                  // lenght of txt
      _conn->append(reinterpret_cast<const char*>(txtPtr->_txt.c_str()), txtLen);// the txt
      txtPtr = txtPtr->_next;    
    }
  }


  //SRV Responce
  if(replyMask & 0x2){
    //Send the name field (ie. "My IOT device._http._tcp.local")
    _conn->append(reinterpret_cast<const char*>(&instanceNameLen), 1);         // lenght of "My IOT device"
    _conn->append(reinterpret_cast<const char*>(instanceName.c_str()), instanceNameLen);// "My IOT device"
    _conn->append(reinterpret_cast<const char*>(&serviceNameLen), 1);          // lenght of "_http"
    _conn->append(reinterpret_cast<const char*>(serviceName), serviceNameLen); // "_http"
    _conn->append(reinterpret_cast<const char*>(&protoNameLen), 1);            // lenght of "_tcp"
    _conn->append(reinterpret_cast<const char*>(protoName), protoNameLen);     // "_tcp"
    _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);            // lenght "local"
    _conn->append(reinterpret_cast<const char*>(localName), localNameLen);     // "local"
    _conn->append(reinterpret_cast<const char*>(&terminator), 1);              // terminator

    //Send the type, class, ttl, rdata length, priority and weight
    uint8_t srvDataSize = hostNameLen + localNameLen + 3; // 3 is 2 lable size bytes and the terminator
    srvDataSize += 6; // Size of Priority, weight and port
    uint8_t srvAttrs[10] = {
      0x00, 0x21,             //Type SRV
      0x80, 0x01,             //Class IN, with cache flush
      0x00, 0x00, 0x00, 0x78, //TTL 120
      0x00, srvDataSize,      //RData length
    };
    _conn->append(reinterpret_cast<const char*>(srvAttrs), 10);
    
    //Send the RData Priority weight and port
    uint8_t srvRData[6] = {
      0x00, 0x00,             //Priority 0
      0x00, 0x00,             //Weight 0
      (uint8_t)((port >> 8) & 0xFF), (uint8_t)(port & 0xFF) 
    };
    _conn->append(reinterpret_cast<const char*>(srvRData), 6);
    //Send the RData (ie. "esp8266.local")
    _conn->append(reinterpret_cast<const char*>(&hostNameLen), 1);             // lenght of "esp8266"
    _conn->append(reinterpret_cast<const char*>(hostName.c_str()), hostNameLen);// "esp8266"
    _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);            // lenght "local"
    _conn->append(reinterpret_cast<const char*>(localName), localNameLen);     // "local"
    _conn->append(reinterpret_cast<const char*>(&terminator), 1);              // terminator

  }

  // A Response
  if(replyMask & 0x1){
    //Send the RData (ie. "esp8266.local")
    _conn->append(reinterpret_cast<const char*>(&hostNameLen), 1);             // lenght of "esp8266"
    _conn->append(reinterpret_cast<const char*>(hostName.c_str()), hostNameLen);// "esp8266"
    _conn->append(reinterpret_cast<const char*>(&localNameLen), 1);            // lenght "local"
    _conn->append(reinterpret_cast<const char*>(localName), localNameLen);     // "local"
    _conn->append(reinterpret_cast<const char*>(&terminator), 1);              // terminator

    uint32_t ip = _getOurIp();
    uint8_t aaaAttrs[10] = {
      0x00, 0x01,             //TYPE A
      0x80, 0x01,             //Class IN, with cache flush
      0x00, 0x00, 0x00, 0x78, //TTL 120
      0x00, 0x04,             //DATA LEN
    };
    _conn->append(reinterpret_cast<const char*>(aaaAttrs), 10);

    // Send RData
    uint8_t aaaRData[4] = {
      (uint8_t)(ip & 0xFF),         //IP first octet 
      (uint8_t)((ip >> 8) & 0xFF),  //IP second octet
      (uint8_t)((ip >> 16) & 0xFF), //IP third octet
      (uint8_t)((ip >> 24) & 0xFF)  //IP fourth octet
    };
    _conn->append(reinterpret_cast<const char*>(aaaRData), 4);
  }

 _conn->send();
}

MDNSResponder MDNS = MDNSResponder();
