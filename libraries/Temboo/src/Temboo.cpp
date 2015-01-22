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


#if defined (ARDUINO_AVR_YUN) || defined (ARDUINO_AVR_TRE)

///////////////////////////////////////////////////////
//  BEGIN ARDUINO YUN AND TRE SUPPORT
///////////////////////////////////////////////////////

#include <Temboo.h>

void TembooChoreo::begin() {
    Process::begin("temboo");
}

void TembooChoreo::setAccountName(const String& accountName) {
    addParameter("-a" + accountName);
}

void TembooChoreo::setAppKeyName(const String& appKeyName) {
    addParameter("-u" + appKeyName);
}

void TembooChoreo::setAppKey(const String& appKey) {
    addParameter("-p" + appKey);
}

void TembooChoreo::setChoreo(const String& choreo) {
    addParameter("-c" + choreo);
}

void TembooChoreo::setCredential(const String& credentialName) {
    addParameter("-e" + credentialName);
}

void TembooChoreo::setSavedInputs(const String& savedInputsName) {
    addParameter("-e" + savedInputsName);
}

void TembooChoreo::setProfile(const String& profileName) {
    addParameter("-e" + profileName);
}

void TembooChoreo::addInput(const String& inputName, const String& inputValue) {
    addParameter("-i" + inputName + ":" + inputValue);
}

void TembooChoreo::addOutputFilter(const String& outputName, const String& filterPath, const String& variableName) {
    addParameter("-o" + outputName + ":" + filterPath + ":" + variableName);
}

void TembooChoreo::setSettingsFileToWrite(const String& filePath) {
    addParameter("-w" + filePath);
}

void TembooChoreo::setSettingsFileToRead(const String& filePath) {
    addParameter("-r" + filePath);
}


#else //ARDUINO_AVR_YUN

///////////////////////////////////////////////////////
//  BEGIN ARDUINO NON-YUN SUPPORT
///////////////////////////////////////////////////////

#include <string.h>
#include <Client.h>
#include <avr/pgmspace.h>
#include <Temboo.h>
#include "utility/TembooGlobal.h"
#include "utility/TembooSession.h"

static const char HTTP_CODE[] PROGMEM = "HTTP_CODE\x0A\x1F";

TembooChoreo::TembooChoreo(Client& client) : m_client(client) {
    m_accountName = NULL;
    m_appKeyName = NULL;
    m_appKeyValue = NULL;
    m_path = NULL;
    m_nextChar = NULL;
    m_nextState = END;
}

void TembooChoreo::setAccountName(const String& accountName) {
    m_accountName = accountName.c_str();
}


void TembooChoreo::setAccountName(const char* accountName) {
    m_accountName = accountName;
}


void TembooChoreo::setAppKeyName(const String& appKeyName) {
    m_appKeyName = appKeyName.c_str();
}


void TembooChoreo::setAppKeyName(const char* appKeyName) {
    m_appKeyName = appKeyName;
}


void TembooChoreo::setAppKey(const String& appKeyValue) {
    m_appKeyValue = appKeyValue.c_str();
}


void TembooChoreo::setAppKey(const char* appKeyValue) {
    m_appKeyValue = appKeyValue;
}


void TembooChoreo::setChoreo(const String& path) {
    m_path = path.c_str();
}


void TembooChoreo::setChoreo(const char* path) {
    m_path = path;
}


void TembooChoreo::setSavedInputs(const String& savedInputsName) {
    m_preset.put(savedInputsName.c_str());
}


void TembooChoreo::setSavedInputs(const char* savedInputsName) {
    m_preset.put(savedInputsName);
}


void TembooChoreo::setCredential(const String& credentialName) {
    m_preset.put(credentialName.c_str());
}


void TembooChoreo::setCredential(const char* credentialName) {
    m_preset.put(credentialName);
}

void TembooChoreo::setProfile(const String& profileName) {
    m_preset.put(profileName.c_str());
}


void TembooChoreo::setProfile(const char* profileName) {
    m_preset.put(profileName);
}



void TembooChoreo::addInput(const String& inputName, const String& inputValue) {
    m_inputs.put(inputName.c_str(), inputValue.c_str());
}


void TembooChoreo::addInput(const char* inputName, const char* inputValue) {
    m_inputs.put(inputName, inputValue);
}


void TembooChoreo::addInput(const char* inputName, const String& inputValue) {
    m_inputs.put(inputName, inputValue.c_str());
}


void TembooChoreo::addInput(const String& inputName, const char* inputValue) {
    m_inputs.put(inputName.c_str(), inputValue);
}


void TembooChoreo::addOutputFilter(const char* outputName, const char* filterPath, const char* variableName) {
    m_outputs.put(outputName, filterPath, variableName);
}


void TembooChoreo::addOutputFilter(const String& outputName, const char* filterPath, const char* variableName) {
    m_outputs.put(outputName.c_str(), filterPath, variableName);
}


void TembooChoreo::addOutputFilter(const char* outputName, const String& filterPath, const char* variableName) {
    m_outputs.put(outputName, filterPath.c_str(), variableName);
}


void TembooChoreo::addOutputFilter(const String& outputName, const String& filterPath, const char* variableName) {
    m_outputs.put(outputName.c_str(), filterPath.c_str(), variableName);
}


