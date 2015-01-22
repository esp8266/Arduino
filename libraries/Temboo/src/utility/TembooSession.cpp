/*
###############################################################################
#
# Temboo Arduino library
#
# Copyright 2014, Temboo Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
# either express or implied. See the License for the specific
# language governing permissions and limitations under the License.
#
###############################################################################
*/

#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "TembooSession.h"
#include "tmbhmac.h"
#include "DataFormatter.h"

static const char EOL[]                    PROGMEM = "\r\n";
static const char POST[]                   PROGMEM = "POST ";
static const char POSTAMBLE[]              PROGMEM = " HTTP/1.0"; // Prevent host from using chunked encoding in response.
static const char HEADER_HOST[]            PROGMEM = "Host: ";
static const char HEADER_ACCEPT[]          PROGMEM = "Accept: application/xml";
static const char HEADER_ORG[]             PROGMEM = "x-temboo-domain: /";
static const char HEADER_DOM[]             PROGMEM = "/master";
static const char HEADER_CONTENT_LENGTH[]  PROGMEM = "Content-Length: ";
static const char HEADER_TIME[]            PROGMEM = "x-temboo-time: ";
static const char BASE_CHOREO_URI[]        PROGMEM = "/arcturus-web/api-1.0/ar";
static const char HEADER_AUTH[]            PROGMEM = "x-temboo-authentication: ";
static const char HEADER_CONTENT_TYPE[]    PROGMEM = "Content-Type: text/plain";
static const char TEMBOO_DOMAIN[]          PROGMEM = ".temboolive.com";
static const char SDK_ID[]                 PROGMEM = "?source_id=arduinoSDK1";

unsigned long TembooSession::s_timeOffset = 0;

TembooSession::TembooSession(Client& client, 
        IPAddress serverAddr, 
        uint16_t port) : m_client(client) {
    m_addr = serverAddr;
    m_port = port;
    m_sendQueueDepth = 0;
}


void TembooSession::setTime(unsigned long currentTime) {
    s_timeOffset = currentTime - (millis()/1000);
}


unsigned long TembooSession::getTime() {
    return s_timeOffset + (millis()/1000);
}



int TembooSession::executeChoreo(
        const char* accountName, 
        const char* appKeyName, 
        const char* appKeyValue, 
        const char* path, 
        const ChoreoInputSet& inputSet, 
        const ChoreoOutputSet& outputSet, 
        const ChoreoPreset& preset) {

    DataFormatter fmt(&inputSet, &outputSet, &preset);
    char auth[HMAC_HEX_SIZE_BYTES + 1];
    char buffer[11];
    
    // We use the current time-of-day as salt on the app key.
    // We keep track of time-of-day by getting the current time
    // from the server and applying an offset (the length of time
    // we've been running.) 
    uint32toa((uint32_t)TembooSession::getTime(), buffer);

    uint16_t contentLength = getAuth(fmt, appKeyValue, buffer, auth);

    m_client.stop();
    m_client.flush();

    int connected = 0;
    TEMBOO_TRACE("Connecting: ");

    // reserve space for the "host" string sufficient to hold either the 
    // (dotted-quad) IP address + port, or the default <account>.temboolive.com
    // host string.
    int hostLen = (m_addr == INADDR_NONE ? (strlen_P(TEMBOO_DOMAIN) + strlen(accountName) + 1):21);
    char host[hostLen];

    // If no explicit IP address was specified (the normal case), construct
    // the "host" string from the account name and the temboo domain name.
    if (m_addr == INADDR_NONE) {
        strcpy(host, accountName);
        strcat_P(host, TEMBOO_DOMAIN);
        TEMBOO_TRACELN(host);
        connected = m_client.connect(host, m_port);
    } else {

        // If an IP address was explicitly specified (presumably for testing purposes),
        // convert it to a dotted-quad text string.
        host[0] = '\0';
        for(int i = 0; i < 4; i++) {
            uint16toa(m_addr[i], &host[strlen(host)]);
            strcat(host, ".");
        }

        // replace the last '.' with ':'
        host[strlen(host)-1] = ':';
        
        // append the port number
        uint16toa(m_port, &host[strlen(host)]);
        
        TEMBOO_TRACELN(host);
        connected = m_client.connect(m_addr, m_port);
    }

    if (connected) {

        TEMBOO_TRACELN("OK. req:");
        qsendProgmem(POST);
        qsendProgmem(BASE_CHOREO_URI);
        qsend(path);
        qsendProgmem(SDK_ID);
        qsendlnProgmem(POSTAMBLE);
        
        // Send our custom authentication header
        // (app-key-name:hmac)
        qsendProgmem(HEADER_AUTH);
        qsend(appKeyName);
        qsend(":");
        qsendln(auth);
        
        // send the standard host header
        qsendProgmem(HEADER_HOST);
        qsendln(host);
        
        // send the standard accept header
        qsendlnProgmem(HEADER_ACCEPT);
        
        // send our custom account name neader
        qsendProgmem(HEADER_ORG);
        qsend(accountName);
        qsendlnProgmem(HEADER_DOM);
        
        // send the standard content type header
        qsendlnProgmem(HEADER_CONTENT_TYPE);
        
        // send our custom client time header
        qsendProgmem(HEADER_TIME);
        qsendln(buffer);
        
        // send the standard content length header
        qsendProgmem(HEADER_CONTENT_LENGTH);
        qsendln(uint16toa(contentLength, buffer));

        qsendProgmem(EOL);
        
        // Format and send the body of the request
        fmt.reset();
        while(fmt.hasNext()) {
            qsend(fmt.next());
        }

        qsendProgmem(EOL);
        qflush();
        return 0;
    } else {
        TEMBOO_TRACELN("FAIL");
        return 1;
    }
}


