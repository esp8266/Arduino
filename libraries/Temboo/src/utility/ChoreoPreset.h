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

#ifndef CHOREOPRESET_H_
#define CHOREOPRESET_H_
#include <stddef.h>
#include "TembooGlobal.h"

class ChoreoPreset {
    public:
        ChoreoPreset()                      {m_name = NULL;}
        ChoreoPreset(const char* name)      {put(name);}
        const char* getName() const          {return m_name;}
        void put(const char* name)           {m_name = name;}
        bool isEmpty() const                 {return m_name == NULL || *m_name == '\0';}

    private:
        const char* m_name;
};

#endif
