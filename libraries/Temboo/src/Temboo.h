/*
###############################################################################
#
# Temboo Arduino Yun library
#
# Copyright 2013, Temboo Inc.
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

#ifndef _TEMBOO_H
#define _TEMBOO_H
#include <Arduino.h>
#include <Process.h>

class TembooChoreo : public Process {

  public:
    void begin();
    void setAccountName(const String& accountName);
    void setAppKeyName(const String& appKeyName);
    void setAppKey(const String& appKey);
    void setChoreo(const String& choreo);
    void setCredential(const String& credentialName);
    void addInput(const String& inputName, const String& inputValue);
    void addOutputFilter(const String& filterName, const String& filterPath, const String& variableName);
    void setSettingsFileToWrite(const String& filePath);
    void setSettingsFileToRead(const String& filePath);

};

#endif
