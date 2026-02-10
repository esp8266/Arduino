These are the warning options for the compiler at different levels.

Because GCC produces code which crashes when a function is declared
to return a value but doesn't (this is undefined per the C++ specs, but legal
for C11 and above code as long as the [non]returned value is ignored), we
cannot warn them if we use "-w" to disable all warnings, and instead have
to delete every warning but "-Wreturn-type"

Generate the C++ variant with the [`make_none-cxxflags.sh`](make_none-cxxflags.sh) script

Modify [`patterns_none-cxxflags.txt`](patterns_none-cxxflags.txt) patterns to ignore incompatible warning types
