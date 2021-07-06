/*
  For details about the GCC command line option "-finstrument-functions" see
  https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html

  This option allows us to track what our sketch was last doing before a crash
  with Hard or Soft WDT. To do this we define functions that get called for each
  function entry and exit.

  The overhead is very high with this option. So we do not want it applied
  everywhere. At the same time if we limit the coverage too much, we may miss
  the event that caused the WDT.

  To a build_opt.txt file in your sketch directory add the following build options:
    -finstrument-functions
    -finstrument-functions-exclude-function-list=app_entry,stack_thunk_get_,ets_intr_,ets_post,Cache_Read_Enable
    -finstrument-functions-exclude-file-list=umm_malloc,hwdt_app_entry,core_esp8266_postmortem,core_esp8266_app_entry_noextra4k

  Various efforts are under devlopment to handle build options for a sketch.
  For now to use your build_opt.txt file, create a platform.local.txt file with
  these lines:
    custom.flags_txt.source.pathname={build.source.path}/build_opt.txt
    custom.flags_txt.build.pathname={build.path}/build_opt.txt
    compiler.c.extra_flags=-include "{custom.flags_txt.build.pathname}.h" @{custom.flags_txt.build.pathname}

    recipe.hooks.prebuild.1.pattern=mkdir -p "{build.path}/custom"
    recipe.hooks.prebuild.2.pattern=bash -c "if [   -f {custom.flags_txt.source.pathname}  ]; then if [ {custom.flags_txt.source.pathname} -nt {custom.flags_txt.build.pathname} ]; then cp -u {custom.flags_txt.source.pathname} {custom.flags_txt.build.pathname}; touch {custom.flags_txt.build.pathname}.h; fi; fi;"
    recipe.hooks.prebuild.3.pattern=bash -c "if [ ! -f {custom.flags_txt.source.pathname}  ]; then if [ -s {custom.flags_txt.build.pathname} ]; then rm {custom.flags_txt.build.pathname}; touch {custom.flags_txt.build.pathname} {custom.flags_txt.build.pathname}.h; fi; fi;"
    recipe.hooks.prebuild.4.pattern=bash -c "if [ ! -f {custom.flags_txt.build.pathname}   ]; then touch -t 170001010000 {custom.flags_txt.build.pathname}; fi;"
    recipe.hooks.prebuild.5.pattern=bash -c "if [ ! -f {custom.flags_txt.build.pathname}.h ]; then touch -t 170001010000 {custom.flags_txt.build.pathname}.h; fi;"

*/
#include <Arduino.h>
#include <hwdt_app_entry.h>
#include <user_interface.h>

extern "C" {

  struct WDTracksLastCall {
    struct {
      void *this_fn;
      void *call_site;
      void *sp;
    } enter;

    struct {
      void *this_fn;
      void *call_site;
      void *sp;
    } exit;

    bool in;
  };
  struct WDTracksLastCall wd_tracks;

  IRAM_ATTR void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));
  IRAM_ATTR void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__((no_instrument_function));
  void print_wdtracks(void) __attribute__((no_instrument_function));

  void custom_crash_callback(struct rst_info * rst_info, uint32_t stack, uint32_t stack_end) __attribute__((no_instrument_function));

  /*
   * Printing from hwdt_pre_sdk_init() requires special considerations. Normal
   * "C" runtime initialization has not run at the time it is called. For
   * printing from this context, we use umm_info_safe_printf_P. It is capable of
   * handling this situation and is a function included in umm_malloc.
   */
  int umm_info_safe_printf_P(const char *fmt, ...);
  #define ETS_PRINTF(fmt, ...) umm_info_safe_printf_P(PSTR(fmt), ##__VA_ARGS__)

#if defined(DEBUG_ESP_HWDT) || defined(DEBUG_ESP_HWDT_NOEXTRA4K)
  void hwdt_pre_sdk_init(void) __attribute__((no_instrument_function));

  // Called from HWDT just before starting SDK
  // Serial speed has been initialized for us.
  void hwdt_pre_sdk_init(void) {
    if (REASON_WDT_RST == hwdt_info.reset_reason) {
      /*
        Note, we rely on the previous values of the wd_tracks structure, still
        being set from before the crash. At the time we are called here, the SDK
        has not been started. The "C" runtime code that will zero the structure
        does not run until later when the SDK calls user_init().
      */
      print_wdtracks();
    }
  }
