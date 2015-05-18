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

#ifndef CHOREOOUTPUTFORMATTER_H_
#define CHOREOOUTPUTFORMATTER_H_
#include "TembooGlobal.h"
#include "BaseFormatter.h"
#include "ChoreoOutputSet.h"

class ChoreoOutputFormatter : public BaseFormatter {

    public:
        ChoreoOutputFormatter(const ChoreoOutputSet* outputSet);
        bool hasNext();
        char next();
        void reset();

    protected:
        const ChoreoOutputSet* m_outputSet;
        const ChoreoOutput* m_currentOutput;
        
        enum State {
            START,
            OUTPUTS_TAG,
            OUTPUT_START,
            NAME_TAG,
            NAME_START,
            NAME,
            NAME_END,
            NAME_PATH_SEPARATOR,
            PATH_TAG,
            PATH_START,
            PATH,
            PATH_END,
            PATH_VAR_SEPARATOR,
            VAR_TAG,
            VAR_START,
            VAR,
            VAR_END,
            OUTPUT_END,
            NEXT_OUTPUT,
            OUTPUTS_END,
            END
        };
};

#endif
