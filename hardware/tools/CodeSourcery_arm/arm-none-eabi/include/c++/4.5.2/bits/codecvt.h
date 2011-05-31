// Locale support (codecvt) -*- C++ -*-

// Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008,
// 2009, 2010  Free Software Foundation, Inc.
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

/** @file bits/codecvt.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

//
// ISO C++ 14882: 22.2.1.5 Template class codecvt
//

// Written by Benjamin Kosnik <bkoz@redhat.com>

#ifndef _CODECVT_H
#define _CODECVT_H 1

#pragma GCC system_header

_GLIBCXX_BEGIN_NAMESPACE(std)

  /// Empty base class for codecvt facet [22.2.1.5].
  class codecvt_base
  {
  public:
    enum result
    {
      ok,
      partial,
      error,
      noconv
    };
  };

  /**
   *  @brief  Common base for codecvt functions.
   *
   *  This template class provides implementations of the public functions
   *  that forward to the protected virtual functions.
   *
   *  This template also provides abstract stubs for the protected virtual
   *  functions.
  */
  template<typename _InternT, typename _ExternT, typename _StateT>
    class __codecvt_abstract_base
    : public locale::facet, public codecvt_base
    {
    public:
      // Types:
      typedef codecvt_base::result	result;
      typedef _InternT			intern_type;
      typedef _ExternT			extern_type;
      typedef _StateT			state_type;

      // 22.2.1.5.1 codecvt members
      /**
       *  @brief  Convert from internal to external character set.
       *
       *  Converts input string of intern_type to output string of
       *  extern_type.  This is analogous to wcsrtombs.  It does this by
       *  calling codecvt::do_out.
       *
       *  The source and destination character sets are determined by the
       *  facet's locale, internal and external types.
       *
       *  The characters in [from,from_end) are converted and written to
       *  [to,to_end).  from_next and to_next are set to point to the
       *  character following the last successfully converted character,
       *  respectively.  If the result needed no conversion, from_next and
       *  to_next are not affected.
       *
       *  The @a state argument should be initialized if the input is at the
       *  beginning and carried from a previous call if continuing
       *  conversion.  There are no guarantees about how @a state is used.
       *
       *  The result returned is a member of codecvt_base::result.  If
       *  all the input is converted, returns codecvt_base::ok.  If no
       *  conversion is necessary, returns codecvt_base::noconv.  If
       *  the input ends early or there is insufficient space in the
       *  output, returns codecvt_base::partial.  Otherwise the
       *  conversion failed and codecvt_base::error is returned.
       *
       *  @param  state  Persistent conversion state data.
       *  @param  from  Start of input.
       *  @param  from_end  End of input.
       *  @param  from_next  Returns start of unconverted data.
       *  @param  to  Start of output buffer.
       *  @param  to_end  End of output buffer.
       *  @param  to_next  Returns start of unused output area.
       *  @return  codecvt_base::result.
      */
      result
      out(state_type& __state, const intern_type* __from,
	  const intern_type* __from_end, const intern_type*& __from_next,
	  extern_type* __to, extern_type* __to_end,
	  extern_type*& __to_next) const
      {
	return this->do_out(__state, __from, __from_end, __from_next,
			    __to, __to_end, __to_next);
      }

      /**
       *  @brief  Reset conversion state.
       *
       *  Writes characters to output that would restore @a state to initial
       *  conditions.  The idea is that if a partial conversion occurs, then
       *  the converting the characters written by this function would leave
       *  the state in initial conditions, rather than partial conversion
       *  state.  It does this by calling codecvt::do_unshift().
       *
       *  For example, if 4 external characters always converted to 1 internal
       *  character, and input to in() had 6 external characters with state
       *  saved, this function would write two characters to the output and
       *  set the state to initialized conditions.
       *
       *  The source and destination character sets are determined by the
       *  facet's locale, internal and external types.
       *
       *  The result returned is a member of codecvt_base::result.  If the
       *  state could be reset and data written, returns codecvt_base::ok.  If
       *  no conversion is necessary, returns codecvt_base::noconv.  If the
       *  output has insufficient space, returns codecvt_base::partial.
       *  Otherwise the reset failed and codecvt_base::error is returned.
       *
       *  @param  state  Persistent conversion state data.
       *  @param  to  Start of output buffer.
       *  @param  to_end  End of output buffer.
       *  @param  to_next  Returns start of unused output area.
       *  @return  codecvt_base::result.
      */
      result
      unshift(state_type& __state, extern_type* __to, extern_type* __to_end,
	      extern_type*& __to_next) const
      { return this->do_unshift(__state, __to,__to_end,__to_next); }

      /**
       *  @brief  Convert from external to internal character set.
       *
       *  Converts input string of extern_type to output string of
       *  intern_type.  This is analogous to mbsrtowcs.  It does this by
       *  calling codecvt::do_in.
       *
       *  The source and destination character sets are determined by the
       *  facet's locale, internal and external types.
       *
       *  The characters in [from,from_end) are converted and written to
       *  [to,to_end).  from_next and to_next are set to point to the
       *  character following the last successfully converted character,
       *  respectively.  If the result needed no conversion, from_next and
       *  to_next are not affected.
       *
       *  The @a state argument should be initialized if the input is at the
       *  beginning and carried from a previous call if continuing
       *  conversion.  There are no guarantees about how @a state is used.
       *
       *  The result returned is a member of codecvt_base::result.  If
       *  all the input is converted, returns codecvt_base::ok.  If no
       *  conversion is necessary, returns codecvt_base::noconv.  If
       *  the input ends early or there is insufficient space in the
       *  output, returns codecvt_base::partial.  Otherwise the
       *  conversion failed and codecvt_base::error is returned.
       *
       *  @param  state  Persistent conversion state data.
       *  @param  from  Start of input.
       *  @param  from_end  End of input.
       *  @param  from_next  Returns start of unconverted data.
       *  @param  to  Start of output buffer.
       *  @param  to_end  End of output buffer.
       *  @param  to_next  Returns start of unused output area.
       *  @return  codecvt_base::result.
      */
      result
      in(state_type& __state, const extern_type* __from,
	 const extern_type* __from_end, const extern_type*& __from_next,
	 intern_type* __to, intern_type* __to_end,
	 intern_type*& __to_next) const
      {
	return this->do_in(__state, __from, __from_end, __from_next,
			   __to, __to_end, __to_next);
      }

      int
      encoding() const throw()
      { return this->do_encoding(); }

      bool
      always_noconv() const throw()
      { return this->do_always_noconv(); }

      int
      length(state_type& __state, const extern_type* __from,
	     const extern_type* __end, size_t __max) const
      { return this->do_length(__state, __from, __end, __max); }

      int
      max_length() const throw()
      { return this->do_max_length(); }

    protected:
      explicit
      __codecvt_abstract_base(size_t __refs = 0) : locale::facet(__refs) { }

      virtual
      ~__codecvt_abstract_base() { }

      /**
       *  @brief  Convert from internal to external character set.
       *
       *  Converts input string of intern_type to output string of
       *  extern_type.  This function is a hook for derived classes to change
       *  the value returned.  @see out for more information.
      */
      virtual result
      do_out(state_type& __state, const intern_type* __from,
	     const intern_type* __from_end, const intern_type*& __from_next,
	     extern_type* __to, extern_type* __to_end,
	     extern_type*& __to_next) const = 0;

      virtual result
      do_unshift(state_type& __state, extern_type* __to,
		 extern_type* __to_end, extern_type*& __to_next) const = 0;

      virtual result
      do_in(state_type& __state, const extern_type* __from,
	    const extern_type* __from_end, const extern_type*& __from_next,
	    intern_type* __to, intern_type* __to_end,
	    intern_type*& __to_next) const = 0;

      virtual int
      do_encoding() const throw() = 0;

      virtual bool
      do_always_noconv() const throw() = 0;

      virtual int
      do_length(state_type&, const extern_type* __from,
		const extern_type* __end, size_t __max) const = 0;

      virtual int
      do_max_length() const throw() = 0;
    };



  /**
   *  @brief  Primary class template codecvt.
   *  @ingroup locales
   *
   *  NB: Generic, mostly useless implementation.
   *
  */
   template<typename _InternT, typename _ExternT, typename _StateT>
    class codecvt
    : public __codecvt_abstract_base<_InternT, _ExternT, _StateT>
    {
    public:
      // Types:
      typedef codecvt_base::result	result;
      typedef _InternT			intern_type;
      typedef _ExternT			extern_type;
      typedef _StateT			state_type;

    protected:
      __c_locale			_M_c_locale_codecvt;

    public:
      static locale::id			id;

      explicit
      codecvt(size_t __refs = 0)
      : __codecvt_abstract_base<_InternT, _ExternT, _StateT> (__refs) { }

      explicit
      codecvt(__c_locale __cloc, size_t __refs = 0);

    protected:
      virtual
      ~codecvt() { }

      virtual result
      do_out(state_type& __state, const intern_type* __from,
	     const intern_type* __from_end, const intern_type*& __from_next,
	     extern_type* __to, extern_type* __to_end,
	     extern_type*& __to_next) const;

      virtual result
      do_unshift(state_type& __state, extern_type* __to,
		 extern_type* __to_end, extern_type*& __to_next) const;

      virtual result
      do_in(state_type& __state, const extern_type* __from,
	    const extern_type* __from_end, const extern_type*& __from_next,
	    intern_type* __to, intern_type* __to_end,
	    intern_type*& __to_next) const;

      virtual int
      do_encoding() const throw();

      virtual bool
      do_always_noconv() const throw();

      virtual int
      do_length(state_type&, const extern_type* __from,
		const extern_type* __end, size_t __max) const;

      virtual int
      do_max_length() const throw();
    };

  template<typename _InternT, typename _ExternT, typename _StateT>
    locale::id codecvt<_InternT, _ExternT, _StateT>::id;

  /// class codecvt<char, char, mbstate_t> specialization.
  template<>
    class codecvt<char, char, mbstate_t>
    : public __codecvt_abstract_base<char, char, mbstate_t>
    {
    public:
      // Types:
      typedef char			intern_type;
      typedef char			extern_type;
      typedef mbstate_t			state_type;

    protected:
      __c_locale			_M_c_locale_codecvt;

    public:
      static locale::id id;

      explicit
      codecvt(size_t __refs = 0);

      explicit
      codecvt(__c_locale __cloc, size_t __refs = 0);

    protected:
      virtual
      ~codecvt();

      virtual result
      do_out(state_type& __state, const intern_type* __from,
	     const intern_type* __from_end, const intern_type*& __from_next,
	     extern_type* __to, extern_type* __to_end,
	     extern_type*& __to_next) const;

      virtual result
      do_unshift(state_type& __state, extern_type* __to,
		 extern_type* __to_end, extern_type*& __to_next) const;

      virtual result
      do_in(state_type& __state, const extern_type* __from,
	    const extern_type* __from_end, const extern_type*& __from_next,
	    intern_type* __to, intern_type* __to_end,
	    intern_type*& __to_next) const;

      virtual int
      do_encoding() const throw();

      virtual bool
      do_always_noconv() const throw();

      virtual int
      do_length(state_type&, const extern_type* __from,
		const extern_type* __end, size_t __max) const;

      virtual int
      do_max_length() const throw();
  };

