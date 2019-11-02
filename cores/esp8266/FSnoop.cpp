/*
 *  no-op implementations
 *  used/linked when no not-weak implementation already exists elsewhere
 */

#include <FS.h>

extern "C"
{

void littlefs_weak_end(void) __attribute__((weak));
void littlefs_weak_end(void)
{
    //ets_printf("debug: noop: littlefs_weak_end\n");
}

void spiffs_weak_end(void) __attribute__((weak));
void spiffs_weak_end(void)
{
    //ets_printf("debug: noop: spiffs_weak_end\n");
}

}
