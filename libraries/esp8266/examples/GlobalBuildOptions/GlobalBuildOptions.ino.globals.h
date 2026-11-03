/*@create-file:build.opt@
  // An embedded build.opt file using a "C" block comment. The starting signature
  // must be on a line by itself. The closing block comment pattern should be on a
  // line by itself. Each line within the block comment will be space trimmed and
  // written to build.opt, skipping blank lines and lines starting with '//', '*'
  // or '#'.
  -DMYTITLE1="\"Running on \""
 * this line is ignored
  *@create-file:build.opt@
  # this line is ignored
*/

/*@create-file:build.opt:debug@
 // Another embedded build.opt, with a different set of flags
  -DMYTITLE1="\"Debugging on \""
  -Og
  -DUMM_STATS_FULL=1
  // -fanalyzer
*/

// Following *raw* contents are also included
#ifndef GLOBALBUILDOPTIONS_INO_GLOBALS_H
#define GLOBALBUILDOPTIONS_INO_GLOBALS_H

#if !defined(__ASSEMBLER__)
// Defines kept away from assembler modules
// i.e. Defines for .cpp, .ino, .c ... modules
#endif

#if defined(__cplusplus)
// Defines kept private to .cpp and .ino modules
//#pragma message("__cplusplus has been seen")
#define MYTITLE2 "Empty"
#endif

#if !defined(__cplusplus) && !defined(__ASSEMBLER__)
// Defines kept private to .c modules
#define MYTITLE2 "~Full"
#endif

#if defined(__ASSEMBLER__)
// Defines kept private to assembler modules
#endif

#endif
