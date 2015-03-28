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
 *  @file bits/regex_nfa.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{regex}
 */

namespace std _GLIBCXX_VISIBILITY(default)
{
namespace __detail
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   * @addtogroup regex-detail
   * @{
   */

  /// Base class for, um, automata.  Could be an NFA or a DFA.  Your choice.
  class _Automaton
  {
  public:
    typedef unsigned int _SizeT;

  public:
    virtual
    ~_Automaton() { }

    virtual _SizeT
    _M_sub_count() const = 0;

#ifdef _GLIBCXX_DEBUG
    virtual std::ostream&
    _M_dot(std::ostream& __ostr) const = 0;
#endif
  };

  /// Generic shared pointer to an automaton.  
  typedef std::shared_ptr<_Automaton> _AutomatonPtr;

  /// Operation codes that define the type of transitions within the base NFA
  /// that represents the regular expression.
  enum _Opcode
  {
      _S_opcode_unknown       =   0,
      _S_opcode_alternative   =   1,
      _S_opcode_subexpr_begin =   4,
      _S_opcode_subexpr_end   =   5,
      _S_opcode_match         = 100,
      _S_opcode_accept        = 255
  };

  /// Provides a generic facade for a templated match_results.
  struct _Results
  {
    virtual void _M_set_pos(int __i, int __j, const _PatternCursor& __p) = 0;
    virtual void _M_set_matched(int __i, bool __is_matched) = 0;
  };

  /// Tags current state (for subexpr begin/end).
  typedef std::function<void (const _PatternCursor&, _Results&)> _Tagger;

  /// Start state tag.
  template<typename _FwdIterT, typename _TraitsT>
    struct _StartTagger
    {
      explicit
      _StartTagger(int __i)
      : _M_index(__i)
      { }

      void
      operator()(const _PatternCursor& __pc, _Results& __r)
      { __r._M_set_pos(_M_index, 0, __pc); }

      int       _M_index;
    };

  /// End state tag.
  template<typename _FwdIterT, typename _TraitsT>
    struct _EndTagger
    {
      explicit
      _EndTagger(int __i)
      : _M_index(__i)
      { }

      void
      operator()(const _PatternCursor& __pc, _Results& __r)
      { __r._M_set_pos(_M_index, 1, __pc); }

      int       _M_index;
      _FwdIterT _M_pos;
    };

  /// Indicates if current state matches cursor current.
  typedef std::function<bool (const _PatternCursor&)> _Matcher;

  /// Matches any character
  inline bool
  _AnyMatcher(const _PatternCursor&)
  { return true; }

  /// Matches a single character
  template<typename _InIterT, typename _TraitsT>
    struct _CharMatcher
    {
      typedef typename _TraitsT::char_type char_type;

      explicit
      _CharMatcher(char_type __c, const _TraitsT& __t = _TraitsT())
      : _M_traits(__t), _M_c(_M_traits.translate(__c))
      { }

      bool
      operator()(const _PatternCursor& __pc) const
      {
	typedef const _SpecializedCursor<_InIterT>& _CursorT;
	_CursorT __c = static_cast<_CursorT>(__pc);
	return _M_traits.translate(__c._M_current()) == _M_c;
      }

      const _TraitsT& _M_traits;
      char_type       _M_c;
    };

  /// Matches a character range (bracket expression)
  template<typename _InIterT, typename _TraitsT>
    struct _RangeMatcher
    {
      typedef typename _TraitsT::char_type _CharT;
      typedef std::basic_string<_CharT>    _StringT;

      explicit
      _RangeMatcher(bool __is_non_matching, const _TraitsT& __t = _TraitsT())
      : _M_traits(__t), _M_is_non_matching(__is_non_matching)
      { }

      bool
      operator()(const _PatternCursor& __pc) const
      {
	typedef const _SpecializedCursor<_InIterT>& _CursorT;
	_CursorT __c = static_cast<_CursorT>(__pc);
	return true;
      }

      void
      _M_add_char(_CharT __c)
      { }

      void
      _M_add_collating_element(const _StringT& __s)
      { }

      void
      _M_add_equivalence_class(const _StringT& __s)
      { }

      void
      _M_add_character_class(const _StringT& __s)
      { }

      void
      _M_make_range()
      { }

      const _TraitsT& _M_traits;
      bool            _M_is_non_matching;
    };

  /// Identifies a state in the NFA.
  typedef int _StateIdT;

  /// The special case in which a state identifier is not an index.
  static const _StateIdT _S_invalid_state_id  = -1;


  /**
   * @brief struct _State
   *
   * An individual state in an NFA
   *
   * In this case a "state" is an entry in the NFA definition coupled
   * with its outgoing transition(s).  All states have a single outgoing
   * transition, except for accepting states (which have no outgoing
   * transitions) and alt states, which have two outgoing transitions.
   */
  struct _State
  {
    typedef int  _OpcodeT;

    _OpcodeT     _M_opcode;    // type of outgoing transition
    _StateIdT    _M_next;      // outgoing transition
    _StateIdT    _M_alt;       // for _S_opcode_alternative
    unsigned int _M_subexpr;   // for _S_opcode_subexpr_*
    _Tagger      _M_tagger;    // for _S_opcode_subexpr_*
    _Matcher     _M_matches;   // for _S_opcode_match

    explicit _State(_OpcodeT __opcode)
    : _M_opcode(__opcode), _M_next(_S_invalid_state_id)
    { }

    _State(const _Matcher& __m)
    : _M_opcode(_S_opcode_match), _M_next(_S_invalid_state_id), _M_matches(__m)
    { }

    _State(_OpcodeT __opcode, unsigned int __s, const _Tagger& __t)
    : _M_opcode(__opcode), _M_next(_S_invalid_state_id), _M_subexpr(__s),
      _M_tagger(__t)
    { }

    _State(_StateIdT __next, _StateIdT __alt)
    : _M_opcode(_S_opcode_alternative), _M_next(__next), _M_alt(__alt)
    { }

#ifdef _GLIBCXX_DEBUG
    std::ostream&
    _M_print(std::ostream& ostr) const;

    // Prints graphviz dot commands for state.
    std::ostream&
    _M_dot(std::ostream& __ostr, _StateIdT __id) const;
#endif
  };

  
  /// The Grep Matcher works on sets of states.  Here are sets of states.
  typedef std::set<_StateIdT> _StateSet;

  /**
   * @brief struct _Nfa
   *
   * A collection of all states making up an NFA.
   *
   * An NFA is a 4-tuple M = (K, S, s, F), where
   *    K is a finite set of states,
   *    S is the alphabet of the NFA,
   *    s is the initial state,
   *    F is a set of final (accepting) states.
   *
   * This NFA class is templated on S, a type that will hold values of the
   * underlying alphabet (without regard to semantics of that alphabet).  The
   * other elements of the tuple are generated during construction of the NFA
   * and are available through accessor member functions.
   */
  class _Nfa
  : public _Automaton, public std::vector<_State>
  {
  public:
    typedef _State                              _StateT;
    typedef unsigned int                        _SizeT;
    typedef regex_constants::syntax_option_type _FlagT;

    _Nfa(_FlagT __f)
    : _M_flags(__f), _M_start_state(0), _M_subexpr_count(0)
    { }

    ~_Nfa()
    { }

    _FlagT
    _M_options() const
    { return _M_flags; }

    _StateIdT
    _M_start() const
    { return _M_start_state; }

    const _StateSet&
    _M_final_states() const
    { return _M_accepting_states; }

    _SizeT
    _M_sub_count() const
    { return _M_subexpr_count; }

    _StateIdT
    _M_insert_accept()
    {
      this->push_back(_StateT(_S_opcode_accept));
      _M_accepting_states.insert(this->size()-1);
      return this->size()-1;
    }

    _StateIdT
    _M_insert_alt(_StateIdT __next, _StateIdT __alt)
    {
      this->push_back(_StateT(__next, __alt));
      return this->size()-1;
    }

    _StateIdT
    _M_insert_matcher(_Matcher __m)
    {
      this->push_back(_StateT(__m));
      return this->size()-1;
    }

    _StateIdT
    _M_insert_subexpr_begin(const _Tagger& __t)
    {
      this->push_back(_StateT(_S_opcode_subexpr_begin, _M_subexpr_count++,
			      __t));
      return this->size()-1;
    }

    _StateIdT 
    _M_insert_subexpr_end(unsigned int __i, const _Tagger& __t)
    {
      this->push_back(_StateT(_S_opcode_subexpr_end, __i, __t));
      return this->size()-1;
    }

#ifdef _GLIBCXX_DEBUG
    std::ostream&
    _M_dot(std::ostream& __ostr) const;
#endif

  private:
    _FlagT     _M_flags;
    _StateIdT  _M_start_state;
    _StateSet  _M_accepting_states;
    _SizeT     _M_subexpr_count;
  };

  /// Describes a sequence of one or more %_State, its current start
  /// and end(s).  This structure contains fragments of an NFA during
  /// construction.
  class _StateSeq
  {
  public:
    // Constructs a single-node sequence
    _StateSeq(_Nfa& __ss, _StateIdT __s, _StateIdT __e = _S_invalid_state_id)
    : _M_nfa(__ss), _M_start(__s), _M_end1(__s), _M_end2(__e)
    { }
    // Constructs a split sequence from two other sequencces
    _StateSeq(const _StateSeq& __e1, const _StateSeq& __e2)
    : _M_nfa(__e1._M_nfa),
      _M_start(_M_nfa._M_insert_alt(__e1._M_start, __e2._M_start)),
      _M_end1(__e1._M_end1), _M_end2(__e2._M_end1)
    { }

    // Constructs a split sequence from a single sequence
    _StateSeq(const _StateSeq& __e, _StateIdT __id)
    : _M_nfa(__e._M_nfa),
      _M_start(_M_nfa._M_insert_alt(__id, __e._M_start)),
      _M_end1(__id), _M_end2(__e._M_end1)
    { }

    // Constructs a copy of a %_StateSeq
    _StateSeq(const _StateSeq& __rhs)
    : _M_nfa(__rhs._M_nfa), _M_start(__rhs._M_start),
      _M_end1(__rhs._M_end1), _M_end2(__rhs._M_end2)
    { }


    _StateSeq& operator=(const _StateSeq& __rhs);

    _StateIdT
    _M_front() const
    { return _M_start; }

    // Extends a sequence by one.
    void
    _M_push_back(_StateIdT __id);

    // Extends and maybe joins a sequence.
    void
    _M_append(_StateIdT __id);

    void
    _M_append(_StateSeq& __rhs);

    // Clones an entire sequence.
    _StateIdT
    _M_clone();

  private:
    _Nfa&     _M_nfa;
    _StateIdT _M_start;
    _StateIdT _M_end1;
    _StateIdT _M_end2;

  };

 //@} regex-detail
_GLIBCXX_END_NAMESPACE_VERSION
} // namespace __detail
} // namespace std

#include <bits/regex_nfa.tcc>

