# WIP

# Missing Dependency
A general way of specifying build options for a sketch. There are other active PRs pursuing this effort. This PR needs to be held back and updated after one of those is finalized. For testing this PR, a limited method for the Arduino IDE/Linux OS environment is provided.

# Watch Dog Tracks
## An Overview
A problem with both Hardware WDT and Soft WDT resets, there is no indication of where the processor is stuck.

This solution uses the [GCC Program Instrumentation Options'](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html) "instrument-functions" to call our tracking functions to capture the most recent functions called. It stores the last caller, callee, and Stack Pointer at each function entry and exit.

Files needed:
* `build_opt.txt` - compiler "instrument-functions" options file in the sketch folder
* `platform.local.txt` - (temporary) causes the compiler to use the sketch's `build_opt.txt` file
* `WDTracks.cpp` - Add to the sketch folder. Handles tracking and printing/reporting.

## Implementation
The overhead is very high with this option. The functions `__cyg_profile_func_enter` and `__cyg_profile_func_exit` are used to track execution. Every function entry and exit is instrumented by the compiler with a call to the respective tracking function. This increases code size and execution time. To reduce overhead, there are three ways to specify the code to exclude from the instrumentation: build options `-finstrument-functions-exclude-function-list=...` and `-finstrument-functions-exclude-file-list=...`; and function attribute `__attribute__((no_instrument_function))`. Note, if we limit the coverage too much, we may miss the event that caused the WDT. For an idea of sketch code size increase, a sketch with a code size of 520973 bytes, grew to 803566 bytes when recompiled with the instrument-functions options shown below.  

Both `-finstrument-functions-exclude-function-list=...` and `-finstrument-functions-exclude-file-list=...` exclude when an item in the list matches as a substring in the actual symbol name or file name, respectively. This feature implementation can easily capture and exclude more functions than you intended. A challenge with this approach is the overhead and tuning the excludes, to reduce overhead without losing too many of the activity tracking calls. Use and modify with caution.

For reporting, we have two times that we can expand on the WDT reset reporting.
1) Hardware WDT - For this HWDT or HWDT_NO4KEXTRA must be enabled from the "Arduino IDE Tools->Debug Level". The newly added tracking results are printed by callback function `hwdt_pre_sdk_init`. This call is made early during the startup process before any substantial initialization occurs that would overwrite interesting information. Care still has to be taken to exclude functions from calling "instrument-functions" at startup leading up to the call to `hwdt_pre_sdk_init`. Otherwise, our tacking information will be lost.
2) Soft WDT - this occurs during postmortem report when `custom_crash_callback` is called. If you already have a `custom_crash_callback` you will need to disable it for this debugging session.

Both of these callback functions are predefined as _weak_ allowing for simple override replacements without the need for registration calls.

After a WDT reset, two or more lines starting with "epc1=0x40..." will appear after the stack dump. When you copy-paste this block into the Exception Decoder, it will only process the first one in the list. The Exception Decoder result of the first "epc1=0x40..." will show as the 1st line of the decode starting with "PC: 0x40...". If the results are not meaningful, you can get the decoder to use the next line by deleting the first "epc1=0x40..." line and press return. The decode results will update. Repeat the process as needed.

## Files Needed
### `build_opt.txt`
Minimum suggested contents for file, `build_opt.txt`, (build options) to use the "-finstrument-functions" option.
```
-finstrument-functions
-finstrument-functions-exclude-function-list=app_entry,stack_thunk_get_,ets_intr_,ets_post,Cache_Read_Enable
-finstrument-functions-exclude-file-list=umm_malloc,hwdt_app_entry,core_esp8266_postmortem,core_esp8266_app_entry_noextra4k

```
Additional exclusions may be needed when using functions that have critical code timing loops, like I<sup>2</sup>C or high-priority interrupt routines, etc.



### `platform.local.txt`
Various efforts are already under development for specifying build options for a sketch in an IDE and OS flexible manner.
For now, under Arduino IDE and Linux OS, I have been using a `platform.local.txt` file with these lines to utilize my sketch `build_opt.txt` file.  
```
custom.flags_txt.source.pathname={build.source.path}/build_opt.txt
custom.flags_txt.build.pathname={build.path}/build_opt.txt
compiler.c.extra_flags=-include "{custom.flags_txt.build.pathname}.h" @{custom.flags_txt.build.pathname}
compiler.cpp.extra_flags=-include "{custom.flags_txt.build.pathname}.h" @{custom.flags_txt.build.pathname}

recipe.hooks.prebuild.1.pattern=mkdir -p "{build.path}/custom"
recipe.hooks.prebuild.2.pattern=bash -c "if [   -f {custom.flags_txt.source.pathname}  ]; then if [ {custom.flags_txt.source.pathname} -nt {custom.flags_txt.build.pathname} ]; then cp -u {custom.flags_txt.source.pathname} {custom.flags_txt.build.pathname}; touch {custom.flags_txt.build.pathname}.h; fi; fi;"
recipe.hooks.prebuild.3.pattern=bash -c "if [ ! -f {custom.flags_txt.source.pathname}  ]; then if [ -s {custom.flags_txt.build.pathname} ]; then rm {custom.flags_txt.build.pathname}; touch {custom.flags_txt.build.pathname} {custom.flags_txt.build.pathname}.h; fi; fi;"
recipe.hooks.prebuild.4.pattern=bash -c "if [ ! -f {custom.flags_txt.build.pathname}   ]; then touch -t 170001010000 {custom.flags_txt.build.pathname}; fi;"
recipe.hooks.prebuild.5.pattern=bash -c "if [ ! -f {custom.flags_txt.build.pathname}.h ]; then touch -t 170001010000 {custom.flags_txt.build.pathname}.h; fi;"
```

### `WDTracks.cpp`
Copy `WDTracks.cpp` from this example to your sketch folder.
