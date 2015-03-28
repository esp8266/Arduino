// class template regex -*- C++ -*-

// Copyright (C) 2010-2013 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/**
 *  @file bits/regex_cursor.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{regex}
 */

namespace std _GLIBCXX_VISIBILITY(default)
{
namespace __detail
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   *  @defgroup regex-detail Base and Implementation Classes
   *  @ingroup regex
   *  @{
   */

  /// ABC for pattern matching
  struct _PatternCursor
  {
    virtual ~_PatternCursor() { };
    virtual void _M_next() = 0;
    virtual bool _M_at_end() const = 0;
  };

  /// Provides a cursor into the specific target string.
  template<typename _FwdIterT>
    class _SpecializedCursor
    : public _PatternCursor
    {
    public:
      _SpecializedCursor(const _FwdIterT& __b, const _FwdIterT __e)
      : _M_b(__b), _M_c(__b), _M_e(__e)
      { }

      typename std::iterator_traits<_FwdIterT>::value_type
      _M_current() const
      { return *_M_c; }

      void
      _M_next()
      { ++_M_c; }

      _FwdIterT
      _M_pos() const
      { return _M_c; }

      const _FwdIterT&
      _M_begin() const
      { return _M_b; }

      const _FwdIterT&
      _M_end() const
      { return _M_e; }

      bool
      _M_at_end() const
      { return _M_c == _M_e; }

    private:
      _FwdIterT _M_b;
      _FwdIterT _M_c;
      _FwdIterT _M_e;
    };

  // Helper function to create a cursor specialized for an iterator class.
  template<typename _FwdIterT>
    inline _SpecializedCursor<_FwdIterT>
    __cursor(const _FwdIterT& __b, const _FwdIterT __e)
    { return _SpecializedCursor<_FwdIterT>(__b, __e); }

 //@} regex-detail
_GLIBCXX_END_NAMESPACE_VERSION
} // namespace __detail
} // namespace
