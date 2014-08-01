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

#include <stddef.h>
#include <string.h>
#include "ChoreoOutputSet.h"


ChoreoOutputSet::ChoreoOutputSet() {
    m_first = NULL;
}

ChoreoOutputSet::~ChoreoOutputSet() {
    ChoreoOutput* i = m_first;
    ChoreoOutput* next = NULL;
    while(i != NULL) {
        next = i->getNext();
        delete i;
        i = next;
    }
}

void ChoreoOutputSet::put(const char* name, const char* path, const char* variable) {
    if (m_first == NULL) {
        m_first = new ChoreoOutput(NULL, name, path, variable);
    } else {
        ChoreoOutput* last = NULL;
        ChoreoOutput* i = m_first;
        while(i != NULL) {
            if (strcmp(i->getName(), name) == 0) {
                i->setPath(path);
                i->setVariable(variable);
                break;
            }
            last = i;
            i = i->getNext();
        }

        if (i == NULL) {
            new ChoreoOutput(last, name, path, variable);
        }
    }
}

const ChoreoOutput* ChoreoOutputSet::get(const char* name) const {
    ChoreoOutput* i = m_first;
    while(i != NULL) {
        if (strcmp(i->getName(), name) == 0) {
            return i;
        }
        i = i->getNext();
    }
    return NULL;
}
