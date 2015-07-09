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
#include "ChoreoOutputFormatter.h"
#include "ChoreoOutputSet.h"

static const char TAG_OUTPUTS_START[] PROGMEM = "\"outputs\":[";
static const char TAG_NAME[]          PROGMEM = "\"name\":";
static const char TAG_PATH[]          PROGMEM = "\"path\":";
static const char TAG_VAR[]           PROGMEM = "\"variable\":";


ChoreoOutputFormatter::ChoreoOutputFormatter(const ChoreoOutputSet* outputSet) {
    m_outputSet = outputSet;
    reset();
}

void ChoreoOutputFormatter::reset() {
    m_currentOutput = NULL;
    m_nextChar = NULL;
    if (m_outputSet == NULL || m_outputSet->isEmpty()) {
        m_nextState = END;
    } else {
        m_nextState = START;
    }
}

bool ChoreoOutputFormatter::hasNext() {
    return m_nextState != END;
}

char ChoreoOutputFormatter::next() {
    char c = '\0';
    switch(m_nextState) {
        case START:
            c = readStartTagChar(TAG_OUTPUTS_START, OUTPUTS_TAG);
            break;

        case OUTPUTS_TAG:
            c = readTagChar(OUTPUT_START);
            if (m_nextState == OUTPUT_START) {
                m_currentOutput = m_outputSet->getFirstOutput();
            }
            break;

        case OUTPUT_START:
            c = '{';
            m_nextChar = TAG_NAME;
            m_nextState = NAME_TAG;
            break;

        case NAME_TAG:
            c = readTagChar(NAME_START);
            break;

        case NAME_START:
            c = '"';
            m_nextChar = m_currentOutput->getName();
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
            m_nextState = NAME_PATH_SEPARATOR;
            break;

        case NAME_PATH_SEPARATOR:
            c = ',';
            m_nextState = PATH_TAG;
            m_nextChar = TAG_PATH;
            break;

        case PATH_TAG:
            c = readTagChar(PATH_START);
            break;

        case PATH_START:
            c = '"';
            m_nextChar = m_currentOutput->getPath();
            if ((NULL == m_nextChar) || ('\0' == *m_nextChar)) {
                m_nextState = PATH_END;
            } else {
                m_nextState = PATH;
            }
            break;

        case PATH:
            c = readValueChar(PATH_END);
            break;

        case PATH_END:
            c = '"';
            m_nextState = PATH_VAR_SEPARATOR;
            break;

        case PATH_VAR_SEPARATOR:
            c = ',';
            m_nextState = VAR_TAG;
            m_nextChar = TAG_VAR;
            break;
            
        case VAR_TAG:
            c = readTagChar(VAR_START);
            break;

        case VAR_START:
            c = '"';
            m_nextChar = m_currentOutput->getVariable();
            if ((NULL == m_nextChar) || ('\0' == *m_nextChar)) {
                m_nextState = VAR_END;
            } else {
                m_nextState = VAR;
            }
            break;

        case VAR:
            c = readValueChar(VAR_END);
            break;

        case VAR_END:
            c = '"';
            m_nextState = OUTPUT_END;
            break;

        case OUTPUT_END:
            c = '}';
            m_currentOutput = m_currentOutput->getNext();
            if (m_currentOutput != NULL) {
                m_nextState = NEXT_OUTPUT;
            } else {
                m_nextState = OUTPUTS_END;
            }
            break;

        case NEXT_OUTPUT:
            c = ',';
            m_nextChar = m_currentOutput->getName();
            m_nextState = OUTPUT_START;
            break;

        case OUTPUTS_END:
            c = ']';
            m_nextState = END;
            break;
        case END:
        default:
            c = '\0';
    }

    return c;
}

