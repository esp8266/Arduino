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

#ifndef DATAFORMATTER_H_
#define DATAFORMATTER_H_
#include "TembooGlobal.h"
#include "ChoreoInputSet.h"
#include "ChoreoOutputSet.h"
#include "ChoreoPreset.h"
#include "ChoreoInputFormatter.h"
#include "ChoreoOutputFormatter.h"
#include "ChoreoPresetFormatter.h"


class DataFormatter {

    public:
        DataFormatter(const ChoreoInputSet* inputSet, const ChoreoOutputSet* outputSet, const ChoreoPreset* preset);
        bool hasNext();
        char next();
        void reset();

    private:
        const ChoreoInputSet* m_inputSet;
        const ChoreoOutputSet* m_outputSet;
        const ChoreoPreset* m_preset;

        ChoreoInputFormatter m_inputFormatter;
        ChoreoOutputFormatter m_outputFormatter;
        ChoreoPresetFormatter m_presetFormatter;
    
        enum State {
            DATA_START,
            FORMATTING_INPUTS,
            FORMATTING_OUTPUTS,
            FORMATTING_PRESET,
            FORMATTING_EMPTY,
            DATA_END
        };

        State m_nextState;

};
#endif
