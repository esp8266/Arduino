/*
 *  This is the original app_entry() not providing extra 4K heap, but allowing
 *  the use of WPS.
 *
 *  see comments in core_esp8266_main.cpp's app_entry()
 *
 */

#include <c_types.h>
#include "cont.h"
#include "coredecls.h"

void disable_extra4k_at_link_time (void)
{
    /*
     * does nothing
     * allows overriding the core_esp8266_main.cpp's app_entry()
     * by this one below, at link time
     *
     */
}

/* the following code is linked only if a call to the above function is made somewhere */

extern "C" void call_user_start();

/* this is the default NONOS-SDK user's heap location */
static cont_t g_cont __attribute__ ((aligned (16)));

#if defined(DEBUG_ESP_HWDT_NOEXTRA4K) || defined(DEBUG_ESP_HWDT)
extern "C" cont_t * IRAM_ATTR get_noextra4k_g_pcont(void)
{
    return &g_cont;
}

#else
extern "C" void app_entry_redefinable(void)
{
    g_pcont = &g_cont;

    /* Call the entry point of the SDK code. */
    call_user_start();
}
#endif
