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

#ifndef CHOREOOUTPUT_H_
#define CHOREOOUTPUT_H_
#include "TembooGlobal.h"

class ChoreoOutput {
    public:
        ChoreoOutput(ChoreoOutput* prev, const char* name, const char* path, const char* var);
        const char* getName() const            {return m_name;}
        const char* getPath() const            {return m_path;}
        const char* getVariable() const        {return m_var;}
        void setPath(const char* path)         {m_path = path;}
        void setVariable(const char* variable) {m_var = variable;}
        ChoreoOutput* getNext() const          {return m_next;}

    private:
        ChoreoOutput* m_next;
        const char*  m_name;
        const char*  m_path;
        const char*  m_var;
};

#endif
