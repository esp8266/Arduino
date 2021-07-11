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
    -finstrument-functions-exclude-function-list=app_entry,stack_thunk_get_,ets_intr_,ets_post,Cache_Read_Enable,non32xfer_exception_handler
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

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE inline __attribute__((always_inline))
#endif

extern "C" {

  constexpr size_t kMaxTracks = 64;
  struct WDTrackMark {
    void *this_fn;
    void *call_site;
    void *sp;
    bool enter;
  };
  struct WDTracksCBuf {
    size_t idx;
    struct WDTrackMark tracks[kMaxTracks];
  };
  struct WDTracksCBuf wd_tracks;

  IRAM_ATTR void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));
  IRAM_ATTR void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__((no_instrument_function));
  void print_wdtracks(void) __attribute__((no_instrument_function));

  void custom_crash_callback(struct rst_info * rst_info, uint32_t stack, uint32_t stack_end) __attribute__((no_instrument_function));

  /*
   * Printing from hwdt_pre_sdk_init() requires special considerations. Normal
   * "C" runtime initialization has not run at the time it is called. For
   * printing from this context, we use umm_info_safe_printf_P. It is capable
   * of handling this situation and is a function included in umm_malloc.
   */
  int umm_info_safe_printf_P(const char *fmt, ...);
#define ETS_PRINTF(fmt, ...) umm_info_safe_printf_P(PSTR(fmt), ##__VA_ARGS__)
#define ETS_PRINTF_P(fmt, ...) umm_info_safe_printf_P(fmt, ##__VA_ARGS__)

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
  // Functions __cyg_profile_func_enter and __cyg_profile_func_exit are
  // identical except for the "bool enter" value. These are a very high call
  // frequency functions. Avoid using a common function to save code space. By
  // forceing inline, we avoid the time to creating a new stack frame and
  // register save event to make the call.
  // Side note, needed to keep __attribute__((no_instrument_function)); on
  // forced inline parts otherwise the resulting ASM was strange and larger.
  static ALWAYS_INLINE void __cyg_profile_func(void *this_fn, void *call_site, bool enter) __attribute__((no_instrument_function));
  void __cyg_profile_func(void *this_fn, void *call_site, bool enter)
  {
    uint32_t saved_ps = xt_rsil(15);

    size_t idx = wd_tracks.idx;
    // Not using the .noinit attribute on structure at this time; however,
    // doing range check here makes an uninitialized structure safe!
    if (kMaxTracks <= idx) {
      idx = 0;
    }

    struct WDTrackMark *track = &wd_tracks.tracks[idx];
    asm volatile (
      "s32i a1, %[track], %[track_sp]\n\t"
      : [track]"+a"(track)
      : [track_sp]"I"(offsetof(struct WDTrackMark, sp))
      :);
    // Extended ASM note, If "track" is an input register the compiler assumes
    // it has been overwriten by the ASM code and recalculates its prior value.
    // As an output read/write register the compiler skips the recalculation.
    // This saved two bytes.
    track->this_fn = this_fn;       // The address of the function called
    track->call_site = call_site;   // The saved return address "a0" of the callee
    track->enter = enter;
    wd_tracks.idx = ++idx;

    xt_wsr_ps(saved_ps);
  }

  /*
    Called at function entry after stackframe setup and registers are saved.
  */
  void __cyg_profile_func_enter(void *this_fn, void *call_site)
  {
    __cyg_profile_func(this_fn, call_site, true);
  }


  /*
    Called at function exit just before saved registers and stackframe are
    restored.
  */
  void __cyg_profile_func_exit(void *this_fn, void *call_site)
  {
    __cyg_profile_func(this_fn, call_site, false);
  }

  void print_wdtracks(void)
  {
    // It is assumed we are called from a context that interrupts will not occur.

    // Print as a fake stack dump with our WDTrack info so that the
    // Exception decoder  will assit with a call trace printout.
    size_t idx = wd_tracks.idx;
    idx--;
    if (kMaxTracks <= idx) {
      idx = kMaxTracks - 1;
    }
    struct WDTrackMark *track = &wd_tracks.tracks[idx];
    ETS_PRINTF("\nepc1=%p\n", track->this_fn);

    uintptr_t fake_start =  (uintptr_t)track->sp - 0x10;
    uintptr_t fake_end   =  fake_start + (uintptr_t)(kMaxTracks * sizeof(struct WDTrackMark));
    ETS_PRINTF("\n>>>stack>>>\n\nctx: WDTracks\n");
    ETS_PRINTF("sp: %08x end: %08x offset: %04x\n", fake_start, fake_end, 0);

    // Print most recent first to stay consistent with how the stack dump/decode
    // would normally appear.
    for (size_t i = 0; i < kMaxTracks; i++) {
      ETS_PRINTF("%08x:  %08x %08x %08x %08x\n", fake_start + i * 0x10u,
                 track->this_fn, track->enter, track->sp, track->call_site);
      idx--;
      if (kMaxTracks <= idx) {
        idx = kMaxTracks - 1;
      }
      track = &wd_tracks.tracks[idx];
    }

    ETS_PRINTF("<<<stack<<<\n");
    ETS_PRINTF("\n--------- OR FOR WDT, CUT HERE FOR EXCEPTION DECODER ---------\n");
  }
};
