
#include "littlefs_mock.h"

LittleFSMock* littlefs_mock = nullptr;

void mock_start_littlefs (const String& fname, size_t size_kb, size_t block_kb, size_t page_b)
{
	littlefs_mock = new LittleFSMock(size_kb * 1024, block_kb * 1024, page_b, fname);
}

void mock_stop_littlefs ()
{
	if (littlefs_mock)
		delete littlefs_mock;
	littlefs_mock = nullptr;
}

