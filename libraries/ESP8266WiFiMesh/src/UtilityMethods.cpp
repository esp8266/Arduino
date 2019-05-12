/*
    TransmissionResult
    Copyright (C) 2018 Anders Löfgren

    License (MIT license):

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include "TypeConversionFunctions.h"
#include "ESP8266WiFiMesh.h"

void ESP8266WiFiMesh::verboseModePrint(const String &stringToPrint, bool newline)
{
    if (_verboseMode)
    {
        if (newline)
        {
            Serial.println(stringToPrint);
        }
        else
        {
            Serial.print(stringToPrint);
        }
    }
}

/**
    Calculate the current lwIP version number and store the numbers in the _lwipVersion array.
    lwIP version can be changed in the "Tools" menu of Arduino IDE.
*/
void ESP8266WiFiMesh::storeLwipVersion()
{
    // ESP.getFullVersion() looks something like:
    // SDK:2.2.1(cfd48f3)/Core:win-2.5.0-dev/lwIP:2.0.3(STABLE-2_0_3_RELEASE/glue:arduino-2.4.1-10-g0c0d8c2)/BearSSL:94e9704
    String fullVersion = ESP.getFullVersion();

    int i = fullVersion.indexOf("lwIP:") + 5;
    char currentChar = fullVersion.charAt(i);

    for (int versionPart = 0; versionPart < 3; versionPart++)
    {
        while (!isdigit(currentChar))
        {
            currentChar = fullVersion.charAt(++i);
        }
        while (isdigit(currentChar))
        {
            _lwipVersion[versionPart] = 10 * _lwipVersion[versionPart] + (currentChar - '0'); // Left shift and add digit value, in base 10.
            currentChar = fullVersion.charAt(++i);
        }
    }
}

/**
    Check if the code is running on a version of lwIP that is at least minLwipVersion.
*/
bool ESP8266WiFiMesh::atLeastLwipVersion(const uint32_t minLwipVersion[3])
{
    for (int versionPart = 0; versionPart < 3; versionPart++)
    {
        if (_lwipVersion[versionPart] > minLwipVersion[versionPart])
        {
            return true;
        }
        else if (_lwipVersion[versionPart] < minLwipVersion[versionPart])
        {
            return false;
        }
    }

    return true;
}
