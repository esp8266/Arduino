/*
 Arduino.cpp - Mocks for common Arduino APIs
 Copyright © 2016 Ivan Grokhotkov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
*/

#define CATCH_CONFIG_MAIN
#include "ArduinoCatch.hpp"

std::ostream& operator<<(std::ostream& out, const String& str)
{
    out.write(str.c_str(), str.length());
    return out;
}

namespace Catch
{

std::string toString(const String& str)
{
    return std::string(str.begin(), str.length());
}

std::string StringMaker<String>::convert(String const& str)
{
    return toString(str);
}

}  // namespace Catch
