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
 * @file bits/regex_nfa.tcc
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{regex}
 */
#include <regex>

namespace std _GLIBCXX_VISIBILITY(default)
{
namespace __detail
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

#ifdef _GLIBCXX_DEBUG
inline std::ostream& _State::
_M_print(std::ostream& ostr) const
{
  switch (_M_opcode)
  {
    case _S_opcode_alternative:
      ostr << "alt next=" << _M_next << " alt=" << _M_alt;
      break;
    case _S_opcode_subexpr_begin:
      ostr << "subexpr begin next=" << _M_next << " index=" << _M_subexpr;
      break;
    case _S_opcode_subexpr_end:
      ostr << "subexpr end next=" << _M_next << " index=" << _M_subexpr;
      break;
    case _S_opcode_match:
      ostr << "match next=" << _M_next;
      break;
    case _S_opcode_accept:
      ostr << "accept next=" << _M_next;
      break;
    default:
      ostr << "unknown next=" << _M_next;
      break;
  }
  return ostr;
}

// Prints graphviz dot commands for state.
inline std::ostream& _State::
_M_dot(std::ostream& __ostr, _StateIdT __id) const
{
  switch (_M_opcode)
  {
    case _S_opcode_alternative:
      __ostr << __id << " [label=\"" << __id << "\\nALT\"];\n" 
             << __id << " -> " << _M_next
             << " [label=\"epsilon\", tailport=\"s\"];\n"
             << __id << " -> " << _M_alt 
             << " [label=\"epsilon\", tailport=\"n\"];\n";
      break;
    case _S_opcode_subexpr_begin:
      __ostr << __id << " [label=\"" << __id << "\\nSBEGIN "
             << _M_subexpr << "\"];\n" 
             << __id << " -> " << _M_next << " [label=\"epsilon\"];\n";
      break;
    case _S_opcode_subexpr_end:
      __ostr << __id << " [label=\"" << __id << "\\nSEND "
             << _M_subexpr << "\"];\n" 
             << __id << " -> " << _M_next << " [label=\"epsilon\"];\n";
      break;
    case _S_opcode_match:
      __ostr << __id << " [label=\"" << __id << "\\nMATCH\"];\n" 
             << __id << " -> " << _M_next << " [label=\"<match>\"];\n";
      break;
    case _S_opcode_accept:
      __ostr << __id << " [label=\"" << __id << "\\nACC\"];\n" ;
      break;
    default:
      __ostr << __id << " [label=\"" << __id << "\\nUNK\"];\n" 
             << __id << " -> " << _M_next << " [label=\"?\"];\n";
      break;
  }
  return __ostr;
}

inline std::ostream& _Nfa::
_M_dot(std::ostream& __ostr) const
{
  __ostr << "digraph _Nfa {\n"
   << "  rankdir=LR;\n";
  for (unsigned int __i = 0; __i < this->size(); ++__i)
  { this->at(__i)._M_dot(__ostr, __i); }
  __ostr << "}\n";
  return __ostr;
}
#endif

inline _StateSeq& _StateSeq::
operator=(const _StateSeq& __rhs)
{
  _M_start = __rhs._M_start;
  _M_end1  = __rhs._M_end1;
  _M_end2  = __rhs._M_end2;
  return *this;
}

inline void _StateSeq::
_M_push_back(_StateIdT __id)
{
  if (_M_end1 != _S_invalid_state_id)
    _M_nfa[_M_end1]._M_next = __id;
  _M_end1 = __id;
}

inline void _StateSeq::
_M_append(_StateIdT __id)
{
  if (_M_end2 != _S_invalid_state_id)
  {
    if (_M_end2 == _M_end1)
      _M_nfa[_M_end2]._M_alt = __id;
    else
      _M_nfa[_M_end2]._M_next = __id;
    _M_end2 = _S_invalid_state_id;
  }
  if (_M_end1 != _S_invalid_state_id)
    _M_nfa[_M_end1]._M_next = __id;
  _M_end1 = __id;
}

inline void _StateSeq::
_M_append(_StateSeq& __rhs)
{
  if (_M_end2 != _S_invalid_state_id)
  {
    if (_M_end2 == _M_end1)
      _M_nfa[_M_end2]._M_alt = __rhs._M_start;
    else
      _M_nfa[_M_end2]._M_next = __rhs._M_start;
    _M_end2 = _S_invalid_state_id;
  }
  if (__rhs._M_end2 != _S_invalid_state_id)
    _M_end2 = __rhs._M_end2;
  if (_M_end1 != _S_invalid_state_id)
    _M_nfa[_M_end1]._M_next = __rhs._M_start;
  _M_end1 = __rhs._M_end1;
}

// @todo implement this function.
inline _StateIdT _StateSeq::
_M_clone()
{ return 0; }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace __detail
} // namespace
