/*
    Review "Inventory of debug options ..." in core/esp8266/heap.cpp
*/

/*@create-file:build.opt@
// -DDEBUG_ESP_OOM=1
// -DDEBUG_ESP_WITHINISR=1
// -DUMM_POISON_CHECK_LITE=1
// -DUMM_INTEGRITY_CHECK=1
// -DUMM_POISON_CHECK=1
// -DUMM_POISON_NONE=1
//
-DUMM_INFO=1
-DUMM_STATS_FULL=1


// C++17 feature for "new" operator provide aligned allocations
// These build options are required for this test program.
-DUMM_ENABLE_MEMALIGN=1
-DDEV_DEBUG_ABI_CPP=1
// -DMIN_ESP_OOM=1
// Use Arduino IDE "Tools->C++ Exceptions" to enable disable C++ exceptions.


// cc1: warning: command-line option '-faligned-new=4' is valid for C++/ObjC++ but not for C
// -faligned-new=4
*/

// @create-file:build.opt:debug@
