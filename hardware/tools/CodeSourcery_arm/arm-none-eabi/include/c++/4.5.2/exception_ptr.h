// Exception Handling support header (exception_ptr class) for -*- C++ -*-

// Copyright (C) 2008, 2009 Free Software Foundation
//
// This file is part of GCC.
//
// GCC is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
// 
// GCC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file exception_ptr.h
 *  This is an internal header file, included by other headers and the
 *  implementation. You should not attempt to use it directly.
 */

#ifndef _EXCEPTION_PTR_H
#define _EXCEPTION_PTR_H

#pragma GCC visibility push(default)

#include <bits/c++config.h>
#include <exception_defines.h>

#if !defined(_GLIBCXX_ATOMIC_BUILTINS_4)
#  error This platform does not support exception propagation.
#endif

extern "C++" {

namespace std 
{
  /**
   * @addtogroup exceptions
   * @{
   */
  namespace __exception_ptr
  {
    class exception_ptr;
  }

  using __exception_ptr::exception_ptr;

  /** Obtain an exception_ptr to the currently handled exception. If there
   *  is none, or the currently handled exception is foreign, return the null
   *  value.
   */
  exception_ptr current_exception() throw();

  /// Throw the object pointed to by the exception_ptr.
  void rethrow_exception(exception_ptr) __attribute__ ((__noreturn__));

  namespace __exception_ptr
  {
    /**
     *  @brief An opaque pointer to an arbitrary exception.
     *  @ingroup exceptions
     */
    class exception_ptr
    {
      void* _M_exception_object;

      explicit exception_ptr(void* __e) throw();

      void _M_addref() throw();
      void _M_release() throw();

      void *_M_get() const throw() __attribute__ ((__pure__));

      void _M_safe_bool_dummy() throw() __attribute__ ((__const__));

      friend exception_ptr std::current_exception() throw();
      friend void std::rethrow_exception(exception_ptr);

    public:
      exception_ptr() throw();

      typedef void (exception_ptr::*__safe_bool)();

      // For construction from nullptr or 0.
      exception_ptr(__safe_bool) throw();

      exception_ptr(const exception_ptr&) throw();

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      exception_ptr(exception_ptr&& __o) throw()
      : _M_exception_object(__o._M_exception_object)
      { __o._M_exception_object = 0; }
#endif

      exception_ptr& 
      operator=(const exception_ptr&) throw();

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      exception_ptr& 
      operator=(exception_ptr&& __o) throw()
      {
        exception_ptr(static_cast<exception_ptr&&>(__o)).swap(*this);
        return *this;
      }
#endif

      ~exception_ptr() throw();

      void 
      swap(exception_ptr&) throw();

#ifdef _GLIBCXX_EH_PTR_COMPAT
      // Retained for compatibility with CXXABI_1.3.
      bool operator!() const throw() __attribute__ ((__pure__));
      operator __safe_bool() const throw();
#endif

      friend bool 
      operator==(const exception_ptr&, const exception_ptr&) throw() 
      __attribute__ ((__pure__));

      const type_info*
      __cxa_exception_type() const throw() __attribute__ ((__pure__));
    };

    bool 
    operator==(const exception_ptr&, const exception_ptr&) throw() 
    __attribute__ ((__pure__));

    bool 
    operator!=(const exception_ptr&, const exception_ptr&) throw() 
    __attribute__ ((__pure__));
  } // namespace __exception_ptr


  /// Obtain an exception_ptr pointing to a copy of the supplied object.
  template<typename _Ex>
    exception_ptr 
    copy_exception(_Ex __ex) throw()
    {
      __try
	{
#ifdef __EXCEPTIONS
	  throw __ex;
#endif
	}
      __catch(...)
	{
	  return current_exception();
	}
    }

  // @} group exceptions
} // namespace std

} // extern "C++"

#pragma GCC visibility pop

#endif
