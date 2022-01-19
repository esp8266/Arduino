
#include "spiffs_mock.h"

SpiffsMock* spiffs_mock = nullptr;

void mock_start_spiffs(const String& fname, size_t size_kb, size_t block_kb, size_t page_b)
{
    spiffs_mock = new SpiffsMock(size_kb * 1024, block_kb * 1024, page_b, fname);
}

void mock_stop_spiffs()
{
    if (spiffs_mock)
    {
        delete spiffs_mock;
    }
    spiffs_mock = nullptr;
}

