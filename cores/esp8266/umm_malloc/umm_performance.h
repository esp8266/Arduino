/*
 * umm_malloc performance measurments and ESP specifics
 */

#ifndef _UMM_PERFORMANCE_H
#define _UMM_PERFORMANCE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -D UMM_CRITICAL_PERIOD_ANALYZE :
 *
 * Build option to collect timing usage data on critical section usage in
 * functions: info, malloc, realloc. Collects MIN, MAX, and number of time
 * IRQs were disabled at request time. Note, for realloc MAX disabled time
 * will not include the time from calling malloc and/or free.
 * Examine code for specifics on what info is available and how to access.
*/
#define UMM_CRITICAL_PERIOD_ANALYZE


/*
  Per Devyte, the core currently doesn't support masking a specific interrupt
  level. That doesn't mean it can't be implemented, only that at this time
  locking is implemented as all or nothing.
  https://github.com/esp8266/Arduino/issues/6246#issuecomment-508612609

  So for now we default to all, 15.
 */
#ifndef DEFAULT_CRITICAL_SECTION_INTLEVEL
#define DEFAULT_CRITICAL_SECTION_INTLEVEL 15
#endif

#if defined(UMM_CRITICAL_PERIOD_ANALYZE)
// This option adds support for gathering time locked data
typedef struct _TIME_STAT {
  uint32_t min;
  uint32_t max;
  uint32_t start;
  uint32_t intlevel;
} time_stat_t;

struct _UMM_TIME_STATS {
  time_stat_t id_malloc;
  time_stat_t id_realloc;
  time_stat_t id_free;
  time_stat_t id_info;
};

extern struct _UMM_TIME_STATS time_stats;

bool get_umm_get_perf_data(struct _UMM_TIME_STATS *p, size_t size);

static inline void _critical_entry(time_stat_t *p, uint32_t *saved_ps) {
    *saved_ps = xt_rsil(DEFAULT_CRITICAL_SECTION_INTLEVEL);
    if (0U != (*saved_ps & 0x0FU)) {
        p->intlevel += 1U;
    }

    p->start = esp_get_cycle_count();
}

static inline void _critical_exit(time_stat_t *p, uint32_t *saved_ps) {
    uint32_t elapse = esp_get_cycle_count() - p->start;
    if (elapse < p->min)
        p->min = elapse;

    if (elapse > p->max)
        p->max = elapse;

    xt_wsr_ps(*saved_ps);
}
#endif

#if defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_ISR)
int _isr_safe_printf_P(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#endif

// Use _UMM_MALLOC_CPP to limit access to umm_malloc.cpp module specific
// definitions that are hardware and platform specific.
#ifdef _UMM_MALLOC_CPP

#if defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_ISR)
#define printf(fmt, ...) _isr_safe_printf_P(PSTR(fmt), ##__VA_ARGS__)
#else
// Macro to place constant strings into PROGMEM and print them properly
#define printf(fmt, ...) printf(PSTR(fmt), ## __VA_ARGS__ )
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif  /* _UMM_PERFORMANCE_H */
