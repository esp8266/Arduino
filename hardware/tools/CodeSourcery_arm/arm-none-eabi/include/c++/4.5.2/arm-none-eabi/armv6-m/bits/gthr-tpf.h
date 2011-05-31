/* Threads compatibility routines for libgcc2 and libobjc.
   Compile this one with gcc.
   Copyright (C) 2004, 2005, 2008, 2009 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/* TPF needs its own version of gthr-*.h because TPF always links to
   the thread library.  However, for performance reasons we still do not
   want to issue thread api calls unless a check is made to see that we
   are running as a thread.  */

#ifndef _GLIBCXX_GCC_GTHR_TPF_H
#define _GLIBCXX_GCC_GTHR_TPF_H

/* POSIX threads specific definitions.
   Easy, since the interface is just one-to-one mapping.  */

#define __GTHREADS 1

/* Some implementations of <pthread.h> require this to be defined.  */
#ifndef _REENTRANT
#define _REENTRANT 1
#endif

#include <pthread.h>
#include <unistd.h>

typedef pthread_key_t __gthread_key_t;
typedef pthread_once_t __gthread_once_t;
typedef pthread_mutex_t __gthread_mutex_t;
typedef pthread_mutex_t __gthread_recursive_mutex_t;

#if defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
#define __GTHREAD_RECURSIVE_MUTEX_INIT PTHREAD_RECURSIVE_MUTEX_INITIALIZER
#elif defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)
#define __GTHREAD_RECURSIVE_MUTEX_INIT PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#endif

#define __GTHREAD_MUTEX_INIT PTHREAD_MUTEX_INITIALIZER
#define __GTHREAD_ONCE_INIT PTHREAD_ONCE_INIT
#define __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION __gthread_recursive_mutex_init_function

#define NOTATHREAD   00
#define ECBBASEPTR (unsigned long int) *(unsigned int *)0x00000514u
#define ECBPG2PTR  ECBBASEPTR + 0x1000
#define CE2THRCPTR *((unsigned char *)(ECBPG2PTR + 16))
#define __tpf_pthread_active() (CE2THRCPTR != NOTATHREAD)

#if __GXX_WEAK__ && _GLIBCXX_GTHREAD_USE_WEAK
# define __gthrw(name) \
  static __typeof(name) __gthrw_ ## name __attribute__ ((__weakref__(#name)));
# define __gthrw_(name) __gthrw_ ## name
#else
# define __gthrw(name)
# define __gthrw_(name) name
#endif

__gthrw(pthread_once)
__gthrw(pthread_key_create)
__gthrw(pthread_key_delete)
__gthrw(pthread_getspecific)
__gthrw(pthread_setspecific)
__gthrw(pthread_create)

__gthrw(pthread_mutex_lock)
__gthrw(pthread_mutex_trylock)
__gthrw(pthread_mutex_unlock)
__gthrw(pthread_mutexattr_init)
__gthrw(pthread_mutexattr_settype)
__gthrw(pthread_mutexattr_destroy)
__gthrw(pthread_mutex_init)
__gthrw(pthread_mutex_destroy)

static inline int
__gthread_active_p (void)
{
  return 1;
}

static inline int
__gthread_once (__gthread_once_t *__once, void (*__func) (void))
{
  if (__tpf_pthread_active ())
    return __gthrw_(pthread_once) (__once, __func);
  else
    return -1;
}

static inline int
__gthread_key_create (__gthread_key_t *__key, void (*__dtor) (void *))
{
  if (__tpf_pthread_active ())
    return __gthrw_(pthread_key_create) (__key, __dtor);
  else
    return -1;
}

static inline int
__gthread_key_delete (__gthread_key_t __key)
{
  if (__tpf_pthread_active ())
    return __gthrw_(pthread_key_delete) (__key);
  else
    return -1;
}

static inline void *
__gthread_getspecific (__gthread_key_t __key)
{
  if (__tpf_pthread_active ())
    return __gthrw_(pthread_getspecific) (__key);
  else
    return NULL;
}

static inline int
__gthread_setspecific (__gthread_key_t __key, const void *__ptr)
{
  if (__tpf_pthread_active ())
    return __gthrw_(pthread_setspecific) (__key, __ptr);
  else
    return -1;
}

static inline int
__gthread_mutex_destroy (__gthread_mutex_t *__mutex)
{
  if (__tpf_pthread_active ())
    return __gthrw_(pthread_mutex_destroy) (__mutex);
  else
    return 0;
}

static inline int
__gthread_mutex_lock (__gthread_mutex_t *__mutex)
{
  if (__tpf_pthread_active ())
    return __gthrw_(pthread_mutex_lock) (__mutex);
  else
    return 0;
}

static inline int
__gthread_mutex_trylock (__gthread_mutex_t *__mutex)
{
  if (__tpf_pthread_active ())
    return __gthrw_(pthread_mutex_trylock) (__mutex);
  else
    return 0;
}

static inline int
__gthread_mutex_unlock (__gthread_mutex_t *__mutex)
{
  if (__tpf_pthread_active ())
    return __gthrw_(pthread_mutex_unlock) (__mutex);
  else
    return 0;
}

static inline int
__gthread_recursive_mutex_lock (__gthread_recursive_mutex_t *__mutex)
{
  if (__tpf_pthread_active ())
    return __gthread_mutex_lock (__mutex);
  else
    return 0;
}

static inline int
__gthread_recursive_mutex_trylock (__gthread_recursive_mutex_t *__mutex)
{
  if (__tpf_pthread_active ())
    return __gthread_mutex_trylock (__mutex);
  else
    return 0;
}

static inline int
__gthread_recursive_mutex_unlock (__gthread_recursive_mutex_t *__mutex)
{
  if (__tpf_pthread_active ())
    return __gthread_mutex_unlock (__mutex);
  else
    return 0;
}

static inline int
__gthread_recursive_mutex_init_function (__gthread_recursive_mutex_t *__mutex)
{
  if (__tpf_pthread_active ())
    {
      pthread_mutexattr_t __attr;
      int __r;

      __r = __gthrw_(pthread_mutexattr_init) (&__attr);
      if (!__r)
	__r = __gthrw_(pthread_mutexattr_settype) (&__attr,
						   PTHREAD_MUTEX_RECURSIVE);
      if (!__r)
	__r = __gthrw_(pthread_mutex_init) (__mutex, &__attr);
      if (!__r)
	__r = __gthrw_(pthread_mutexattr_destroy) (&__attr);
      return __r;
    }
  return 0;
}


#endif /* ! _GLIBCXX_GCC_GTHR_TPF_H */