uint16_t TembooSession::getAuth(DataFormatter& fmt, const char* appKeyValue, const char* salt, char* result) const {

    // We need the length of the data for other things, and
    // this method is a convenient place to calculate it.
    uint16_t len = 0;

    HMAC hmac;

    //combine the salt and the key and give it to the HMAC calculator
    size_t keyLength = strlen(appKeyValue) + strlen(salt);
    char key[keyLength + 1];
    strcpy(key, salt);
    strcat(key, appKeyValue);
    hmac.init((uint8_t*)key, keyLength);

    // process the data a block at a time.
    uint8_t buffer[HMAC_BLOCK_SIZE_BYTES];
    int blockCount = 0;
    fmt.reset();
    while(fmt.hasNext()) {
        uint8_t c = fmt.next();
        len++;
        buffer[blockCount++] = c;
        if (blockCount == HMAC_BLOCK_SIZE_BYTES) {
            hmac.process(buffer, blockCount);
            blockCount = 0;
        }
    }
    hmac.process(buffer, blockCount);

    // Finalize the HMAC calculation and store the (ASCII HEX) value in *result.
    hmac.finishHex(result);

    // Return the number of characters processed.
    return len;
}


void TembooSession::qsend(const char* s) {
    char c = *s++;
    while(c != '\0') {
        qsend(c);
        c = *s++;
    }
}


void TembooSession::qsendProgmem(const char* s) {
    char c = pgm_read_byte(s++);
    while(c != '\0') {
        qsend(c);
        c = pgm_read_byte(s++);
    }
}


void TembooSession::qsend(char c) {
    m_sendQueue[m_sendQueueDepth++] = c;
    if (m_sendQueueDepth >= TEMBOO_SEND_QUEUE_SIZE) {
        qflush();
    }
}


void TembooSession::qflush() {
    m_client.write((const uint8_t*)m_sendQueue, m_sendQueueDepth);
    TEMBOO_TRACE_BYTES(m_sendQueue, m_sendQueueDepth);
    m_sendQueueDepth = 0;
}


void TembooSession::qsendln(const char* s) {
    qsend(s);
    qsendProgmem(EOL);
}


void TembooSession::qsendlnProgmem(const char* s) {
    qsendProgmem(s);
    qsendProgmem(EOL);
}


