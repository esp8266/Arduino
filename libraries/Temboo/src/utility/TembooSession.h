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

#ifndef TEMBOOSESSIONCLASS_H_
#define TEMBOOSESSIONCLASS_H_

#include <stdint.h>
#include <Arduino.h>
#include <IPAddress.h>
#include <Client.h>
#include "TembooGlobal.h"

#ifndef TEMBOO_SEND_QUEUE_SIZE

// Some network interfaces (i.e. ethernet or WiFi shields) can only accept
// a limited amount of data.  If you try to send more than the limit, the excess
// is just lost.  However, sending one character at a time is very inefficient.
// To deal with this situation, we queue up TEMBOO_SEND_QUEUE_SIZE bytes to send
// to the network device at one time.  This is a compromise between RAM usage 
// and performance.
#define TEMBOO_SEND_QUEUE_SIZE (32)
#endif

class ChoreoInputSet;
class ChoreoOutputSet;
class ChoreoPreset;
class DataFormatter;

class TembooSession {
    public:

        //TembooSession constructor
        //client: REQUIRED TCP/IP client object.  Usually either an EthernetClient or a WiFiClient
        //IPAddress: OPTIONAL IP address of the server to connect to.  Usually only used for testing.
        //port: OPTIONAL port number to use with the IPAddress. Usually only used for testing.
        TembooSession(Client& client, IPAddress serverAddr=INADDR_NONE, uint16_t port=80);

        //executeChoreo sends a choreo execution request to the Temboo system.
        //              Does not wait for a response (that's a job for whoever owns the Client.)
        //accountName: the name of the user's account at Temboo.
        //appKeyName: the name of an application key in the user's account to use 
        //            for this execution (analogous to a user name).
        //appKeyValue: the value of the application key named in appKeyName.
        //             Used to authenticate the user (analogous to a password)
        //path: The full path to the choreo to be executed (relative to the root of the 
        //      user's account.)
        //inputSet: the set of inputs needed by the choreo.
        //          May be an empty ChoreoInputSet.
        //outputSet: the set of output filters to be applied to the choreo results.
        //           May be an empty ChoreoOutputSet
        //preset: the ChoreoPreset to be used with the choreo execution. 
        //        May be an empty ChoreoPreset.
        int executeChoreo(const char* accountName, 
                const char* appKeyName, 
                const char* appKeyValue, 
                const char* path, 
                const ChoreoInputSet& inputSet, 
                const ChoreoOutputSet& outputSet, 
                const ChoreoPreset& preset);

        // setTime sets the current time in Unix timestamp format.  Needed for execution request authentication.
        //         NOTE: This method is usually called by TembooChoreo.run() with the current time returned by 
        //         an error response from the Temboo system, thus automatically setting the time.  However, it 
        //         MAY be called from user code if the particular board has a way of determining the current
        //         time in the proper format.
        // currentTime: the number of seconds since 1970-01-01 00:00:00 UTC.
        static void setTime(unsigned long currentTime);

        //getTime returns the current time in Unix timestamp format (seconds since 1970-01-01 00:00:00 UTC).
        //        Only valid after setTime has been called.
        static unsigned long getTime();

    private:
        static unsigned long s_timeOffset;

        IPAddress m_addr;
        uint16_t m_port;
        
        Client& m_client;
        char m_sendQueue[TEMBOO_SEND_QUEUE_SIZE];
        size_t m_sendQueueDepth;
        
        // calculate the authentication code value of the formatted request body
        // using the salted application key value as the key.  
        // Returns the number of characters processed (i.e. the length of the request body)
        uint16_t getAuth(DataFormatter& fmt, const char* appKeyValue, const char* salt, char* hexAuth) const;
        
        
        // queue an entire nul-terminated char array
        // from RAM followed by a newline.
        void qsendln(const char* str);
        
        // queue an entire nul-terminated char array
        // from flash memory (PROGMEM) one byte at a time,
        // followed by a newline.
        void qsendlnProgmem(const char* str);
        
        // queue an entire nul-terminated char array
        // from RAM one byte at a time.
        void qsend(const char*);

        // queue an entire nul-terminated char array 
        // from flash memory (PROGMEM) one byte at a time.
        void qsendProgmem(const char*);

        // queue a single character to be sent when the queue is full.
        void qsend(char);
        
        // send the current contents of the send queue to the client.
        void qflush();

};

#endif

