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

#ifndef TEMBOO_H_
#define TEMBOO_H_

#include <Arduino.h>

#if defined (ARDUINO_AVR_YUN) || defined (ARDUINO_AVR_TRE)
///////////////////////////////////////////////////////
//  BEGIN ARDUINO YUN AND TRE SUPPORT
///////////////////////////////////////////////////////

#include <Process.h>

class TembooChoreo : public Process {

    public:
        void begin();
        void setAccountName(const String& accountName);
        void setAppKeyName(const String& appKeyName);
        void setAppKey(const String& appKey);
        void setChoreo(const String& choreo);
        void setCredential(const String& credentialName);
        void setSavedInputs(const String& saveInputsName);
        void setProfile(const String& profileName);
        void addInput(const String& inputName, const String& inputValue);
        void addOutputFilter(const String& filterName, const String& filterPath, const String& variableName);
        void setSettingsFileToWrite(const String& filePath);
        void setSettingsFileToRead(const String& filePath);

};

#else //ARDUINO_AVR_YUN

///////////////////////////////////////////////////////
//  BEGIN ARDUINO NON-YUN SUPPORT
///////////////////////////////////////////////////////

#include <Stream.h>
#include <Client.h>
#include <IPAddress.h>
#include "utility/ChoreoInputSet.h"
#include "utility/ChoreoOutputSet.h"
#include "utility/ChoreoPreset.h"

#define TEMBOO_ERROR_OK                   (0)
#define TEMBOO_ERROR_ACCOUNT_MISSING      (201)
#define TEMBOO_ERROR_CHOREO_MISSING       (203)
#define TEMBOO_ERROR_APPKEY_NAME_MISSING  (205)
#define TEMBOO_ERROR_APPKEY_MISSING       (207)
#define TEMBOO_ERROR_HTTP_ERROR           (223)

class TembooChoreo : public Stream {
    public:
        
        // Constructor.
        // client - an instance of an Arduino Client, usually an EthernetClient
        //          or a WiFiClient.  Used to communicate with Temboo.
        TembooChoreo(Client& client);

        // Does nothing. Just for source compatibility with Yun code.
        void begin() {}; 
        
        // Sets the account name to use when communicating with Temboo.
        // (required)
        void setAccountName(const String& accountName);
        void setAccountName(const char* accountName);

        // Sets the application key name to use with choreo execution requests.
        // (required)
        void setAppKeyName(const String& appKeyName);
        void setAppKeyName(const char* appKeyName);
        
        // Sets the application key value to use with choreo execution requests
        // (required)
        void setAppKey(const String& appKey);
        void setAppKey(const char* appKey);
        
        // sets the name of the choreo to be executed.
        // (required)
        void setChoreo(const String& choreoPath);
        void setChoreo(const char* choreoPath);
        
        // sets the name of the saved inputs to use when executing the choreo
        // (optional)
        void setSavedInputs(const String& savedInputsName);
        void setSavedInputs(const char* savedInputsName);

        void setCredential(const String& credentialName);
        void setCredential(const char* credentialName);

        void setProfile(const String& profileName);
        void setProfile(const char* profileName);

        // sets an input to be used when executing a choreo.
        // (optional or required, depending on the choreo being executed.)
        void addInput(const String& inputName, const String& inputValue);
        void addInput(const char* inputName, const char* inputValue);
        void addInput(const char* inputName, const String& inputValue);
        void addInput(const String& inputName, const char* inputValue);
        
        // sets an output filter to be used to process the choreo output
        // (optional)
        void addOutputFilter(const char* filterName, const char* filterPath, const char* variableName);
        void addOutputFilter(const String& filterName, const char* filterPath, const char* variableName);
        void addOutputFilter(const char* filterName, const String& filterPath, const char* variableName);
        void addOutputFilter(const String& filterName, const String& filterPath, const char* variableName);
        void addOutputFilter(const char* filterName, const char* filterPath, const String& variableName);
        void addOutputFilter(const String& filterName, const char* filterPath, const String& variableName);
        void addOutputFilter(const char* filterName, const String& filterPath, const String& variableName);
        void addOutputFilter(const String& filterName, const String& filterPath, const String& variableName);
       
        // run the choreo using the current input info
        int run();

        // run the choreo on the Temboo server at the given IP address and port
        // (used only when instructed by Temboo customer support.)
        int run(IPAddress addr, uint16_t port);

        void close();

        // Stream interface - see the Arduino library documentation.
        int available();
        int read();
        int peek();
        void flush();

        //Print interface - see the Arduino library documentation
        size_t write(uint8_t data);


    protected:
        ChoreoInputSet m_inputs;
        ChoreoOutputSet m_outputs;
        ChoreoPreset m_preset;

        const char* m_accountName;
        const char* m_appKeyValue;
        const char* m_appKeyName;
        const char* m_path;
        Client& m_client;
        char m_httpCodeStr[6];
        const char* m_nextChar;
        enum State {START, HTTP_CODE_TAG, HTTP_CODE_VALUE, END};
        State m_nextState;

};

#endif //ARDUINO_AVR_YUN

#endif //TEMBOO_H_
