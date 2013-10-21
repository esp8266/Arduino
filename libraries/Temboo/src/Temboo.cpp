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

