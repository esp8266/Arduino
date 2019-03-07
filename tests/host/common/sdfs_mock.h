/*
 sdfs.h - SDFS mock for host side testing
 Copyright (c) 2019 Earle F. Philhower, III

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
*/

#ifndef sdfs_mock_hpp
#define sdfs_mock_hpp

#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <FS.h>

class SDFSMock {
public:
    SDFSMock() { }
    void reset() { }
    ~SDFSMock() { }
};

#define SDFS_MOCK_DECLARE() SDFSMock sdfs_mock();

#endif /* spiffs_mock_hpp */
