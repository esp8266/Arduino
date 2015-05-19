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

#include <stddef.h>
#include <avr/pgmspace.h>
#include "ChoreoInputFormatter.h"
#include "ChoreoInputSet.h"

static const char TAG_INPUTS_START[] PROGMEM = "\"inputs\":{";

ChoreoInputFormatter::ChoreoInputFormatter(const ChoreoInputSet* inputSet) {
    m_inputSet = inputSet;
    reset();
}

void ChoreoInputFormatter::reset() {
    m_currentInput = NULL;
    m_nextChar = NULL;
    if (m_inputSet == NULL || m_inputSet->isEmpty()) {
        m_nextState = END;
    } else {
        m_nextState = START;
    }
}

bool ChoreoInputFormatter::hasNext() {
    return m_nextState != END;
}

char ChoreoInputFormatter::next() {
    char c;
    switch(m_nextState) {
        case START:
            c = readStartTagChar(TAG_INPUTS_START, INPUTS_TAG);
            break;
        
        case INPUTS_TAG:
            c = readTagChar(NAME_START);
            if (m_nextState == NAME_START) {
                m_currentInput= m_inputSet->getFirstInput();
            }
            break;
        
        case NAME_START: 
            c = '"';
            m_nextChar = m_currentInput->getName();
            if ((NULL == m_nextChar) || ('\0' == *m_nextChar)) {
                m_nextState = NAME_END;
            } else {
                m_nextState = NAME;
            }
            break;

        case NAME:
            c = readValueChar(NAME_END);
            break;

        case NAME_END: 
            c = '"';
            m_nextState = NAME_VALUE_SEPARATOR;
            break;

        case NAME_VALUE_SEPARATOR:
            c = ':';
            m_nextState = VALUE_START;
            break;

        case VALUE_START: 
            c = '"';
            m_nextChar = m_currentInput->getValue();
            if ((NULL == m_nextChar) || ('\0' == *m_nextChar)) {
                m_nextState = VALUE_END;
            } else {
                m_nextState = VALUE;
            }
            break;

        case VALUE:
            c = readValueChar(VALUE_END);
            break;

        case VALUE_END: 
            c = '"';
            m_currentInput = m_currentInput->getNext();
            if (m_currentInput != NULL) {
                m_nextState = NEXT_INPUT;
            } else {
                m_nextState = INPUTS_END;
            }
            break;
        case NEXT_INPUT:
            c = ',';
            m_nextChar = m_currentInput->getName();
            m_nextState = NAME_START;
            break;

        case INPUTS_END:
            c = '}';
            m_nextState = END;
            break;
        case END: 
        default:
            c = '\0';
    }
    return c;
}
