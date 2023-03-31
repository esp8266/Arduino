Improving Exception Decoder Results
===================================


Due to the limited resources on the device, our default compiler optimizations
focus on creating the smallest code size (``.bin`` file). The GCC compiler's
option ``-Os`` contains the base set of optimizations used. This set is fine for
release but not ideal for debugging.

Our view of a crash is often the `Stack Dump <stack_dump.rst>`__
which gets copy/pasted into an Exception Decoder.
For some situations, the optimizer doesn't write caller return addresses to the
stack. When we crash, the list of functions called is missing. And when the
crash occurs in a leaf function, there is seldom if ever any evidence of who
called.

With the ``-Os`` option, functions called once are inlined into the calling
function. A chain of these functions can optimize down to the calling function.
When the crash occurs in one of these chain functions, the actual location in
the source code is no longer available.

When you select ``Debug Optimization: Lite`` on the Arduino IDE Tools menu, it
turns off ``optimize-sibling-calls``. Turning off this optimization allows more
caller addresses to be written to the stack, improving the results from the
Exception Decoder. Without this option, the callers involved in the crash may be
missing from the Decoder results. Because of the limited stack space, there is
the remote possibility that removing this optimization could lead to more
frequent stack overflows. You only want to do this in a debug setting. This
option does not help the chained function issue.

When you select ``Debug Optimization: Optimum``, you get an even more complete
stack trace. For example, chained function calls may show up. This selection
uses the compiler option ``-Og``. GCC considers this the ideal optimization for
the "edit-compile-debug cycle" ... "producing debuggable code." You can read the
specifics at `GCC's Optimize Options <https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html>`__

When global optimization creates build size issues or stack overflow issues,
select ``Debug Optimization: None``, and use a targeted approach with
``#pragma GCC optimize("Og")`` at the module level. Or, if you want to use a
different set of optimizations, you can set optimizations through build options.
Read more at `Global Build Options <../faq/a06-global-build-options.rst>`__.

For non-Arduino IDE build platforms, you may need to research how to add build
options. Some build platforms already use ``-Og`` for debug builds.

A crash in a leaf function may not leave the caller's address on the stack.
The return address can stay in a register for the duration of the call.
Resulting in a crash report identifying the crashing function without a
trace of who called. You can encourage the compiler to save the caller's
return address by adding an inline assembly trick
``__asm__ __volatile__("" ::: "a0", "memory");`` at the beginning of the
function's body. Or instead, for a debug build conditional option, use the
macro ``DEBUG_LEAF_FUNCTION()`` from ``#include <debug.h>``. For compiler
toolchain 3.2.0 and above, the ``-Og`` option is an alternative solution.

In some cases, adding ``#pragma GCC optimize("Og,no-ipa-pure-const")`` to a
module as well as using ``DEBUG_LEAF_FUNCTION()`` in a leaf function were
needed to display a complete call chain. Or use
``#pragma GCC optimize("Os,no-inline,no-optimize-sibling-calls,no-ipa-pure-const")``
if you require optimization ``-Os``.