#ifdef _GLIBCXX_USE_WCHAR_T
  /// class codecvt<wchar_t, char, mbstate_t> specialization.
  template<>
    class codecvt<wchar_t, char, mbstate_t>
    : public __codecvt_abstract_base<wchar_t, char, mbstate_t>
    {
    public:
      // Types:
      typedef wchar_t			intern_type;
      typedef char			extern_type;
      typedef mbstate_t			state_type;

    protected:
      __c_locale			_M_c_locale_codecvt;

    public:
      static locale::id			id;

      explicit
      codecvt(size_t __refs = 0);

      explicit
      codecvt(__c_locale __cloc, size_t __refs = 0);

    protected:
      virtual
      ~codecvt();

      virtual result
      do_out(state_type& __state, const intern_type* __from,
	     const intern_type* __from_end, const intern_type*& __from_next,
	     extern_type* __to, extern_type* __to_end,
	     extern_type*& __to_next) const;

      virtual result
      do_unshift(state_type& __state,
		 extern_type* __to, extern_type* __to_end,
		 extern_type*& __to_next) const;

      virtual result
      do_in(state_type& __state,
	     const extern_type* __from, const extern_type* __from_end,
	     const extern_type*& __from_next,
	     intern_type* __to, intern_type* __to_end,
	     intern_type*& __to_next) const;

      virtual
      int do_encoding() const throw();

      virtual
      bool do_always_noconv() const throw();

      virtual
      int do_length(state_type&, const extern_type* __from,
		    const extern_type* __end, size_t __max) const;

      virtual int
      do_max_length() const throw();
    };
