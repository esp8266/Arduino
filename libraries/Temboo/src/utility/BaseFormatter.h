/*
###############################################################################
#
# Temboo Arduino library
#
# Copyright 2015, Temboo Inc.
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

#ifndef BASEFORMATTER_H_
#define BASEFORMATTER_H_
#include "TembooGlobal.h"

class BaseFormatter {
    public:
        BaseFormatter() {m_escapedChar = '\0';}

    protected:
        const char* m_nextChar;
        int m_nextState;
        char m_escapedChar;

        char escape(char c);
        bool isEscaping() {return m_escapedChar != '\0';}
        char finishEscape();

        char readTagChar(int nextState);
        char readValueChar(int nextState);
        char readStartTagChar(const char* tag, int nextState);

};

#endif
