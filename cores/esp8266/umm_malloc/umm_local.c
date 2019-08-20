/*
 * Local Additions/Enhancements
 *
 */
#if defined(UMM_CRITICAL_METRICS)
/*
 * umm_malloc performance measurments for critical sections
 */
UMM_TIME_STATS time_stats = {
    {0xFFFFFFFF, 0U, 0U, 0U},
    {0xFFFFFFFF, 0U, 0U, 0U},
    {0xFFFFFFFF, 0U, 0U, 0U},
#ifdef UMM_INFO
    {0xFFFFFFFF, 0U, 0U, 0U},
#endif
#ifdef UMM_POISON_CHECK
    {0xFFFFFFFF, 0U, 0U, 0U},
#endif
#ifdef UMM_INTEGRITY_CHECK
    {0xFFFFFFFF, 0U, 0U, 0U},
#endif
    {0xFFFFFFFF, 0U, 0U, 0U} };

bool ICACHE_FLASH_ATTR get_umm_get_perf_data(UMM_TIME_STATS *p, size_t size)
{
    UMM_CRITICAL_DECL(id_no_tag, true);
    if (p && sizeof(time_stats) == size)
    {
        UMM_CRITICAL_ENTRY(id_no_tag);
        memcpy(p, &time_stats, size);
        UMM_CRITICAL_EXIT(id_no_tag);
        return true;
    }
    return false;
}
#endif

#if defined(UMM_POISON_CHECK)
// Alternate functions from upstream
int umm_poison_check_result;

/* ------------------------------------------------------------------------ */

void *umm_poison_realloc_fl(void *ptr, size_t size, const char* file, int line)
{
    void *ret;

    umm_poison_check_result = 1;
    ptr = get_unpoisoned(ptr);
    if (!umm_poison_check_result)
    {
        if (file)
            __panic_func(file, line, "");
        else
            abort();
    }

    size += poison_size(size);
    ret = umm_realloc(ptr, size);

    ret = get_poisoned(ret, size);

    return ret;
}

/* ------------------------------------------------------------------------ */

void umm_poison_free_fl(void *ptr, const char* file, int line)
{
    umm_poison_check_result = 1;
    ptr = get_unpoisoned(ptr);
    if (!umm_poison_check_result)
    {
        if (file)
            __panic_func(file, line, "");
        else
            abort();
    }

    umm_free(ptr);
}
#endif

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
size_t umm_free_heap_size( void ) __attribute__ ((alias("umm_free_heap_size_lw")));
size_t xPortGetFreeHeapSize(void) __attribute__ ((alias("umm_free_heap_size_lw")));
#elif defined(UMM_INFO)
size_t umm_free_heap_size( void ) __attribute__ ((alias("umm_free_heap_size_info")));
size_t xPortGetFreeHeapSize(void) __attribute__ ((alias("umm_free_heap_size_info")));
#endif