#endif

  // Called from Postmortem
  void custom_crash_callback(struct rst_info * rst_info, uint32_t stack, uint32_t stack_end)
  {
    (void)stack;
    (void)stack_end;
    if (REASON_SOFT_WDT_RST == rst_info->reason) {
      print_wdtracks();
    }
  }


  /*
    Called at function entry after stackframe setup and registers are saved.
  */
  void __cyg_profile_func_enter(void *this_fn, void *call_site)
  {
    wd_tracks.enter.this_fn = this_fn;       // The address of the function called
    wd_tracks.enter.call_site = call_site;   // The saved return address "a0" of the callee
    asm volatile (
        "s32i a1, %[wd_tracks], %[enter_sp]\n\t"
        : // No output constraints
        : [wd_tracks]"a"(&wd_tracks), [enter_sp]"I"(offsetof(struct WDTracksLastCall, enter.sp))
        :);
    wd_tracks.in = true;
  }


  /*
    Called at function exit just before saved registers and stackframe are
    restored.
  */
  void __cyg_profile_func_exit(void *this_fn, void *call_site)
  {
    wd_tracks.exit.this_fn = this_fn;      // The address of the function called
    wd_tracks.exit.call_site = call_site;  // The saved return address "a0" of the callee
    asm volatile (
        "s32i a1, %[wd_tracks], %[exit_sp]\n\t"
        : // No output constraints
        : [wd_tracks]"a"(&wd_tracks), [exit_sp]"I"(offsetof(struct WDTracksLastCall, exit.sp))
        :);
    wd_tracks.in = false;
  }

  void print_wdtracks(void)
  {
      if (NULL == wd_tracks.enter.this_fn)  {
        // Compiler flags -finstrument-functions, etc. are most likely missing.
        return;
      }
      if (wd_tracks.in) {
        ETS_PRINTF("\nepc1=%p\nepc1=%p\n",
                   wd_tracks.enter.this_fn, wd_tracks.enter.call_site);

        if (wd_tracks.exit.this_fn != wd_tracks.enter.this_fn &&
            wd_tracks.exit.this_fn != wd_tracks.enter.call_site)
          ETS_PRINTF("epc1=%p\n", wd_tracks.exit.this_fn);
        if (wd_tracks.exit.call_site != wd_tracks.enter.this_fn &&
            wd_tracks.exit.call_site != wd_tracks.enter.call_site)
          ETS_PRINTF("epc1=%p\n",  wd_tracks.exit.call_site);

      } else {
        ETS_PRINTF("\nepc1=%p\nepc1=%p\n",
                   wd_tracks.exit.this_fn, wd_tracks.exit.call_site);

        if (wd_tracks.enter.this_fn != wd_tracks.exit.this_fn &&
            wd_tracks.enter.this_fn != wd_tracks.exit.call_site)
          ETS_PRINTF("epc1=%p\n", wd_tracks.enter.this_fn);
        if (wd_tracks.enter.call_site != wd_tracks.exit.this_fn &&
            wd_tracks.enter.call_site != wd_tracks.exit.call_site)
          ETS_PRINTF("epc1=%p\n",  wd_tracks.enter.call_site);
      }
      ETS_PRINTF("\n--------- OR FOR WDT, CUT HERE FOR EXCEPTION DECODER ---------\n");

      if (wd_tracks.in) {
        ETS_PRINTF("\nLast function entered:\n"
                   "  called function: %p\n"
                   "  callee return:   %p\n"
                   "  SP:              %p\n",
                   wd_tracks.enter.this_fn, wd_tracks.enter.call_site, wd_tracks.enter.sp);
      } else {
        ETS_PRINTF("\nLast function exited:\n"
                   "  called function: %p\n"
                   "  callee return:   %p\n"
                   "  SP:              %p\n",
                   wd_tracks.exit.this_fn, wd_tracks.exit.call_site, wd_tracks.exit.sp);
      }
  }
};
