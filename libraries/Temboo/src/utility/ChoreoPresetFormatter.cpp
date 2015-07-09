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
#include "ChoreoPresetFormatter.h"
#include "ChoreoPreset.h"

static const char TAG_PRESET[] PROGMEM = "\"preset\":";

ChoreoPresetFormatter::ChoreoPresetFormatter(const ChoreoPreset* preset) {
    m_preset = preset;
    reset();
}

void ChoreoPresetFormatter::reset() {
    m_nextChar = NULL;
    if (m_preset == NULL || m_preset->isEmpty()) {
        m_nextState = END;
    } else {
        m_nextState = START;
    }
}

bool ChoreoPresetFormatter::hasNext() {
    return m_nextState != END;
}

char ChoreoPresetFormatter::next() {
    char c = '\0';
    switch(m_nextState) {
        case START:
            c = readStartTagChar(TAG_PRESET, PRESET_TAG);
            break;

        case PRESET_TAG:
            c = readTagChar(NAME_START);
            break;

        case NAME_START:
            c = '"';
            m_nextChar = m_preset->getName();
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
            m_nextState = END;
            break;

        case END:
        default:
            c = '\0';
    }
    return c;
}

