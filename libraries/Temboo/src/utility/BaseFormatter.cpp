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

#include "BaseFormatter.h"

char BaseFormatter::escape(char c) {
    char outChar = c;
    switch(c) {
        case '\\':
        case '"':
            outChar = '\\';
            m_escapedChar = c;
            break;
        case '\b':
            outChar = '\\';
            m_escapedChar = 'b';
            break;
        case '\f':
            outChar = '\\';
            m_escapedChar = 'f';
            break;
        case '\n':
            outChar = '\\';
            m_escapedChar = 'n';
            break;
        case '\r':
            outChar = '\\';
            m_escapedChar = 'r';
            break;
        case '\t':
            outChar = '\\';
            m_escapedChar = 't';
            break;
        default:
            m_escapedChar = '\0';
    }
    return outChar;
}

char BaseFormatter::finishEscape() {
    char c = m_escapedChar;
    m_escapedChar = '\0';
    return c;
}

char BaseFormatter::readTagChar(int nextState) {
    char c = pgm_read_byte(m_nextChar++);
    if (pgm_read_byte(m_nextChar) == '\0') {
        m_nextState = nextState;
    }
    return c;
}

char BaseFormatter::readValueChar(int nextState) {
    char c;
    if (isEscaping()) {
        c = finishEscape();
        if (*m_nextChar == '\0') {
            m_nextState = nextState;
        }
    } else {
        c = escape(*m_nextChar++);
        if (!isEscaping()) {
            if(*m_nextChar == '\0') {
                m_nextState = nextState;
            }
        }
    }
    return c;
}

char BaseFormatter::readStartTagChar(const char* tag, int nextState) {
    m_nextChar = tag;
    char c = pgm_read_byte(m_nextChar++);
    m_nextState = nextState;
    return c;
}
