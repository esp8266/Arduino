/*@create-file:build.opt:debug@
// For this block to work, you must have
// `mkbuildoptglobals.extra_flags={build.debug_port}` in `platform.local.txt`
// Or move contents to the block with the signature "@create-file:build.opt@"


// Removing the optimization for "sibling and tail recursive calls"  will clear
// up some gaps in the stack decoder report. Preserves stack frames created at
// each level as you call down to the next.
-fno-optimize-sibling-calls


// Adds a pointer toward the end of the stack frame that points to the beginning
// of the stack frame. The stack dump will annotate the line where it occurs
// with a `<` mark.
-fno-omit-frame-pointer


// Options for HWDT Stack Dump (hwdt_app_entry.cpp)

// Alter the UART serial speed used for printing the Hardware WDT reset stack
// dump. Without this option on an HWDT reset, the existing default speed of
// 115200 bps will be used. If you are using this default speed, you can skip
// this option. Note this option only changes the speed while the stack dump is
// printing. Prior settings are restored.
// -DDEBUG_ESP_HWDT_UART_SPEED=19200
// -DDEBUG_ESP_HWDT_UART_SPEED=74880
// -DDEBUG_ESP_HWDT_UART_SPEED=115200
// -DDEBUG_ESP_HWDT_UART_SPEED=230400

// HWDT Stack Dump defaults to print a simple introduction to let you know the
// tool is active and in the build. At power-on, this may not be viewable on
// some devices. Use the DEBUG_ESP_HWDT_UART_SPEED option above to improve.
// Or uncomment line below to turn off greeting
// -DDEBUG_ESP_HWDT_PRINT_GREETING=0

// Demos
-DDEMO_THUNK=1
// -DDEMO_NOEXTRA4K=1
-DDEMO_WIFI=1
*/

/*@create-file:build.opt@
// -fno-optimize-sibling-calls
// -fno-omit-frame-pointer

// Demos
-DDEMO_THUNK=1
// -DDEMO_NOEXTRA4K=1
-DDEMO_WIFI=1
*/
