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

#ifndef CHOREOPROFILEFORMATTER_H_
#define CHOREOPROFILEFORMATTER_H_
#include "TembooGlobal.h"
#include "BaseFormatter.h"
#include "ChoreoPreset.h"

class ChoreoPresetFormatter : public BaseFormatter {

    public:
        ChoreoPresetFormatter(const ChoreoPreset* preset);
        bool hasNext();
        char next();
        void reset();

    protected:
        const ChoreoPreset* m_preset;
        
        enum State {
            START,
            PRESET_TAG,
            NAME_START,
            NAME,
            NAME_END,
            END
        };
};

#endif