#endif //_GLIBCXX_USE_WCHAR_T

  /// class codecvt_byname [22.2.1.6].
  template<typename _InternT, typename _ExternT, typename _StateT>
    class codecvt_byname : public codecvt<_InternT, _ExternT, _StateT>
    {
    public:
      explicit
      codecvt_byname(const char* __s, size_t __refs = 0)
      : codecvt<_InternT, _ExternT, _StateT>(__refs)
      {
	if (__builtin_strcmp(__s, "C") != 0
	    && __builtin_strcmp(__s, "POSIX") != 0)
	  {
	    this->_S_destroy_c_locale(this->_M_c_locale_codecvt);
	    this->_S_create_c_locale(this->_M_c_locale_codecvt, __s);
	  }
      }

    protected:
      virtual
      ~codecvt_byname() { }
    };

  // Inhibit implicit instantiations for required instantiations,
  // which are defined via explicit instantiations elsewhere.
  // NB: This syntax is a GNU extension.
#if _GLIBCXX_EXTERN_TEMPLATE
  extern template class codecvt_byname<char, char, mbstate_t>;

  extern template
    const codecvt<char, char, mbstate_t>&
    use_facet<codecvt<char, char, mbstate_t> >(const locale&);

  extern template
    bool
    has_facet<codecvt<char, char, mbstate_t> >(const locale&);

#ifdef _GLIBCXX_USE_WCHAR_T
  extern template class codecvt_byname<wchar_t, char, mbstate_t>;

  extern template
    const codecvt<wchar_t, char, mbstate_t>&
    use_facet<codecvt<wchar_t, char, mbstate_t> >(const locale&);

  extern template
    bool
    has_facet<codecvt<wchar_t, char, mbstate_t> >(const locale&);
#endif
#endif

_GLIBCXX_END_NAMESPACE

#endif // _CODECVT_H
