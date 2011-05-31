/*
	assert.h
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "_ansi.h"

#undef assert

  /* Indicate that we honor AEABI portability if requested.  */
#if defined _AEABI_PORTABILITY_LEVEL && _AEABI_PORTABILITY_LEVEL != 0 && !defined _AEABI_PORTABLE
# define _AEABI_PORTABLE
#endif

#ifdef NDEBUG           /* required by ANSI standard */
# define assert(__e) ((void)0)
#else
# if defined _AEABI_PORTABLE
  void _EXFUN(__aeabi_assert, (const char *, const char *, int)
              _ATTRIBUTE ((__noreturn__)));
#  define assert(__e) ((__e) ? (void)0 : __aeabi_assert (#__e, __FILE__, __LINE__))
# else
#  define assert(__e) ((__e) ? (void)0 : __assert_func (__FILE__, __LINE__, \
						        __ASSERT_FUNC, #__e))
#  ifndef __ASSERT_FUNC
   /* Use g++'s demangled names in C++.  */
#   if defined __cplusplus && defined __GNUC__
#    define __ASSERT_FUNC __PRETTY_FUNCTION__

   /* C99 requires the use of __func__.  */
#   elif __STDC_VERSION__ >= 199901L
#    define __ASSERT_FUNC __func__

   /* Older versions of gcc don't have __func__ but can use __FUNCTION__.  */
#   elif __GNUC__ >= 2
#    define __ASSERT_FUNC __FUNCTION__

   /* failed to detect __func__ support.  */
#   else
#    define __ASSERT_FUNC ((char *) 0)
#   endif /* !__cplusplus && __GNUC__ */
#  endif /* !__ASSERT_FUNC */
# endif /* _AEABI_PORTABLE */
#endif /* !NDEBUG */

void _EXFUN(__assert, (const char *, int, const char *)
	    _ATTRIBUTE ((__noreturn__)));
void _EXFUN(__assert_func, (const char *, int, const char *, const char *)
	    _ATTRIBUTE ((__noreturn__)));

#ifdef __cplusplus
}
#endif
