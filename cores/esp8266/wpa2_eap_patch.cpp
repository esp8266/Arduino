/*
 * To implement this patch, the SDK module `eap.o` from archive `libwpa2.a` must
 * be patched to call `z2EapFree` instead of `vPortFree`. This limits extending
 * the execution time of vPortFree to that module only. Not impacting other
 * modules.
 *
 */
#include <string.h>
#include <ets_sys.h>
#include <pgmspace.h>
#include "coredecls.h"

#if defined(NONOSDK22x_190703) || \
    defined(NONOSDK22x_191122) || \
    defined(NONOSDK22x_191105) || \
    defined(NONOSDK22x_191124) || \
    defined(NONOSDK22x_190313) || \
    defined(NONOSDK221)        || \
    defined(NONOSDK3V0)        || \
    defined(NONOSDK300)        || \
    defined(NONOSDK301)        || \
    defined(NONOSDK302)        || \
    defined(NONOSDK303)        || \
    defined(NONOSDK304)        || \
    defined(NONOSDK305)

// eap_peer_config_deinit() - For this list of SDKs there are no significant
// changes in the function. Just the line number reference for when vPortFree
// is called. When vPortFree is called, register a12 continues to hold a pointer
// to the struct StateMachine. Our cleanup routine should continue to work.
#if defined(NONOSDK300) || defined(NONOSDK301)
    // Minor changes only line number changed
    #define SDK_LEAK_LINE 809
#elif defined(NONOSDK302) || defined(NONOSDK303) || defined(NONOSDK304)
    // Minor changes only line number changed
    #define SDK_LEAK_LINE 831
#elif defined(NONOSDK305)
    // At v3.0.5 Espressif moved `.text.eap_peer_config_deinit` to
    // `eap_peer_config_deinit` then later in latest git they moved it
    // back. For our linker script both are placed in flash.
    #define SDK_LEAK_LINE 831
#else
    #define SDK_LEAK_LINE 799
#endif

#ifdef  DEBUG_WPA2_EAP_PATCH
#include "esp8266_undocumented.h"
#define DEBUG_PRINTF ets_uart_printf
#else
#define DEBUG_PRINTF(...)
#endif

