
#include "spiffs_mock.h"

#if 0
#include "../common/spiffs_mock.h"
#include <spiffs/spiffs.h>
SPIFFS_MOCK_DECLARE(/*size_kb*/64, /*block_kb*/8, /*page_b*/512, true);
#endif

SpiffsMock* spiffs_mock = nullptr;

void mock_start_spiffs (size_t size_kb, size_t block_kb, size_t page_b)
{
	spiffs_mock = new SpiffsMock(size_kb * 1024, block_kb * 1024, page_b, true);
}

void mock_stop_spiffs ()
{
	delete spiffs_mock;
	spiffs_mock = nullptr;
}

