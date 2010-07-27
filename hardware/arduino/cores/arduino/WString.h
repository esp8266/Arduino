/*
 WString.h - String library for Wiring & Arduino
 Copyright (c) 2009-10 Hernando Barragan.  All right reserved.
 Changes for version 1.0 by Xiaoyang Feng

 
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

#ifndef String_h
#define String_h

//#include "WProgram.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

class String
{
public:
    // constructors
    String( const char* value = "" );
    String( const String &value );
    explicit String( const char );
    explicit String( const unsigned char );
    explicit String( const int, const int base=10);
    explicit String( const unsigned int, const int base=10 );//negative sign only works on decimal
    explicit String( const long, const int base=10 );
    explicit String( const unsigned long, const int base=10 );//need add function to detect if number is bigger than the limit
    ~String() { free(_buffer); _length = _capacity = 0;}     //added _length = _capacity = 0;
	
    // operators
    const String & operator = ( const String &rhs );
    const String & operator +=( const String &rhs );
    const String & operator +=( const char );
    int operator ==( const String &rhs ) const;				
    int	operator !=( const String &rhs ) const;
	//is this useful?
    int	operator < ( const String &rhs ) const;
    int	operator > ( const String &rhs ) const;
    int	operator <=( const String &rhs ) const;
    int	operator >=( const String &rhs ) const;
	
    friend String operator + ( String lhs, const String &rhs );
	
    char operator []( unsigned int index ) const;
    char& operator []( unsigned int index );
	
    // general methods
    char charAt( unsigned int index ) const;
    void setCharAt(unsigned int index, const char ch);
    boolean startsWith( const String &prefix ) const;
    boolean startsWith( const String &prefix, unsigned int toffset ) const;
    boolean endsWith( const String &suffix ) const;
	boolean contains( const String &str ) const;		//added
    boolean equals( const String &str ) const;
    boolean equalsIgnoreCase( const String &str ) const;
	
    String trim( ) const;
    String toLowerCase( ) const;
    String toUpperCase( ) const;
    String replace( char oldChar, char newChar );
    String replace( const String& match, const String& replace );
    String substring( unsigned int beginIndex ) const;
    String substring( unsigned int beginIndex, unsigned int endIndex ) const;
    const String& append( const String &str );
	const String& append( const char );
    
	int	compareTo( const String &str ) const;
	int	compareTo( const char* str ) const;			//added
	
    int	indexOf( char ch ) const;
    int	indexOf( char ch, unsigned int fromIndex ) const;
    int	indexOf( const String &str ) const;
    int	indexOf( const String &str, unsigned int fromIndex ) const;
    int	lastIndexOf( char ch ) const;
    int	lastIndexOf( char ch, unsigned int fromIndex ) const;	  //from right to left?
    int	lastIndexOf( const String &str ) const;
    int	lastIndexOf( const String &str, unsigned int fromIndex ) const;
	
    const unsigned int length( ) const { return _length; }
    const byte *getBytes() const { return (byte*)_buffer; }
    const char* toCharArray() const { return _buffer; }
	
	//version checking
	String version(void);	// the version number
	
protected:
    char *_buffer;	     // the actual char array
    unsigned int _capacity;  // the array length minus one (for the '\0')
    unsigned int _length;    // the String length (not counting the '\0')
	
    void getBuffer(unsigned int maxStrLen);
    //void doubleBuffer( );
	
private:
	
};

// allocate buffer space
inline void String::getBuffer(unsigned int maxStrLen)
{
	_capacity = maxStrLen;
	_buffer = (char *) malloc(_capacity + 1);
	if(_buffer == NULL){							//added for memory alloc error
		_length = _capacity = 0;
	}
}

/* used only once, really not effcient 
 // double the buffer size
 inline void String::doubleBuffer( )
 {
 char *temp = _buffer;
 getBuffer( ++_capacity * 2 );
 strcpy( _buffer, temp );
 free(temp);
 }
 */


inline String operator+( String lhs, const String &rhs )
{
	return lhs += rhs;
}


#endif