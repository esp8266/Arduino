/*
 Arduino EEPROM emulation
 Copyright (c) 2018 david gauchard. All rights reserved.

 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal with the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 - Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimers.

 - Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimers in the
   documentation and/or other materials provided with the distribution.

 - The names of its contributors may not be used to endorse or promote
   products derived from this Software without specific prior written
   permission.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS WITH THE SOFTWARE.
*/

#ifndef EEPROM_MOCK
#define EEPROM_MOCK

class EEPROMClass
{
public:
    EEPROMClass(uint32_t sector);
    EEPROMClass(void);
    ~EEPROMClass();

    void    begin(size_t size);
    uint8_t read(int address);
    void    write(int address, uint8_t val);
    bool    commit();
    void    end();

    template <typename T>
    T& get(int const address, T& t)
    {
        if (address < 0 || address + sizeof(T) > _size)
            return t;
        for (size_t i = 0; i < sizeof(T); i++)
            ((uint8_t*)&t)[i] = read(i);
        return t;
    }

    template <typename T>
    const T& put(int const address, const T& t)
    {
        if (address < 0 || address + sizeof(T) > _size)
            return t;
        for (size_t i = 0; i < sizeof(T); i++)
            write(i, ((uint8_t*)&t)[i]);
        return t;
    }

    size_t  length() { return _size; }

    //uint8_t& operator[](int const address) { return read(address); }
    uint8_t operator[](int address) { return read(address); }

protected:
    size_t _size = 0;
    int    _fd   = -1;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
extern EEPROMClass EEPROM;
#endif

#endif
