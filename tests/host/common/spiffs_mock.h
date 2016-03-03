//
//  spiffs_mock.hpp
//  esp8266-host-tests
//
//  Created by Ivan Grokhotkov on 03/03/16.
//  Copyright © 2016 esp8266.com. All rights reserved.
//

#ifndef spiffs_mock_hpp
#define spiffs_mock_hpp

#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <FS.h>

class SpiffsMock {
public:
    SpiffsMock(size_t fs_size, size_t fs_block, size_t fs_page);
    ~SpiffsMock();
    
protected:
    std::vector<uint8_t> m_fs;
};


#endif /* spiffs_mock_hpp */
