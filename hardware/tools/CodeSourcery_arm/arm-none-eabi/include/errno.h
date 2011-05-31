#ifndef __ERRNO_H__
#define __ERRNO_H__

  /* Indicate that we honor AEABI portability if requested.  */
#if defined _AEABI_PORTABILITY_LEVEL && _AEABI_PORTABILITY_LEVEL != 0 && !defined _AEABI_PORTABLE
# define _AEABI_PORTABLE
#endif

#if !defined __error_t_defined && !defined _AEABI_PORTABLE
typedef int error_t;
#define __error_t_defined 1
#endif

#include <sys/errno.h>

#endif /* !__ERRNO_H__ */
