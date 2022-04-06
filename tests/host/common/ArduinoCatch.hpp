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

#include <Arduino.h>
#include <sys/time.h>

#include <ostream>

#include "catch.hpp"

// Since Catch does not know about Arduino types, help it out so we could have these displayed in the tests output

std::ostream& operator<<(std::ostream&, const String&);

namespace Catch {

std::string toString(const String&);

template<>
struct StringMaker<String> {
    static std::string convert(const String&);
};

} // namespace Catch