void TembooChoreo::addOutputFilter(const char* outputName, const char* filterPath, const String& variableName) {
    m_outputs.put(outputName, filterPath, variableName.c_str());
}


void TembooChoreo::addOutputFilter(const String& outputName, const char* filterPath, const String& variableName) {
    m_outputs.put(outputName.c_str(), filterPath, variableName.c_str());
}


void TembooChoreo::addOutputFilter(const char* outputName, const String& filterPath, const String& variableName) {
    m_outputs.put(outputName, filterPath.c_str(), variableName.c_str());
}


void TembooChoreo::addOutputFilter(const String& outputName, const String& filterPath, const String& variableName) {
    m_outputs.put(outputName.c_str(), filterPath.c_str(), variableName.c_str());
}


int TembooChoreo::run() {
    return run(INADDR_NONE, 80);
}


int TembooChoreo::run(IPAddress addr, uint16_t port) {

    m_nextChar = NULL;

    if (m_accountName == NULL || *m_accountName == '\0') {
        return TEMBOO_ERROR_ACCOUNT_MISSING;
    }

    if (m_path == NULL || *m_path == '\0') {
        return TEMBOO_ERROR_CHOREO_MISSING;
    }

    if (m_appKeyName == NULL || *m_appKeyName == '\0') {
        return TEMBOO_ERROR_APPKEY_NAME_MISSING;
    }

    if (m_appKeyValue == NULL || *m_appKeyValue == '\0') {
        return TEMBOO_ERROR_APPKEY_MISSING;
    }


    TembooSession session(m_client, addr, port);
    uint16_t httpCode = 0;
    
    for (int i = 0; i < 2; i++) {
        if (0 != session.executeChoreo(m_accountName, m_appKeyName, m_appKeyValue, m_path, m_inputs, m_outputs, m_preset)) {
            httpCode = 0;
            break;
        }

        while(!m_client.available()) {
            if (!m_client.connected()) {
                TEMBOO_TRACELN("Disconnected");
                return TEMBOO_ERROR_HTTP_ERROR;
            }
            delay(10);
        }

        if (!m_client.find("HTTP/1.1 ")) {
            TEMBOO_TRACELN("No HTTP");
            return TEMBOO_ERROR_HTTP_ERROR;
        }

        httpCode = (uint16_t)m_client.parseInt();

        // We expect HTTP response codes to be <= 599, but 
        // we need to be prepared for anything.
        if (httpCode >= 600) {
            TEMBOO_TRACELN("Invalid HTTP");
            httpCode = 0;
        }

        // if we get an auth error AND there was an x-temboo-time header,
        // update the session timeOffset
        if ((httpCode == 401) && (i == 0)) {
            if (m_client.find("x-temboo-time:")) {
                TembooSession::setTime((unsigned long)m_client.parseInt());
                while(m_client.available()) {
                    m_client.read();
                }
                m_client.stop();
            }
        } else {
            break;
        }
    }

    uint16toa(httpCode, m_httpCodeStr);
    strcat_P(m_httpCodeStr, PSTR("\x0A\x1E"));
    m_nextState = START;
    m_nextChar = HTTP_CODE;

    if (httpCode < 200 || httpCode >= 300) {
        return TEMBOO_ERROR_HTTP_ERROR;
    }

    if (!m_client.find("\x0D\x0A\x0D\x0A")) {
        return TEMBOO_ERROR_HTTP_ERROR;
    }

    return TEMBOO_ERROR_OK;
}

void TembooChoreo::close() {
    m_client.stop();
}

int TembooChoreo::available() {
    // If we're still sending the HTTP response code,
    // report at least one character available.
    if (m_nextChar != NULL) {
        return m_client.available() + 1;
    }

    // Otherwise, return however many characters the client has.
    return m_client.available();
}


int TembooChoreo::peek() {
    // If we're still sending the HTTP response code,
    // return the next character in that sequence.
    if (m_nextChar != NULL) {
        return (int)*m_nextChar;
    }

    // Otherwise, return whatever is in the client buffer.
    return m_client.peek();
}


int TembooChoreo::read() {

    int c = 0;
    switch(m_nextState) {
        case START:
            m_nextChar = HTTP_CODE;
            c = (int)pgm_read_byte(m_nextChar++);
            m_nextState = HTTP_CODE_TAG;
            break;

        case HTTP_CODE_TAG:
            c = (int)pgm_read_byte(m_nextChar++);
            if (pgm_read_byte(m_nextChar) == '\0') {
                m_nextState = HTTP_CODE_VALUE;
                m_nextChar = m_httpCodeStr;
            }
            break;

        case HTTP_CODE_VALUE:
            c = (int)(*m_nextChar++);
            if (*m_nextChar == '\0') {
                m_nextState = END;
                m_nextChar = NULL;
            }
            break;

        default:
            c = m_client.read();
    }
    return c;
}


size_t TembooChoreo::write(uint8_t data) {
    return m_client.write(data);
}


void TembooChoreo::flush() {
    m_nextChar = NULL;
    m_nextState = END;
    m_client.flush();
}

#endif //ARDUINO_AVR_YUN
