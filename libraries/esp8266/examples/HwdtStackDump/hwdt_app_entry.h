#ifndef HWDT_STACK_DUMP_H
#define HWDT_STACK_DUMP_H

#ifndef DEBUG_HWDT
#define DEBUG_HWDT
#endif

typedef struct HWDT_INFO {
    uint32_t rom;
    uint32_t sys;
    uint32_t cont;
    uint32_t rtc_sys_reason;
    uint32_t rom_api_reason;
    uint32_t cont_integrity;
    uint32_t reset_reason;
    bool g_pcont_valid;
} HWDT_INFO_t;

void enable_debug_hwdt_at_link_time(void);

extern uint32_t *g_rom_stack;
extern HWDT_INFO_t hwdt_info;

#endif
