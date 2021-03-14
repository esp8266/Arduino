/*
 * Copyright (C) 2020 Anders Löfgren
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

namespace
{
  bool _staticVerboseMode = false;
  bool _printWarnings = true;
}

void ConditionalPrinter::setVerboseModeState(const bool enabled) {_verboseMode = enabled;}
bool ConditionalPrinter::verboseMode() const {return _verboseMode;}

void ConditionalPrinter::verboseModePrint(const String &stringToPrint, const bool newline) const
{
  if(verboseMode())
  {
    if(newline)
      Serial.println(stringToPrint);
    else
      Serial.print(stringToPrint);
  }
}

void ConditionalPrinter::setStaticVerboseModeState(const bool enabled) {_staticVerboseMode = enabled;};
bool ConditionalPrinter::staticVerboseMode() {return _staticVerboseMode;}

void ConditionalPrinter::staticVerboseModePrint(const String &stringToPrint, const bool newline)
{
  if(staticVerboseMode())
  {
    if(newline)
      Serial.println(stringToPrint);
    else
      Serial.print(stringToPrint);
  }
}

void ConditionalPrinter::setPrintWarnings(const bool printEnabled) {_printWarnings = printEnabled;}
bool ConditionalPrinter::printWarnings() {return _printWarnings;}

void ConditionalPrinter::warningPrint(const String &stringToPrint, const bool newline)
{
  if(printWarnings())
  {
    if(newline)
      Serial.println(stringToPrint);
    else
      Serial.print(stringToPrint);
  }
}