extern "C" {

// extern "C" void z2EapFree(void *ptr, const char* file, int line) __attribute__((weak, alias("vPortFree")));

/*
 * Limited 2-part wrapper for vPortFree calls made in SDK module `eap.o` from
 * archive `libwpa2.a`.
 *
 * vPortFree calls from eap.o are monitored for calls from line 799. This is
 * the location of the memory leak. At entry register a12 contains the structure
 * address which has the addresses of the allocations that will be leaked.
 *
 * Part 1 of this wrapper, z2EapFree, appends the value of register a12 as a
 * 4th argument to part2 of this wrapper, patch_wpa2_eap_vPortFree_a12(). Which
 * in turn checks and frees the additional allocations, that would have been
 * lost.
 *
 * extern "C" z2EapFree(void*);
 */

/*
 * Part 1 of Limited vPortFree Wrapper
 */
asm(
    // ".section     .iram.text.z2EapFree,\"ax\",@progbits\n\t"
    // Since all the possible callers in eap.o are in sections starting with
    // .text and not .iram.text we should be safe putting these wrappers in .text.
    ".section     .text.z2EapFree,\"ax\",@progbits\n\t"
    ".literal_position\n\t"
    ".literal     .patch_wpa2_eap_vPortFree_a12, patch_wpa2_eap_vPortFree_a12\n\t"
    ".align       4\n\t"
    ".global      z2EapFree\n\t"
    ".type        z2EapFree, @function\n\t"
    "\n"
"z2EapFree:\n\t"
    "addi         a1,     a1,     -16\n\t"
    "s32i         a0,     a1,     0\n\t"
    "mov          a5,     a12\n\t"
    "l32r         a0,     .patch_wpa2_eap_vPortFree_a12\n\t"
    "callx0       a0\n\t"
    "l32i         a0,     a1,     0\n\t"
    "addi         a1,     a1,     16\n\t"
    "ret\n\t"
    ".size z2EapFree, .-z2EapFree\n\t"
);

/*
 * While some insight can be gained from the ESP32 repo for this structure.
 * It does not match exactly. This alternate structure focuses on correct offset
 * rather than trying to exactly reconstruct the original labels.
 * These offset were found in libwpa2.a:eap.o .text.eap_peer_config_init
 */
struct StateMachine { // size 200 bytes
    void* beforeConfig[16];
    void* config[26];
    // 0  - s32i a2, a12, 64  // username / Identity
    // 1  - s32i a2, a12, 68  //   length
    // 2  - s32i a2, a12, 72  // anonymous Identity
    // 3  - s32i a2, a12, 76
    // 4  - s32i a2, a12, 80  // password
    // 5  - s32i a2, a12, 84
    //
    // "new password" - From wifi_station_set_enterprise_new_password(), we see
    // global saved value .bss+32 and .bss+36 which are later used to populate
    // ".config" in eap_peer_config_init(). I do not have an environment to
    // exercise this parameter. In my tests, the "new password" element in the
    // ".config" is never initialized. At the moment, I don't see any code that
    // would free the allocation.
    // allocated via pvPortZalloc from line 0x30f, 783
    // 21 - s32i a2, a12, 148 // new password
    // 22 - s32i a2, a12, 152

    void* afterConfig[8];
};

/*
 * Part 2 of Limited vPortFree Wrapper
 *
 * Presently, all SDKs have the same memory leaks in the same module at the
 * same line.
 */
void patch_wpa2_eap_vPortFree_a12(void *ptr, const char* file, int line, void* a12) {
    if (SDK_LEAK_LINE == line) {
        // This caller is eap_peer_config_deinit()
        struct StateMachine* sm = (struct StateMachine*)a12;
        if (ptr == sm->config[0]) {
            // Fix leaky frunction - eap.o only frees one out of 4 config items
            // finish the other 3 first
            vPortFree(sm->config[2], file, line);
            vPortFree(sm->config[4], file, line);
            vPortFree(sm->config[21], file, line);
            // ptr is sm->config[0], let fall through handle it
        }
#ifdef  DEBUG_WPA2_EAP_PATCH
        DEBUG_PRINTF("\nz2EapFree/vPortFree patch struct StateMachine * = %8p\n", a12);
        DEBUG_PRINTF("  config[0]   vPortFree(%8p, file, line);\n", ptr);
        DEBUG_PRINTF("  config[2]   vPortFree(%8p, file, line);\n", sm->config[2]);
        DEBUG_PRINTF("  config[4]   vPortFree(%8p, file, line);\n", sm->config[4]);
        DEBUG_PRINTF("  config[21]  vPortFree(%8p, file, line);\n", sm->config[21]);
        if (a12) {
            void** pw = (void**)a12;
            DEBUG_PRINTF("\nhexdump struct StateMachine:\n");
            for (size_t i=0; i<200/4; i+=4) {
                DEBUG_PRINTF("%03u: %8p %8p %8p %8p\n", i*4, pw[i], pw[i+1], pw[i+2], pw[i+3]);
            }
        }
#endif
    }

#if defined(NONOSDK300) || defined(NONOSDK301)
    else if (682 == line) {
        // This caller is wpa2_sm_rx_eapol()
        // 1st of a double free
        // let the 2nd free handle it.
        return;
    }
#elif defined(NONOSDK302) || defined(NONOSDK303) || defined(NONOSDK304) || defined(NONOSDK305)
    // It looks like double free is fixed. WPA2 Enterpise connections work
    // without crashing. wpa2_sm_rx_eapol() has a few changes between NONOSDK301
    // and NONOSDK302. However, this set of releases still have memory leaks.
#else
    // This is not needed because the call was NO-OPed in the library.
    // Keep code snippit for reference.
    // else if (672 == line) {
    //     // This caller is wpa2_sm_rx_eapol()
    //     // 1st of a double free
    //     // let the 2nd free handle it.
    //     return;
    // }
#endif
    vPortFree(ptr, file, line);
}

};

#else
#error "Internal error: A new SDK has been added. This module must be updated."
#error "  Need to test WPA2 Enterpise connectivity."
#endif

/*
 * This will minimize code space for non-wifi enterprise sketches which do not
 * need the patch and disable_extra4k_at_link_time().
 */
void enable_wifi_enterprise_patch(void) {
    /*
     * Calling this from setup or anywhere ensures that the patch code is
     * included in the build.
     *
     * Also, WiFi Enterprise uses a lot of system stack space and may crash
     * unless we:
     */
    disable_extra4k_at_link_time();
}
