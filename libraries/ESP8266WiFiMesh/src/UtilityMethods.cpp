/*
 * UtilityMethods
 * Copyright (C) 2018 Anders Löfgren
 *
 * License (MIT license):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "TypeConversionFunctions.h"
#include "MeshBackendBase.h"
#include "EspnowMeshBackend.h"

void MeshBackendBase::setVerboseModeState(const bool enabled) {_verboseMode = enabled;}
bool MeshBackendBase::verboseMode() const {return _verboseMode;}

void MeshBackendBase::verboseModePrint(const String &stringToPrint, const bool newline) const
{
  if(verboseMode())
  {
    if(newline)
      Serial.println(stringToPrint);
    else
      Serial.print(stringToPrint);
  }
}

void EspnowMeshBackend::setVerboseModeState(const bool enabled) {MeshBackendBase::setVerboseModeState(enabled); _staticVerboseMode = enabled;}
bool EspnowMeshBackend::verboseMode() const {return staticVerboseMode();}

void EspnowMeshBackend::verboseModePrint(const String &stringToPrint, const bool newline) const
{
  if(verboseMode())
  {
    if(newline)
      Serial.println(stringToPrint);
    else
      Serial.print(stringToPrint);
  }
}

bool EspnowMeshBackend::staticVerboseMode() {return _staticVerboseMode;}

void EspnowMeshBackend::staticVerboseModePrint(const String &stringToPrint, const bool newline)
{
  if(staticVerboseMode())
  {
    if(newline)
      Serial.println(stringToPrint);
    else
      Serial.print(stringToPrint);
  }
}

void MeshBackendBase::setPrintWarnings(const bool printEnabled) {_printWarnings = printEnabled;}
bool MeshBackendBase::printWarnings() {return _printWarnings;}

void MeshBackendBase::warningPrint(const String &stringToPrint, const bool newline)
{
  if(printWarnings())
  {
    if(newline)
      Serial.println(stringToPrint);
    else
      Serial.print(stringToPrint);
  }
}
