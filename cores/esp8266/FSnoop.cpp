/*
 *  no-op implementations
 *  used/linked when no strong implementation already exists elsewhere
 */

#include <FS.h>

extern "C"
{

void close_all_fs(void)
{
    littlefs_request_end();
    spiffs_request_end();
}

// default weak definitions
// they are overriden in their respective real implementation
// hint: https://github.com/esp8266/Arduino/pull/6699#issuecomment-549085382

void littlefs_request_end(void) __attribute__((weak));
void littlefs_request_end(void)
{
    //ets_printf("debug: noop: littlefs_request_end\n");
}

void spiffs_request_end(void) __attribute__((weak));
void spiffs_request_end(void)
{
    //ets_printf("debug: noop: spiffs_request_end\n");
}

}
