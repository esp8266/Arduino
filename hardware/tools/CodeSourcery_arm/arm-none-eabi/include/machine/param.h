/* ARM configuration file; HZ is 100 rather than the default 60 */

#ifndef _MACHINE_PARAM_H
# define _MACHINE_PARAM_H

# define HZ (100)

#ifdef __ARMEB__
#define BYTE_ORDER BIG_ENDIAN
#else
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#endif
