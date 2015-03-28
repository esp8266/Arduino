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
 *  @file bits/regex_grep_matcher.tcc
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{regex}
 */

#include <regex>

namespace std _GLIBCXX_VISIBILITY(default)
{
namespace
{
  // A stack of states used in evaluating the NFA.
  typedef std::stack<std::__detail::_StateIdT,
                     std::vector<std::__detail::_StateIdT>
		     > _StateStack;

  // Obtains the next state set given the current state set __s and the current
  // input character.
  inline std::__detail::_StateSet
  __move(const std::__detail::_PatternCursor& __p,
         const std::__detail::_Nfa& __nfa,
         const std::__detail::_StateSet& __s)
  {
    std::__detail::_StateSet __m;
    for (std::__detail::_StateSet::const_iterator __i = __s.begin();
	 __i != __s.end(); ++__i)
      {
	if (*__i == std::__detail::_S_invalid_state_id)
	  continue;

	const std::__detail::_State& __state = __nfa[*__i];
	if (__state._M_opcode == std::__detail::_S_opcode_match
	    && __state._M_matches(__p))
	  __m.insert(__state._M_next);
      }
    return __m;
  }

  // returns true if (__s intersect __t) is not empty
  inline bool
  __includes_some(const std::__detail::_StateSet& __s,
                  const std::__detail::_StateSet& __t)
  {
    if (__s.size() > 0 && __t.size() > 0)
      {
	std::__detail::_StateSet::const_iterator __first = __s.begin();
	std::__detail::_StateSet::const_iterator __second = __t.begin();
	while (__first != __s.end() && __second != __t.end())
	  {
	    if (*__first < *__second)
	      ++__first;
	    else if (*__second < *__first)
	      ++__second;
	    else
	      return true;
	  }
      }
    return false;
  }

  // If an identified state __u is not already in the current state set __e,
  // insert it and push it on the current state stack __s.
  inline void
  __add_visited_state(const std::__detail::_StateIdT __u,
                      _StateStack&                  __s,
                      std::__detail::_StateSet&      __e)
  {
    if (__e.count(__u) == 0)
      {
	__e.insert(__u);
	__s.push(__u);
      }
  }

} // anonymous namespace

namespace __detail
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  inline _Grep_matcher::
  _Grep_matcher(_PatternCursor& __p, _Results& __r,
		const _AutomatonPtr& __nfa,
		regex_constants::match_flag_type __flags)
  : _M_nfa(static_pointer_cast<_Nfa>(__nfa)), _M_pattern(__p), _M_results(__r)
  {
    __detail::_StateSet __t = this->_M_e_closure(_M_nfa->_M_start());
    for (; !_M_pattern._M_at_end(); _M_pattern._M_next())
      __t = this->_M_e_closure(__move(_M_pattern, *_M_nfa, __t));

    _M_results._M_set_matched(0,
                              __includes_some(_M_nfa->_M_final_states(), __t));
  }

  // Creates the e-closure set for the initial state __i.
  inline _StateSet _Grep_matcher::
  _M_e_closure(_StateIdT __i)
  {
    _StateSet __s;
    __s.insert(__i);
    _StateStack __stack;
    __stack.push(__i);
    return this->_M_e_closure(__stack, __s);
  }

  // Creates the e-closure set for an arbitrary state set __s.
  inline _StateSet _Grep_matcher::
  _M_e_closure(const _StateSet& __s)
  {
    _StateStack __stack;
    for (_StateSet::const_iterator __i = __s.begin(); __i != __s.end(); ++__i)
      __stack.push(*__i);
    return this->_M_e_closure(__stack, __s);
  }

  inline _StateSet _Grep_matcher::
  _M_e_closure(_StateStack& __stack, const _StateSet& __s)
  {
    _StateSet __e = __s;
    while (!__stack.empty())
      {
	_StateIdT __t = __stack.top(); __stack.pop();
	if (__t == _S_invalid_state_id)
	  continue;
	// for each __u with edge from __t to __u labeled e do ...
	const _State& __state = _M_nfa->operator[](__t);
	switch (__state._M_opcode)
	  {
	  case _S_opcode_alternative:
	    __add_visited_state(__state._M_next, __stack, __e);
	    __add_visited_state(__state._M_alt, __stack, __e);
	    break;
	  case _S_opcode_subexpr_begin:
	    __add_visited_state(__state._M_next, __stack, __e);
	    __state._M_tagger(_M_pattern, _M_results);
	    break;
	  case _S_opcode_subexpr_end:
	    __add_visited_state(__state._M_next, __stack, __e);
	    __state._M_tagger(_M_pattern, _M_results);
	    _M_results._M_set_matched(__state._M_subexpr, true);
	    break;
	  case _S_opcode_accept:
	    __add_visited_state(__state._M_next, __stack, __e);
	    break;
	  default:
	    break;
	  }
      }
    return __e;
  }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace __detail
} // namespace
