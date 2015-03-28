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
 *  @file bits/regex.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{regex}
 */

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   * @addtogroup regex
   * @{
   */

  /**
   * @brief Class regex_traits. Describes aspects of a regular expression.
   *
   * A regular expression traits class that satisfies the requirements of 
   * section [28.7].
   *
   * The class %regex is parameterized around a set of related types and
   * functions used to complete the definition of its semantics.  This class
   * satisfies the requirements of such a traits class.
   */
  template<typename _Ch_type>
    struct regex_traits
    {
    public:
      typedef _Ch_type                     	char_type;
      typedef std::basic_string<char_type> 	string_type;
      typedef std::locale                  	locale_type;
      typedef std::ctype_base::mask        	char_class_type;

    public:
      /**
       * @brief Constructs a default traits object.
       */
      regex_traits() { }
      
      /**
       * @brief Gives the length of a C-style string starting at @p __p.
       *
       * @param __p a pointer to the start of a character sequence.
       *
       * @returns the number of characters between @p *__p and the first
       * default-initialized value of type @p char_type.  In other words, uses
       * the C-string algorithm for determining the length of a sequence of
       * characters.
       */
      static std::size_t
      length(const char_type* __p)
      { return string_type::traits_type::length(__p); }

      /**
       * @brief Performs the identity translation.
       *
       * @param __c A character to the locale-specific character set.
       *
       * @returns __c.
       */
      char_type
      translate(char_type __c) const
      { return __c; }
      
      /**
       * @brief Translates a character into a case-insensitive equivalent.
       *
       * @param __c A character to the locale-specific character set.
       *
       * @returns the locale-specific lower-case equivalent of __c.
       * @throws std::bad_cast if the imbued locale does not support the ctype
       *         facet.
       */
      char_type
      translate_nocase(char_type __c) const
      { 
	typedef std::ctype<char_type> __ctype_type;
	const __ctype_type& __fctyp(use_facet<__ctype_type>(_M_locale));
	return __fctyp.tolower(__c); 
      }
      
      /**
       * @brief Gets a sort key for a character sequence.
       *
       * @param __first beginning of the character sequence.
       * @param __last  one-past-the-end of the character sequence.
       *
       * Returns a sort key for the character sequence designated by the
       * iterator range [F1, F2) such that if the character sequence [G1, G2)
       * sorts before the character sequence [H1, H2) then
       * v.transform(G1, G2) < v.transform(H1, H2).
       *
       * What this really does is provide a more efficient way to compare a
       * string to multiple other strings in locales with fancy collation
       * rules and equivalence classes.
       *
       * @returns a locale-specific sort key equivalent to the input range.
       *
       * @throws std::bad_cast if the current locale does not have a collate
       *         facet.
       */
      template<typename _Fwd_iter>
        string_type
        transform(_Fwd_iter __first, _Fwd_iter __last) const
        {
	  typedef std::collate<char_type> __collate_type;
	  const __collate_type& __fclt(use_facet<__collate_type>(_M_locale));
	  string_type __s(__first, __last);
	  return __fclt.transform(__s.data(), __s.data() + __s.size());
	}

      /**
       * @brief Gets a sort key for a character sequence, independent of case.
       *
       * @param __first beginning of the character sequence.
       * @param __last  one-past-the-end of the character sequence.
       *
       * Effects: if typeid(use_facet<collate<_Ch_type> >) ==
       * typeid(collate_byname<_Ch_type>) and the form of the sort key
       * returned by collate_byname<_Ch_type>::transform(__first, __last) 
       * is known and can be converted into a primary sort key
       * then returns that key, otherwise returns an empty string.
       *
       * @todo Implement this function.
       */
      template<typename _Fwd_iter>
        string_type
        transform_primary(_Fwd_iter __first, _Fwd_iter __last) const
        { return string_type(); }

      /**
       * @brief Gets a collation element by name.
       *
       * @param __first beginning of the collation element name.
       * @param __last  one-past-the-end of the collation element name.
       * 
       * @returns a sequence of one or more characters that represents the
       * collating element consisting of the character sequence designated by
       * the iterator range [__first, __last). Returns an empty string if the
       * character sequence is not a valid collating element.
       *
       * @todo Implement this function.
       */
      template<typename _Fwd_iter>
        string_type
        lookup_collatename(_Fwd_iter __first, _Fwd_iter __last) const
        { return string_type(); }

      /**
       * @brief Maps one or more characters to a named character
       *        classification.
       *
       * @param __first beginning of the character sequence.
       * @param __last  one-past-the-end of the character sequence.
       * @param __icase ignores the case of the classification name.
       *
       * @returns an unspecified value that represents the character
       * classification named by the character sequence designated by
       * the iterator range [__first, __last). If @p icase is true,
       * the returned mask identifies the classification regardless of
       * the case of the characters to be matched (for example,
       * [[:lower:]] is the same as [[:alpha:]]), otherwise a
       * case-dependent classification is returned.  The value
       * returned shall be independent of the case of the characters
       * in the character sequence. If the name is not recognized then
       * returns a value that compares equal to 0.
       *
       * At least the following names (or their wide-character equivalent) are
       * supported.
       * - d
       * - w
       * - s
       * - alnum
       * - alpha
       * - blank
       * - cntrl
       * - digit
       * - graph
       * - lower
       * - print
       * - punct
       * - space
       * - upper
       * - xdigit
       *
       * @todo Implement this function.
       */
      template<typename _Fwd_iter>
        char_class_type
        lookup_classname(_Fwd_iter __first, _Fwd_iter __last,
	                 bool __icase = false) const
	{ return 0; }

      /**
       * @brief Determines if @p c is a member of an identified class.
       *
       * @param __c a character.
       * @param __f a class type (as returned from lookup_classname).
       *
       * @returns true if the character @p __c is a member of the classification
       * represented by @p __f, false otherwise.
       *
       * @throws std::bad_cast if the current locale does not have a ctype
       *         facet.
       */
      bool
      isctype(_Ch_type __c, char_class_type __f) const;

      /**
       * @brief Converts a digit to an int.
       *
       * @param __ch    a character representing a digit.
       * @param __radix the radix if the numeric conversion (limited to 8, 10,
       *              or 16).
       * 
       * @returns the value represented by the digit __ch in base radix if the
       * character __ch is a valid digit in base radix; otherwise returns -1.
       */
      int
      value(_Ch_type __ch, int __radix) const;
      
      /**
       * @brief Imbues the regex_traits object with a copy of a new locale.
       *
       * @param __loc A locale.
       *
       * @returns a copy of the previous locale in use by the regex_traits
       *          object.
       *
       * @note Calling imbue with a different locale than the one currently in
       *       use invalidates all cached data held by *this.
       */
      locale_type
      imbue(locale_type __loc)
      {
	std::swap(_M_locale, __loc);
	return __loc;
      }
      
      /**
       * @brief Gets a copy of the current locale in use by the regex_traits
       * object.
       */
      locale_type
      getloc() const
      { return _M_locale; }
      
    protected:
      locale_type _M_locale;
    };

  template<typename _Ch_type>
    bool
    regex_traits<_Ch_type>::
    isctype(_Ch_type __c, char_class_type __f) const
    {
      typedef std::ctype<char_type> __ctype_type;
      const __ctype_type& __fctyp(use_facet<__ctype_type>(_M_locale));
      
      if (__fctyp.is(__f, __c))
	return true;
      
      // special case of underscore in [[:w:]]
      if (__c == __fctyp.widen('_'))
	{
	  const char __wb[] = "w";
	  char_class_type __wt = this->lookup_classname(__wb,
							__wb + sizeof(__wb));
	  if (__f | __wt)
	    return true;
	}
    
      // special case of [[:space:]] in [[:blank:]]
      if (__fctyp.is(std::ctype_base::space, __c))
	{
	  const char __bb[] = "blank";
	  char_class_type __bt = this->lookup_classname(__bb,
							__bb + sizeof(__bb));
	  if (__f | __bt)
	    return true;
	}
      
      return false;
    }

  template<typename _Ch_type>
    int
    regex_traits<_Ch_type>::
    value(_Ch_type __ch, int __radix) const
    {
      std::basic_istringstream<char_type> __is(string_type(1, __ch));
      int __v;
      if (__radix == 8)
	__is >> std::oct;
      else if (__radix == 16)
	__is >> std::hex;
      __is >> __v;
      return __is.fail() ? -1 : __v;
    }

  // [7.8] Class basic_regex
  /**
   * Objects of specializations of this class represent regular expressions
   * constructed from sequences of character type @p _Ch_type.
   *
   * Storage for the regular expression is allocated and deallocated as
   * necessary by the member functions of this class.
   */
  template<typename _Ch_type, typename _Rx_traits = regex_traits<_Ch_type> >
    class basic_regex
    {
    public:
      // types:
      typedef _Ch_type                            value_type;
      typedef _Rx_traits                          traits_type;
      typedef typename traits_type::string_type   string_type;
      typedef regex_constants::syntax_option_type flag_type;
      typedef typename traits_type::locale_type   locale_type;

      /**
       * @name Constants
       * std [28.8.1](1)
       */
      //@{
      static constexpr flag_type icase = regex_constants::icase;
      static constexpr flag_type nosubs = regex_constants::nosubs;
      static constexpr flag_type optimize = regex_constants::optimize;
      static constexpr flag_type collate = regex_constants::collate;
      static constexpr flag_type ECMAScript = regex_constants::ECMAScript;
      static constexpr flag_type basic = regex_constants::basic;
      static constexpr flag_type extended = regex_constants::extended;
      static constexpr flag_type awk = regex_constants::awk;
      static constexpr flag_type grep = regex_constants::grep;
      static constexpr flag_type egrep = regex_constants::egrep;
      //@}

      // [7.8.2] construct/copy/destroy
      /**
       * Constructs a basic regular expression that does not match any
       * character sequence.
       */
      basic_regex()
      : _M_flags(ECMAScript),
        _M_automaton(__detail::__compile<const _Ch_type*, _Rx_traits>(0, 0,
                     _M_traits, _M_flags))
      { }

      /**
       * @brief Constructs a basic regular expression from the
       * sequence [__p, __p + char_traits<_Ch_type>::length(__p))
       * interpreted according to the flags in @p __f.
       *
       * @param __p A pointer to the start of a C-style null-terminated string
       *          containing a regular expression.
       * @param __f Flags indicating the syntax rules and options.
       *
       * @throws regex_error if @p __p is not a valid regular expression.
       */
      explicit
      basic_regex(const _Ch_type* __p, flag_type __f = ECMAScript)
      : _M_flags(__f),
        _M_automaton(__detail::__compile(__p, __p + _Rx_traits::length(__p),
        				_M_traits, _M_flags))
      { }

      /**
       * @brief Constructs a basic regular expression from the sequence
       * [p, p + len) interpreted according to the flags in @p f.
       *
       * @param __p   A pointer to the start of a string containing a regular
       *              expression.
       * @param __len The length of the string containing the regular
       *              expression.
       * @param __f   Flags indicating the syntax rules and options.
       *
       * @throws regex_error if @p __p is not a valid regular expression.
       */
      basic_regex(const _Ch_type* __p, std::size_t __len, flag_type __f)
      : _M_flags(__f),
        _M_automaton(__detail::__compile(__p, __p + __len, _M_traits, _M_flags))
      { }

      /**
       * @brief Copy-constructs a basic regular expression.
       *
       * @param __rhs A @p regex object.
       */
      basic_regex(const basic_regex& __rhs)
      : _M_flags(__rhs._M_flags), _M_traits(__rhs._M_traits),
        _M_automaton(__rhs._M_automaton)
      { }

      /**
       * @brief Move-constructs a basic regular expression.
       *
       * @param __rhs A @p regex object.
       */
      basic_regex(const basic_regex&& __rhs) noexcept
      : _M_flags(__rhs._M_flags), _M_traits(__rhs._M_traits),
        _M_automaton(std::move(__rhs._M_automaton))
      { }

      /**
       * @brief Constructs a basic regular expression from the string
       * @p s interpreted according to the flags in @p f.
       *
       * @param __s A string containing a regular expression.
       * @param __f Flags indicating the syntax rules and options.
       *
       * @throws regex_error if @p __s is not a valid regular expression.
       */
      template<typename _Ch_traits, typename _Ch_alloc>
        explicit
        basic_regex(const std::basic_string<_Ch_type, _Ch_traits, 
					    _Ch_alloc>& __s,
		    flag_type __f = ECMAScript)
	: _M_flags(__f),
	  _M_automaton(__detail::__compile(__s.begin(), __s.end(),
	  				  _M_traits, _M_flags))
        { }

      /**
       * @brief Constructs a basic regular expression from the range
       * [first, last) interpreted according to the flags in @p f.
       *
       * @param __first The start of a range containing a valid regular
       *                expression.
       * @param __last  The end of a range containing a valid regular
       *                expression.
       * @param __f     The format flags of the regular expression.
       *
       * @throws regex_error if @p [__first, __last) is not a valid regular
       *         expression.
       */
      template<typename _InputIterator>
        basic_regex(_InputIterator __first, _InputIterator __last, 
		    flag_type __f = ECMAScript)
	: _M_flags(__f),
	  _M_automaton(__detail::__compile(__first, __last, _M_traits, _M_flags))
        { }

      /**
       * @brief Constructs a basic regular expression from an initializer list.
       *
       * @param __l  The initializer list.
       * @param __f  The format flags of the regular expression.
       *
       * @throws regex_error if @p __l is not a valid regular expression.
       */
      basic_regex(initializer_list<_Ch_type> __l,
		  flag_type __f = ECMAScript)
      : _M_flags(__f),
        _M_automaton(__detail::__compile(__l.begin(), __l.end(),
        				_M_traits, _M_flags))
      { }

      /**
       * @brief Destroys a basic regular expression.
       */
      ~basic_regex()
      { }
      
      /**
       * @brief Assigns one regular expression to another.
       */
      basic_regex&
      operator=(const basic_regex& __rhs)
      { return this->assign(__rhs); }

      /**
       * @brief Move-assigns one regular expression to another.
       */
      basic_regex&
      operator=(basic_regex&& __rhs) noexcept
      { return this->assign(std::move(__rhs)); }

      /**
       * @brief Replaces a regular expression with a new one constructed from
       * a C-style null-terminated string.
       *
       * @param __p A pointer to the start of a null-terminated C-style string
       *        containing a regular expression.
       */
      basic_regex&
      operator=(const _Ch_type* __p)
      { return this->assign(__p, flags()); }
      
      /**
       * @brief Replaces a regular expression with a new one constructed from
       * a string.
       *
       * @param __s A pointer to a string containing a regular expression.
       */
      template<typename _Ch_typeraits, typename _Alloc>
        basic_regex&
        operator=(const basic_string<_Ch_type, _Ch_typeraits, _Alloc>& __s)
        { return this->assign(__s, flags()); }

      // [7.8.3] assign
      /**
       * @brief the real assignment operator.
       *
       * @param __rhs Another regular expression object.
       */
      basic_regex&
      assign(const basic_regex& __rhs)
      {
	basic_regex __tmp(__rhs);
	this->swap(__tmp);
	return *this;
      }
      
      /**
       * @brief The move-assignment operator.
       *
       * @param __rhs Another regular expression object.
       */
      basic_regex&
      assign(basic_regex&& __rhs) noexcept
      {
	basic_regex __tmp(std::move(__rhs));
	this->swap(__tmp);
	return *this;
      }

      /**
       * @brief Assigns a new regular expression to a regex object from a
       * C-style null-terminated string containing a regular expression
       * pattern.
       *
       * @param __p     A pointer to a C-style null-terminated string containing
       *              a regular expression pattern.
       * @param __flags Syntax option flags.
       *
       * @throws regex_error if __p does not contain a valid regular
       * expression pattern interpreted according to @p __flags.  If
       * regex_error is thrown, *this remains unchanged.
       */
      basic_regex&
      assign(const _Ch_type* __p, flag_type __flags = ECMAScript)
      { return this->assign(string_type(__p), __flags); }

      /**
       * @brief Assigns a new regular expression to a regex object from a
       * C-style string containing a regular expression pattern.
       *
       * @param __p     A pointer to a C-style string containing a
       *                regular expression pattern.
       * @param __len   The length of the regular expression pattern string.
       * @param __flags Syntax option flags.
       *
       * @throws regex_error if p does not contain a valid regular
       * expression pattern interpreted according to @p __flags.  If
       * regex_error is thrown, *this remains unchanged.
       */
      basic_regex&
      assign(const _Ch_type* __p, std::size_t __len, flag_type __flags)
      { return this->assign(string_type(__p, __len), __flags); }

      /**
       * @brief Assigns a new regular expression to a regex object from a 
       * string containing a regular expression pattern.
       *
       * @param __s     A string containing a regular expression pattern.
       * @param __flags Syntax option flags.
       *
       * @throws regex_error if __s does not contain a valid regular
       * expression pattern interpreted according to @p __flags.  If
       * regex_error is thrown, *this remains unchanged.
       */
      template<typename _Ch_typeraits, typename _Alloc>
        basic_regex&
        assign(const basic_string<_Ch_type, _Ch_typeraits, _Alloc>& __s,
	       flag_type __flags = ECMAScript)
        { 
	  basic_regex __tmp(__s, __flags);
	  this->swap(__tmp);
	  return *this;
	}

      /**
       * @brief Assigns a new regular expression to a regex object.
       *
       * @param __first The start of a range containing a valid regular
       *                expression.
       * @param __last  The end of a range containing a valid regular
       *                expression.
       * @param __flags Syntax option flags.
       *
       * @throws regex_error if p does not contain a valid regular
       * expression pattern interpreted according to @p __flags.  If
       * regex_error is thrown, the object remains unchanged.
       */
      template<typename _InputIterator>
        basic_regex&
        assign(_InputIterator __first, _InputIterator __last,
	       flag_type __flags = ECMAScript)
        { return this->assign(string_type(__first, __last), __flags); }

      /**
       * @brief Assigns a new regular expression to a regex object.
       *
       * @param __l     An initializer list representing a regular expression.
       * @param __flags Syntax option flags.
       *
       * @throws regex_error if @p __l does not contain a valid
       * regular expression pattern interpreted according to @p
       * __flags.  If regex_error is thrown, the object remains
       * unchanged.
       */
      basic_regex&
      assign(initializer_list<_Ch_type> __l, flag_type __flags = ECMAScript)
      { return this->assign(__l.begin(), __l.end(), __flags); }

      // [7.8.4] const operations
      /**
       * @brief Gets the number of marked subexpressions within the regular
       * expression.
       */
      unsigned int
      mark_count() const
      { return _M_automaton->_M_sub_count() - 1; }
      
      /**
       * @brief Gets the flags used to construct the regular expression
       * or in the last call to assign().
       */
      flag_type
      flags() const
      { return _M_flags; }
      
      // [7.8.5] locale
      /**
       * @brief Imbues the regular expression object with the given locale.
       *
       * @param __loc A locale.
       */
      locale_type
      imbue(locale_type __loc)
      { return _M_traits.imbue(__loc); }
      
      /**
       * @brief Gets the locale currently imbued in the regular expression
       *        object.
       */
      locale_type
      getloc() const
      { return _M_traits.getloc(); }
      
      // [7.8.6] swap
      /**
       * @brief Swaps the contents of two regular expression objects.
       *
       * @param __rhs Another regular expression object.
       */
      void
      swap(basic_regex& __rhs)
      {
	std::swap(_M_flags, __rhs._M_flags);
	std::swap(_M_traits, __rhs._M_traits);
	std::swap(_M_automaton, __rhs._M_automaton);
      }

#ifdef _GLIBCXX_DEBUG
      void
      _M_dot(std::ostream& __ostr)
      { _M_automaton->_M_dot(__ostr); }
#endif
      
      const __detail::_AutomatonPtr&
      _M_get_automaton() const
      { return _M_automaton; }

    protected:
      flag_type              _M_flags;
      _Rx_traits             _M_traits;
      __detail::_AutomatonPtr _M_automaton;
    };
  
  /** @brief Standard regular expressions. */
  typedef basic_regex<char>    regex;

#ifdef _GLIBCXX_USE_WCHAR_T
  /** @brief Standard wide-character regular expressions. */
  typedef basic_regex<wchar_t> wregex;
#endif


  // [7.8.6] basic_regex swap
  /**
   * @brief Swaps the contents of two regular expression objects.
   * @param __lhs First regular expression.
   * @param __rhs Second regular expression.
   */
  template<typename _Ch_type, typename _Rx_traits>
    inline void
    swap(basic_regex<_Ch_type, _Rx_traits>& __lhs,
	 basic_regex<_Ch_type, _Rx_traits>& __rhs)
    { __lhs.swap(__rhs); }


  // [7.9] Class template sub_match
  /**
   * A sequence of characters matched by a particular marked sub-expression.
   *
   * An object of this class is essentially a pair of iterators marking a
   * matched subexpression within a regular expression pattern match. Such
   * objects can be converted to and compared with std::basic_string objects
   * of a similar base character type as the pattern matched by the regular
   * expression.
   *
   * The iterators that make up the pair are the usual half-open interval
   * referencing the actual original pattern matched.
   */
  template<typename _BiIter>
    class sub_match : public std::pair<_BiIter, _BiIter>
    {
      typedef iterator_traits<_BiIter>			__iter_traits;
	
    public:
      typedef typename __iter_traits::value_type      	value_type;
      typedef typename __iter_traits::difference_type 	difference_type;
      typedef _BiIter                                   iterator;
      typedef std::basic_string<value_type>             string_type;

      bool matched;
      
      constexpr sub_match() : matched() { }

      /**
       * Gets the length of the matching sequence.
       */
      difference_type
      length() const
      { return this->matched ? std::distance(this->first, this->second) : 0; }

      /**
       * @brief Gets the matching sequence as a string.
       *
       * @returns the matching sequence as a string.
       *
       * This is the implicit conversion operator.  It is identical to the
       * str() member function except that it will want to pop up in
       * unexpected places and cause a great deal of confusion and cursing
       * from the unwary.
       */
      operator string_type() const
      {
	return this->matched
	  ? string_type(this->first, this->second)
	  : string_type();
      }
      
      /**
       * @brief Gets the matching sequence as a string.
       *
       * @returns the matching sequence as a string.
       */
      string_type
      str() const
      {
	return this->matched
	  ? string_type(this->first, this->second)
	  : string_type();
      }
      
      /**
       * @brief Compares this and another matched sequence.
       *
       * @param __s Another matched sequence to compare to this one.
       *
       * @retval <0 this matched sequence will collate before @p __s.
       * @retval =0 this matched sequence is equivalent to @p __s.
       * @retval <0 this matched sequence will collate after @p __s.
       */
      int
      compare(const sub_match& __s) const
      { return this->str().compare(__s.str()); }

      /**
       * @brief Compares this sub_match to a string.
       *
       * @param __s A string to compare to this sub_match.
       *
       * @retval <0 this matched sequence will collate before @p __s.
       * @retval =0 this matched sequence is equivalent to @p __s.
       * @retval <0 this matched sequence will collate after @p __s.
       */
      int
      compare(const string_type& __s) const
      { return this->str().compare(__s); }
      
      /**
       * @brief Compares this sub_match to a C-style string.
       *
       * @param __s A C-style string to compare to this sub_match.
       *
       * @retval <0 this matched sequence will collate before @p __s.
       * @retval =0 this matched sequence is equivalent to @p __s.
       * @retval <0 this matched sequence will collate after @p __s.
       */
      int
      compare(const value_type* __s) const
      { return this->str().compare(__s); }
    };
  
  
  /** @brief Standard regex submatch over a C-style null-terminated string. */
  typedef sub_match<const char*>             csub_match;

  /** @brief Standard regex submatch over a standard string. */
  typedef sub_match<string::const_iterator>  ssub_match;

#ifdef _GLIBCXX_USE_WCHAR_T
  /** @brief Regex submatch over a C-style null-terminated wide string. */
  typedef sub_match<const wchar_t*>          wcsub_match;

  /** @brief Regex submatch over a standard wide string. */
  typedef sub_match<wstring::const_iterator> wssub_match;
#endif

  // [7.9.2] sub_match non-member operators
  
  /**
   * @brief Tests the equivalence of two regular expression submatches.
   * @param __lhs First regular expression submatch.
   * @param __rhs Second regular expression submatch.
   * @returns true if @a __lhs  is equivalent to @a __rhs, false otherwise.
   */
  template<typename _BiIter>
    inline bool
    operator==(const sub_match<_BiIter>& __lhs, const sub_match<_BiIter>& __rhs)
    { return __lhs.compare(__rhs) == 0; }

  /**
   * @brief Tests the inequivalence of two regular expression submatches.
   * @param __lhs First regular expression submatch.
   * @param __rhs Second regular expression submatch.
   * @returns true if @a __lhs  is not equivalent to @a __rhs, false otherwise.
   */
  template<typename _BiIter>
    inline bool
    operator!=(const sub_match<_BiIter>& __lhs, const sub_match<_BiIter>& __rhs)
    { return __lhs.compare(__rhs) != 0; }

  /**
   * @brief Tests the ordering of two regular expression submatches.
   * @param __lhs First regular expression submatch.
   * @param __rhs Second regular expression submatch.
   * @returns true if @a __lhs precedes @a __rhs, false otherwise.
   */
  template<typename _BiIter>
    inline bool
    operator<(const sub_match<_BiIter>& __lhs, const sub_match<_BiIter>& __rhs)
    { return __lhs.compare(__rhs) < 0; }

  /**
   * @brief Tests the ordering of two regular expression submatches.
   * @param __lhs First regular expression submatch.
   * @param __rhs Second regular expression submatch.
   * @returns true if @a __lhs does not succeed @a __rhs, false otherwise.
   */
  template<typename _BiIter>
    inline bool
    operator<=(const sub_match<_BiIter>& __lhs, const sub_match<_BiIter>& __rhs)
    { return __lhs.compare(__rhs) <= 0; }

  /**
   * @brief Tests the ordering of two regular expression submatches.
   * @param __lhs First regular expression submatch.
   * @param __rhs Second regular expression submatch.
   * @returns true if @a __lhs does not precede @a __rhs, false otherwise.
   */
  template<typename _BiIter>
    inline bool
    operator>=(const sub_match<_BiIter>& __lhs, const sub_match<_BiIter>& __rhs)
    { return __lhs.compare(__rhs) >= 0; }

  /**
   * @brief Tests the ordering of two regular expression submatches.
   * @param __lhs First regular expression submatch.
   * @param __rhs Second regular expression submatch.
   * @returns true if @a __lhs succeeds @a __rhs, false otherwise.
   */
  template<typename _BiIter>
    inline bool
    operator>(const sub_match<_BiIter>& __lhs, const sub_match<_BiIter>& __rhs)
    { return __lhs.compare(__rhs) > 0; }

  // Alias for sub_match'd string.
  template<typename _Bi_iter, typename _Ch_traits, typename _Ch_alloc>
    using __sub_match_string = basic_string<
                              typename iterator_traits<_Bi_iter>::value_type,
                              _Ch_traits, _Ch_alloc>;

  /**
   * @brief Tests the equivalence of a string and a regular expression
   *        submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs  is equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, typename _Ch_traits, typename _Ch_alloc>
    inline bool
    operator==(const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    { return __rhs.compare(__lhs.c_str()) == 0; }

  /**
   * @brief Tests the inequivalence of a string and a regular expression
   *        submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs  is not equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, typename _Ch_traits, typename _Ch_alloc>
    inline bool
    operator!=(const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    { return !(__lhs == __rhs); }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs precedes @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, typename _Ch_traits, typename _Ch_alloc>
    inline bool
    operator<(const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __lhs,
	      const sub_match<_Bi_iter>& __rhs)
     { return __rhs.compare(__lhs.c_str()) > 0; }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs succeeds @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, typename _Ch_traits, typename _Ch_alloc>
    inline bool
    operator>(const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __lhs,
	      const sub_match<_Bi_iter>& __rhs)
    { return __rhs < __lhs; }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs does not precede @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, typename _Ch_traits, typename _Ch_alloc>
    inline bool
    operator>=(const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    { return !(__lhs < __rhs); }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs does not succeed @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, typename _Ch_traits, typename _Ch_alloc>
    inline bool
    operator<=(const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    { return !(__rhs < __lhs); }

  /**
   * @brief Tests the equivalence of a regular expression submatch and a
   *        string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A string.
   * @returns true if @a __lhs is equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, typename _Ch_traits, typename _Ch_alloc>
    inline bool
    operator==(const sub_match<_Bi_iter>& __lhs,
	       const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __rhs)
    { return __lhs.compare(__rhs.c_str()) == 0; }

  /**
   * @brief Tests the inequivalence of a regular expression submatch and a
   *        string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A string.
   * @returns true if @a __lhs is not equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, typename _Ch_traits, typename _Ch_alloc>
    inline bool
    operator!=(const sub_match<_Bi_iter>& __lhs,
	       const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __rhs)
    { return !(__lhs == __rhs); }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A string.
   * @returns true if @a __lhs precedes @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, class _Ch_traits, class _Ch_alloc>
    inline bool
    operator<(const sub_match<_Bi_iter>& __lhs,
	      const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __rhs)
    { return __lhs.compare(__rhs.c_str()) < 0; }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A string.
   * @returns true if @a __lhs succeeds @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, class _Ch_traits, class _Ch_alloc>
    inline bool
    operator>(const sub_match<_Bi_iter>& __lhs,
	      const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __rhs)
    { return __rhs < __lhs; }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A string.
   * @returns true if @a __lhs does not precede @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, class _Ch_traits, class _Ch_alloc>
    inline bool
    operator>=(const sub_match<_Bi_iter>& __lhs,
	       const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __rhs)
    { return !(__lhs < __rhs); }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A string.
   * @returns true if @a __lhs does not succeed @a __rhs, false otherwise.
   */
  template<typename _Bi_iter, class _Ch_traits, class _Ch_alloc>
    inline bool
    operator<=(const sub_match<_Bi_iter>& __lhs,
	       const __sub_match_string<_Bi_iter, _Ch_traits, _Ch_alloc>& __rhs)
    { return !(__rhs < __lhs); }

  /**
   * @brief Tests the equivalence of a C string and a regular expression
   *        submatch.
   * @param __lhs A C string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs  is equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator==(typename iterator_traits<_Bi_iter>::value_type const* __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    { return __rhs.compare(__lhs) == 0; }

  /**
   * @brief Tests the inequivalence of an iterator value and a regular
   *        expression submatch.
   * @param __lhs A regular expression submatch.
   * @param __rhs A string.
   * @returns true if @a __lhs is not equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator!=(typename iterator_traits<_Bi_iter>::value_type const* __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    { return !(__lhs == __rhs); }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs precedes @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator<(typename iterator_traits<_Bi_iter>::value_type const* __lhs,
	      const sub_match<_Bi_iter>& __rhs)
    { return __rhs.compare(__lhs) > 0; }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs succeeds @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator>(typename iterator_traits<_Bi_iter>::value_type const* __lhs,
	      const sub_match<_Bi_iter>& __rhs)
    { return __rhs < __lhs; }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs does not precede @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator>=(typename iterator_traits<_Bi_iter>::value_type const* __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    { return !(__lhs < __rhs); }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs does not succeed @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator<=(typename iterator_traits<_Bi_iter>::value_type const* __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    { return !(__rhs < __lhs); }

  /**
   * @brief Tests the equivalence of a regular expression submatch and a
   *        string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A pointer to a string?
   * @returns true if @a __lhs  is equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator==(const sub_match<_Bi_iter>& __lhs,
	       typename iterator_traits<_Bi_iter>::value_type const* __rhs)
    { return __lhs.compare(__rhs) == 0; }

  /**
   * @brief Tests the inequivalence of a regular expression submatch and a
   *        string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A pointer to a string.
   * @returns true if @a __lhs is not equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator!=(const sub_match<_Bi_iter>& __lhs,
	       typename iterator_traits<_Bi_iter>::value_type const* __rhs)
    { return !(__lhs == __rhs); }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A string.
   * @returns true if @a __lhs precedes @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator<(const sub_match<_Bi_iter>& __lhs,
	      typename iterator_traits<_Bi_iter>::value_type const* __rhs)
    { return __lhs.compare(__rhs) < 0; }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A string.
   * @returns true if @a __lhs succeeds @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator>(const sub_match<_Bi_iter>& __lhs,
	      typename iterator_traits<_Bi_iter>::value_type const* __rhs)
    { return __rhs < __lhs; }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A string.
   * @returns true if @a __lhs does not precede @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator>=(const sub_match<_Bi_iter>& __lhs,
	       typename iterator_traits<_Bi_iter>::value_type const* __rhs)
    { return !(__lhs < __rhs); }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A string.
   * @returns true if @a __lhs does not succeed @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator<=(const sub_match<_Bi_iter>& __lhs,
	       typename iterator_traits<_Bi_iter>::value_type const* __rhs)
    { return !(__rhs < __lhs); }

  /**
   * @brief Tests the equivalence of a string and a regular expression
   *        submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs is equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator==(typename iterator_traits<_Bi_iter>::value_type const& __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    {
      typedef typename sub_match<_Bi_iter>::string_type string_type;
      return __rhs.compare(string_type(1, __lhs)) == 0;
    }

  /**
   * @brief Tests the inequivalence of a string and a regular expression
   *        submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs is not equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator!=(typename iterator_traits<_Bi_iter>::value_type const& __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    { return !(__lhs == __rhs); }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs precedes @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator<(typename iterator_traits<_Bi_iter>::value_type const& __lhs,
	      const sub_match<_Bi_iter>& __rhs)
    {
      typedef typename sub_match<_Bi_iter>::string_type string_type;
      return __rhs.compare(string_type(1, __lhs)) > 0;
    }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs succeeds @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator>(typename iterator_traits<_Bi_iter>::value_type const& __lhs,
	      const sub_match<_Bi_iter>& __rhs)
    { return __rhs < __lhs; }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs does not precede @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator>=(typename iterator_traits<_Bi_iter>::value_type const& __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    { return !(__lhs < __rhs); }

  /**
   * @brief Tests the ordering of a string and a regular expression submatch.
   * @param __lhs A string.
   * @param __rhs A regular expression submatch.
   * @returns true if @a __lhs does not succeed @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator<=(typename iterator_traits<_Bi_iter>::value_type const& __lhs,
	       const sub_match<_Bi_iter>& __rhs)
    { return !(__rhs < __lhs); }

  /**
   * @brief Tests the equivalence of a regular expression submatch and a
   *        string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A const string reference.
   * @returns true if @a __lhs  is equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator==(const sub_match<_Bi_iter>& __lhs,
	       typename iterator_traits<_Bi_iter>::value_type const& __rhs)
    {
      typedef typename sub_match<_Bi_iter>::string_type string_type;
      return __lhs.compare(string_type(1, __rhs)) == 0;
    }

  /**
   * @brief Tests the inequivalence of a regular expression submatch and a
   *        string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A const string reference.
   * @returns true if @a __lhs is not equivalent to @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator!=(const sub_match<_Bi_iter>& __lhs,
	       typename iterator_traits<_Bi_iter>::value_type const& __rhs)
    { return !(__lhs == __rhs); }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A const string reference.
   * @returns true if @a __lhs precedes @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator<(const sub_match<_Bi_iter>& __lhs,
	      typename iterator_traits<_Bi_iter>::value_type const& __rhs)
    {
      typedef typename sub_match<_Bi_iter>::string_type string_type;
      return __lhs.compare(string_type(1, __rhs)) < 0;
    }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A const string reference.
   * @returns true if @a __lhs succeeds @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator>(const sub_match<_Bi_iter>& __lhs,
	      typename iterator_traits<_Bi_iter>::value_type const& __rhs)
    { return __rhs < __lhs; }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A const string reference.
   * @returns true if @a __lhs does not precede @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator>=(const sub_match<_Bi_iter>& __lhs,
	       typename iterator_traits<_Bi_iter>::value_type const& __rhs)
    { return !(__lhs < __rhs); }

  /**
   * @brief Tests the ordering of a regular expression submatch and a string.
   * @param __lhs A regular expression submatch.
   * @param __rhs A const string reference.
   * @returns true if @a __lhs does not succeed @a __rhs, false otherwise.
   */
  template<typename _Bi_iter>
    inline bool
    operator<=(const sub_match<_Bi_iter>& __lhs,
	       typename iterator_traits<_Bi_iter>::value_type const& __rhs)
    { return !(__rhs < __lhs); }

  /**
   * @brief Inserts a matched string into an output stream.
   *
   * @param __os The output stream.
   * @param __m  A submatch string.
   *
   * @returns the output stream with the submatch string inserted.
   */
  template<typename _Ch_type, typename _Ch_traits, typename _Bi_iter>
    inline
    basic_ostream<_Ch_type, _Ch_traits>&
    operator<<(basic_ostream<_Ch_type, _Ch_traits>& __os,
	       const sub_match<_Bi_iter>& __m)
    { return __os << __m.str(); }

  // [7.10] Class template match_results

  /*
   * Special sub_match object representing an unmatched sub-expression.
   */
  template<typename _Bi_iter>
    inline const sub_match<_Bi_iter>&
    __unmatched_sub()
    {
      static const sub_match<_Bi_iter> __unmatched = sub_match<_Bi_iter>();
      return __unmatched;
    }

  /**
   * @brief The results of a match or search operation.
   *
   * A collection of character sequences representing the result of a regular
   * expression match.  Storage for the collection is allocated and freed as
   * necessary by the member functions of class template match_results.
   *
   * This class satisfies the Sequence requirements, with the exception that
   * only the operations defined for a const-qualified Sequence are supported.
   *
   * The sub_match object stored at index 0 represents sub-expression 0, i.e.
   * the whole match. In this case the %sub_match member matched is always true.
   * The sub_match object stored at index n denotes what matched the marked
   * sub-expression n within the matched expression. If the sub-expression n
   * participated in a regular expression match then the %sub_match member
   * matched evaluates to true, and members first and second denote the range
   * of characters [first, second) which formed that match. Otherwise matched
   * is false, and members first and second point to the end of the sequence
   * that was searched.
   *
   * @nosubgrouping
   */
  template<typename _Bi_iter,
	   typename _Alloc = allocator<sub_match<_Bi_iter> > >
    class match_results
    : private std::vector<sub_match<_Bi_iter>, _Alloc>
    {
    private:
      /*
       * The vector base is empty if this does not represent a successful match.
       * Otherwise it contains n+3 elements where n is the number of marked
       * sub-expressions:
       * [0] entire match
       * [1] 1st marked subexpression
       * ...
       * [n] nth marked subexpression
       * [n+1] prefix
       * [n+2] suffix
       */
      typedef std::vector<sub_match<_Bi_iter>, _Alloc>     _Base_type;
      typedef std::iterator_traits<_Bi_iter>   	   	   __iter_traits;
      typedef regex_constants::match_flag_type		   match_flag_type;

    public:
      /**
       * @name 10.? Public Types
       */
      //@{
      typedef _Alloc                                       allocator_type;
      typedef sub_match<_Bi_iter>                          value_type;
      typedef const value_type&                            const_reference;
      typedef const_reference                              reference;
      typedef typename _Base_type::const_iterator          const_iterator;
      typedef const_iterator                               iterator;
      typedef typename __iter_traits::difference_type	   difference_type;
      typedef typename __iter_traits::value_type 	   char_type;
      typedef typename allocator_traits<_Alloc>::size_type size_type;


      typedef std::basic_string<char_type>                 string_type;
      //@}
  
    public:
      /**
       * @name 28.10.1 Construction, Copying, and Destruction
       */
      //@{

      /**
       * @brief Constructs a default %match_results container.
       * @post size() returns 0 and str() returns an empty string.
       */
      explicit
      match_results(const _Alloc& __a = _Alloc())
      : _Base_type(__a)
      { }

      /**
       * @brief Copy constructs a %match_results.
       */
      match_results(const match_results& __rhs)
      : _Base_type(__rhs)
      { }

      /**
       * @brief Move constructs a %match_results.
       */
      match_results(match_results&& __rhs) noexcept
      : _Base_type(std::move(__rhs))
      { }

      /**
       * @brief Assigns rhs to *this.
       */
      match_results&
      operator=(const match_results& __rhs)
      {
	match_results(__rhs).swap(*this);
	return *this;
      }

      /**
       * @brief Move-assigns rhs to *this.
       */
      match_results&
      operator=(match_results&& __rhs)
      {
	match_results(std::move(__rhs)).swap(*this);
	return *this;
      }

      /**
       * @brief Destroys a %match_results object.
       */
      ~match_results()
      { }
      
      //@}

      // 28.10.2, state:
      /**
       * @brief Indicates if the %match_results is ready.
       * @retval true   The object has a fully-established result state.
       * @retval false  The object is not ready.
       */
      bool ready() const { return !_Base_type::empty(); }

      /**
       * @name 28.10.2 Size
       */
      //@{

      /**
       * @brief Gets the number of matches and submatches.
       *
       * The number of matches for a given regular expression will be either 0
       * if there was no match or mark_count() + 1 if a match was successful.
       * Some matches may be empty.
       *
       * @returns the number of matches found.
       */
      size_type
      size() const
      {
      	size_type __size = _Base_type::size();
      	return (__size && _Base_type::operator[](0).matched) ? __size - 2 : 0;
      }
      
      size_type
      max_size() const
      { return _Base_type::max_size(); }

      /**
       * @brief Indicates if the %match_results contains no results.
       * @retval true The %match_results object is empty.
       * @retval false The %match_results object is not empty.
       */
      bool
      empty() const
      { return size() == 0; }
      
      //@}

      /**
       * @name 10.3 Element Access
       */
      //@{

      /**
       * @brief Gets the length of the indicated submatch.
       * @param __sub indicates the submatch.
       * @pre   ready() == true
       *
       * This function returns the length of the indicated submatch, or the
       * length of the entire match if @p __sub is zero (the default).
       */
      difference_type
      length(size_type __sub = 0) const
      { return (*this)[__sub].length(); }

      /**
       * @brief Gets the offset of the beginning of the indicated submatch.
       * @param __sub indicates the submatch.
       * @pre   ready() == true
       *
       * This function returns the offset from the beginning of the target
       * sequence to the beginning of the submatch, unless the value of @p __sub
       * is zero (the default), in which case this function returns the offset
       * from the beginning of the target sequence to the beginning of the
       * match.
       *
       * Returns -1 if @p __sub is out of range.
       */
      difference_type
      position(size_type __sub = 0) const
      {
	return __sub < size() ? std::distance(this->prefix().first,
					      (*this)[__sub].first) : -1;
      }

      /**
       * @brief Gets the match or submatch converted to a string type.
       * @param __sub indicates the submatch.
       * @pre   ready() == true
       *
       * This function gets the submatch (or match, if @p __sub is
       * zero) extracted from the target range and converted to the
       * associated string type.
       */
      string_type
      str(size_type __sub = 0) const
      { return (*this)[__sub].str(); }
      
      /**
       * @brief Gets a %sub_match reference for the match or submatch.
       * @param __sub indicates the submatch.
       * @pre   ready() == true
       *
       * This function gets a reference to the indicated submatch, or
       * the entire match if @p __sub is zero.
       *
       * If @p __sub >= size() then this function returns a %sub_match with a
       * special value indicating no submatch.
       */
      const_reference
      operator[](size_type __sub) const
      { 
      	_GLIBCXX_DEBUG_ASSERT( ready() );
      	return __sub < size()
	       ?  _Base_type::operator[](__sub)
	       : __unmatched_sub<_Bi_iter>();
      }

      /**
       * @brief Gets a %sub_match representing the match prefix.
       * @pre   ready() == true
       *
       * This function gets a reference to a %sub_match object representing the
       * part of the target range between the start of the target range and the
       * start of the match.
       */
      const_reference
      prefix() const
      {
      	_GLIBCXX_DEBUG_ASSERT( ready() );
      	return !empty()
      	       ? _Base_type::operator[](_Base_type::size() - 2)
	       : __unmatched_sub<_Bi_iter>();
      }

      /**
       * @brief Gets a %sub_match representing the match suffix.
       * @pre   ready() == true
       *
       * This function gets a reference to a %sub_match object representing the
       * part of the target range between the end of the match and the end of
       * the target range.
       */
      const_reference
      suffix() const
      {
	_GLIBCXX_DEBUG_ASSERT( ready() );
	return !empty()
	       ? _Base_type::operator[](_Base_type::size() - 1)
	       : __unmatched_sub<_Bi_iter>();
      }

      /**
       * @brief Gets an iterator to the start of the %sub_match collection.
       */
      const_iterator
      begin() const
      { return _Base_type::begin(); }
      
      /**
       * @brief Gets an iterator to the start of the %sub_match collection.
       */
      const_iterator
      cbegin() const
      { return _Base_type::cbegin(); }

      /**
       * @brief Gets an iterator to one-past-the-end of the collection.
       */
      const_iterator
      end() const
      { return !empty() ? _Base_type::end() - 2 : _Base_type::end(); }
      
      /**
       * @brief Gets an iterator to one-past-the-end of the collection.
       */
      const_iterator
      cend() const
      { return end(); }

      //@}

      /**
       * @name 10.4 Formatting
       *
       * These functions perform formatted substitution of the matched
       * character sequences into their target.  The format specifiers and
       * escape sequences accepted by these functions are determined by
       * their @p flags parameter as documented above.
       */
       //@{

      /**
       * @pre   ready() == true
       * @todo Implement this function.
       */
      template<typename _Out_iter>
        _Out_iter
        format(_Out_iter __out, const char_type* __fmt_first,
	       const char_type* __fmt_last,
	       match_flag_type __flags = regex_constants::format_default) const
        { return __out; }

      /**
       * @pre   ready() == true
       */
      template<typename _Out_iter, typename _St, typename _Sa>
        _Out_iter
        format(_Out_iter __out, const basic_string<char_type, _St, _Sa>& __fmt,
	       match_flag_type __flags = regex_constants::format_default) const
        {
          return format(__out, __fmt.data(), __fmt.data() + __fmt.size(),
                        __flags);
        }

      /**
       * @pre   ready() == true
       */
      template<typename _Out_iter, typename _St, typename _Sa>
        basic_string<char_type, _St, _Sa>
        format(const basic_string<char_type, _St, _Sa>& __fmt,
	       match_flag_type __flags = regex_constants::format_default) const
        {
          basic_string<char_type, _St, _Sa> __result;
          format(std::back_inserter(__result), __fmt, __flags);
          return __result;
        }

      /**
       * @pre   ready() == true
       */
      string_type
      format(const char_type* __fmt,
	     match_flag_type __flags = regex_constants::format_default) const
      {
        string_type __result;
        format(std::back_inserter(__result),
               __fmt + char_traits<char_type>::length(__fmt),
               __flags);
        return __result;
      }

      //@} 

      /**
       * @name 10.5 Allocator
       */
      //@{ 

      /**
       * @brief Gets a copy of the allocator.
       */
      allocator_type
      get_allocator() const
      { return _Base_type::get_allocator(); }
      
      //@} 

      /**
       * @name 10.6 Swap
       */
       //@{ 

      /**
       * @brief Swaps the contents of two match_results.
       */
      void
      swap(match_results& __that)
      { _Base_type::swap(__that); }
      //@} 
      
    private:
      friend class __detail::_SpecializedResults<_Bi_iter, _Alloc>;
    };
  
  typedef match_results<const char*>             cmatch;
  typedef match_results<string::const_iterator>  smatch;
#ifdef _GLIBCXX_USE_WCHAR_T
  typedef match_results<const wchar_t*>          wcmatch;
  typedef match_results<wstring::const_iterator> wsmatch;
#endif

  // match_results comparisons
  /**
   * @brief Compares two match_results for equality.
   * @returns true if the two objects refer to the same match,
   * false otherwise.
   */
  template<typename _Bi_iter, typename _Alloc>
    inline bool
    operator==(const match_results<_Bi_iter, _Alloc>& __m1,
	       const match_results<_Bi_iter, _Alloc>& __m2)
    {
      if (__m1.ready() != __m2.ready())
        return false;
      if (!__m1.ready())  // both are not ready
        return true;
      if (__m1.empty() != __m2.empty())
        return false;
      if (__m1.empty())   // both are empty
        return true;
      return __m1.prefix() == __m2.prefix()
        && __m1.size() == __m2.size()
        && std::equal(__m1.begin(), __m1.end(), __m2.begin())
        && __m1.suffix() == __m2.suffix();
    }

  /**
   * @brief Compares two match_results for inequality.
   * @returns true if the two objects do not refer to the same match,
   * false otherwise.
   */
  template<typename _Bi_iter, class _Alloc>
    inline bool
    operator!=(const match_results<_Bi_iter, _Alloc>& __m1,
	       const match_results<_Bi_iter, _Alloc>& __m2)
    { return !(__m1 == __m2); }

  // [7.10.6] match_results swap
  /**
   * @brief Swaps two match results.
   * @param __lhs A match result.
   * @param __rhs A match result.
   *
   * The contents of the two match_results objects are swapped.
   */
  template<typename _Bi_iter, typename _Alloc>
    inline void
    swap(match_results<_Bi_iter, _Alloc>& __lhs,
	 match_results<_Bi_iter, _Alloc>& __rhs)
    { __lhs.swap(__rhs); }

  // [7.11.2] Function template regex_match
  /**
   * @name Matching, Searching, and Replacing
   */
  //@{

  /**
   * @brief Determines if there is a match between the regular expression @p e
   * and all of the character sequence [first, last).
   *
   * @param __s     Start of the character sequence to match.
   * @param __e     One-past-the-end of the character sequence to match.
   * @param __m     The match results.
   * @param __re    The regular expression.
   * @param __flags Controls how the regular expression is matched.
   *
   * @retval true  A match exists.
   * @retval false Otherwise.
   *
   * @throws an exception of type regex_error.
   *
   * @todo Implement this function.
   */
  template<typename _Bi_iter, typename _Alloc,
	   typename _Ch_type, typename _Rx_traits>
    bool
    regex_match(_Bi_iter                                 __s,
                _Bi_iter                                 __e,
                match_results<_Bi_iter, _Alloc>&     __m,
                const basic_regex<_Ch_type, _Rx_traits>& __re,
                regex_constants::match_flag_type         __flags
                               = regex_constants::match_default)
    {
      __detail::_AutomatonPtr __a = __re._M_get_automaton();
      __detail::_Automaton::_SizeT __sz = __a->_M_sub_count();
      __detail::_SpecializedCursor<_Bi_iter> __cs(__s, __e);
      __detail::_SpecializedResults<_Bi_iter, _Alloc> __r(__sz, __cs, __m);
      __detail::_Grep_matcher __matcher(__cs, __r, __a, __flags);
      return __m[0].matched;
    }

  /**
   * @brief Indicates if there is a match between the regular expression @p e
   * and all of the character sequence [first, last).
   *
   * @param __first Beginning of the character sequence to match.
   * @param __last  One-past-the-end of the character sequence to match.
   * @param __re    The regular expression.
   * @param __flags Controls how the regular expression is matched.
   *
   * @retval true  A match exists.
   * @retval false Otherwise.
   *
   * @throws an exception of type regex_error.
   */
  template<typename _Bi_iter, typename _Ch_type, typename _Rx_traits>
    bool
    regex_match(_Bi_iter __first, _Bi_iter __last,
		const basic_regex<_Ch_type, _Rx_traits>& __re,
		regex_constants::match_flag_type __flags
		= regex_constants::match_default)
    { 
      match_results<_Bi_iter> __what;
      return regex_match(__first, __last, __what, __re, __flags);
    }

  /**
   * @brief Determines if there is a match between the regular expression @p e
   * and a C-style null-terminated string.
   *
   * @param __s  The C-style null-terminated string to match.
   * @param __m  The match results.
   * @param __re The regular expression.
   * @param __f  Controls how the regular expression is matched.
   *
   * @retval true  A match exists.
   * @retval false Otherwise.
   *
   * @throws an exception of type regex_error.
   */
  template<typename _Ch_type, typename _Alloc, typename _Rx_traits>
    inline bool
    regex_match(const _Ch_type* __s,
		match_results<const _Ch_type*, _Alloc>& __m,
		const basic_regex<_Ch_type, _Rx_traits>& __re,
		regex_constants::match_flag_type __f
		= regex_constants::match_default)
    { return regex_match(__s, __s + _Rx_traits::length(__s), __m, __re, __f); }

  /**
   * @brief Determines if there is a match between the regular expression @p e
   * and a string.
   *
   * @param __s     The string to match.
   * @param __m     The match results.
   * @param __re    The regular expression.
   * @param __flags Controls how the regular expression is matched.
   *
   * @retval true  A match exists.
   * @retval false Otherwise.
   *
   * @throws an exception of type regex_error.
   */
  template<typename _Ch_traits, typename _Ch_alloc,
	   typename _Alloc, typename _Ch_type, typename _Rx_traits>
    inline bool
    regex_match(const basic_string<_Ch_type, _Ch_traits, _Ch_alloc>& __s,
		match_results<typename basic_string<_Ch_type, 
		_Ch_traits, _Ch_alloc>::const_iterator, _Alloc>& __m,
		const basic_regex<_Ch_type, _Rx_traits>& __re,
		regex_constants::match_flag_type __flags
		= regex_constants::match_default)
    { return regex_match(__s.begin(), __s.end(), __m, __re, __flags); }

  /**
   * @brief Indicates if there is a match between the regular expression @p e
   * and a C-style null-terminated string.
   *
   * @param __s  The C-style null-terminated string to match.
   * @param __re The regular expression.
   * @param __f  Controls how the regular expression is matched.
   *
   * @retval true  A match exists.
   * @retval false Otherwise.
   *
   * @throws an exception of type regex_error.
   */
  template<typename _Ch_type, class _Rx_traits>
    inline bool
    regex_match(const _Ch_type* __s,
		const basic_regex<_Ch_type, _Rx_traits>& __re,
		regex_constants::match_flag_type __f
		= regex_constants::match_default)
    { return regex_match(__s, __s + _Rx_traits::length(__s), __re, __f); }

  /**
   * @brief Indicates if there is a match between the regular expression @p e
   * and a string.
   *
   * @param __s     [IN] The string to match.
   * @param __re    [IN] The regular expression.
   * @param __flags [IN] Controls how the regular expression is matched.
   *
   * @retval true  A match exists.
   * @retval false Otherwise.
   *
   * @throws an exception of type regex_error.
   */
  template<typename _Ch_traits, typename _Str_allocator,
	   typename _Ch_type, typename _Rx_traits>
    inline bool
    regex_match(const basic_string<_Ch_type, _Ch_traits, _Str_allocator>& __s,
		const basic_regex<_Ch_type, _Rx_traits>& __re,
		regex_constants::match_flag_type __flags
		= regex_constants::match_default)
    { return regex_match(__s.begin(), __s.end(), __re, __flags); }

  // [7.11.3] Function template regex_search
  /**
   * Searches for a regular expression within a range.
   * @param __first [IN]  The start of the string to search.
   * @param __last  [IN]  One-past-the-end of the string to search.
   * @param __m     [OUT] The match results.
   * @param __re    [IN]  The regular expression to search for.
   * @param __flags [IN]  Search policy flags.
   * @retval true  A match was found within the string.
   * @retval false No match was found within the string, the content of %m is
   *               undefined.
   *
   * @throws an exception of type regex_error.
   *
   * @todo Implement this function.
   */
  template<typename _Bi_iter, typename _Alloc,
	   typename _Ch_type, typename _Rx_traits>
    inline bool
    regex_search(_Bi_iter __first, _Bi_iter __last,
		 match_results<_Bi_iter, _Alloc>& __m,
		 const basic_regex<_Ch_type, _Rx_traits>& __re,
		 regex_constants::match_flag_type __flags
		 = regex_constants::match_default)
    { return false; }

  /**
   * Searches for a regular expression within a range.
   * @param __first [IN]  The start of the string to search.
   * @param __last  [IN]  One-past-the-end of the string to search.
   * @param __re    [IN]  The regular expression to search for.
   * @param __flags [IN]  Search policy flags.
   * @retval true  A match was found within the string.
   * @retval false No match was found within the string.
   * @doctodo
   *
   * @throws an exception of type regex_error.
   */
  template<typename _Bi_iter, typename _Ch_type, typename _Rx_traits>
    inline bool
    regex_search(_Bi_iter __first, _Bi_iter __last,
		 const basic_regex<_Ch_type, _Rx_traits>& __re,
		 regex_constants::match_flag_type __flags
		 = regex_constants::match_default)
    {
      match_results<_Bi_iter> __what;
      return regex_search(__first, __last, __what, __re, __flags);
    }

  /**
   * @brief Searches for a regular expression within a C-string.
   * @param __s [IN]  A C-string to search for the regex.
   * @param __m [OUT] The set of regex matches.
   * @param __e [IN]  The regex to search for in @p s.
   * @param __f [IN]  The search flags.
   * @retval true  A match was found within the string.
   * @retval false No match was found within the string, the content of %m is
   *               undefined.
   * @doctodo
   *
   * @throws an exception of type regex_error.
   */
  template<typename _Ch_type, class _Alloc, class _Rx_traits>
    inline bool
    regex_search(const _Ch_type* __s,
		 match_results<const _Ch_type*, _Alloc>& __m,
		 const basic_regex<_Ch_type, _Rx_traits>& __e,
		 regex_constants::match_flag_type __f
		 = regex_constants::match_default)
    { return regex_search(__s, __s + _Rx_traits::length(__s), __m, __e, __f); }

  /**
   * @brief Searches for a regular expression within a C-string.
   * @param __s [IN]  The C-string to search.
   * @param __e [IN]  The regular expression to search for.
   * @param __f [IN]  Search policy flags.
   * @retval true  A match was found within the string.
   * @retval false No match was found within the string.
   * @doctodo
   *
   * @throws an exception of type regex_error.
   */
  template<typename _Ch_type, typename _Rx_traits>
    inline bool
    regex_search(const _Ch_type* __s,
		 const basic_regex<_Ch_type, _Rx_traits>& __e,
		 regex_constants::match_flag_type __f
		 = regex_constants::match_default)
    { return regex_search(__s, __s + _Rx_traits::length(__s), __e, __f); }

  /**
   * @brief Searches for a regular expression within a string.
   * @param __s     [IN]  The string to search.
   * @param __e     [IN]  The regular expression to search for.
   * @param __flags [IN]  Search policy flags.
   * @retval true  A match was found within the string.
   * @retval false No match was found within the string.
   * @doctodo
   *
   * @throws an exception of type regex_error.
   */
  template<typename _Ch_traits, typename _String_allocator,
	   typename _Ch_type, typename _Rx_traits>
    inline bool
    regex_search(const basic_string<_Ch_type, _Ch_traits,
		 _String_allocator>& __s,
		 const basic_regex<_Ch_type, _Rx_traits>& __e,
		 regex_constants::match_flag_type __flags
		 = regex_constants::match_default)
    { return regex_search(__s.begin(), __s.end(), __e, __flags); }

  /**
   * @brief Searches for a regular expression within a string.
   * @param __s [IN]  A C++ string to search for the regex.
   * @param __m [OUT] The set of regex matches.
   * @param __e [IN]  The regex to search for in @p s.
   * @param __f [IN]  The search flags.
   * @retval true  A match was found within the string.
   * @retval false No match was found within the string, the content of %m is
   *               undefined.
   *
   * @throws an exception of type regex_error.
   */
  template<typename _Ch_traits, typename _Ch_alloc,
	   typename _Alloc, typename _Ch_type,
	   typename _Rx_traits>
    inline bool
    regex_search(const basic_string<_Ch_type, _Ch_traits, _Ch_alloc>& __s,
		 match_results<typename basic_string<_Ch_type,
		 _Ch_traits, _Ch_alloc>::const_iterator, _Alloc>& __m,
		 const basic_regex<_Ch_type, _Rx_traits>& __e,
		 regex_constants::match_flag_type __f
		 = regex_constants::match_default)
    { return regex_search(__s.begin(), __s.end(), __m, __e, __f); }

  // std [28.11.4] Function template regex_replace
  /**
   * @doctodo
   * @param __out
   * @param __first
   * @param __last
   * @param __e
   * @param __fmt
   * @param __flags
   *
   * @returns out
   * @throws an exception of type regex_error.
   *
   * @todo Implement this function.
   */
  template<typename _Out_iter, typename _Bi_iter,
	   typename _Rx_traits, typename _Ch_type>
    inline _Out_iter
    regex_replace(_Out_iter __out, _Bi_iter __first, _Bi_iter __last,
		  const basic_regex<_Ch_type, _Rx_traits>& __e,
		  const basic_string<_Ch_type>& __fmt,
		  regex_constants::match_flag_type __flags
		  = regex_constants::match_default)
    { return __out; }

  /**
   * @doctodo
   * @param __s
   * @param __e
   * @param __fmt
   * @param __flags
   *
   * @returns a copy of string @p s with replacements.
   *
   * @throws an exception of type regex_error.
   */
  template<typename _Rx_traits, typename _Ch_type>
    inline basic_string<_Ch_type>
    regex_replace(const basic_string<_Ch_type>& __s,
		  const basic_regex<_Ch_type, _Rx_traits>& __e,
		  const basic_string<_Ch_type>& __fmt,
		  regex_constants::match_flag_type __flags
		  = regex_constants::match_default)
    {
      basic_string<_Ch_type> __result;
      regex_replace(std::back_inserter(__result),
		    __s.begin(), __s.end(), __e, __fmt, __flags);
      return __result;
    }

  //@}

  // std [28.12] Class template regex_iterator
  /**
   * An iterator adaptor that will provide repeated calls of regex_search over 
   * a range until no more matches remain.
   */
  template<typename _Bi_iter,
	   typename _Ch_type = typename iterator_traits<_Bi_iter>::value_type,
	   typename _Rx_traits = regex_traits<_Ch_type> >
    class regex_iterator
    {
    public:
      typedef basic_regex<_Ch_type, _Rx_traits>  regex_type;
      typedef match_results<_Bi_iter>            value_type;
      typedef std::ptrdiff_t                     difference_type;
      typedef const value_type*                  pointer;
      typedef const value_type&                  reference;
      typedef std::forward_iterator_tag          iterator_category;

      /**
       * @brief Provides a singular iterator, useful for indicating
       * one-past-the-end of a range.
       * @todo Implement this function.
       * @doctodo
       */
      regex_iterator();
      
      /**
       * Constructs a %regex_iterator...
       * @param __a  [IN] The start of a text range to search.
       * @param __b  [IN] One-past-the-end of the text range to search.
       * @param __re [IN] The regular expression to match.
       * @param __m  [IN] Policy flags for match rules.
       * @todo Implement this function.
       * @doctodo
       */
      regex_iterator(_Bi_iter __a, _Bi_iter __b, const regex_type& __re,
		     regex_constants::match_flag_type __m
		     = regex_constants::match_default);

      /**
       * Copy constructs a %regex_iterator.
       * @todo Implement this function.
       * @doctodo
       */
      regex_iterator(const regex_iterator& __rhs);
      
      /**
       * @todo Implement this function.
       * @doctodo
       */
      regex_iterator&
      operator=(const regex_iterator& __rhs);
      
      /**
       * @todo Implement this function.
       * @doctodo
       */
      bool
      operator==(const regex_iterator& __rhs);
      
      /**
       * @todo Implement this function.
       * @doctodo
       */
      bool
      operator!=(const regex_iterator& __rhs);
      
      /**
       * @todo Implement this function.
       * @doctodo
       */
      const value_type&
      operator*();
      
      /**
       * @todo Implement this function.
       * @doctodo
       */
      const value_type*
      operator->();
      
      /**
       * @todo Implement this function.
       * @doctodo
       */
      regex_iterator&
      operator++();
      
      /**
       * @todo Implement this function.
       * @doctodo
       */
      regex_iterator
      operator++(int);
      
    private:
      // these members are shown for exposition only:
      _Bi_iter                         begin;
      _Bi_iter                         end;
      const regex_type*                pregex;
      regex_constants::match_flag_type flags;
      match_results<_Bi_iter>          match;
    };
  
  typedef regex_iterator<const char*>             cregex_iterator;
  typedef regex_iterator<string::const_iterator>  sregex_iterator;
#ifdef _GLIBCXX_USE_WCHAR_T
  typedef regex_iterator<const wchar_t*>          wcregex_iterator;
  typedef regex_iterator<wstring::const_iterator> wsregex_iterator;
#endif

  // [7.12.2] Class template regex_token_iterator
  /**
   * Iterates over submatches in a range (or @a splits a text string).
   *
   * The purpose of this iterator is to enumerate all, or all specified,
   * matches of a regular expression within a text range.  The dereferenced
   * value of an iterator of this class is a std::sub_match object.
   */
  template<typename _Bi_iter,
	   typename _Ch_type = typename iterator_traits<_Bi_iter>::value_type,
	   typename _Rx_traits = regex_traits<_Ch_type> >
    class regex_token_iterator
    {
    public:
      typedef basic_regex<_Ch_type, _Rx_traits> regex_type;
      typedef sub_match<_Bi_iter>               value_type;
      typedef std::ptrdiff_t                    difference_type;
      typedef const value_type*                 pointer;
      typedef const value_type&                 reference;
      typedef std::forward_iterator_tag         iterator_category;
      
    public:
      /**
       * @brief Default constructs a %regex_token_iterator.
       * @todo Implement this function.
       * 
       * A default-constructed %regex_token_iterator is a singular iterator
       * that will compare equal to the one-past-the-end value for any
       * iterator of the same type.
       */
      regex_token_iterator();
      
      /**
       * Constructs a %regex_token_iterator...
       * @param __a          [IN] The start of the text to search.
       * @param __b          [IN] One-past-the-end of the text to search.
       * @param __re         [IN] The regular expression to search for.
       * @param __submatch   [IN] Which submatch to return.  There are some
       *                        special values for this parameter:
       *                        - -1 each enumerated subexpression does NOT
       *                          match the regular expression (aka field
       *                          splitting)
       *                        - 0 the entire string matching the
       *                          subexpression is returned for each match
       *                          within the text.
       *                        - >0 enumerates only the indicated
       *                          subexpression from a match within the text.
       * @param __m          [IN] Policy flags for match rules.
       *
       * @todo Implement this function.
       * @doctodo
       */
      regex_token_iterator(_Bi_iter __a, _Bi_iter __b, const regex_type& __re,
			   int __submatch = 0,
			   regex_constants::match_flag_type __m
			   = regex_constants::match_default);

      /**
       * Constructs a %regex_token_iterator...
       * @param __a          [IN] The start of the text to search.
       * @param __b          [IN] One-past-the-end of the text to search.
       * @param __re         [IN] The regular expression to search for.
       * @param __submatches [IN] A list of subexpressions to return for each
       *                        regular expression match within the text.
       * @param __m          [IN] Policy flags for match rules.
       *
       * @todo Implement this function.
       * @doctodo
       */
      regex_token_iterator(_Bi_iter __a, _Bi_iter __b,
			   const regex_type& __re,
			   const std::vector<int>& __submatches,
			   regex_constants::match_flag_type __m
			     = regex_constants::match_default);

      /**
       * Constructs a %regex_token_iterator...
       * @param __a          [IN] The start of the text to search.
       * @param __b          [IN] One-past-the-end of the text to search.
       * @param __re         [IN] The regular expression to search for.
       * @param __submatches [IN] A list of subexpressions to return for each
       *                          regular expression match within the text.
       * @param __m          [IN] Policy flags for match rules.
       
       * @todo Implement this function.
       * @doctodo
       */
      template<std::size_t _Nm>
        regex_token_iterator(_Bi_iter __a, _Bi_iter __b,
			     const regex_type& __re,
			     const int (&__submatches)[_Nm],
			     regex_constants::match_flag_type __m
			     = regex_constants::match_default);

      /**
       * @brief Copy constructs a %regex_token_iterator.
       * @param __rhs [IN] A %regex_token_iterator to copy.
       * @todo Implement this function.
       */
      regex_token_iterator(const regex_token_iterator& __rhs);
      
      /**
       * @brief Assigns a %regex_token_iterator to another.
       * @param __rhs [IN] A %regex_token_iterator to copy.
       * @todo Implement this function.
       */
      regex_token_iterator&
      operator=(const regex_token_iterator& __rhs);
      
      /**
       * @brief Compares a %regex_token_iterator to another for equality.
       * @todo Implement this function.
       */
      bool
      operator==(const regex_token_iterator& __rhs);
      
      /**
       * @brief Compares a %regex_token_iterator to another for inequality.
       * @todo Implement this function.
       */
      bool
      operator!=(const regex_token_iterator& __rhs);
      
      /**
       * @brief Dereferences a %regex_token_iterator.
       * @todo Implement this function.
       */
      const value_type&
      operator*();
      
      /**
       * @brief Selects a %regex_token_iterator member.
       * @todo Implement this function.
       */
      const value_type*
      operator->();
      
      /**
       * @brief Increments a %regex_token_iterator.
       * @todo Implement this function.
       */
      regex_token_iterator&
      operator++();
      
      /**
       * @brief Postincrements a %regex_token_iterator.
       * @todo Implement this function.
       */
      regex_token_iterator
      operator++(int);
      
    private: // data members for exposition only:
      typedef regex_iterator<_Bi_iter, _Ch_type, _Rx_traits> position_iterator;

      position_iterator __position;
      const value_type* __result;
      value_type        __suffix;
      std::size_t       __n;
      std::vector<int>  __subs;
    };

  /** @brief Token iterator for C-style NULL-terminated strings. */
  typedef regex_token_iterator<const char*>             cregex_token_iterator;

  /** @brief Token iterator for standard strings. */
  typedef regex_token_iterator<string::const_iterator>  sregex_token_iterator;

#ifdef _GLIBCXX_USE_WCHAR_T
  /** @brief Token iterator for C-style NULL-terminated wide strings. */
  typedef regex_token_iterator<const wchar_t*>          wcregex_token_iterator;

  /** @brief Token iterator for standard wide-character strings. */
  typedef regex_token_iterator<wstring::const_iterator> wsregex_token_iterator;
#endif
  
  //@} // group regex
_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

