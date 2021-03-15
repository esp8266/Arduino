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
 
#ifndef __CONDITIONALPRINTER_H__
#define __CONDITIONALPRINTER_H__

class ConditionalPrinter
{

public:

  /**
   * Set whether the normal events occurring in the library will be printed to Serial or not.
   * 
   * @param enabled If true, library Serial prints are activated.
   */
  void setVerboseModeState(const bool enabled);
  bool verboseMode() const;
  
  /**
   * Only print stringToPrint if verboseMode() returns true.
   * 
   * @param stringToPrint String to print.
   * @param newline If true, will end the print with a newline. True by default.
   */
  void verboseModePrint(const String &stringToPrint, const bool newline = true) const;

  /**
   * Same as verboseMode(), but used for printing from static functions.
   * 
   * @param enabled If true, the normal events occurring in the library will be printed to Serial.
   */
   static void setStaticVerboseModeState(const bool enabled);
   static bool staticVerboseMode();
  
  /**
   * Only print stringToPrint if staticVerboseMode() returns true.
   * 
   * @param stringToPrint String to print.
   * @param newline If true, will end the print with a newline. True by default.
   */
  static void staticVerboseModePrint(const String &stringToPrint, const bool newline = true);
  
  /**
   * Set whether the warnings occurring in the library will be printed to Serial or not. On by default.
   * 
   * @param printEnabled If true, warning Serial prints from the library are activated.
   */
  static void setPrintWarnings(const bool printEnabled);
  static bool printWarnings();

  /**
   * Only print stringToPrint if printWarnings() returns true.
   * 
   * @param stringToPrint String to print.
   * @param newline If true, will end the print with a newline. True by default.
   */
  static void warningPrint(const String &stringToPrint, const bool newline = true);
  
private:

  bool _verboseMode = false;

};

#endif
