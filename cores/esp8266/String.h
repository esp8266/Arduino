/*
	String library for Wiring & Arduino
	...mostly rewritten by Paul Stoffregen...
	Copyright (c) 2009-10 Hernando Barragan.  All right reserved.
	Copyright 2011, Paul Stoffregen, paul@pjrc.com

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifdef __cplusplus

#ifndef __ARDUINO_STRINGS__
#define __ARDUINO_STRINGS__

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if defined(__AVR__)
#include "avr/pgmspace.h"
#else
#include "avr/pgmspace.h"
#endif

namespace arduino
{

	// When compiling programs with this class, the following gcc parameters
	// dramatically increase performance and memory (RAM) efficiency, typically
	// with little or no increase in code size.
	//     -felide-constructors
	//     -std=c++0x

	class __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))

	// An inherited class for holding the result of a concatenation.  These
	// result objects are assumed to be writable by subsequent concatenations.
	class StringSumHelper;

	// The string class
	class String
	{
		friend class StringSumHelper;
		// use a function pointer to allow for "if (s)" without the
		// complications of an operator bool(). for more information, see:
		// http://www.artima.com/cppsource/safebool.html
		typedef void (String::*StringIfHelperType)() const;
		void StringIfHelper() const {}

		static size_t const FLT_MAX_DECIMAL_PLACES = 10;
		static size_t const DBL_MAX_DECIMAL_PLACES = FLT_MAX_DECIMAL_PLACES;

	public:
		// constructors
		// creates a copy of the initial value.
		// if the initial value is null or invalid, or if memory allocation
		// fails, the string will be marked as invalid (i.e. "if (s)" will
		// be false).
		String(const char *cstr = "");
		String(const char *cstr, unsigned int length);
		String(const uint8_t *cstr, unsigned int length) : String((const char *)cstr, length) {}
		String(const String &str);
		String(const __FlashStringHelper *str);
		String(String &&rval);
		explicit String(char c);
		explicit String(unsigned char, unsigned char base = 10);
		explicit String(int, unsigned char base = 10);
		explicit String(unsigned int, unsigned char base = 10);
		explicit String(long, unsigned char base = 10);
		explicit String(unsigned long, unsigned char base = 10);
		explicit String(float, unsigned char decimalPlaces = 2);
		explicit String(double, unsigned char decimalPlaces = 2);
		~String(void);

		// memory management
		// return true on success, false on failure (in which case, the string
		// is left unchanged).  reserve(0), if successful, will validate an
		// invalid string (i.e., "if (s)" will be true afterwards)
		bool reserve(unsigned int size);
		inline unsigned int length(void) const { return len; }
		inline bool isEmpty(void) const { return length() == 0; }

		// creates a copy of the assigned value.  if the value is null or
		// invalid, or if the memory allocation fails, the string will be
		// marked as invalid ("if (s)" will be false).
		String &operator=(const String &rhs);
		String &operator=(const char *cstr);
		String &operator=(const __FlashStringHelper *str);
		String &operator=(String &&rval);

		// concatenate (works w/ built-in types)

		// returns true on success, false on failure (in which case, the string
		// is left unchanged).  if the argument is null or invalid, the
		// concatenation is considered unsuccessful.
		bool concat(const String &str);
		bool concat(const char *cstr);
		bool concat(const char *cstr, unsigned int length);
		bool concat(const uint8_t *cstr, unsigned int length) { return concat((const char *)cstr, length); }
		bool concat(char c);
		bool concat(unsigned char num);
		bool concat(int num);
		bool concat(unsigned int num);
		bool concat(long num);
		bool concat(unsigned long num);
		bool concat(float num);
		bool concat(double num);
		bool concat(const __FlashStringHelper *str);

		// if there's not enough memory for the concatenated value, the string
		// will be left unchanged (but this isn't signalled in any way)
		String &operator+=(const String &rhs)
		{
			concat(rhs);
			return (*this);
		}
		String &operator+=(const char *cstr)
		{
			concat(cstr);
			return (*this);
		}
		String &operator+=(char c)
		{
			concat(c);
			return (*this);
		}
		String &operator+=(unsigned char num)
		{
			concat(num);
			return (*this);
		}
		String &operator+=(int num)
		{
			concat(num);
			return (*this);
		}
		String &operator+=(unsigned int num)
		{
			concat(num);
			return (*this);
		}
		String &operator+=(long num)
		{
			concat(num);
			return (*this);
		}
		String &operator+=(unsigned long num)
		{
			concat(num);
			return (*this);
		}
		String &operator+=(float num)
		{
			concat(num);
			return (*this);
		}
		String &operator+=(double num)
		{
			concat(num);
			return (*this);
		}
		String &operator+=(const __FlashStringHelper *str)
		{
			concat(str);
			return (*this);
		}

		friend StringSumHelper &operator+(const StringSumHelper &lhs, const String &rhs);
		friend StringSumHelper &operator+(const StringSumHelper &lhs, const char *cstr);
		friend StringSumHelper &operator+(const StringSumHelper &lhs, char c);
		friend StringSumHelper &operator+(const StringSumHelper &lhs, unsigned char num);
		friend StringSumHelper &operator+(const StringSumHelper &lhs, int num);
		friend StringSumHelper &operator+(const StringSumHelper &lhs, unsigned int num);
		friend StringSumHelper &operator+(const StringSumHelper &lhs, long num);
		friend StringSumHelper &operator+(const StringSumHelper &lhs, unsigned long num);
		friend StringSumHelper &operator+(const StringSumHelper &lhs, float num);
		friend StringSumHelper &operator+(const StringSumHelper &lhs, double num);
		friend StringSumHelper &operator+(const StringSumHelper &lhs, const __FlashStringHelper *rhs);

		// comparison (only works w/ Strings and "strings")
		operator StringIfHelperType() const { return buffer ? &String::StringIfHelper : 0; }
		int compareTo(const String &s) const;
		int compareTo(const char *cstr) const;
		bool equals(const String &s) const;
		bool equals(const char *cstr) const;

		friend bool operator==(const String &a, const String &b) { return a.equals(b); }
		friend bool operator==(const String &a, const char *b) { return a.equals(b); }
		friend bool operator==(const char *a, const String &b) { return b == a; }
		friend bool operator<(const String &a, const String &b) { return a.compareTo(b) < 0; }
		friend bool operator<(const String &a, const char *b) { return a.compareTo(b) < 0; }
		friend bool operator<(const char *a, const String &b) { return b.compareTo(a) > 0; }

		friend bool operator!=(const String &a, const String &b) { return !(a == b); }
		friend bool operator!=(const String &a, const char *b) { return !(a == b); }
		friend bool operator!=(const char *a, const String &b) { return !(a == b); }
		friend bool operator>(const String &a, const String &b) { return b < a; }
		friend bool operator>(const String &a, const char *b) { return b < a; }
		friend bool operator>(const char *a, const String &b) { return b < a; }
		friend bool operator<=(const String &a, const String &b) { return !(b < a); }
		friend bool operator<=(const String &a, const char *b) { return !(b < a); }
		friend bool operator<=(const char *a, const String &b) { return !(b < a); }
		friend bool operator>=(const String &a, const String &b) { return !(a < b); }
		friend bool operator>=(const String &a, const char *b) { return !(a < b); }
		friend bool operator>=(const char *a, const String &b) { return !(a < b); }

		bool equalsIgnoreCase(const String &s) const;
		bool startsWith(const String &prefix) const;
		bool startsWith(const String &prefix, unsigned int offset) const;
		bool endsWith(const String &suffix) const;

		// character access
		char charAt(unsigned int index) const;
		void setCharAt(unsigned int index, char c);
		char operator[](unsigned int index) const;
		char &operator[](unsigned int index);
		void getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index = 0) const;
		void toCharArray(char *buf, unsigned int bufsize, unsigned int index = 0) const
		{
			getBytes((unsigned char *)buf, bufsize, index);
		}
		const char *c_str() const { return buffer; }
		char *begin() { return buffer; }
		char *end() { return buffer + length(); }
		const char *begin() const { return c_str(); }
		const char *end() const { return c_str() + length(); }

		// search
		int indexOf(char ch) const;
		int indexOf(char ch, unsigned int fromIndex) const;
		int indexOf(const String &str) const;
		int indexOf(const String &str, unsigned int fromIndex) const;
		int lastIndexOf(char ch) const;
		int lastIndexOf(char ch, unsigned int fromIndex) const;
		int lastIndexOf(const String &str) const;
		int lastIndexOf(const String &str, unsigned int fromIndex) const;
		String substring(unsigned int beginIndex) const { return substring(beginIndex, len); };
		String substring(unsigned int beginIndex, unsigned int endIndex) const;

		// modification
		void replace(char find, char replace);
		void replace(const String &find, const String &replace);
		void remove(unsigned int index);
		void remove(unsigned int index, unsigned int count);
		void toLowerCase(void);
		void toUpperCase(void);
		void trim(void);

		// parsing/conversion
		long toInt(void) const;
		float toFloat(void) const;
		double toDouble(void) const;

	protected:
		char *buffer;					 // the actual char array
		unsigned int capacity; // the array length minus one (for the '\0')
		unsigned int len;			 // the String length (not counting the '\0')
	protected:
		void init(void);
		void invalidate(void);
		bool changeBuffer(unsigned int maxStrLen);

		// copy and move
		String &copy(const char *cstr, unsigned int length);
		String &copy(const __FlashStringHelper *pstr, unsigned int length);
		void move(String &rhs);
	};

	class StringSumHelper : public String
	{
	public:
		StringSumHelper(const String &s) : String(s) {}
		StringSumHelper(const char *p) : String(p) {}
		StringSumHelper(char c) : String(c) {}
		StringSumHelper(unsigned char num) : String(num) {}
		StringSumHelper(int num) : String(num) {}
		StringSumHelper(unsigned int num) : String(num) {}
		StringSumHelper(long num) : String(num) {}
		StringSumHelper(unsigned long num) : String(num) {}
		StringSumHelper(float num) : String(num) {}
		StringSumHelper(double num) : String(num) {}
	};

} // namespace arduino

using arduino::__FlashStringHelper;
using arduino::String;

#endif // __cplusplus
#endif // __ARDUINO_STRINGS__
