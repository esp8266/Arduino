/*
	String library for Wiring & Arduino
	...mostly rewritten by Paul Stoffregen...
	Copyright (c) 2009-10 Hernando Barragan.  All rights reserved.
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

#include "String.h"
#include "Common.h"
#include "itoa.h"
#include "avr/dtostrf.h"

#include <float.h>

namespace arduino
{

	/*********************************************/
	/*  Static Member Initialisation             */
	/*********************************************/

	size_t const String::FLT_MAX_DECIMAL_PLACES;
	size_t const String::DBL_MAX_DECIMAL_PLACES;

	/*********************************************/
	/*  Constructors                             */
	/*********************************************/

	String::String(const char *cstr)
	{
		init();
		if (cstr)
			copy(cstr, strlen(cstr));
	}

	String::String(const char *cstr, unsigned int length)
	{
		init();
		if (cstr)
			copy(cstr, length);
	}

	String::String(const String &value)
	{
		init();
		*this = value;
	}

	String::String(const __FlashStringHelper *pstr)
	{
		init();
		*this = pstr;
	}

	String::String(String &&rval)
			: buffer(rval.buffer), capacity(rval.capacity), len(rval.len)
	{
		rval.buffer = NULL;
		rval.capacity = 0;
		rval.len = 0;
	}

	String::String(char c)
	{
		init();
		char buf[2];
		buf[0] = c;
		buf[1] = 0;
		*this = buf;
	}

	String::String(unsigned char value, unsigned char base)
	{
		init();
		char buf[1 + 8 * sizeof(unsigned char)];
		utoa(value, buf, base);
		*this = buf;
	}

	String::String(int value, unsigned char base)
	{
		init();
		char buf[2 + 8 * sizeof(int)];
		itoa(value, buf, base);
		*this = buf;
	}

	String::String(unsigned int value, unsigned char base)
	{
		init();
		char buf[1 + 8 * sizeof(unsigned int)];
		utoa(value, buf, base);
		*this = buf;
	}

	String::String(long value, unsigned char base)
	{
		init();
		char buf[2 + 8 * sizeof(long)];
		ltoa(value, buf, base);
		*this = buf;
	}

	String::String(unsigned long value, unsigned char base)
	{
		init();
		char buf[1 + 8 * sizeof(unsigned long)];
		ultoa(value, buf, base);
		*this = buf;
	}

	String::String(float value, unsigned char decimalPlaces)
	{
		static size_t const FLOAT_BUF_SIZE = FLT_MAX_10_EXP + FLT_MAX_DECIMAL_PLACES + 1 /* '-' */ + 1 /* '.' */ + 1 /* '\0' */;
		init();
		char buf[FLOAT_BUF_SIZE];
		decimalPlaces = min(decimalPlaces, FLT_MAX_DECIMAL_PLACES);
		*this = dtostrf(value, (decimalPlaces + 2), decimalPlaces, buf);
	}

	String::String(double value, unsigned char decimalPlaces)
	{
		static size_t const DOUBLE_BUF_SIZE = DBL_MAX_10_EXP + DBL_MAX_DECIMAL_PLACES + 1 /* '-' */ + 1 /* '.' */ + 1 /* '\0' */;
		init();
		char buf[DOUBLE_BUF_SIZE];
		decimalPlaces = min(decimalPlaces, DBL_MAX_DECIMAL_PLACES);
		*this = dtostrf(value, (decimalPlaces + 2), decimalPlaces, buf);
	}

	String::~String()
	{
		if (buffer)
			free(buffer);
	}

	/*********************************************/
	/*  Memory Management                        */
	/*********************************************/

	inline void String::init(void)
	{
		buffer = NULL;
		capacity = 0;
		len = 0;
	}

	void String::invalidate(void)
	{
		if (buffer)
			free(buffer);
		buffer = NULL;
		capacity = len = 0;
	}

	bool String::reserve(unsigned int size)
	{
		if (buffer && capacity >= size)
			return 1;
		if (changeBuffer(size))
		{
			if (len == 0)
				buffer[0] = 0;
			return true;
		}
		return false;
	}

	bool String::changeBuffer(unsigned int maxStrLen)
	{
		char *newbuffer = (char *)realloc(buffer, maxStrLen + 1);
		if (newbuffer)
		{
			buffer = newbuffer;
			capacity = maxStrLen;
			return true;
		}
		return false;
	}

	/*********************************************/
	/*  Copy and Move                            */
	/*********************************************/

	String &String::copy(const char *cstr, unsigned int length)
	{
		if (!reserve(length))
		{
			invalidate();
			return *this;
		}
		len = length;
		memcpy(buffer, cstr, length);
		buffer[len] = '\0';
		return *this;
	}

	String &String::copy(const __FlashStringHelper *pstr, unsigned int length)
	{
		if (!reserve(length))
		{
			invalidate();
			return *this;
		}
		len = length;
		strcpy_P(buffer, (PGM_P)pstr);
		return *this;
	}

	void String::move(String &rhs)
	{
		if (this != &rhs)
		{
			free(buffer);

			buffer = rhs.buffer;
			len = rhs.len;
			capacity = rhs.capacity;

			rhs.buffer = NULL;
			rhs.len = 0;
			rhs.capacity = 0;
		}
	}

	String &String::operator=(const String &rhs)
	{
		if (this == &rhs)
			return *this;

		if (rhs.buffer)
			copy(rhs.buffer, rhs.len);
		else
			invalidate();

		return *this;
	}

	String &String::operator=(String &&rval)
	{
		move(rval);
		return *this;
	}

	String &String::operator=(const char *cstr)
	{
		if (cstr)
			copy(cstr, strlen(cstr));
		else
			invalidate();

		return *this;
	}

	String &String::operator=(const __FlashStringHelper *pstr)
	{
		if (pstr)
			copy(pstr, strlen_P((PGM_P)pstr));
		else
			invalidate();

		return *this;
	}

	/*********************************************/
	/*  concat                                   */
	/*********************************************/

	bool String::concat(const String &s)
	{
		return concat(s.buffer, s.len);
	}

	bool String::concat(const char *cstr, unsigned int length)
	{
		unsigned int newlen = len + length;
		if (!cstr)
			return false;
		if (length == 0)
			return true;
		if (!reserve(newlen))
			return false;
		memcpy(buffer + len, cstr, length);
		len = newlen;
		buffer[len] = '\0';
		return true;
	}

	bool String::concat(const char *cstr)
	{
		if (!cstr)
			return false;
		return concat(cstr, strlen(cstr));
	}

	bool String::concat(char c)
	{
		return concat(&c, 1);
	}

	bool String::concat(unsigned char num)
	{
		char buf[1 + 3 * sizeof(unsigned char)];
		itoa(num, buf, 10);
		return concat(buf);
	}

	bool String::concat(int num)
	{
		char buf[2 + 3 * sizeof(int)];
		itoa(num, buf, 10);
		return concat(buf);
	}

	bool String::concat(unsigned int num)
	{
		char buf[1 + 3 * sizeof(unsigned int)];
		utoa(num, buf, 10);
		return concat(buf);
	}

	bool String::concat(long num)
	{
		char buf[2 + 3 * sizeof(long)];
		ltoa(num, buf, 10);
		return concat(buf);
	}

	bool String::concat(unsigned long num)
	{
		char buf[1 + 3 * sizeof(unsigned long)];
		ultoa(num, buf, 10);
		return concat(buf);
	}

	bool String::concat(float num)
	{
		char buf[20];
		char *string = dtostrf(num, 4, 2, buf);
		return concat(string);
	}

	bool String::concat(double num)
	{
		char buf[20];
		char *string = dtostrf(num, 4, 2, buf);
		return concat(string);
	}

	bool String::concat(const __FlashStringHelper *str)
	{
		if (!str)
			return false;
		int length = strlen_P((const char *)str);
		if (length == 0)
			return true;
		unsigned int newlen = len + length;
		if (!reserve(newlen))
			return false;
		strcpy_P(buffer + len, (const char *)str);
		len = newlen;
		return true;
	}

	/*********************************************/
	/*  Concatenate                              */
	/*********************************************/

	StringSumHelper &operator+(const StringSumHelper &lhs, const String &rhs)
	{
		StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
		if (!a.concat(rhs.buffer, rhs.len))
			a.invalidate();
		return a;
	}

	StringSumHelper &operator+(const StringSumHelper &lhs, const char *cstr)
	{
		StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
		if (!cstr || !a.concat(cstr))
			a.invalidate();
		return a;
	}

	StringSumHelper &operator+(const StringSumHelper &lhs, char c)
	{
		StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
		if (!a.concat(c))
			a.invalidate();
		return a;
	}

	StringSumHelper &operator+(const StringSumHelper &lhs, unsigned char num)
	{
		StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
		if (!a.concat(num))
			a.invalidate();
		return a;
	}

	StringSumHelper &operator+(const StringSumHelper &lhs, int num)
	{
		StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
		if (!a.concat(num))
			a.invalidate();
		return a;
	}

	StringSumHelper &operator+(const StringSumHelper &lhs, unsigned int num)
	{
		StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
		if (!a.concat(num))
			a.invalidate();
		return a;
	}

	StringSumHelper &operator+(const StringSumHelper &lhs, long num)
	{
		StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
		if (!a.concat(num))
			a.invalidate();
		return a;
	}

	StringSumHelper &operator+(const StringSumHelper &lhs, unsigned long num)
	{
		StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
		if (!a.concat(num))
			a.invalidate();
		return a;
	}

	StringSumHelper &operator+(const StringSumHelper &lhs, float num)
	{
		StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
		if (!a.concat(num))
			a.invalidate();
		return a;
	}

	StringSumHelper &operator+(const StringSumHelper &lhs, double num)
	{
		StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
		if (!a.concat(num))
			a.invalidate();
		return a;
	}

	StringSumHelper &operator+(const StringSumHelper &lhs, const __FlashStringHelper *rhs)
	{
		StringSumHelper &a = const_cast<StringSumHelper &>(lhs);
		if (!a.concat(rhs))
			a.invalidate();
		return a;
	}

	/*********************************************/
	/*  Comparison                               */
	/*********************************************/

	int String::compareTo(const String &s) const
	{
		if (!buffer || !s.buffer)
		{
			if (s.buffer && s.len > 0)
				return 0 - *(unsigned char *)s.buffer;
			if (buffer && len > 0)
				return *(unsigned char *)buffer;
			return 0;
		}
		return strcmp(buffer, s.buffer);
	}

	int String::compareTo(const char *cstr) const
	{
		if (!buffer || !cstr)
		{
			if (cstr && *cstr)
				return 0 - *(unsigned char *)cstr;
			if (buffer && len > 0)
				return *(unsigned char *)buffer;
			return 0;
		}
		return strcmp(buffer, cstr);
	}

	bool String::equals(const String &s2) const
	{
		return (len == s2.len && compareTo(s2) == 0);
	}

	bool String::equals(const char *cstr) const
	{
		if (len == 0)
			return (cstr == NULL || *cstr == 0);
		if (cstr == NULL)
			return buffer[0] == 0;
		return strcmp(buffer, cstr) == 0;
	}

	bool String::equalsIgnoreCase(const String &s2) const
	{
		if (this == &s2)
			return true;
		if (len != s2.len)
			return false;
		if (len == 0)
			return true;
		const char *p1 = buffer;
		const char *p2 = s2.buffer;
		while (*p1)
		{
			if (tolower(*p1++) != tolower(*p2++))
				return false;
		}
		return true;
	}

	bool String::startsWith(const String &s2) const
	{
		if (len < s2.len)
			return false;
		return startsWith(s2, 0);
	}

	bool String::startsWith(const String &s2, unsigned int offset) const
	{
		if (offset > len - s2.len || !buffer || !s2.buffer)
			return false;
		return strncmp(&buffer[offset], s2.buffer, s2.len) == 0;
	}

	bool String::endsWith(const String &s2) const
	{
		if (len < s2.len || !buffer || !s2.buffer)
			return false;
		return strcmp(&buffer[len - s2.len], s2.buffer) == 0;
	}

	/*********************************************/
	/*  Character Access                         */
	/*********************************************/

	char String::charAt(unsigned int loc) const
	{
		return operator[](loc);
	}

	void String::setCharAt(unsigned int loc, char c)
	{
		if (loc < len)
			buffer[loc] = c;
	}

	char &String::operator[](unsigned int index)
	{
		static char dummy_writable_char;
		if (index >= len || !buffer)
		{
			dummy_writable_char = 0;
			return dummy_writable_char;
		}
		return buffer[index];
	}

	char String::operator[](unsigned int index) const
	{
		if (index >= len || !buffer)
			return 0;
		return buffer[index];
	}

	void String::getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index) const
	{
		if (!bufsize || !buf)
			return;
		if (index >= len)
		{
			buf[0] = 0;
			return;
		}
		unsigned int n = bufsize - 1;
		if (n > len - index)
			n = len - index;
		strncpy((char *)buf, buffer + index, n);
		buf[n] = 0;
	}

	/*********************************************/
	/*  Search                                   */
	/*********************************************/

	int String::indexOf(char c) const
	{
		return indexOf(c, 0);
	}

	int String::indexOf(char ch, unsigned int fromIndex) const
	{
		if (fromIndex >= len)
			return -1;
		const char *temp = strchr(buffer + fromIndex, ch);
		if (temp == NULL)
			return -1;
		return temp - buffer;
	}

	int String::indexOf(const String &s2) const
	{
		return indexOf(s2, 0);
	}

	int String::indexOf(const String &s2, unsigned int fromIndex) const
	{
		if (fromIndex >= len)
			return -1;
		const char *found = strstr(buffer + fromIndex, s2.buffer);
		if (found == NULL)
			return -1;
		return found - buffer;
	}

	int String::lastIndexOf(char theChar) const
	{
		return lastIndexOf(theChar, len - 1);
	}

	int String::lastIndexOf(char ch, unsigned int fromIndex) const
	{
		if (fromIndex >= len)
			return -1;
		char tempchar = buffer[fromIndex + 1];
		buffer[fromIndex + 1] = '\0';
		char *temp = strrchr(buffer, ch);
		buffer[fromIndex + 1] = tempchar;
		if (temp == NULL)
			return -1;
		return temp - buffer;
	}

	int String::lastIndexOf(const String &s2) const
	{
		return lastIndexOf(s2, len - s2.len);
	}

	int String::lastIndexOf(const String &s2, unsigned int fromIndex) const
	{
		if (s2.len == 0 || len == 0 || s2.len > len)
			return -1;
		if (fromIndex >= len)
			fromIndex = len - 1;
		int found = -1;
		for (char *p = buffer; p <= buffer + fromIndex; p++)
		{
			p = strstr(p, s2.buffer);
			if (!p)
				break;
			if ((unsigned int)(p - buffer) <= fromIndex)
				found = p - buffer;
		}
		return found;
	}

	String String::substring(unsigned int left, unsigned int right) const
	{
		if (left > right)
		{
			unsigned int temp = right;
			right = left;
			left = temp;
		}
		String out;
		if (left >= len)
			return out;
		if (right > len)
			right = len;
		out.copy(buffer + left, right - left);
		return out;
	}

	/*********************************************/
	/*  Modification                             */
	/*********************************************/

	void String::replace(char find, char replace)
	{
		if (!buffer)
			return;
		for (char *p = buffer; *p; p++)
		{
			if (*p == find)
				*p = replace;
		}
	}

	void String::replace(const String &find, const String &replace)
	{
		if (len == 0 || find.len == 0)
			return;
		int diff = replace.len - find.len;
		char *readFrom = buffer;
		char *foundAt;
		if (diff == 0)
		{
			while ((foundAt = strstr(readFrom, find.buffer)) != NULL)
			{
				memcpy(foundAt, replace.buffer, replace.len);
				readFrom = foundAt + replace.len;
			}
		}
		else if (diff < 0)
		{
			unsigned int size = len; // compute size needed for result
			diff = 0 - diff;
			while ((foundAt = strstr(readFrom, find.buffer)) != NULL)
			{
				readFrom = foundAt + find.len;
				size -= diff;
			}
			if (size == len)
				return;
			int index = len - 1;
			while (index >= 0 && (index = lastIndexOf(find, index)) >= 0)
			{
				readFrom = buffer + index + find.len;
				memmove(readFrom - diff, readFrom, len - (readFrom - buffer));
				len -= diff;
				buffer[len] = 0;
				memcpy(buffer + index, replace.buffer, replace.len);
				index--;
			}
		}
		else
		{
			unsigned int size = len; // compute size needed for result
			while ((foundAt = strstr(readFrom, find.buffer)) != NULL)
			{
				readFrom = foundAt + find.len;
				size += diff;
			}
			if (size == len)
				return;
			if (size > capacity && !changeBuffer(size))
				return; // XXX: tell user!
			int index = len - 1;
			while (index >= 0 && (index = lastIndexOf(find, index)) >= 0)
			{
				readFrom = buffer + index + find.len;
				memmove(readFrom + diff, readFrom, len - (readFrom - buffer));
				len += diff;
				buffer[len] = 0;
				memcpy(buffer + index, replace.buffer, replace.len);
				index--;
			}
		}
	}

	void String::remove(unsigned int index)
	{
		// Pass the biggest integer as the count. The remove method
		// below will take care of truncating it at the end of the
		// string.
		remove(index, (unsigned int)-1);
	}

	void String::remove(unsigned int index, unsigned int count)
	{
		if (index >= len)
		{
			return;
		}
		if (count <= 0)
		{
			return;
		}
		if (count > len - index)
		{
			count = len - index;
		}
		char *writeTo = buffer + index;
		len = len - count;
		memmove(writeTo, buffer + index + count, len - index);
		buffer[len] = 0;
	}

	void String::toLowerCase(void)
	{
		if (!buffer)
			return;
		for (char *p = buffer; *p; p++)
		{
			*p = tolower(*p);
		}
	}

	void String::toUpperCase(void)
	{
		if (!buffer)
			return;
		for (char *p = buffer; *p; p++)
		{
			*p = toupper(*p);
		}
	}

	void String::trim(void)
	{
		if (!buffer || len == 0)
			return;
		char *begin = buffer;
		while (isspace(*begin))
			begin++;
		char *end = buffer + len - 1;
		while (isspace(*end) && end >= begin)
			end--;
		len = end + 1 - begin;
		if (begin > buffer)
			memmove(buffer, begin, len);
		buffer[len] = 0;
	}

	/*********************************************/
	/*  Parsing / Conversion                     */
	/*********************************************/

	long String::toInt(void) const
	{
		if (buffer)
			return atol(buffer);
		return 0;
	}

	float String::toFloat(void) const
	{
		return float(toDouble());
	}

	double String::toDouble(void) const
	{
		if (buffer)
			return atof(buffer);
		return 0;
	}

} // namespace arduino
