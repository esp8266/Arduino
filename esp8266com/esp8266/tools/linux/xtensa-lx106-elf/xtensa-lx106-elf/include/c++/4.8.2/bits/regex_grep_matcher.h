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
 *  @file bits/regex_grep_matcher.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{regex}
 */

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<typename _BiIter>
    class sub_match;

  template<typename _Bi_iter, typename _Allocator>
    class match_results;

_GLIBCXX_END_NAMESPACE_VERSION
  
namespace __detail
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   *  @defgroup regex-detail Base and Implementation Classes
   *  @ingroup regex
   *  @{
   */

  /// A _Results facade specialized for wrapping a templated match_results.
  template<typename _FwdIterT, typename _Alloc>
    class _SpecializedResults
    : public _Results
    {
    public:
      _SpecializedResults(const _Automaton::_SizeT __size,
			  const _SpecializedCursor<_FwdIterT>& __cursor,
			  match_results<_FwdIterT, _Alloc>& __m);

      void
      _M_set_pos(int __i, int __j, const _PatternCursor& __pc);

      void
      _M_set_matched(int __i, bool __is_matched)
      { _M_results.at(__i).matched = __is_matched; }

    private:
      match_results<_FwdIterT, _Alloc>& _M_results;
    };

  template<typename _FwdIterT, typename _Alloc>
    _SpecializedResults<_FwdIterT, _Alloc>::
    _SpecializedResults(const _Automaton::_SizeT __size,
    			const _SpecializedCursor<_FwdIterT>& __cursor,
                        match_results<_FwdIterT, _Alloc>& __m)
    : _M_results(__m)
    {
      _M_results.clear();
      _M_results.reserve(__size + 2);
      _M_results.resize(__size);
      typename match_results<_FwdIterT, _Alloc>::value_type __sm;
      __sm.first = __sm.second = __cursor._M_begin();
      _M_results.push_back(__sm);
      __sm.first = __sm.second = __cursor._M_end();
      _M_results.push_back(__sm);
    }

  template<typename _FwdIterT, typename _Alloc>
    void
    _SpecializedResults<_FwdIterT, _Alloc>::
    _M_set_pos(int __i, int __j, const _PatternCursor& __pc)
    { 
      typedef const _SpecializedCursor<_FwdIterT>& _CursorT;
      _CursorT __c = static_cast<_CursorT>(__pc);
      if (__j == 0)
	_M_results.at(__i).first = __c._M_pos();
      else
	_M_results.at(__i).second = __c._M_pos()+1;
    }

  /// A stack of states used in evaluating the NFA.
  typedef std::stack<_StateIdT, std::vector<_StateIdT> > _StateStack;

  /// Executes a regular expression NFA/DFA over a range using a
  /// variant of the parallel execution algorithm featured in the grep
  /// utility, modified to use Laurikari tags.
  class _Grep_matcher
  {
  public:
    _Grep_matcher(_PatternCursor&                   __p,
		  _Results&                         __r,
		  const _AutomatonPtr&              __automaton,
		  regex_constants::match_flag_type  __flags);

  private:
    _StateSet
    _M_e_closure(_StateIdT __i);

    _StateSet
    _M_e_closure(const _StateSet& __s);

    _StateSet
    _M_e_closure(_StateStack& __stack, const _StateSet& __s);

    const std::shared_ptr<_Nfa>        _M_nfa;
    _PatternCursor&                    _M_pattern;
    _Results&                          _M_results;
  };

 //@} regex-detail
_GLIBCXX_END_NAMESPACE_VERSION
} // namespace __detail
} // namespace std

#include <bits/regex_grep_matcher.tcc>
