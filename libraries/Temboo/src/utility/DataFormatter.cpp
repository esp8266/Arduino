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

#include "DataFormatter.h"


DataFormatter::DataFormatter(
        const ChoreoInputSet* inputSet, 
        const ChoreoOutputSet* outputSet, 
        const ChoreoPreset* preset) : 
    m_inputFormatter(inputSet), 
    m_outputFormatter(outputSet), 
    m_presetFormatter(preset) {

    m_inputSet = inputSet;
    m_outputSet = outputSet;
    m_preset = preset;
   
    reset();
}

void DataFormatter::reset() {
    m_nextState = DATA_START;
    m_inputFormatter.reset();
    m_outputFormatter.reset();
    m_presetFormatter.reset();
}

bool DataFormatter::hasNext() {
    return m_nextState != DATA_END;
}

char DataFormatter::next() {
    char c;
    switch(m_nextState) {
        case DATA_START:
            c = '{';
            if (m_inputFormatter.hasNext()) {
                m_nextState = FORMATTING_INPUTS;
            } else if (m_outputFormatter.hasNext()) {
                m_nextState = FORMATTING_OUTPUTS;
            } else if (m_presetFormatter.hasNext()) {
                m_nextState = FORMATTING_PRESET;
            } else {
                m_nextState = FORMATTING_EMPTY;
            }
            break;
        case FORMATTING_INPUTS:
            if (m_inputFormatter.hasNext()) {
                c = m_inputFormatter.next();
            } else if (m_outputFormatter.hasNext()) {
                c = ',';
                m_nextState = FORMATTING_OUTPUTS;
            } else if (m_presetFormatter.hasNext()) {
                c = ',';
                m_nextState = FORMATTING_PRESET;
            } else {
                c = '}';
                m_nextState = DATA_END;
            }
            break;
        case FORMATTING_OUTPUTS:
            if (m_outputFormatter.hasNext()) {
                c = m_outputFormatter.next();
            } else if (m_presetFormatter.hasNext()) {
                c = ',';
                m_nextState = FORMATTING_PRESET;
            } else {
                c = '}';
                m_nextState = DATA_END;
            }
            break;

        case FORMATTING_PRESET:
            if (m_presetFormatter.hasNext()) {
                c = m_presetFormatter.next();
            } else {
                c = '}';
                m_nextState = DATA_END;
            }
            break;

        case FORMATTING_EMPTY:
            c = '}';
            m_nextState = DATA_END;
            break;

        case DATA_END:
        default:
            c = '\0';
            break;
    }
    return c;
}

