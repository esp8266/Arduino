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

#ifndef CHOREOINPUTFORMATTER_H_
#define CHOREOINPUTFORMATTER_H_
#include "TembooGlobal.h"
#include "BaseFormatter.h"
#include "ChoreoInputSet.h"

class ChoreoInputFormatter : public BaseFormatter {

    public:
        ChoreoInputFormatter(const ChoreoInputSet* inputSet);
        bool hasNext();
        char next();
        void reset();

    protected:
        const ChoreoInputSet* m_inputSet;
        const ChoreoInput* m_currentInput;
        
        
        enum State {
            START,
            INPUTS_TAG,
            NAME_START,
            NAME,
            NAME_END,
            NAME_VALUE_SEPARATOR,
            VALUE_START,
            VALUE,
            VALUE_END,
            NEXT_INPUT,
            INPUTS_END,
            END
        };
};

#endif
